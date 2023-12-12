#include "Mesh.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "TextureManager.h"

void Mesh::Create(std::string name)
{
    name_ = name;

    HRESULT result = S_FALSE;
    
     std::string line;//ファイルから読んだ1行を格納するもの
     std::vector<Vector3> positions;//位置
     std::vector<Vector3> normals;//法線
     std::vector<Vector2> texcoords;//テクスチャ座標
     std::string directoryPath = "Resources/models/" + name_ + "/";
     std::ifstream file(directoryPath + name_ + ".obj"); //ファイルを開く
     assert(file.is_open());//とりあえず開けなったら止める

     while (std::getline(file, line)) {
         std::string identifier;
         std::istringstream s(line);
         s >> identifier;//先頭の識別子を読む

         if (identifier == "v") {
             Vector3 position;
             s >> position.x >> position.y >> position.z;
             position.z *= -1.0f;
             /*position.w = 1.0f;*/
             positions.push_back(position);
         }
         else if (identifier == "vt") {
             Vector2 texcoord;
             s >> texcoord.x >> texcoord.y;
             texcoord.y = 1.0f - texcoord.y;
             texcoords.push_back(texcoord);
         }
         else if (identifier == "vn") {
             Vector3 normal;
             s >> normal.x >> normal.y >> normal.z;
             normal.z *= -1.0f;
             normals.push_back(normal);
         }
         else if (identifier == "f") {
             VertexData triangle[3];
             //面は三角形限定。その他は未対応
             for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                 std::string vertexDefinition;
                 s >> vertexDefinition;
                 //頂点の要素へのIndexは[位置/UV/法線]で格納されているので、分解してIndexを取得する
                 std::istringstream v(vertexDefinition);
                 uint32_t elementIndices[3];
                 for (int32_t element = 0; element < 3; ++element) {
                     std::string index;
                     std::getline(v, index, '/');//区切りでインデックスを読んでいく
                     elementIndices[element] = std::stoi(index);
                 }
                 //要素へのIndexから、実際の要素の要素の値
                 Vector3 position = positions[elementIndices[0] - 1];
                 Vector2 texcoord = texcoords[elementIndices[1] - 1];
                 Vector3 normal = normals[elementIndices[2] - 1];

                 //頂点を逆順で登録することで、回り順を逆にする
                 triangle[faceVertex] = { position,normal,texcoord };

             }
             //頂点を逆順で登録することで、周り順を逆にする
             vertices_.push_back(triangle[2]);
             vertices_.push_back(triangle[1]);
             vertices_.push_back(triangle[0]);
         }
         else if (identifier == "mtllib") {
             //materialTemplateLibraryファイルの名前を取得する
             std::string materialFilename;
             s >> materialFilename;
             //基本的にobjファイルと同一階層にmtlは存在させるので,ディレクトリ名とファイル名を渡す

             std::string uvFilePass;//構築するMaterialData
             std::string line;//fileから読んだ１行を格納するもの
             std::ifstream file("Resources/models/" + name_ + "/" + name_ + ".mtl"); //ファイルを開く
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
                     uvHandle_ = TextureManager::LoadUv(textureFilename, uvFilePass);
                 }
             }
         }
     }

     // 頂点データのサイズ
     UINT sizeVB = static_cast<UINT>(sizeof(VertexData) * vertices_.size());

     vertexBuffer_.Create(sizeVB);

     vertexBuffer_.Copy(vertices_.data(), sizeVB);

     // 頂点バッファビューの作成
     vbView_.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
     vbView_.SizeInBytes = sizeVB;
     vbView_.StrideInBytes = sizeof(vertices_[0]);
  
}

