#include "ModelManager.h"
#include <DirectXTex.h>
#include <cassert>
#include "DirectXCommon.h"
#include "TextureManager.h"
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace DirectX;

uint32_t ModelManager::Load(const std::string& fileName) {
	return ModelManager::GetInstance()->LoadInternal(fileName);
}

void ModelManager::CreateMeshes(ModelData& modelData)
{
	HRESULT result = S_FALSE;

	//拡張子がない名前を取得
	std::size_t dotPos = modelData.name.find_last_of('.');
	std::string n = modelData.name.substr(0, dotPos);

	std::string directoryPath = "Resources/models/" + n + "/";

	Assimp::Importer importer;
	std::string filePath = directoryPath + modelData.name;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());
	modelData.meshes.resize(scene->mNumMeshes);

	Vector3 minModelSize{};
	Vector3 maxModelSize{ FLT_MIN,FLT_MIN,FLT_MIN };

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			Mesh::VertexData vertex;
			vertex.pos = { position.x,position.y,position.z };
			vertex.normal = { normal.x,normal.y,normal.z };
			vertex.uv = { texcoord.x,texcoord.y };

			vertex.pos.z *= -1.0f;
			vertex.normal.z *= -1.0f;

			if (minModelSize.x > position.x) {
				minModelSize.x = position.x;
			}
			if (maxModelSize.x < position.x) {
				maxModelSize.x = position.x;
			}
			if (minModelSize.y > position.y) {
				minModelSize.y = position.y;
			}
			if (maxModelSize.y < position.y) {
				maxModelSize.y = position.y;
			}
			if (minModelSize.z > position.z) {
				minModelSize.z = position.z;
			}
			if (maxModelSize.z < position.z) {
				maxModelSize.z = position.z;
			}

			modelData.meshes[meshIndex].vertices_.push_back(vertex);
		}

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.meshes[meshIndex].indices_.push_back(vertexIndex);
			}
		}

		//material
		
		int materialIndex = mesh->mMaterialIndex;
		aiMaterial* material = scene->mMaterials[materialIndex];
		aiString textureFilePath;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
		std::string a = textureFilePath.C_Str();
		if (a != "") {
			modelData.meshes[meshIndex].uvHandle_ = TextureManager::LoadUv(textureFilePath.C_Str(), directoryPath + textureFilePath.C_Str());
		}
		
	}

	modelData.modelSize = (maxModelSize - minModelSize);
	modelData.modelCenter = maxModelSize - Vector3(modelData.modelSize / 2.0f);
	//もしmodelの原点を一番下にしていたら
	if (minModelSize.y < 0.5f && minModelSize.y > -0.5f) {
		modelData.modelCenter.y = minModelSize.y;
	}


	for (auto& mesh : modelData.meshes) {

		// 頂点データのサイズ
		UINT sizeVB = static_cast<UINT>(sizeof(Mesh::VertexData) * mesh.vertices_.size());

		mesh.vertexBuffer_.Create(sizeVB);

		mesh.vertexBuffer_.Copy(mesh.vertices_.data(), sizeVB);

		// 頂点バッファビューの作成
		mesh.vbView_.BufferLocation = mesh.vertexBuffer_->GetGPUVirtualAddress();
		mesh.vbView_.SizeInBytes = sizeVB;
		mesh.vbView_.StrideInBytes = sizeof(mesh.vertices_[0]);


		// インデックスデータのサイズ
		UINT sizeIB = static_cast<UINT>(sizeof(uint32_t) * mesh.indices_.size());

		mesh.indexBuffer_.Create(sizeIB);

		mesh.indexBuffer_.Copy(mesh.indices_.data(), sizeIB);

		// インデックスバッファビューの作成
		mesh.ibView_.BufferLocation = mesh.indexBuffer_->GetGPUVirtualAddress();
		mesh.ibView_.Format = DXGI_FORMAT_R32_UINT;
		mesh.ibView_.SizeInBytes = sizeIB;
	}

	//node
	modelData.rootNode = ReadNode(scene->mRootNode);
}

void ModelManager::Finalize()
{
	models_.reset();
}


ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;
	if (!instance.models_) {
		instance.models_ = std::make_unique<std::array<ModelData, kNumModels>>();
	}
	return &instance;
}

void ModelManager::DrawInstanced(CommandContext* commandContext, uint32_t modelHandle)
{
	assert(modelHandle < kNumModels);

	const auto& modelItem = (*models_)[modelHandle];

	for (const auto& mesh : modelItem.meshes) {

		// 頂点バッファの設定
		commandContext->SetVertexBuffer(0, 1, mesh.GetVbView());

		// インデックスバッファの設定
		commandContext->SetIndexBuffer(*mesh.GetIbView());
		// 描画コマンド
		commandContext->DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), 1, 0, 0, 0);
	}
}

void ModelManager::DrawInstanced(CommandContext* commandContext, uint32_t modelHandle, UINT textureRootParamterIndex) {
	assert(modelHandle < kNumModels);

	const auto& modelItem = (*models_)[modelHandle];

	for (const auto& mesh : modelItem.meshes) {
		// srvセット
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandContext, textureRootParamterIndex, mesh.GetUv());
		// 頂点バッファの設定
		commandContext->SetVertexBuffer(0, 1, mesh.GetVbView());
		// インデックスバッファの設定
		commandContext->SetIndexBuffer(*mesh.GetIbView());
		// 描画コマンド
		commandContext->DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), 1, 0, 0, 0);
	}
}

void ModelManager::DrawInstanced(CommandContext* commandContext, uint32_t modelHandle, UINT textureRootParamterIndex, DescriptorHandle descriptorHandle) {
	assert(modelHandle < kNumModels);

	const auto& modelItem = (*models_)[modelHandle];

	for (const auto& mesh : modelItem.meshes) {
		// srvセット
		commandContext->SetDescriptorTable(textureRootParamterIndex, descriptorHandle);
		// 頂点バッファの設定
		commandContext->SetVertexBuffer(0, 1, mesh.GetVbView());
		// インデックスバッファの設定
		commandContext->SetIndexBuffer(*mesh.GetIbView());
		// 描画コマンド
		commandContext->DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), 1, 0, 0, 0);
	}
}


void ModelManager::DrawInstanced(CommandContext* commandContext, uint32_t modelHandle, UINT textureRootParamterIndex, uint32_t textureHandle)
{
	assert(modelHandle < kNumModels);

	const auto& modelItem = (*models_)[modelHandle];
	
	for (const auto& mesh : modelItem.meshes) {
		// srvセット
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandContext, textureRootParamterIndex, textureHandle);
		// 頂点バッファの設定
		commandContext->SetVertexBuffer(0, 1, mesh.GetVbView());
		// インデックスバッファの設定
		commandContext->SetIndexBuffer(*mesh.GetIbView());
		// 描画コマンド
		commandContext->DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), 1, 0, 0, 0);
	}
}

uint32_t ModelManager::LoadInternal(const std::string& fileName) {

	assert(useModelCount_ < kNumModels);
	uint32_t handle = useModelCount_;

	// 読み込み済みmodelを検索
	auto it = std::find_if(models_->begin(), models_->end(), [&](const auto& texture) {return texture.name == fileName; });

	if (it != models_->end()) {
		// 読み込み済みmodelの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(models_->begin(), it));
		return handle;
	}

	// 書き込むmodelの参照
	auto& modelData = (models_->at(useModelCount_));

	modelData.name = fileName;

	CreateMeshes(modelData);

	useModelCount_++;
	return handle;
}

Vector3 ModelManager::GetModelSize(uint32_t modelHandle)
{
	return (*models_)[modelHandle].modelSize;
}

Vector3 ModelManager::GetModelCenter(uint32_t modelHandle)
{
	return  (*models_)[modelHandle].modelCenter;
}

Node& ModelManager::GetRootNode(uint32_t modelHandle)
{
	return  (*models_)[modelHandle].rootNode;
}


void ModelManager::DrawInstancing(CommandContext* commandContext, uint32_t modelHandle, UINT instancingNum, UINT textureRootParamterIndex) {
	assert(modelHandle < kNumModels);

	const auto& modelItem = (*models_)[modelHandle];
	

	for (const auto& mesh : modelItem.meshes) {
		// srvセット
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandContext, textureRootParamterIndex, mesh.GetUv());
		// 頂点バッファの設定
		commandContext->SetVertexBuffer(0, 1, mesh.GetVbView());
		// インデックスバッファの設定
		commandContext->SetIndexBuffer(*mesh.GetIbView());
		// 描画コマンド
		commandContext->DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), instancingNum, 0, 0, 0);
	}
}

Node ModelManager::ReadNode(aiNode* node)
{
	Node result;
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	aiLocalMatrix.Transpose();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.localMatrix.m[i][j] = aiLocalMatrix[i][j];
		}
	}

	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}
