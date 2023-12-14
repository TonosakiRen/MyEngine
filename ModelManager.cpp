#include "ModelManager.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <cassert>
#include "DirectXCommon.h"
#include "TextureManager.h"
#include <fstream>
#include <sstream>
using namespace DirectX;

uint32_t ModelManager::Load(const std::string& fileName) {
	return ModelManager::GetInstance()->LoadInternal(fileName);
}

std::vector<Mesh> ModelManager::CreateMeshes(const std::string& fileName)
{
    HRESULT result = S_FALSE;

    std::vector<Mesh> meshes;
    meshes.resize(1);

    std::string line;//ファイルから読んだ1行を格納するもの
    std::vector<Vector3> positions;//位置
    std::vector<Vector3> normals;//法線
    std::vector<Vector2> texcoords;//テクスチャ座標
    std::vector<uint16_t> indexes; //index
    std::unordered_map<std::string, uint16_t> vertexDefinitionMap;
    std::string directoryPath = "Resources/models/" + fileName + "/";
    std::ifstream file(directoryPath + fileName + ".obj"); //ファイルを開く
    assert(file.is_open());//とりあえず開けなったら止める

    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;//先頭の識別子を読む

        if (identifier == "v") {
            Vector3 position;
            s >> position.x >> position.y >> position.z;
           /* position.z *= -1.0f;*/
            /*position.w = 1.0f;*/
            positions.push_back(position);
        }
        else if (identifier == "vn") {
            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
           /* normal.z *= -1.0f;*/
            normals.push_back(normal);
        }
        else if (identifier == "vt") {
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y;
            texcoord.y = 1.0f - texcoord.y;
            texcoords.push_back(texcoord);
        }
        else if (identifier == "f") {
            // 面の頂点を取得
            std::vector<std::string> vertexDefinitions;
            while (true) {
                std::string vertexDefinition;
                s >> vertexDefinition;
                if (vertexDefinition.empty()) {
                    break;
                }
                vertexDefinitions.emplace_back(std::move(vertexDefinition));
            }
            assert(vertexDefinitions.size() > 2);
            std::vector<uint16_t> face(vertexDefinitions.size());
            for (uint32_t i = 0; i < vertexDefinitions.size(); ++i) {
                // 頂点が登録済み
                if (vertexDefinitionMap.contains(vertexDefinitions[i])) {
                    face[i] = vertexDefinitionMap[vertexDefinitions[i]];
                }
                else {
                    std::istringstream v(vertexDefinitions[i]);
                    uint16_t elementIndices[3] = {0,0,0};
                    bool useElement[3]{};
                    for (int32_t element = 0; element < 3; ++element) {
                        std::string index;
                        std::getline(v, index, '/');//区切りでインデックスを読んでいく
                        if (!index.empty()) {
                            elementIndices[element] = static_cast<uint16_t>(std::stoi(index)) - 1;
                            useElement[element] = true;
                        }
                    }
                    auto& vertex = meshes[0].vertices_.emplace_back();
                    vertex.pos = positions[elementIndices[0]];
                    if (useElement[1]) {
                        vertex.uv = texcoords[elementIndices[1]];
                    }
                    if (useElement[2]) {
                        vertex.normal = normals[elementIndices[2]];
                    }
                    face[i] = vertexDefinitionMap[vertexDefinitions[i]] = static_cast<uint16_t>(meshes[0].vertices_.size() - 1);
                }
            }


            for (uint32_t i = 0; i < face.size() - 2; ++i) {
                //反対から
               /* meshes[0].indices_.emplace_back(face[i + 2ull]);
                meshes[0].indices_.emplace_back(face[i + 1ull]);
                meshes[0].indices_.emplace_back(face[0]);*/

                meshes[0].indices_.emplace_back(face[0]);
                meshes[0].indices_.emplace_back(face[i + 1ull]);
                meshes[0].indices_.emplace_back(face[i + 2ull]);
            }
        }
        else if (identifier == "mtllib") {
            //materialTemplateLibraryファイルの名前を取得する
            std::string materialFilename;
            s >> materialFilename;
            //基本的にobjファイルと同一階層にmtlは存在させるので,ディレクトリ名とファイル名を渡す

            std::string uvFilePass;//構築するMaterialData
            std::string line;//fileから読んだ１行を格納するもの
            std::ifstream file("Resources/models/" + fileName + "/" + fileName + ".mtl"); //ファイルを開く
            assert(file.is_open());//とりあえず開けなかったら止める
            while (std::getline(file, line)) {
                std::string identifier;
                std::istringstream s(line);
                s >> identifier;
                //identifierに応じた処理
                if (identifier == "map_Kd") {
                    std::string textureFilename;
                    s >> textureFilename;
                    //連結してファイルパスにする
                    uvFilePass = directoryPath + textureFilename;
                    meshes[0].uvHandle_ = TextureManager::LoadUv(textureFilename, uvFilePass);
                }
            }
        }
    }

    // 頂点データのサイズ
    UINT sizeVB = static_cast<UINT>(sizeof(Mesh::VertexData) * meshes[0].vertices_.size());

    meshes[0].vertexBuffer_.Create(sizeVB);

    meshes[0].vertexBuffer_.Copy(meshes[0].vertices_.data(), sizeVB);

    // 頂点バッファビューの作成
    meshes[0].vbView_.BufferLocation = meshes[0].vertexBuffer_.GetGPUVirtualAddress();
    meshes[0].vbView_.SizeInBytes = sizeVB;
    meshes[0].vbView_.StrideInBytes = sizeof(meshes[0].vertices_[0]);

    // インデックスデータのサイズ
    UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * meshes[0].indices_.size());

    meshes[0].indexBuffer_.Create(sizeIB);

    meshes[0].indexBuffer_.Copy(meshes[0].indices_.data(), sizeIB);

    // インデックスバッファビューの作成
    meshes[0].ibView_.BufferLocation = meshes[0].indexBuffer_.GetGPUVirtualAddress();
    meshes[0].ibView_.Format = DXGI_FORMAT_R16_UINT;
    meshes[0].ibView_.SizeInBytes = sizeIB;

    return meshes;
}


ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;
	return &instance;
}

void ModelManager::DrawInstanced(ID3D12GraphicsCommandList* commandList, uint32_t modelHandle, UINT textureRootParamterIndex) {
	assert(modelHandle < kNumModels);
    for (auto& mesh : models_[modelHandle].meshes) {
        // srvセット
        TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureRootParamterIndex, mesh.GetUv());
        // 頂点バッファの設定
        commandList->IASetVertexBuffers(0, 1, mesh.GetVbView());
        // インデックスバッファの設定
        commandList->IASetIndexBuffer(mesh.GetIbView());
        // 描画コマンド
        commandList->DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), 1, 0, 0, 0);
    }
}

uint32_t ModelManager::LoadInternal(const std::string& name) {

	assert(useModelCount_ < kNumModels);
	uint32_t handle = useModelCount_;

	// 読み込み済みmodelを検索
	auto it = std::find_if(models_.begin(), models_.end(), [&](const auto& texture) {return texture.name == name; });

	if (it != models_.end()) {
		// 読み込み済みmodelの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(models_.begin(), it));
		return handle;
	}

	// 書き込むmodelの参照
	ModelIndex& modelIndex = models_.at(useModelCount_);
	modelIndex.name = name;

	modelIndex.meshes = CreateMeshes(name);

	useModelCount_++;
	return handle;
}

