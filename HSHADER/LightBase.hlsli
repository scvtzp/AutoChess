struct LightData
{
    //                       2                    10
    float4 DifColor; // 
    float4 SpcColor; // 
    float4 AmbColor; // 
    float4 LightPower; // x dif, y spc, z ambi, w 전체다 
    float4 SpecPow;
    float4 LightDir;
    float4 LightDirInv; // 이게 L 빛의 역방향
    float4 LightPos; // 포인트 라이트일때 
    float4 LightRange; // 포인트 라이트 범위
    float4 ShadowOption; // 그림자를 위한 옵션
    float4 CamPos; // 이게 L 빛의 역방향
    float4 OnOff; // 이게 L 빛의 역방향
    float4 m_ShadowRange; // 그림자를 볼수 있는 범위라고 생각하면 됩니다.
    Matrix m_View;
    Matrix m_Proj;
    Matrix m_ViewProj;
    Matrix m_CamViewInverse;
};

struct CalLightResult
{
    float4 Dif; // 난반사광
    float4 Spc; // 정반사광
    float4 Amb; // 환경광
};

// LightData 뷰공간에 있는 라이트 
CalLightResult CalDirLight(float4 _vViewPos, float4 _vViewNormal, LightData _Data)
{
    CalLightResult Result;
    
    _vViewNormal = normalize(_vViewNormal);
    
    // _Data.LightDirInv = LightPos - _ViewPos
    
    //                              N                        L
    Result.Dif.xyzw = max(0, dot(_vViewNormal.xyz, _Data.LightDirInv.xyz));
    
    float3 vR; // 반사방향 N
    float3 vE; // 내가 바라보는 눈의 방향이 L
    
    // 반사방향
    // reflect()
    vR = normalize(2.0f * _vViewNormal.xyz * dot(_Data.LightDirInv.xyz, _vViewNormal.xyz) - _Data.LightDirInv.xyz);
    
    // 리플렉트
    vE = normalize(_Data.CamPos.xyz - _vViewPos.xyz);
    
    //                              N                         L
    Result.Spc.x = max(0, dot(vR.xyz, vE.xyz));
    
    Result.Spc.xyzw = pow(Result.Spc.x, _Data.SpecPow.x);
    
    Result.Amb = _Data.AmbColor;
    
    Result.Dif *= _Data.LightPower.x * _Data.LightPower.w;
    Result.Spc *= _Data.LightPower.y * _Data.LightPower.w;
    Result.Amb *= _Data.LightPower.z * _Data.LightPower.w;
    // 내가 바라보는 곳도 알아야 한다.
    
    return Result;
}


// 포워드 빛입니다.
// 상수버퍼 최대 크기 제한이 512kb일것이다.
struct ObjectLightData 
{
    int4 LightCount; // x 5
    LightData ArrLightData[128];
};