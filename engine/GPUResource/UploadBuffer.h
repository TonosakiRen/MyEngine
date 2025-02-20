#pragma once
/**
 * @file UploadBuffer.h
 * @brief UploadHeap用Buffer
 */
#include "GPUResource/GPUResource.h"
#include "Graphics/Helper.h"
#include "Framework/Framework.h"

class UploadBuffer : public GPUResource {
public:
    ~UploadBuffer();
    void Create(const std::wstring& name, size_t bufferSize);
    void Create(const std::wstring& name, size_t numElements, size_t elementSize) {
        Create(name, numElements * elementSize);
    }

    void Copy(const void* srcData, size_t copySize) ;
    template<class T>
    void Copy(const T& srcData) { Copy(&srcData, sizeof(srcData)); }

    //データに0をセット
    void SetZero();

    //Getter
    size_t GetBufferSize() const { return bufferSize_; }
    void* GetCPUData() const { 
        size_t offset = Framework::kFrameRemainder * bufferSize_;
        return static_cast<char*>(cpuData_) + offset; }
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { 
        size_t offset = Framework::kFrameRemainder * Helper::AlignUp(bufferSize_, 256);
        return resource_->GetGPUVirtualAddress() + static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(offset);
    }



protected:
   
    size_t bufferSize_ = 0;
    void* cpuData_ = nullptr;
    bool isFirstUpload_ = true;
};