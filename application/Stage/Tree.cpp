#include "Stage/Tree.h"
#include "Draw/DrawManager.h"
#include "GameComponent/Loader.h"

void Tree::Initialize()
{

	GameObject::Initialize("tree.obj");


	std::vector<Vector3> positions;
	Load::Positions("treeGrass.txt", positions);
	for (Vector3 position : positions) {
		WorldTransform w{};
		w.SetParent(&worldTransform_, false);
		w.scale_ = { 0.2f,0.2f,0.2f };
		w.translation_ = position;
		w.Update();
		grassTransforms_.push_back(w);
	}

	grassesData_ = std::make_unique<ParticleModelData>(uint32_t(grassTransforms_.size()));
	grassesData_->Initialize();

	worldTransform_.scale_ = {5.0f,5.0f,5.0f };

	Vector3 color = ColorCodeToVector3(0x84331F);
	material_.color_ = { color.x,color.y,color.z,1.0f };
	material_.Update();

	grassMaterial_ = std::make_unique<Material>();
	color = ColorCodeToVector3(0x40BA8D);
	grassMaterial_->Initialize();
	grassMaterial_->color_ = { color.x,color.y,color.z,0.5f };
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

	DrawManager::GetInstance()->DrawPostParticleModel(*grassesData_, ModelManager::GetInstance()->Load("box1x1.obj"), *grassMaterial_);
	DrawManager::GetInstance()->DrawModel(worldTransform_,modelHandle_,0,material_);
}
