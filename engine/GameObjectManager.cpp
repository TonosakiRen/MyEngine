#include <json.hpp>
#include <iostream>
#include "GameObjectManager.h"
#include <fstream>
#include <assert.h>
#include "ModelManager.h"

#include "LightManager.h"

using namespace nlohmann;

GameObjectManager* GameObjectManager::GetInstance() {
	static GameObjectManager instance;
	return &instance;
}

void GameObjectManager::Load()
{
	const std::string fullPath = "Resources/data.json";

	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(fullPath);
	//ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	// JSON文字列から解答したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	//正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"を文字列として取得
	std::string name =
		deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	//レベルデータ格納用インスタンスを生成
	

	// "objects"の全オブジェクト
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		//種別を取得
		std::string type = object["type"].get<std::string>();

		// MESH
		if (type.compare("MESH") == 0) {
			//要素追加
			gameObjects_.emplace_back(std::make_unique<GameObject>());
			// 今追加した要素の参照を得る
			GameObject& gameObject = *gameObjects_.back();

			//file_nameでモデルをロード
			if (object.contains("file_name")) {
				gameObject.SetModelHandle(ModelManager::GetInstance()->ModelManager::Load(object["file_name"]));
			}

			// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];

			WorldTransform& worldTransform = *gameObject.GetWorldTransform();
			//平行移動
			worldTransform.translation_.x = (float)transform["translation"][0];
			worldTransform.translation_.y = (float)transform["translation"][2];
			worldTransform.translation_.z = (float)transform["translation"][1];
			//回転角
			Vector3 euler = {
				-(float)transform["rotation"][0] ,
				-(float)transform["rotation"][2] ,
				-(float)transform["rotation"][1]
			};
			worldTransform.quaternion_ = MakeFromEulerAngle(euler);
			//スケーリング
			worldTransform.scale_.x = (float)transform["scaling"][0];
			worldTransform.scale_.y = (float)transform["scaling"][2];
			worldTransform.scale_.z = (float)transform["scaling"][1];

			// TODO: オブジェクト捜査を再起関数にまとめ、再起呼出で枝を走査する
			if (object.contains("children")) {

			}

		}

		// LIGHT
		if (type.compare("LIGHT") == 0) {

			// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];


			if (object.contains("pointLight")) {
				PointLights::PointLight pointLight;
				//平行移動
				pointLight.worldTransform.translation_.x = (float)transform["translation"][0];
				pointLight.worldTransform.translation_.y = (float)transform["translation"][2];
				pointLight.worldTransform.translation_.z = (float)transform["translation"][1];

				nlohmann::json& light = object["pointLight"];
				nlohmann::json& color = light["color"];
				pointLight.color.x = color[0];
				pointLight.color.y = color[1];
				pointLight.color.z = color[2];
				pointLight.isActive = true;
				pointLight.decay = 1.0f;
				pointLight.intensity = 3.0f;
				pointLight.radius = 3.0f;
				PointLights& pointLights = *LightManager::GetInstance()->pointLights_;
				for (int i = 0; i < PointLights::lightNum; i++) {
					if (pointLights.lights_[i].isActive == false) {
						pointLights.lights_[i] = pointLight;
						break;
					}
				}
			}
		}
	}
}

void GameObjectManager::Finalize()
{
	for (std::unique_ptr<GameObject>& gameObject : gameObjects_) {
		gameObject.reset();
	}
}
