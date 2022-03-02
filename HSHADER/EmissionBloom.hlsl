

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

VtxTargetOut VS_EmissionBloom(VtxTargetIn _In)
{
    // �ﰢ���� �ѹ� �׷��������� 3�� ���´�.
    // 6�� ������ �ȴ�. 
    VtxTargetOut Out = (VtxTargetIn) 0;
    Out.Pos = _In.Pos;
    Out.Uv = _In.Uv;
    return Out;
}

static float g_vFilter[25] =
{
    1.0F, 2.0f, 4.0f, 2.0f, 1.0f, // 10
    2.0F, 4.0f, 8.0f, 4.0f, 2.0f, // 20
    4.0F, 8.0f, 16.0f, 8.0f, 4.0f, // 40
    2.0F, 4.0f, 8.0f, 4.0f, 2.0f, // 20
    1.0F, 2.0f, 4.0f, 2.0f, 1.0f, // 10
};


// SmallTarget�� ����� �ʿ��ϴ�.
cbuffer TargetUvSize : register(b1)
{
    float4 SmallUvSize;
}


// ��ҵǾ��ִ� ����� ���� ����� �ִ�.
// �̰� ������ �ؼ�
// �� Ÿ�ٿ��ٰ� ���� ���� ���̴�.
Texture2D SmallTex : register(t0);
SamplerState Smp : register(s0);
float4 PS_EmissionBloom(VtxTargetOut _In) : SV_Target0
{
    // float4 BloomColor = SmallTex.Sample(Smp, _In.Uv.xy);
    
    float4 Sum;
    
    for (int y = 0; y < 5; ++y)
    {
        for (int x = 0; x < 5; ++x)
        {
            float2 Uv = _In.Uv.xy + float2((y - 2) * SmallUvSize.x, (x - 2) * SmallUvSize.y);
            Sum += SmallTex.Sample(Smp, Uv) * g_vFilter[y * 5 + x];
        }
    }
    
    Sum /= 100.0f;
    
    Sum.a = Sum.x;
    
    
    
    return Sum;
}