#pragma once
#include "DirectXCommon.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Input.h"
#include "Sprite.h"
#include "Triangle.h"
#include "DirectionalLight.h"
#include "Sphere.h"
/// <summary>
/// ゲームシーン
/// </summary>
class GameScene
{

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: //　メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	//ヴュープロジェクション
	ViewProjection viewProjection_;
	DirectionalLight directionalLight_;
	Material material_;
	transform texTransfrom;
	uint32_t textureHandle_;

	Model model_;
	WorldTransform modelTransform_;
	Sprite sprite_;
	Vector2 spritePosition_;
	Vector2 spriteScale_;
	float spriteRotate_;
	WorldTransform spriteTransform_;
	Triangle triangle_;
	WorldTransform triangleTransform_;
	Triangle triangle2_;
	WorldTransform triangleTransform2_;
	Sphere sphere_;
	WorldTransform sphereTransform_;
};

