#include "Engine.h"
namespace Engine {
	void Initialize(){
		winApp = WinApp::GetInstance();
		winApp->Initialize("ooo", kWindowWidth, kWindowHeight);
		renderManager = RenderManager::GetInstance();
		renderManager->Initialize();
		Model::CreatePipeline(renderManager->GetMainBufferRTVFormat(), renderManager->GetMainDepthDSVFormat());
		textureManager = TextureManager::GetInstance();
        renderManager->Reset();
		renderManager->BeginRender();


        isEndRequest = false;
        referenceTime = std::chrono::steady_clock::now();
	}
	void Shutdown() {
		renderManager->Shutdown();
		Model::DestroyPipeline();
		winApp->Shutdown();
	}
	bool BeginFrame() {
		renderManager->BeginRender();
        // FPS固定
        {
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            std::chrono::microseconds elapsed =
                std::chrono::duration_cast<std::chrono::microseconds>(now - referenceTime);

            static const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 62.0f));
            static const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
            std::chrono::microseconds check = kMinCheckTime - elapsed;
            if (check > std::chrono::microseconds(0)) {
                std::chrono::microseconds waitTime = kMinTime - elapsed;

                std::chrono::steady_clock::time_point waitStart = std::chrono::steady_clock::now();
                do {
                    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
                } while (std::chrono::steady_clock::now() - waitStart < waitTime);
            }

            elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - referenceTime);
            referenceTime = std::chrono::steady_clock::now();
        }

        if (!winApp->ProcessMessage() ||
            isEndRequest) {
            return false;
        }

        return true;
	}
    void BeginDraw() {
        renderManager->BeginRender();
    }
    void EndDraw() {
        renderManager->EndRender();
        renderManager->Reset();
    }
    void RequestQuit() {
        isEndRequest = true;
    }
    uint32_t LoadTexture(const std::string& name) {
        return textureManager->Load(name);
    }

}