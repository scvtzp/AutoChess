

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

VtxTargetOut VS_EmissionBloomCheck(VtxTargetIn _In)
{
    // 삼각형이 한번 그려질때마다 3번 들어온다.
    // 6번 들어오게 된다. 
    VtxTargetOut Out = (VtxTargetIn) 0;
    Out.Pos = _In.Pos;
    Out.Uv = _In.Uv;
    return Out;
}

Texture2D EmissionTex : register(t0);
Texture2D LightTex : register(t1);
SamplerState Smp : register(s0);
float4 PS_EmissionBloomCheck(VtxTargetOut _In) : SV_Target0
{
    float4 EColor = EmissionTex.Sample(Smp, _In.Uv.xy);
    
    if (0 == EColor.x)
    {
        clip(-1);
    }

    return LightTex.Sample(Smp, _In.Uv.xy);
}