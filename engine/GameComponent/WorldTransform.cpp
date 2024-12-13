/**
 * @file WorldTransform.cpp
 * @brief WorldTransform
 */
#include "GameComponent/WorldTransform.h"
#include "Model/ModelManager.h"
ModelManager* modelManager_ = ModelManager::GetInstance();

void WorldTransform::Initialize(bool srtChange) {
    parent_ = nullptr;
    if (srtChange) {
        scale_ = { 1.0f,1.0f,1.0f };
        maxScale_ = 1.0f;
        quaternion_ = IdentityQuaternion();
        translation_ = { 0.0f,0.0f,0.0f };
    }
    matWorld_ = MakeIdentity4x4();
    worldInverseTranspose_ = MakeIdentity4x4();
    constBuffer_.Create(L"worldTransformBuffer", (sizeof(ConstBufferData) + 0xff) & ~0xff);
}

void WorldTransform::Reset() {
    parent_ = nullptr;
   
    scale_ = { 1.0f,1.0f,1.0f };
    maxScale_ = 1.0f;
    quaternion_ = IdentityQuaternion();
    translation_ = { 0.0f,0.0f,0.0f };
    
    matWorld_ = MakeIdentity4x4();
    worldInverseTranspose_ = MakeIdentity4x4();
}


void WorldTransform::Update() {

    // スケール、回転、平行移動行列の計算
    Matrix4x4 scaleMatrix = MakeScaleMatrix(scale_);

    Matrix4x4 rotateMatrix = MakeIdentity4x4();
    rotateMatrix = MakeRotateMatrix(quaternion_);
    Matrix4x4 translationMatrix = MakeTranslateMatrix(translation_);

    // ワールド行列の合成
    matWorld_ = MakeIdentity4x4();
    matWorld_ *= scaleMatrix;
    matWorld_ *= rotateMatrix;
    matWorld_ *= translationMatrix;


    // 親行列の指定がある場合は、掛け算する
    if (parent_) {
        //scaleを反映させない
        Matrix4x4 inverseMatrix = MakeIdentity4x4();

        if (!isScaleParent_) {
            inverseMatrix = Inverse(MakeScaleMatrix(MakeScale(parent_->matWorld_)));
            matWorld_ *= inverseMatrix;
        }

        if (!isRotateParent_) {
            inverseMatrix = Inverse(NormalizeMakeRotateMatrix(parent_->matWorld_));
            matWorld_ *= inverseMatrix;
        }

        matWorld_ *= parent_->matWorld_;
        worldInverseTranspose_ = Transpose(Inverse(matWorld_));
    }

    Vector3 scale = MakeScale(matWorld_);
    maxScale_ = (std::max)({ scale.x,scale.y,scale.z });

    if (constBuffer_.GetCPUData()) {
        constBuffer_.Copy(ConstBufferData{ matWorld_, worldInverseTranspose_ ,maxScale_ });
    }
}

void WorldTransform::Update(Matrix4x4 localMatrix) {

    // スケール、回転、平行移動行列の計算
    Matrix4x4 scaleMatrix = MakeScaleMatrix(scale_);

    Matrix4x4 rotateMatrix = MakeIdentity4x4();
    rotateMatrix = MakeRotateMatrix(quaternion_);
    Matrix4x4 translationMatrix = MakeTranslateMatrix(translation_);

    // ワールド行列の合成
    matWorld_ = MakeIdentity4x4();
    matWorld_ *= scaleMatrix;
    matWorld_ *= rotateMatrix;
    matWorld_ *= translationMatrix;


    // 親行列の指定がある場合は、掛け算する
    if (parent_) {
        //scaleを反映させない
        Matrix4x4 inverseMatrix = MakeIdentity4x4();

        if (!isScaleParent_) {
            inverseMatrix = Inverse(MakeScaleMatrix(MakeScale(parent_->matWorld_)));
            matWorld_ *= inverseMatrix;
        }

        if (!isRotateParent_) {
            inverseMatrix = Inverse(NormalizeMakeRotateMatrix(parent_->matWorld_));
            matWorld_ *= inverseMatrix;
        }

        matWorld_ *= parent_->matWorld_;
        matWorld_ = localMatrix * matWorld_;
        worldInverseTranspose_ = Transpose(Inverse(matWorld_));
    }

    Vector3 scale = MakeScale(matWorld_);
    maxScale_ = (std::max)({ scale.x,scale.y,scale.z });

    if (constBuffer_.GetCPUData()) {
        constBuffer_.Copy(ConstBufferData{ matWorld_, worldInverseTranspose_ ,maxScale_ });
    }
}

void WorldTransform::Update(uint32_t modelIndex) {

    // スケール、回転、平行移動行列の計算
    Matrix4x4 scaleMatrix = MakeScaleMatrix(scale_);

    Matrix4x4 rotateMatrix = MakeIdentity4x4();
    rotateMatrix = MakeRotateMatrix(quaternion_);
    Matrix4x4 translationMatrix = MakeTranslateMatrix(translation_);

    // ワールド行列の合成
    matWorld_ = MakeIdentity4x4();
    matWorld_ *= scaleMatrix;
    matWorld_ *= rotateMatrix;
    matWorld_ *= translationMatrix;


    // 親行列の指定がある場合は、掛け算する
    if (parent_) {
        //scaleを反映させない
        Matrix4x4 inverseMatrix = MakeIdentity4x4();

        if (!isScaleParent_) {
            inverseMatrix = Inverse(MakeScaleMatrix(MakeScale(parent_->matWorld_)));
            matWorld_ *= inverseMatrix;
        }

        if (!isRotateParent_) {
            inverseMatrix = Inverse(NormalizeMakeRotateMatrix(parent_->matWorld_));
            matWorld_ *= inverseMatrix;
        }

        matWorld_ *= parent_->matWorld_;

        Node* rootNode = &modelManager_->GetRootNode(modelIndex);

        if (rootNode) {
            matWorld_ *= rootNode->localMatrix;
        }

        worldInverseTranspose_ = Transpose(Inverse(matWorld_));
    }

    Vector3 scale = MakeScale(matWorld_);
    maxScale_ = (std::max)({ scale.x,scale.y,scale.z });

    if (constBuffer_.GetCPUData()) {
        constBuffer_.Copy(ConstBufferData{ matWorld_, worldInverseTranspose_ ,maxScale_ });
    }
}
