struct VtxTargetIn
{
    float4 Pos : POSITION;
    float4 Uv : TEXCOORD;
};
struct VtxTargetOut
{
    float4 Pos : SV_Position;
    float4 Uv : TEXCOORD;
};

VtxTargetOut VS_TargetMerge(VtxTargetIn _In)
{
    // 삼각형이 한번 그려질때마다 3번 들어온다.
    // 6번 들어오게 된다. 
    
    VtxTargetOut Out = (VtxTargetIn) 0;
    Out.Pos = _In.Pos;
    Out.Uv = _In.Uv;
    return Out;
}

cbuffer MergeOption : register(b0)
{
    float AlphaOption;
    float Dummy01;
    float Dummy02;
    float Dummy03;
}

Texture2D TargetTex : register(t0);
SamplerState SAMPLER : register(s0);

float4 PS_TargetMerge(VtxTargetOut _In) : SV_Target0
{
    float4 Color = TargetTex.Sample(SAMPLER, _In.Uv.xy);
    
    if (Color.r >= 1.0f)
    {
        Color.r = 1.0f;
    }
    
    if (Color.g >= 1.0f)
    {
        Color.g = 1.0f;
    }
    
    if (Color.b >= 1.0f)
    {
        Color.b = 1.0f;
    }
    
    //if (Color.a >= 1.0f)
    //{
    //    Color.a = 1.0f;
    //}
    
    if (0 == AlphaOption)
    {
        if (1.0f < Color.a)
        {
            Color.a = 1.0f;
        }
    }
    else
    {
        if (1.0f < Color.a)
        {
            Color.a = Color.x;
        }
    }
    
    return Color;
}