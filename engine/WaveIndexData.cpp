#include "WaveIndexData.h"
#include "ImGuiManager.h"

void WaveIndexData::Initialize() {
    constBuffer_.Create(L"viewProjectionConstBuffer", sizeof(ConstBufferData));
}