#include "Texture.h"

#include "Externals/DirectXTex/DirectXTex.h"

#include "Helper.h"
#include "Graphics.h"
#include "CommandContext.h"

void Texture::CreateFromWICFile(const std::wstring& path) {

    auto graphics = Graphics::GetInstance();
    auto device = graphics->GetDevice();

    auto& wname = path;

    // ファイルを読み込む
    DirectX::ScratchImage image{};
    ASSERT_IF_FAILED(DirectX::LoadFromWICFile(wname.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image));

    // ミップマップを生成
    DirectX::ScratchImage mipImages{};
    ASSERT_IF_FAILED(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages));

    // リソースを生成
    auto& metadata = mipImages.GetMetadata();
    desc_.Width = UINT(metadata.width);
    desc_.Height = UINT(metadata.height);
    desc_.MipLevels = UINT16(metadata.mipLevels);
    desc_.DepthOrArraySize = UINT16(metadata.arraySize);
    desc_.Format = metadata.format;
    desc_.SampleDesc.Count = 1;
    desc_.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);
    D3D12_HEAP_PROPERTIES heapPropeteies = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ASSERT_IF_FAILED(device->CreateCommittedResource(
        &heapPropeteies,
        D3D12_HEAP_FLAG_NONE,
        &desc_,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(resource_.GetAddressOf())));
    state_ = D3D12_RESOURCE_STATE_COPY_DEST;

    // 中間リソースをコピーする
    auto& commandQueue = graphics->GetCommandQueue();
    CommandContext commandContext;
    commandContext.Create();

    // 中間リソースを読み込む
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(resource_.Get(), 0, UINT(subresources.size()));

    D3D12_RESOURCE_DESC intermediateResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(intermediateSize);
    heapPropeteies = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
    ASSERT_IF_FAILED(device->CreateCommittedResource(
        &heapPropeteies,
        D3D12_HEAP_FLAG_NONE,
        &intermediateResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(intermediateResource.GetAddressOf())));
    UpdateSubresources(commandContext, resource_.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

    commandContext.TransitionResource(*this, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.Close();
    commandQueue.Excute(commandContext);
    commandQueue.Signal();
    commandQueue.WaitForGPU();

    // ビューを生成
    CreateView();
}

void Texture::CreateView() {
    auto graphics = Graphics::GetInstance();
    auto device = graphics->GetDevice();

    if (srvHandle_.IsNull()) {
        srvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = desc_.Format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc_.MipLevels;
    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
}
