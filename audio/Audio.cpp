#include "Audio.h"

#include <cassert>
#include <filesystem>

#pragma comment(lib,"xaudio2.lib")
#pragma comment(lib,"Mf.lib")
#pragma comment(lib,"mfplat.lib")
#pragma comment(lib,"Mfreadwrite.lib")
#pragma comment(lib,"mfuuid.lib")


Audio* Audio::GetInstance() {
    static Audio instans;
    return &instans;
}

Audio::~Audio() {
    for (size_t i = 0; i < kMaxNumPlayHandles; ++i) {
        DestroyPlayHandle(i);
    }

    if (masterVoice_) {
        masterVoice_->DestroyVoice();
        masterVoice_ = nullptr;
    }
}

void Audio::Initialize() {
    HRESULT result;
    // XAudioエンジンのインスタンスを作成
    result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
    assert(SUCCEEDED(result));
    // マスターボイスを作成
    result = xAudio2_->CreateMasteringVoice(&masterVoice_);
    assert(SUCCEEDED(result));

    for (size_t i = 0; i < kMaxNumPlayHandles; ++i) {
        DestroyPlayHandle(i);
    }

    result = CoInitialize(nullptr);
    assert(SUCCEEDED(result));
    MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);

 
}

void Audio::Update() {
    for (size_t i = 0; i < kMaxNumPlayHandles; ++i) {
        if (sourceVoices_[i]) {
            XAUDIO2_VOICE_STATE state{ };
            sourceVoices_[i]->GetState(&state);
            if (state.BuffersQueued == 0) {
                DestroyPlayHandle(i);
            }
        }
    }
}

size_t Audio::SoundPlayWave(size_t soundHandle) {
    HRESULT result;
    const SoundData& soundData = soundData_.at(soundHandle);

    // 再生する波形データの設定
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.pBuffer.data();
    buf.AudioBytes = soundData.bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    size_t playHandle = FindUnusedPlayHandle();
    // プレイハンドルがいっぱい
    assert(playHandle < kMaxNumPlayHandles);

    // SourceVoice の作成
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
    assert(SUCCEEDED(result));

    result = pSourceVoice->SubmitSourceBuffer(&buf);
    assert(SUCCEEDED(result));

    result = pSourceVoice->Start();
    assert(SUCCEEDED(result));

    sourceVoices_[playHandle] = pSourceVoice;
    return playHandle;
}

size_t Audio::SoundPlayLoopStart(size_t soundHandle) {
    HRESULT result;
    const SoundData& soundData = soundData_.at(soundHandle);

    // 再生する波形データの設定
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.pBuffer.data();
    buf.AudioBytes = soundData.bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;
    buf.LoopCount = XAUDIO2_LOOP_INFINITE;

    size_t playHandle = FindUnusedPlayHandle();
    // プレイハンドルがいっぱい
    assert(playHandle < kMaxNumPlayHandles);

    // SourceVoice の作成
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
    assert(SUCCEEDED(result));

    result = pSourceVoice->SubmitSourceBuffer(&buf);
    assert(SUCCEEDED(result));

    result = pSourceVoice->Start();
    assert(SUCCEEDED(result));

    sourceVoices_[playHandle] = pSourceVoice;
    return playHandle;
}

void Audio::SoundPlayLoopEnd(size_t playHandle) {
    // soundHandle に対応する SourceVoice を取得
    if (IsValidPlayHandle(playHandle)) {
        sourceVoices_[playHandle]->Stop();
        DestroyPlayHandle(playHandle);
    }
}

size_t Audio::SoundLoadWave(const char* filename) {
    auto iter = std::find_if(soundData_.begin(), soundData_.end(), [&](const SoundData& soundData) {
        return soundData.filename == filename;
        });
    if (iter != soundData_.end()) {
        return std::distance(soundData_.begin(), iter);
    }

#pragma region ファイルオープン
    //directryPass
    std::string directryPass = "Resources/sounds/";
    // ファイル入出ストリームのインスタンス
    std::ifstream file;
    // wavファイルをバイナリモードで開く
    file.open(directryPass + filename, std::ios_base::binary);
    // ファイルオープン失敗を検出する
    assert(file.is_open());
#pragma endregion

    IMFSourceReader* pMFSourceReader{ nullptr };
    std::filesystem::path path = { directryPass + filename };
    MFCreateSourceReaderFromURL(path.wstring().c_str(), NULL, &pMFSourceReader);

    IMFMediaType* pMFMediaType{ nullptr };
    MFCreateMediaType(&pMFMediaType);
    pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    pMFSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType);

    pMFMediaType->Release();
    pMFMediaType = nullptr;
    pMFSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType);

    WAVEFORMATEX* waveFormat{ nullptr };
    MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);

    std::vector<BYTE> mediaData;
    while (true)
    {
        IMFSample* pMFSample{ nullptr };
        DWORD dwStreamFlags{ 0 };
        pMFSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

        if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            break;
        }

        IMFMediaBuffer* pMFMediaBuffer{ nullptr };
        pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

        BYTE* pBuffer{ nullptr };
        DWORD cbCurrentLength{ 0 };
        pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

        mediaData.resize(mediaData.size() + cbCurrentLength);
        memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

        pMFMediaBuffer->Unlock();

        pMFMediaBuffer->Release();
        pMFSample->Release();
    }

#pragma region 読み込んだ音声データのreturn
    // returnする為の音声データ
    SoundData soundData = {};
    soundData.filename = filename;
    soundData.wfex = *waveFormat;
    soundData.bufferSize = uint32_t(mediaData.size());
    soundData.pBuffer = std::move(mediaData);
#pragma endregion
    soundData_.emplace_back(soundData);

    return soundData_.size() - 1;
}

void Audio::StopSound(size_t playHandle) {
    assert(playHandle < kMaxNumPlayHandles);
    DestroyPlayHandle(playHandle);
}

void Audio::SetPitch(size_t playHandle, float pitch) {
    assert(playHandle < kMaxNumPlayHandles);
    sourceVoices_[playHandle]->SetFrequencyRatio(pitch);
}

void Audio::SetValume(size_t playHandle, float volume) {
    assert(playHandle < kMaxNumPlayHandles);
    sourceVoices_[playHandle]->SetVolume(volume);
}

bool Audio::IsValidPlayHandle(size_t playHandle) {
    return playHandle < kMaxNumPlayHandles && sourceVoices_[playHandle] != nullptr;
}


size_t Audio::FindUnusedPlayHandle() {
    for (size_t i = 0; i < kMaxNumPlayHandles; ++i) {
        if (sourceVoices_[i] == nullptr) {
            return i;
        }
    }
    return size_t(-1);
}

void Audio::DestroyPlayHandle(size_t playHandle) {
    assert(playHandle < kMaxNumPlayHandles);
    if (sourceVoices_[playHandle]) {
        sourceVoices_[playHandle]->DestroyVoice();
        sourceVoices_[playHandle] = nullptr;
    }
}


void Audio::SoundUnload(size_t soundHandle) {
    // バッファのメモリを解放
    soundData_.at(soundHandle).pBuffer.clear();
    soundData_.at(soundHandle).bufferSize = 0;
    soundData_.at(soundHandle).wfex = {};
    soundData_.erase(soundData_.begin() + soundHandle);
}
