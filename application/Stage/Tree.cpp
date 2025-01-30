/**
 * @file Tree.cpp
 * @brief 木
 */
#include "Stage/Tree.h"
#include "Draw/DrawManager.h"
#include "GameComponent/Loader.h"

void Tree::Initialize()
{

	GameObject::Initialize("tree.obj");
	std::vector<Vector3> positions;
	Load::Positions("treeGrass.txt", positions);

	const Vector3 color = ColorCodeToVector3(0x84331F);
	material_.color_ = { color.x,color.y,color.z,1.0f };
	material_.Update();

	const float leafScale = 0.2f;
	const float scale = 5.0f;

	//葉っぱのworldTransformを初期化
	for (Vector3 position : positions) {
		WorldTransform w{};
		w.SetParent(&worldTransform_, false);
		w.scale_ = { leafScale,leafScale,leafScale };
		w.translation_ = position;
		w.Update();
		grassTransforms_.push_back(w);
	}

	worldTransform_.scale_ = {scale,scale,scale };

	grassesData_ = std::make_unique<ParticleModelData>(uint32_t(grassTransforms_.size()));
	grassesData_->Initialize();

	grassMaterial_ = std::make_unique<Material>();
	//color = ColorCodeToVector3(0x40BA8D);
	grassMaterial_->Initialize();
	grassMaterial_->color_;
	grassMaterial_->Update();
}

void Tree::Update()
{
	worldTransform_.Update();
	grassMaterial_->Update();
	material_.Update();
	for (auto& grassTransform : grassTransforms_) {
		grassTransform.Update();
	}
}

void Tree::Draw()
{

	for (size_t i = 0; i < grassTransforms_.size(); i++)
	{
		ParticleModelData::Data data;
		data.matWorld = grassTransforms_[i].matWorld_;
		data.worldInverseTranspose = Inverse(data.matWorld);
		grassesData_->PushBackData(data);
		
	}

	Engine::DrawManager::GetInstance()->DrawParticleModel(*grassesData_, Engine::ModelManager::GetInstance()->Load("box1x1.obj"), *grassMaterial_);
	Engine::DrawManager::GetInstance()->DrawModel(worldTransform_,modelHandle_,0,material_);
}
