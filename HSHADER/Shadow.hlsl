#include "LightBase.hlsli"
#include "RenderBase.hlsli"
#include "AniBase.hlsli"

struct SHADOWANI_IN
{
    float4 vPOSITION : POSITION;
    float4 vUV : TEXCOORD;
    float4 vCOLOR : COLOR;
    float4 vNORMAL : NORMAL;
    float4 vBINORMAL : BINORMAL;
    float4 vTANGENT : TANGENT;
    float4 vWEIGHT : BLENDWEIGHT;
    int4 vINDEX : BLENDINDICES;
};



struct ShadowOut
{
    float4 vPos : SV_Position;
    float4 vPorjPos : POSITION;
};

cbuffer RenderOption : register(b7)
{
    RenderOptionStruct RenderOptionData;
}


cbuffer HTRANSDATA : register(b0)
{
    TransDataStruct MatrixData;
}


Texture2D FrameAniTex : register(t0);
ShadowOut VS_Shadow(SHADOWANI_IN _In)
{
    _In.vPOSITION.w = 1.0f;
    // πª«ÿ¡‡æﬂ ∞°¥…«—∞°ø‰?
    if (0 != RenderOptionData.IsAni)
    {
        SkinningPos(_In.vPOSITION, _In.vWEIGHT, _In.vINDEX, FrameAniTex);
    }
    
    ShadowOut Out = (ShadowOut) 0.0f;
    Out.vPos = mul(_In.vPOSITION, MatrixData.WVP);
    Out.vPorjPos = Out.vPos;
    return Out;
}

float4 PS_Shadow(ShadowOut _In) : SV_Target
{
    return float4(_In.vPorjPos.z / _In.vPorjPos.w, 0.0, 0.0f, 1.0f);
}
