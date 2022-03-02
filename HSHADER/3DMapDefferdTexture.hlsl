#include "CalBase.hlsli"

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
    
    float4 ViewPos : POSITION;
    float4 ViewNormal : NORMAL;
	float4 ViewBiNormal : BINORMAL;
	float4 ViewTangent : TANGENT;
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


cbuffer MapOption : register(b3)
{
    float XCount;
    float ZCount;
    float PixelRatio;
    float YScale;
    float4 PixelUv;
    float4 PixelScale;
}

Texture2D HTex : register(t0);
SamplerState Smp : register(s0);


VTX3DOUT VS_MapDefferdTexture(VTX3DIN _In)
{
    
    // 계산을 한번 하고 들어가야 한다.
    // _In.Normal
    
    float2 YValueUv = float2(_In.Uv.x / XCount, _In.Uv.y / ZCount);
    float2 RightUv = float2(YValueUv.x + PixelUv.x * PixelRatio, YValueUv.y);
    float2 FowardUv = float2(YValueUv.x, YValueUv.y - PixelUv.y * PixelRatio);
    
    // 색깔로 높이를 가져옵니다.
    
    float4 MapColor = HTex.SampleLevel(Smp, YValueUv.xy, 0);
    float4 ColorRight = HTex.SampleLevel(Smp, RightUv.xy, 0);
    float4 ColorFoward = HTex.SampleLevel(Smp, FowardUv.xy, 0);
    
    float4 Pos = _In.Pos;
    float4 Right = float4(Pos.x + PixelScale.x * PixelRatio, 0.0f, Pos.z, 0.0f);
    float4 Foward = float4(Pos.x, 0.0f, Pos.z + PixelScale.y * PixelRatio, 0.0f);
    _In.Pos.y = MapColor.x;
    Pos.y = MapColor.x * YScale;
    Right.y = ColorRight.x * YScale;
    Foward.y = ColorFoward.x * YScale;
    
    
    _In.Normal.xyz = normalize(cross(Foward.xyz - Pos.xyz, Right.xyz - Pos.xyz));
    
    Pos.w = 1.0f;
    _In.Normal.w = 0.0f;
    VTX3DOUT Out = (VTX3DOUT) 0;
    
    Out.Pos = mul(_In.Pos, WVP);
    Out.Uv = _In.Uv;
    Out.Color = _In.Color;
    Out.ViewPos = mul(_In.Pos, WV);
    
    
    Out.ViewNormal = normalize(mul(_In.Normal, WV));

	_In.BiNormal.w = 0.0f;
    Out.ViewBiNormal = normalize(mul(_In.BiNormal, WV));

	_In.Tangent.w = 0.0f;
    Out.ViewTangent = normalize(mul(_In.Tangent, WV));

    return Out;
}

struct DEFFERDOUT
{
    float4 DifColor : SV_Target0;
    float4 PosColor : SV_Target1;
    float4 NorColor : SV_Target2;
    float4 DepColor : SV_Target3;
};

cbuffer RenderOption : register(b8)
{
    int IsDifTexture;
    int IsNormalTexture;
    int IsOutLine;
    int IsShadow;
    int IsAni;
    int IsDummy0;
    int IsDummy1;
    int IsDummy2;
	float4 BasicColor;
}


// 텍스처를 사용하겠다고 한것.
Texture2D BaseTex : register(t0);
Texture2D NormalTexture : register(t1);
DEFFERDOUT PS_MapDefferdTexture(VTX3DOUT _In)
{
    DEFFERDOUT Out = (DEFFERDOUT) 0;
    
	//if (0 != IsNormalTexture)
	//{
	//	_In.ViewNormal = CalBump(_In.ViewNormal, _In.ViewBiNormal, _In.ViewTangent, _In.Uv.xy, NormalTexture, Smp);
	//}
    
    Out.DifColor = BaseTex.Sample(Smp, _In.Uv.xy);
    Out.NorColor = _In.ViewNormal;
    Out.PosColor = _In.ViewPos;
    Out.DepColor.x = _In.ViewPos.z;
    Out.DepColor.w = Out.PosColor.w = Out.NorColor.w = 1.0f;
    return Out;
}