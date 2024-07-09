#pragma once
#include "GPUResource.h"
#include "DefaultBuffer.h"
#include "UploadBuffer.h"
#include "DescriptorHandle.h"

class CommandContext;

class DefaultStructuredBuffer : public DefaultBuffer {
public:
    ~DefaultStructuredBuffer();

    void Create(const std::wstring& name, size_t bufferSize, UINT numElements);

    void Copy(const void* srcData, size_t copySize);
    template<class T>
    void Copy(const T& srcData)  { Copy(&srcData, sizeof(srcData)); }

    size_t GetBufferSize() const { return bufferSize_; }
    void* GetCPUData() const { return uploadBuffer_.GetCPUData(); }

    //コピーするやつ
    DescriptorHandle GetSRV (CommandContext& commandContext) {
        if (isCopy_) {
            isCopy_ = false;
            BufferCopy(commandContext);
        }
        return srvHandle_;
    }

    DescriptorHandle GetSRV() {
        return srvHandle_;
    }

protected:
    void BufferCopy(CommandContext& commandContext);
    void Destroy();

    UploadBuffer uploadBuffer_;
    DescriptorHandle srvHandle_;
    bool isCopy_ = false;
};