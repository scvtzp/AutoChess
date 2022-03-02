struct VtxTargetIn
{
    float4 Pos : POSITION;
    float4 Uv  : TEXCOORD;
};
struct VtxTargetOut
{
    float4 Pos : SV_Position;
    float4 Uv : TEXCOORD;
};


VtxTargetOut VS_OUTLINEDRAW(VtxTargetIn _In)
{
    // 삼각형이 한번 그려질때마다 3번 들어온다.
    // 6번 들어오게 된다. 
    VtxTargetOut Out = (VtxTargetIn) 0;
    Out.Pos = _In.Pos;
    Out.Uv = _In.Uv;
    return Out;
}


cbuffer OUTLINEDATA : register(b0)
{
    float4 PixelUv[4];
    float4 LineColor;
}

Texture2D Tex : register(t0);
SamplerState Smp : register(s0);
float4 PS_OUTLINEDRAW(VtxTargetOut _In) : SV_Target0
{
    float4 TexColor = Tex.Sample(Smp, _In.Uv.xy);
    
    if (0 != TexColor.x)
    {
        clip(-1);
    }
    
    TexColor = Tex.Sample(Smp, _In.Uv.xy + PixelUv[0].xy);
    if (0 != TexColor.x)    {        return LineColor;    }
    TexColor = Tex.Sample(Smp, _In.Uv.xy + PixelUv[1].xy);
    if (0 != TexColor.x)    {        return LineColor;    }
    TexColor = Tex.Sample(Smp, _In.Uv.xy + PixelUv[2].xy);
    if (0 != TexColor.x)    {        return LineColor;    }
    TexColor = Tex.Sample(Smp, _In.Uv.xy + PixelUv[3].xy);
    if (0 != TexColor.x)    {           return LineColor;    }
    
    clip(-1);
    
    return LineColor;
}