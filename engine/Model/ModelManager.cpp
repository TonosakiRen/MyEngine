#include "Model/ModelManager.h"
#include <DirectXTex.h>
#include <cassert>
#include "Graphics/DirectXCommon.h"
#include "Texture/TextureManager.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <DirectXMesh.h>
#include "Graphics/Helper.h"
#include "GPUResource/ByteAddressBuffer.h"


#include "Mesh/MeshletModel.h"

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

	//debug
	//directoryPath = "Resources/models/box1x1/";

	Assimp::Importer importer;
	std::string filePath = directoryPath + modelData.name;

	//debug
	//filePath = directoryPath + "box1x1.obj";

	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());
	modelData.meshes.resize(scene->mNumMeshes);

	Vector3 minModelSize{};
	Vector3 maxModelSize{ FLT_MIN,FLT_MIN,FLT_MIN };

	//mesh
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			XMFLOAT3 p;
			Mesh::VertexData vertex;
			vertex.pos = { position.x,position.y,position.z };
			p = { position.x,position.y,position.z };
			vertex.normal = { normal.x,normal.y,normal.z };
			vertex.uv = { texcoord.x,texcoord.y };

			vertex.pos.x *= -1.0f;
			vertex.normal.x *= -1.0f;

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
			//meshlet用
			modelData.meshes[meshIndex].positions_.push_back(p);
		}

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			modelData.meshes[meshIndex].indices_.push_back(face.mIndices[0]);
			modelData.meshes[meshIndex].indices_.push_back(face.mIndices[1]);
			modelData.meshes[meshIndex].indices_.push_back(face.mIndices[2]);
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
		
		//skincluster
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData= modelData.skinClusterData[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale{}, translate{};
			aiQuaternion rotate{};
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix4x4 bindPoseMatrix = MakeAffineMatrix
			({ scale.x,scale.y,scale.z }, { rotate.x,-rotate.y,-rotate.z,rotate.w }, { -translate.x,translate.y,translate.z });
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight , bone->mWeights[weightIndex].mVertexId });
			}
		}
	}

	modelData.modelSize = (maxModelSize - minModelSize);
	modelData.modelCenter = maxModelSize - Vector3(modelData.modelSize / 2.0f);
	
	modelData.modelSphere.center = modelData.modelCenter;
	modelData.modelSphere.radius = Length(modelData.modelSize) * 0.5f;

	for (auto& mesh : modelData.meshes) {

		// 頂点データのサイズ
		UINT sizeVB = static_cast<UINT>(sizeof(Mesh::VertexData) * mesh.vertices_.size());

		mesh.vertexBuffer_.Create(L"vertexBuffer",sizeof(Mesh::VertexData), UINT(mesh.vertices_.size()));

		mesh.vertexBuffer_.Copy(mesh.vertices_.data(), sizeVB, *commandContext_);

		mesh.vertexBuffer_.DestroyCopyBuffer();

		// 頂点バッファビューの作成
		mesh.vbView_.BufferLocation = mesh.vertexBuffer_->GetGPUVirtualAddress();
		mesh.vbView_.SizeInBytes = sizeVB;
		mesh.vbView_.StrideInBytes = sizeof(mesh.vertices_[0]);


		// インデックスデータのサイズ
		UINT sizeIB = static_cast<UINT>(sizeof(uint32_t) * mesh.indices_.size());

		mesh.indexBuffer_.Create(L"indexBuffer", sizeof(uint32_t), UINT(mesh.indices_.size()));

		mesh.indexBuffer_.Copy(mesh.indices_.data(), sizeIB, *commandContext_);

		mesh.indexBuffer_.DestroyCopyBuffer();

		// インデックスバッファビューの作成
		mesh.ibView_.BufferLocation = mesh.indexBuffer_->GetGPUVirtualAddress();
		mesh.ibView_.Format = DXGI_FORMAT_R32_UINT;
		mesh.ibView_.SizeInBytes = sizeIB;


		// Meshletの作成
		Helper::AssertIfFailed(DirectX::ComputeMeshlets(
			mesh.indices_.data(), mesh.indices_.size() / 3,
			mesh.positions_.data(), mesh.positions_.size(),
			nullptr,
			mesh.meshlets_,
			mesh.uniqueVertexIndex,
			mesh.primitiveIndices_));

		mesh.meshletBuffer_.Create(L"meshletBuffer", sizeof(DirectX::Meshlet), UINT(mesh.meshlets_.size()));
		mesh.meshletBuffer_.Copy(mesh.meshlets_.data(), sizeof(DirectX::Meshlet) * mesh.meshlets_.size(),*commandContext_);
		mesh.meshletBuffer_.DestroyCopyBuffer();

		mesh.uniqueVertexIndexBuffer_.Create(L"uniqueVertexIndexBuffer", sizeof(uint8_t), UINT(mesh.uniqueVertexIndex.size()));
		mesh.uniqueVertexIndexBuffer_.Copy(mesh.uniqueVertexIndex.data(), sizeof(uint8_t)* mesh.uniqueVertexIndex.size(), *commandContext_);
		mesh.uniqueVertexIndexBuffer_.DestroyCopyBuffer();

		mesh.primitiveIndicesBuffer_.Create(L"primitiveIndicesBuffer", sizeof(DirectX::MeshletTriangle), UINT(mesh.primitiveIndices_.size()));
		mesh.primitiveIndicesBuffer_.Copy(mesh.primitiveIndices_.data(), sizeof(DirectX::MeshletTriangle)* mesh.primitiveIndices_.size(), *commandContext_);
		mesh.primitiveIndicesBuffer_.DestroyCopyBuffer();

		mesh.meshletInfo_.Create(L"meshletInfoBuffer", sizeof(uint32_t));
		mesh.meshletInfo_.Copy(uint32_t(mesh.meshlets_.size()),*commandContext_);
		mesh.meshletInfo_.DestroyCopyBuffer();

		//meshletカリング作成
		mesh.cullData_.resize(mesh.meshlets_.size());
		DirectX::ComputeCullData(mesh.positions_.data(), mesh.positions_.size(),
			mesh.meshlets_.data(), mesh.meshlets_.size(),
			reinterpret_cast<uint32_t*>(mesh.uniqueVertexIndex.data()), mesh.uniqueVertexIndex.size(),
			mesh.primitiveIndices_.data(), mesh.primitiveIndices_.size(),
			mesh.cullData_.data());

		for (auto& cullData : mesh.cullData_) {
			//なぜか反対
			cullData.BoundingSphere.Center.x *= -1.0f;
		}

		mesh.cullDataBuffer_.Create(L"cullDataBuffer", sizeof(DirectX::CullData), UINT(mesh.cullData_.size()));
		mesh.cullDataBuffer_.Copy(mesh.cullData_.data(), sizeof(DirectX::CullData) * mesh.cullData_.size(), *commandContext_);
		mesh.cullDataBuffer_.DestroyCopyBuffer();
	}

	//node
	modelData.rootNode = ReadNode(scene->mRootNode);

	//blasの生成
	for (auto& mesh : modelData.meshes) {

		commandContext_->TransitionResource(mesh.indexBuffer_, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		commandContext_->TransitionResource(mesh.vertexBuffer_, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		D3D12_RAYTRACING_GEOMETRY_DESC geomDesc{};
		geomDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
		geomDesc.Triangles.VertexBuffer.StartAddress = mesh.vertexBuffer_.GetGPUVirtualAddress();
		geomDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Mesh::VertexData);
		geomDesc.Triangles.VertexCount = UINT(mesh.vertices_.size());

		geomDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;

		geomDesc.Triangles.IndexBuffer = mesh.indexBuffer_->GetGPUVirtualAddress();
		geomDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;  // インデックスのフォーマット
		geomDesc.Triangles.IndexCount = static_cast<UINT>(mesh.indices_.size());

		geomDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE; //不透明

		mesh.blasBuffer_.Create(L"BlasBuffer", geomDesc,*commandContext_);
	}
	
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

void ModelManager::Initialize(CommandContext& commandContext)
{
	commandContext_ = &commandContext;
	directXCommon_ = DirectXCommon::GetInstance();
}

void ModelManager::DrawInstanced(CommandContext& commandContext, const  uint32_t modelHandle)
{
	assert(modelHandle < kNumModels);

	auto& modelItem = (*models_)[modelHandle];

	for (auto& mesh : modelItem.meshes) {

		commandContext_->TransitionResource(mesh.vertexBuffer_, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		commandContext_->TransitionResource(mesh.indexBuffer_, D3D12_RESOURCE_STATE_INDEX_BUFFER);

		// 頂点バッファの設定
		commandContext.SetVertexBuffer(0, 1, mesh.GetVbView());

		// インデックスバッファの設定
		commandContext.SetIndexBuffer(*mesh.GetIbView());
		// 描画コマンド
		commandContext.DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), 1, 0, 0, 0);
	}
}

void ModelManager::DrawInstanced(CommandContext& commandContext, const  uint32_t modelHandle,const UINT textureRootParamterIndex , const uint32_t textureHandle) {
	assert(modelHandle < kNumModels);

	auto& modelItem = (*models_)[modelHandle];

	for (auto& mesh : modelItem.meshes) {
		// srvセット
		if (mesh.GetUv() != 0) {
			TextureManager::GetInstance()->SetDescriptorTable(commandContext, textureRootParamterIndex, mesh.GetUv());
		}
		else {
			TextureManager::GetInstance()->SetDescriptorTable(commandContext, textureRootParamterIndex, textureHandle);
		}
		commandContext_->TransitionResource(mesh.vertexBuffer_, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		commandContext_->TransitionResource(mesh.indexBuffer_, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		// 頂点バッファの設定
		commandContext.SetVertexBuffer(0, 1, mesh.GetVbView());
		// インデックスバッファの設定
		commandContext.SetIndexBuffer(*mesh.GetIbView());
		// 描画コマンド
		commandContext.DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), 1, 0, 0, 0);
	}
}

void ModelManager::DrawInstanced(CommandContext& commandContext, const  uint32_t modelHandle, UINT textureRootParamterIndex, DescriptorHandle descriptorHandle) {
	assert(modelHandle < kNumModels);

	auto& modelItem = (*models_)[modelHandle];

	for (auto& mesh : modelItem.meshes) {
		commandContext_->TransitionResource(mesh.vertexBuffer_, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		commandContext_->TransitionResource(mesh.indexBuffer_, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		// srvセット
		commandContext.SetDescriptorTable(textureRootParamterIndex, descriptorHandle);
		// 頂点バッファの設定
		commandContext.SetVertexBuffer(0, 1, mesh.GetVbView());
		// インデックスバッファの設定
		commandContext.SetIndexBuffer(*mesh.GetIbView());
		// 描画コマンド
		commandContext.DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), 1, 0, 0, 0);
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

const Vector3& ModelManager::GetModelSize(const uint32_t modelHandle)
{
	return (*models_)[modelHandle].modelSize;
}

const Vector3& ModelManager::GetModelCenter(const uint32_t modelHandle)
{
	return  (*models_)[modelHandle].modelCenter;
}

const Sphere& ModelManager::GetModelSphere(const uint32_t modelHandle)
{
	return  (*models_)[modelHandle].modelSphere;
}

Node& ModelManager::GetRootNode(const uint32_t modelHandle)
{
	return  (*models_)[modelHandle].rootNode;
}

ModelData& ModelManager::GetModelData(const uint32_t modelHandle)
{
	return  (*models_)[modelHandle];
}

const std::vector<CullData>& ModelManager::GetCullDataData(const uint32_t modelHandle)
{
	return  (*models_)[modelHandle].meshes[0].cullData_;
}


void ModelManager::DrawInstancing(CommandContext& commandContext, const uint32_t modelHandle, UINT instancingNum, UINT textureRootParamterIndex) {
	assert(modelHandle < kNumModels);

	auto& modelItem = (*models_)[modelHandle];
	

	for (auto& mesh : modelItem.meshes) {
		commandContext_->TransitionResource(mesh.vertexBuffer_, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		commandContext_->TransitionResource(mesh.indexBuffer_, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		// srvセット
		TextureManager::GetInstance()->SetDescriptorTable(commandContext, textureRootParamterIndex, mesh.GetUv());

		// 頂点バッファの設定
		commandContext.SetVertexBuffer(0, 1, mesh.GetVbView());
		// インデックスバッファの設定
		commandContext.SetIndexBuffer(*mesh.GetIbView());
		// 描画コマンド
		commandContext.DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), instancingNum, 0, 0, 0);
	}
}

void ModelManager::DrawInstanced(CommandContext& commandContext, const uint32_t modelHandle, SkinCluster& skincluster, const UINT textureRootParamterIndex, const uint32_t textureHandle)
{
	assert(modelHandle < kNumModels);

	auto& modelItem = (*models_)[modelHandle];

	for (auto& mesh : modelItem.meshes) {
		commandContext_->TransitionResource(mesh.vertexBuffer_, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		commandContext_->TransitionResource(mesh.indexBuffer_, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		// srvセット
		if (mesh.GetUv() != 0) {
			TextureManager::GetInstance()->SetDescriptorTable(commandContext, textureRootParamterIndex, mesh.GetUv());
		}
		else {
			TextureManager::GetInstance()->SetDescriptorTable(commandContext, textureRootParamterIndex, textureHandle);
		}
		commandContext.TransitionResource(skincluster.GetSkinnedVertices(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		// 頂点バッファの設定
		commandContext.SetVertexBuffer(0, 1, &skincluster.GetSkinnedVertexBufferView());
		// インデックスバッファの設定
		commandContext.SetIndexBuffer(*mesh.GetIbView());
		// 描画コマンド
		commandContext.DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), 1, 0, 0, 0);
	}
}

Node ModelManager::ReadNode(aiNode* node)
{
	Node result;

	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);
	result.transform.scale_ = { scale.x,scale.y,scale.z };
	result.transform.quaternion_ = { rotate.x,-rotate.y,-rotate.z,rotate.w };
	result.transform.translation_ = { -translate.x,translate.y,translate.z };
	result.localMatrix = MakeAffineMatrix(result.transform.scale_, result.transform.quaternion_, result.transform.translation_);


	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}
