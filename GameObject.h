#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "Material.h"
#include "WorldTransform.h"
#include "DirectionalLight.h"
class GameObject
{
public:
	static GameObject* Create(const std::string name);
	void Initialize(const std::string name);
	void Update();
	void Draw(ViewProjection& viewProjection, const DirectionalLight& directionalLight,Vector4 color = {1.0f,1.0f,1.0f,1.0f});
protected:
	WorldTransform worldTransform_;
	Material material_;
private:
	Model model_;
};

