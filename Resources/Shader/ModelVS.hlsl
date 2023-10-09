struct Transformation {
    float4x4 worldMatrix;
    float4x4 worldViewProjMatrix;
    float3 cameraPosition;
};

ConstantBuffer<Transformation> transformation_ : register(b0);

struct VSInput {
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION0;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

VSOutput main(VSInput input) {
    VSOutput output;
    output.position = mul(float4(input.position.xyz, 1.0f), transformation_.worldViewProjMatrix);
    output.worldPosition = mul(float4(input.position.xyz, 1.0f), transformation_.worldMatrix).xyz;
    output.normal = mul(input.normal, (float3x3)transformation_.worldMatrix);
    output.texcoord = input.texcoord;
    return output;
}