
float4x4 TexToMat(Texture2D _FrameAniTex, int _Idx)
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
        _FrameAniTex.Load(int3((_Idx * 4) + 0, 0, 0)),
        _FrameAniTex.Load(int3((_Idx * 4) + 1, 0, 0)),
        _FrameAniTex.Load(int3((_Idx * 4) + 2, 0, 0)),
        _FrameAniTex.Load(int3((_Idx * 4) + 3, 0, 0)),
    };
    return BoneMat;
}

// inout 쉐이더 레퍼런스
// 바깥에서도 수정되죠.
void SkinningTex(inout float4 _Pos, inout float4 _Normal, inout float4 _Weight, inout int4 _Indices, Texture2D _FrameAniTex)
{
    float4 CalPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 CalNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float fWeight[4];
    fWeight[0] = _Weight.x;
    fWeight[1] = _Weight.y;
    fWeight[2] = _Weight.z;
    fWeight[3] = 1.f - _Weight.x - _Weight.y - _Weight.z;
    
    _Pos.w = 1.0f;
    
    _Normal.w = 0.0f;
    
    for (int i = 0; i < 4; ++i)
    {
        matrix matBone = TexToMat(_FrameAniTex, _Indices[i]);
        CalPos += fWeight[i] * mul(_Pos, matBone);
        CalNormal += fWeight[i] * mul(_Normal, matBone);
    }
    
    _Pos = CalPos;
    _Normal.xyz = CalNormal.xyz;

    return;
}



// inout 쉐이더 레퍼런스
// 바깥에서도 수정되죠.
void SkinningPos(inout float4 _Pos, inout float4 _Weight, inout int4 _Indices, Texture2D _FrameAniTex)
{
    float4 CalPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 CalNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float fWeight[4];
    fWeight[0] = _Weight.x;
    fWeight[1] = _Weight.y;
    fWeight[2] = _Weight.z;
    fWeight[3] = 1.f - _Weight.x - _Weight.y - _Weight.z;
    
    _Pos.w = 1.0f;
    
    for (int i = 0; i < 4; ++i)
    {
        matrix matBone = TexToMat(_FrameAniTex, _Indices[i]);
        CalPos += fWeight[i] * mul(_Pos, matBone);
    }
    
    _Pos = CalPos;

    return;
}
