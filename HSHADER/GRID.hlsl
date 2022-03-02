struct VTXGRIDIN
{
    float4 Pos : POSITION;
    float4 Uv  : TEXCOORD;
    float4 Color : COLOR;
};
struct VTXGRIDOUT
{
    float4 Pos : SV_Position;
    float4 WPos : POSITION; // ���� ������ �ȴ�.
    float4 Uv : TEXCOORD;
    float4 Color : COLOR;
};

cbuffer HTRANSDATA : register(b0)
{
    matrix POS;
    matrix SCALE;
    matrix ROT;
    matrix REVOL;
    matrix PARENT;
    matrix LWORLD;
    matrix WWORLD;
    matrix VIEW;
    matrix PROJ;
    matrix WV;
    matrix VP;
    matrix WVP;
}

VTXGRIDOUT VS_GRID(VTXGRIDIN _In)
{
    // �ﰢ���� �ѹ� �׷��������� 3�� ���´�.
    // 6�� ������ �ȴ�. 
    
    VTXGRIDOUT Out = (VTXGRIDOUT) 0;
    Out.Pos = mul(_In.Pos, WVP);
    Out.WPos = mul(_In.Pos, WWORLD);
    Out.Uv = _In.Uv;
    Out.Color = _In.Color;
    return Out;
}

float4 PS_GRID(VTXGRIDOUT _In) : SV_Target0
{
    float Color = _In.Color;
    float4 Pos = _In.WPos;
    Pos.y = 0.0f;
    
    float Len = length(Pos - float4(0.0f, 0.0f, 0.0f, 0.0f));
    
    // 10 ������ a = 1.0f
    // �� �ڷ� 100.0F ������ �Ÿ��� ���� A�� �Ծ����� �Ѵ�.
    Color.r = 50.0f/ Len;
    if (0.05f > Color.r)
    {
        clip(-1);
    }
    
    float ModX = _In.WPos.x % 10.0f;
    float ModZ = _In.WPos.z % 10.0f;
    
    ModX = abs(ModX);
    ModZ = abs(ModZ);
    
    if (0.1f > ModX || 9.9f < ModX)
    {
        return Color;
    }
    if (0.1f > ModZ || 9.9f < ModZ)
    {
        return Color;
    }
    
    clip(-1);
    return _In.Color;
}