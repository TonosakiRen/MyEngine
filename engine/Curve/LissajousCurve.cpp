/**
 * @file LissajousCurve.cpp
 * @brief LissajousCurveを生成
 */
#include "Curve/LissajousCurve.h"
#include "Particle/Particle.h"
#include "Light/ShaderManager.h"
#include "Render/Renderer.h"
#include "Texture/TextureManager.h"
#include "Framework/Framework.h"

using namespace Microsoft::WRL;

void LissajousCurve::Initialize()
{
	structuredBuffer_.Create(L"LissajousCurveDataBuffer", sizeof(Vector2), kPointNum);

    const float A = 0.5f;    // X軸の振幅
    const float B = 0.5f;    // Y軸の振幅
    const float a = 4.0f;      // X軸の周波数
    const float b = 5.0f;      // Y軸の周波数
   
    for (int i = 0; i < kAllPointNum; ++i) {
        float t = i * float(2 * M_PI / kAllPointNum);  
        float x = A * std::sin(a * t);
        float y = B * std::sin(b * t);

        points_[i] = {x,y};
    }
}

void LissajousCurve::Update()
{

	Vector2* data = static_cast<Vector2*>(structuredBuffer_.GetCPUData());
    startPoint_+= 5;
    if (startPoint_ >= kAllPointNum) {
        startPoint_ = 0;
    }

    for (int i = 0; i < kPointNum; i++) {
        int index = startPoint_ - i;
        if (index < 0) {
            index = kAllPointNum - i;
        }
        data[i] = points_[index];
    }

}
