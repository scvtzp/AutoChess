#include "CalBase.hlsli"
#include "RenderBase.hlsli"
#include "AniBase.hlsli"

struct VTX3DIN
{
    float4 Pos : POSITION;
    float4 Uv : TEXCOORD;
    float4 Color : COLOR;
    float4 Normal : NORMAL; // �̳༮�� Y��
    float4 BiNormal : BINORMAL; // �̳༮�� X��
    float4 Tangent : TANGENT; // �̳༮�� Z�� // ���߿� �ٲܼ� �ִ�.
    float4 Weight : BLENDWEIGHT; // �󸶳� ������ �޾ƾ� �ϴ°�? 0.5 ������ �޴´�.
    int4 Index : BLENDINDICES; // 21��° ����
};

struct VTX3DOUT
{
    float4 Pos : SV_Position;
    float4 Uv : TEXCOORD;
    float4 Color : COLOR;
};


cbuffer HTRANSDATA : register(b0)
{
    TransDataStruct MatrixData;
}

cbuffer RenderOption : register(b8)
{
    RenderOptionStruct RenderOptionData;
}
VTX3DOUT VS_DebugMesh(VTX3DIN _In)
{

    VTX3DOUT Out = (VTX3DOUT) 0;
    Out.Pos = mul(_In.Pos, MatrixData.WVP);
    Out.Color = _In.Color;
    return Out;
}

struct FORWARDOUT
{
    float4 ForwardColor : SV_Target0;
};

FORWARDOUT PS_DebugMesh(VTX3DOUT _In)
{
    FORWARDOUT Out = (FORWARDOUT) 0.0f;
    
    Out.ForwardColor = _In.Color;
    return Out;
}