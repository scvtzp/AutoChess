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



cbuffer CUTDATA : register(b1)
{
    float4 CUTVECTOR;
}


// 아까 만들었던 134의 현재 애니메이션 프레임의 본정보를
// 텍스처로 만들었는데. 여기에 넣어주는 거에요.
Texture2D FrameAniTex : register(t0);

float4x4 TexToMat(int _Idx)
{
    // Load는 가감없이 텍스처의 Index를 통해서 
    // 값을 얻어오는 함수이다.
    // 텍스처의 색깔값 얻어오는 함수는 
    // DifTex.Sample(Smp, _In.Uv.xy);
    // 행렬값을 가져오는거라.
    // 비율로 가져오면 되나요?
    // 17번 본 가져와
    // 17 16번 사이의 보간된 어떤 값으로 드릴께요.
    // 17번 본의 행렬을 가져와야죠?
    // 134 * 4
    // 17번 행렬 가져오려면?
    // 17 * 4 를 해줘야 그 행렬의 시작점이죠.
    //                     R G B A
    // 시작점 더하기 + 0 16 1 0 0 0
    // 시작점 더하기 + 1 16 0 1 0 0
    // 시작점 더하기 + 2 16 0 0 1 0
    // 시작점 더하기 + 3 16 0 0 0 1
    
    // 0 0                  0 1 
    // 0.1 0.1 0.1 0.0      0.5 0.5 0.5
    
    // 100 100
    // 0 0
    
    // 4  10
    // 4 * 10 
    
    float4x4 BoneMat =
    {
        FrameAniTex.Load(int3((_Idx * 4) + 0, 0, 0)),
        FrameAniTex.Load(int3((_Idx * 4) + 1, 0, 0)),
        FrameAniTex.Load(int3((_Idx * 4) + 2, 0, 0)),
        FrameAniTex.Load(int3((_Idx * 4) + 3, 0, 0)),
    };
    return BoneMat;
}

// inout 쉐이더 레퍼런스
// 바깥에서도 수정되죠.
void SkinningTex(inout float4 _Pos, inout float4 _Normal, inout float4 _Weight, inout int4 _Indices)
{
    float4 CalPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 CalNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float fWeight[4];
    fWeight[0] = _Weight.x;
    fWeight[1] = _Weight.y;
    fWeight[2] = _Weight.z;
    fWeight[3] = 1.f - _Weight.x - _Weight.y - _Weight.z;
    
    // 10  20  40  0
    // 0.1 0.8 0.1 1.0
    
    // 가중치의 의미는
    // index 134
    
    // 0.5 0.5
    // 10  17
    // 10번행렬(본)에 50%영향받고
    // 17번행렬(본)에 50%영향받은
    // 값이 너의 최종 위치다.
    
    
    // _Indices    [0]    [1]    [0]   [0] 
    // fWeight     [0.5]  [0.5]  []    []
    
    _Pos.w = 1.0f;
    _Normal.w = 0.0f;
    
    for (int i = 0; i < 4; ++i)
    {
        matrix matBone = TexToMat(_Indices[i]);
        CalPos += fWeight[i] * mul(_Pos, matBone);
        CalNormal += fWeight[i] * mul(_Normal, matBone);
    }
    
    _Pos = CalPos;
    _Normal = CalNormal;

    return;
}

VTX3DOUT VS_3DFORWARD(VTX3DIN _In)
{
    // 애니메이션 된다는건
    // 정점의 포지션이 바뀌는 것이다.
    // _In.Pos이 바뀌어서 나올수 있다.
    SkinningTex(_In.Pos, _In.Normal, _In.Weight, _In.Index);
    
    VTX3DOUT Out = (VTX3DOUT) 0;
    // _In.Pos * ANIMAT[_In.Index[0]]
    // mul(_In.Pos, ANIMAT[_In.Index[0]]);
    Out.Pos = mul(_In.Pos, WVP); 
    Out.Uv = _In.Uv;
    Out.Color = _In.Color;
    return Out;
}

struct FORWARDOUT
{
    float4 ForwardColor : SV_Target0;
};

// 텍스처를 사용하겠다고 한것.
Texture2D DifTex : register(t0);
SamplerState Smp : register(s0);
FORWARDOUT PS_3DFORWARD(VTX3DOUT _In) 
{
    FORWARDOUT Out = (FORWARDOUT) 0;
    // Out.ForwardColor = _In.Color;
    Out.ForwardColor = DifTex.Sample(Smp, _In.Uv.xy);
    return Out;
}