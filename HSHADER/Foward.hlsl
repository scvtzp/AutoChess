#include "LightBase.hlsli"
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
    
    float4 ViewPos : POSITION;
    float4 ViewNormal : NORMAL;
    float4 ViewBiNormal : BINORMAL;
    float4 ViewTangent : TANGENT;
};


cbuffer HTRANSDATA : register(b0)
{
    TransDataStruct MatrixData;
}

cbuffer RenderOption : register(b8)
{
    RenderOptionStruct RenderOptionData;
}
Texture2D FrameAniTex : register(t0);

VTX3DOUT VS_Foward(VTX3DIN _In)
{

    if (0 != RenderOptionData.IsAni)
    {
        SkinningTex(_In.Pos, _In.Normal, _In.Weight, _In.Index, FrameAniTex);
    }
    
    VTX3DOUT Out = (VTX3DOUT) 0;
    Out.Pos = mul(_In.Pos, MatrixData.WVP);
    Out.Uv = _In.Uv;
    Out.Color = _In.Color;
    
    // ������ ������ ������ �����ϰ� �ؾ� �Ѵ�.
    _In.Pos.w = 1.0f;
    Out.ViewPos = mul(_In.Pos, MatrixData.WV);
    
    _In.Normal.w = 0.0f;
    Out.ViewNormal = mul(_In.Normal, MatrixData.WV);

    _In.BiNormal.w = 0.0f;
    Out.ViewBiNormal = mul(_In.BiNormal, MatrixData.WV);

    _In.Tangent.w = 0.0f;
    Out.ViewTangent = mul(_In.Tangent, MatrixData.WV);
    
    // ������ �Լ��� ����µ�.
    // �װ� ���ؽ� ���̴����� �ϸ� gouraud shading
    
    return Out;
}

struct FORWARDOUT
{
    float4 ForwardColor : SV_Target0;
};

cbuffer LIGHTDATA : register(b7)
{
    ObjectLightData ArrLightData;
    // int4 LightCount; // x 5
    // LightData Arr[128];
}

// �ؽ�ó�� ����ϰڴٰ� �Ѱ�.
Texture2D Tex : register(t0);
Texture2D NormalTexture : register(t1);
SamplerState Smp : register(s0);
FORWARDOUT PS_Foward(VTX3DOUT _In)
{
    if (0 != RenderOptionData.IsNormalTexture)
    {
        _In.ViewNormal = CalBump(_In.ViewNormal, _In.ViewBiNormal, _In.ViewTangent, _In.Uv.xy, NormalTexture, Smp);
    }

    CalLightResult ResultLightColor = (CalLightResult)0;
    
    for (int i = 0; i < ArrLightData.LightCount[0]; ++i)
    {
        if (0.0f == ArrLightData.ArrLightData[i].OnOff.x)
        {
            continue;
        }
        
        CalLightResult LightColor = CalDirLight(_In.ViewPos, _In.ViewNormal, ArrLightData.ArrLightData[i]);
        ResultLightColor.Amb += LightColor.Amb;
        ResultLightColor.Dif += LightColor.Dif;
        ResultLightColor.Spc += LightColor.Spc;
    }
    
    FORWARDOUT Out = (FORWARDOUT) 0;
    
    if (RenderOptionData.IsDifTexture != 0)
    {
        Out.ForwardColor = Tex.Sample(Smp, _In.Uv.xy);
        Out.ForwardColor.xyz *= (ResultLightColor.Dif.xyz + ResultLightColor.Spc.xyz + ResultLightColor.Amb.xyz);
    }
    else
    {
        Out.ForwardColor.xyz = RenderOptionData.BasicColor.xyz * (ResultLightColor.Dif.xyz + ResultLightColor.Spc.xyz + ResultLightColor.Amb.xyz);
        Out.ForwardColor.w = RenderOptionData.BasicColor.w;
    }
    
    
    return Out;
}