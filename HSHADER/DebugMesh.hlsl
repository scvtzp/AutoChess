#include "CalBase.hlsli"
#include "RenderBase.hlsli"
#include "AniBase.hlsli"

struct VTX3DIN
{
    float4 Pos : POSITION;
    float4 Uv : TEXCOORD;
    float4 Color : COLOR;
    float4 Normal : NORMAL; // 이녀석이 Y축
    float4 BiNormal : BINORMAL; // 이녀석이 X축
    float4 Tangent : TANGENT; // 이녀석이 Z축 // 나중에 바꿀수 있다.
    float4 Weight : BLENDWEIGHT; // 얼마나 영향을 받아야 하는가? 0.5 영향을 받는다.
    int4 Index : BLENDINDICES; // 21번째 본에
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