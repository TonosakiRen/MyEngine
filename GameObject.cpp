#include "GameObject.h"

#include "Helper.h"
#include "ShaderManager.h"


void GameObject::Initialize(std::string name) {
    std::string pass = "Resources/Model/" + name + ".obj";
    model_.CreateFromObj("pass");
}

void GameObject::Update() {
    
}

void GameObject::Draw(CommandContext& commandContext, const Camera& camera) {
    transform_.UpdateMatrix();
    model_.Draw(commandContext, transform_.worldMatrix, camera);
}

