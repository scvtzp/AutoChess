struct LightData
{
    //                       2                    10
    float4 DifColor; // 
    float4 SpcColor; // 
    float4 AmbColor; // 
    float4 LightPower; // x dif, y spc, z ambi, w ��ü�� 
    float4 SpecPow;
    float4 LightDir;
    float4 LightDirInv; // �̰� L ���� ������
    float4 LightPos; // ����Ʈ ����Ʈ�϶� 
    float4 LightRange; // ����Ʈ ����Ʈ ����
    float4 ShadowOption; // �׸��ڸ� ���� �ɼ�
    float4 CamPos; // �̰� L ���� ������
    float4 OnOff; // �̰� L ���� ������
    float4 m_ShadowRange; // �׸��ڸ� ���� �ִ� ������� �����ϸ� �˴ϴ�.
    Matrix m_View;
    Matrix m_Proj;
    Matrix m_ViewProj;
    Matrix m_CamViewInverse;
};

struct CalLightResult
{
    float4 Dif; // ���ݻ籤
    float4 Spc; // ���ݻ籤
    float4 Amb; // ȯ�汤
};

// LightData ������� �ִ� ����Ʈ 
CalLightResult CalDirLight(float4 _vViewPos, float4 _vViewNormal, LightData _Data)
{
    CalLightResult Result;
    
    _vViewNormal = normalize(_vViewNormal);
    
    // _Data.LightDirInv = LightPos - _ViewPos
    
    //                              N                        L
    Result.Dif.xyzw = max(0, dot(_vViewNormal.xyz, _Data.LightDirInv.xyz));
    
    float3 vR; // �ݻ���� N
    float3 vE; // ���� �ٶ󺸴� ���� ������ L
    
    // �ݻ����
    // reflect()
    vR = normalize(2.0f * _vViewNormal.xyz * dot(_Data.LightDirInv.xyz, _vViewNormal.xyz) - _Data.LightDirInv.xyz);
    
    // ���÷�Ʈ
    vE = normalize(_Data.CamPos.xyz - _vViewPos.xyz);
    
    //                              N                         L
    Result.Spc.x = max(0, dot(vR.xyz, vE.xyz));
    
    Result.Spc.xyzw = pow(Result.Spc.x, _Data.SpecPow.x);
    
    Result.Amb = _Data.AmbColor;
    
    Result.Dif *= _Data.LightPower.x * _Data.LightPower.w;
    Result.Spc *= _Data.LightPower.y * _Data.LightPower.w;
    Result.Amb *= _Data.LightPower.z * _Data.LightPower.w;
    // ���� �ٶ󺸴� ���� �˾ƾ� �Ѵ�.
    
    return Result;
}


// ������ ���Դϴ�.
// ������� �ִ� ũ�� ������ 512kb�ϰ��̴�.
struct ObjectLightData 
{
    int4 LightCount; // x 5
    LightData ArrLightData[128];
};