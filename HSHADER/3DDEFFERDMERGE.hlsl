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
VTX3DOUT VS_DEFFERDMERGE(VTX3DIN _In)
{
    VTX3DOUT Out = (VTX3DOUT) 0;
    Out.Pos = _In.Pos;
    Out.Uv = _In.Uv;
    return Out;
}

struct DEFFERDOUT
{
    float4 DefferdColor : SV_Target0;
};

Texture2D DifColor : register(t0);
Texture2D LightColor : register(t1);
SamplerState SAMPLER : register(s0);


DEFFERDOUT PS_DEFFERDMERGE(VTX3DOUT _In)
{
    float4 DifOut = DifColor.Sample(SAMPLER, _In.Uv.xy);
    
    if (0 >= DifOut.a)
    {
        clip(-1);
    }
    

    
    float4 DifLightOut = LightColor.Sample(SAMPLER, _In.Uv.xy);
    
    DEFFERDOUT Out = (DEFFERDOUT)0;
    Out.DefferdColor = DifOut;
    Out.DefferdColor.xyz *= DifLightOut.xyz;
    
    if (1 <= Out.DefferdColor.a)
    {
        Out.DefferdColor.a = 1.0f;
    }
    return Out;
}