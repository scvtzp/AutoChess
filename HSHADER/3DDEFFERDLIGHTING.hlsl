#include "LightBase.hlsli"

struct VTX3DIN
{
    float4 Pos : POSITION;
    float4 Uv : TEXCOORD;
};

struct VTX3DOUT
{
    float4 Pos : SV_Position;
    float4 Uv : TEXCOORD;
};
VTX3DOUT VS_DEFFERDLIGHT(VTX3DIN _In)
{
    VTX3DOUT Out = (VTX3DOUT) 0;
    Out.Pos = _In.Pos;
    Out.Uv = _In.Uv;
    return Out;
}

struct LIGHTOUT
{
    float4 DifColor : SV_Target0;
    float4 SpcColor : SV_Target1;
    float4 AmbColor : SV_Target2;
    float4 LightColor : SV_Target3;
};

cbuffer ONELIGHTDATA : register(b5)
{
    LightData LData;
}

Texture2D PosTex : register(t0);
Texture2D NorTex : register(t1);
Texture2D ShadowTex : register(t2);
SamplerState SAMPLER : register(s0);
SamplerState SHADOWSAMPLER : register(s1);


LIGHTOUT PS_DEFFERDLIGHT(VTX3DOUT _In)
{
    float4 Position = PosTex.Sample(SAMPLER, _In.Uv.xy);
    
    if (0 >= Position.a)
    {
        clip(-1);
    }
    
    float4 Normal = NorTex.Sample(SAMPLER, _In.Uv.xy);
    
    Position.w = 1.0f;
    Normal.w = 0.0f;
    
    CalLightResult LightColor = CalDirLight(Position, Normal, LData);
    LIGHTOUT Out = (LIGHTOUT) 0;
    Out.DifColor = LightColor.Dif;
    Out.SpcColor = LightColor.Spc;
    Out.AmbColor = LightColor.Amb;
    
    if (0 < Out.DifColor.x)
    {
        // 카메라의 세상에 있다. 월드 상태로 돌린다.
        float4 vWorld = mul(float4(Position.xyz, 1.0f), LData.m_CamViewInverse);
        
        // 빛을 기준으로한 VIEW와 PROJECT으로 갈려면
        float4 vShadowProjPos = mul(vWorld, LData.m_ViewProj);
        float fDepth = vShadowProjPos.z / vShadowProjPos.w;
        
        float2 vShadowUv = float2((vShadowProjPos.x / vShadowProjPos.w) * 0.5f + 0.5f, (vShadowProjPos.y / vShadowProjPos.w) * -0.5f + 0.5f);
        
        if (0.001f < vShadowUv.x && 0.999f > vShadowUv.x &&
            0.001f < vShadowUv.y && 0.999f > vShadowUv.y)
        {
            float fShadowDepth = ShadowTex.Sample(SHADOWSAMPLER, vShadowUv.xy).r;
            
            if (0.0f < fShadowDepth && fDepth > fShadowDepth + 0.001f)
            {
                Out.DifColor *= 0.01f;
                Out.SpcColor = 0.0f;
            }
        }
    }
    
    Out.LightColor = Out.DifColor + Out.SpcColor + Out.AmbColor;
    
    return Out;
}