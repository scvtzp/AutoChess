#include "HBoneAnimationCom.h"
#include "HRENDERER.h"
#include <HGAMETIME.h>
#include "HGAMEACTOR.h"

bool HBoneAnimationCom::MESHANI::IsEnd() 
{
	return m_CurFrame == m_End || m_CurFrame == m_End + 1;
}

void HBoneAnimationCom::MESHANI::RDON()
{
	for (auto& _RP : m_RenderPlayerList)
	{
		_RP->On();
	}
}

void HBoneAnimationCom::MESHANI::RDOFF()
{
	for (auto& _RP: m_RenderPlayerList)
	{
		_RP->Off();
	}
}

void HBoneAnimationCom::MESHANI::Reset()
{
	m_CurTime = m_StartTime;
	m_CurFrame = m_Start;
	m_NextFrame = m_CurFrame + 1;
	RDON();
}

void HBoneAnimationCom::MESHANI::SetStartFrame(int _Frame) 
{
	if (m_End < _Frame)
	{
		return;
	}

	if (m_AniData->TimeEndCount < _Frame)
	{
		return;
	}

	if (m_AniData->StartTime > _Frame)
	{
		return;
	}

	m_Start = _Frame;
	m_StartTime = m_Start / (float)m_AniData->FrameModeCount();
	// 재계산 하고.
	Reset();
	// NewAni->m_End = _End;
	//                    600
	//NewAni->m_EndTime = NewAni->m_End / (float)NewAni->m_AniData->FrameModeCount();

}

void HBoneAnimationCom::MESHANI::SetEndFrame(int _Frame)
{
	if (m_Start > _Frame)
	{
		return;
	}


	if (m_AniData->TimeEndCount < _Frame)
	{
		return;
	}

	if (m_AniData->StartTime > _Frame)
	{
		return;
	}


	m_End = _Frame;
	m_EndTime = m_End / (float)m_AniData->FrameModeCount();
	// 재계산 하고.
	Reset();
	// NewAni->m_End = _End;
	//                    600
	//NewAni->m_EndTime = NewAni->m_End / (float)NewAni->m_AniData->FrameModeCount();

}


void HBoneAnimationCom::MESHANI::Update()
{
	// 시간 재서
	// 프레임을 전진시킨다.
	// I + 1
	// n 100 + 1

	// 0부터 재는 시간이 아니다.
	// 600 680
	m_CurTime += Game_Time::DeltaTime();

	if (m_EndTime <= m_CurTime)
	{
		if (false == m_Loop)
		{
			m_CurTime = m_EndTime;
		}
		else {
			m_CurTime = m_StartTime;
		}
	}

	// m_AniData->StartTime == 0 일것이다.
	float m_CalTime = ((float)m_AniData->StartTime.GetSecondDouble()) + m_CurTime;

	// 현재 프레임
	m_CurFrame = (int)(m_CalTime * m_AniData->FbxModeCount);
	m_NextFrame = m_CurFrame + 1;

	if (m_CurFrame >= m_AniData->TimeEndCount - 1)
	{
		if (true == m_Loop)
		{
			m_CurFrame = m_Start;
			m_CurTime = 0.0f;
		}
		else 
		{
			m_CurFrame = m_CurFrame - 1;
			m_NextFrame = m_CurFrame;
		}
	}

	// 600 600
}

void HBoneAnimationCom::MESHANI::UpdateNext()
{
	// Update 다음에서.
	// 100 101
	// 텍스처에 담아줘야할 본이 몇개죠?
	// m_UserBoneData <== 본의 정보를 담고 있는 자료구조
	// == 본이 134개니까 
	// m_UserBoneData 
	for (int i = 0; i < (int)m_Fbx->m_UserBoneData.size(); ++i)
	{

		// 134
		// std::vector<BoneFrame> m_AniFrameData;

		//m_AniData->
		// 혹여나 
		// empty라는 말은?
		if (true == m_AniData->m_AniFrameData[i].m_Data.empty())
		{
			// 혹시나 키프레임 데이터가 없으면
			// 애니메이션에서 움직이지 않는놈. 
			// 보통 루트거나 움직이지 않는 기준점에 해당하는 본이다.
			// 기본 행렬 서있을때 형렬이다.
			FrameMatData[i] = m_Fbx->m_UserBoneData[i].Bone;
			continue;
		}


		// 1434개중 내가 선택한 프레임이다.
		BoneFrameData& CurFrameData = m_AniData->m_AniFrameData[i].m_Data[m_CurFrame];
		// 1434개중 내가 선택한 프레임 + 1이다.
		BoneFrameData& NextFrameData = m_AniData->m_AniFrameData[i].m_Data[m_NextFrame];

		// 이거 형변환 안하는게 좋은거 같다.
		// 델타타임 1이?
		// 1초
		// 10
		// 300 * 1
		// 301
		// 10
		// 10.0333333333 초
		float fCurFrameTime = (float)CurFrameData.Time;
		float fNextFrameTime = (float)NextFrameData.Time;

		//                      10 ~ 10.3333333 안의 시간이겠죠?
		//                      0~ 0.03333333333 내의 어떤 숫     1/ 30 0.033333333
		//                      0.0123458
		//                      0~0.033333333 내에서   
		float Percent = (float)(fNextFrameTime - m_CurTime) / (1.0f / (float)m_AniData->FbxModeCount);

		// 선형보간.
		// Lerp( 100, 200, 0.5) => 150
		// Lerp( 0, 90, 0.5) => 45
		// Lerp( 10, 110, 0.5) => 60
		// 비율이다.
		// 벡터는 곱셈이 가능하고
		// 덧셈도 가능하죠?
		// float lerp(float p1, float p2, float d1) {
		// return (1 - d1)p1 + d1p2;
		// }
		// float lerp(hvector p1, hvector p2, hvector d1) {
		// return (1 - d1)p1 + d1p2;
		// }

		// 0 0 0 // 10 10 10 0.5 == 5 5 5
		// 두 프레임사이의 비율값에 들어간다는 것.
		Game_Vector S = Game_Vector::Lerp(CurFrameData.S, NextFrameData.S, Percent);
		// Q는 쿼터니온
		// 공식자체를 모르고 쿼터니온의 개념만 파악하고 있다.
		// 쿼터니온은 내부에 비유하자면 내부에 XYZ의 회전값을 다 가지고 있고
		// 그것을 float 4개로 표현할수 있는 녀석이다.
		// 디그리로
		// 이건 어디만 본거죠?
		// x회전 만 본것.
		// 그때는 단순하지
		// 0 0 0 => 40 0 0   0.5 20 0 0
		// 회전도 중간값으로 보간되고
		// 구면선형보간을 사용한다.
		Game_Vector Q = Game_Vector::QSLerp(CurFrameData.Q, NextFrameData.Q, Percent);
		Game_Vector T = Game_Vector::Lerp(CurFrameData.T, NextFrameData.T, Percent);

		// 추후에 쓸일이 있어서 저장해놓은것이기는 한데
		// 애니메이션이랑은 전혀 관련 없어요.
		FrameBoneData[i].AniScale = S;
		FrameBoneData[i].AniQ = Q;
		FrameBoneData[i].AniPos = T;

		// Affine == LWorld
		// LWorld행렬 크자이
		// 이녀석은 특정한 회전 공간에서의 월드를 의미하는데.
		FrameMatData[i] = HMATRIX::Affine(S, Q, T);
		FrameBoneData[i].AniMat = FrameMatData[i];
		// 기준 행렬과 곱해서 FrameMatData[i]
		// FrameMatData[i] 134개가 들어가게 된다.
		// 이게 애니메이션한 행렬이다.
		HMATRIX Mat;
		Mat.Scale3D({ 2.0f, 2.0f, 2.0f });
		FrameMatData[i] = Mat * m_Fbx->m_UserBoneData[i].Offset * FrameMatData[i];
	}

	for (size_t i = 0; i < FrameMatData.size(); i++)
	{
		FrameMatData[i].IDEN();
	}

	// 텍스처가 수정되고
	// 그녀석은 랜더링할때 FrameAniTex로 세팅되죠.
	m_BoneTex->SetPixel(&FrameMatData[0], (int)(sizeof(HMATRIX) * FrameMatData.size()));
}
/// //////////////////////////////////////HBoneAnimationCom

Game_Ptr<HBoneAnimationCom::MESHANI> HBoneAnimationCom::FindAni(const Game_String& _AniName)
{
	if (m_AllAni.end() == m_AllAni.find(_AniName))
	{
		return nullptr;
	}

	return m_AllAni[_AniName];
}

void HBoneAnimationCom::SetCurFrame(int _Frame)
{
	if (nullptr != m_CurAni)
	{
		m_CurAni->m_CurFrame = _Frame; // 500
		m_CurAni->m_NextFrame = _Frame + 1;

		m_CurAni->m_CurTime = m_CurAni->m_AniData->FrameTime(m_CurAni->m_CurFrame);
	}


}

void HBoneAnimationCom::ChangeAni(const Game_String& _AniName)
{
	// 애니메이션이 랜더플레이어를 가지고 있다.
	if (nullptr != m_CurAni)
	{
		m_CurAni->RDOFF();
	}

	// 이제 해야할 애니메이션
	m_CurAni = FindAni(_AniName);

	if (nullptr == m_CurAni)
	{
		AMSG(_AniName + L"존재하지 않는 애니메이션으로 체인지 하려고 했습니다.");
	}

	m_CurAni->Reset();
}


std::list<Game_Ptr<HRENDERPLAYER>> HBoneAnimationCom::CreateAni(
	const Game_String& _Fbx, 		 // const Game_String& _Fbx				Monster03.fbx 에서
	const Game_String& _TakeName,	 // const Game_String& _TakeName 		Take 001로 1434
	const Game_String& _AniName,	 // const Game_String& _AniName 		Attack을 만들어라.
	int _Start,						 // 500
	int _End,						 // 580
	const Game_String& _MatName ,	 // 를 어떤 메테리얼로 랜더링 해라.
	float _Speed ,					// 속력으로
	bool _Loop 				// 속력으로
)
{
	Game_Ptr<HFBX> FbxPtr = HFBX::Find(_Fbx);

	if (nullptr == FbxPtr)
	{
		AMSG(L"존재하지 않는 FBX에서 애니메이션을 만들려고 했습니다.");
	}

	HFBXANIDATA* AniPtr = FbxPtr->FindAniData(_TakeName);

	if (nullptr == AniPtr)
	{
		AMSG(L"존재하지 않는 FBX내의 애니메이션에서 애니메이션을 만들려고 했습니다.");
	}

	if (nullptr != FindAni(_AniName))
	{
		AMSG(_AniName + L"이미 존재하는 애니메이션을 또 만들려고 했습니다.");
	}

	Game_Ptr<MESHANI> NewAni = new MESHANI();

	NewAni->m_Fbx = FbxPtr;
	NewAni->m_AniData = AniPtr;
	NewAni->m_Name = _AniName;
	NewAni->m_Loop = _Loop;
	NewAni->m_Speed = _Speed;
	NewAni->m_Start = _Start;
	NewAni->m_End = _End;

	//                    600
	NewAni->m_StartTime = NewAni->m_Start / (float)NewAni->m_AniData->FrameModeCount();
	NewAni->m_EndTime = NewAni->m_End / (float)NewAni->m_AniData->FrameModeCount();

	NewAni->m_CurTime = NewAni->m_StartTime;

	//NewAni->CurFrame= NewAni->m_StartTime;
	//NewAni->NextFrame = NewAni->m_StartTime;

	NewAni->FrameBoneData.resize(FbxPtr->m_UserBoneData.size());
	NewAni->FrameMatData.resize(FbxPtr->m_UserBoneData.size());

	// _Fbx의 매쉬에서 애니메이션이 만들어 진적이 없다.
	std::map<Game_String, std::vector<Game_Ptr<HRENDERPLAYER>>>::iterator FindRenderPlayer = m_AllRenderPlayer.find(_Fbx);
	if (m_AllRenderPlayer.end() == m_AllRenderPlayer.find(_Fbx))
	{
		// 134개의 본의 행렬을 담아 주기 위한 녀석이다.
		m_AniBoneTex[_Fbx] = new HTEXTURE();

		// 픽셀하나는 벡터가 한개가 들어가는데.
		// 행렬은 픽셀 4개라는 이야기 이므로
		// FbxPtr->m_UserBoneData.size() 본은? 134 * 4
		// 픽셀하나에 벡터하나를 담아야 하니까 
		// DXGI_FORMAT_R32G32B32A32_FLOAT 16바이트 포맷을 쓴다.
		// RED를 32비트로 
		// 1바이트 몇 비트입니까?
		// 32비트는? 4바이트
		// R 4 G 4 B 4 A 4 == 합쳐서 16바이트
		// 벡터 몇바이트? float X float Y float Z float W
		// 메모리 메모리 => 비트와 바이트로 이루어져 있다.
		// 궁극적으로 가면 텍스처나 float int divice나 전부다.
		// 비트와 바이트로 이루어져있고 크기가 같다면.
		// 넣어줄수도 있다.
		// memset memcpy

		// 넣어줄 것이다..
		// Texture2D FrameAniTex : register(t0);
		m_AniBoneTex[_Fbx]->Create(
			Game_Vector((float)(FbxPtr->m_UserBoneData.size() * 4.0F), 1.0F),  // 크기
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			D3D11_BIND_SHADER_RESOURCE,
			D3D11_USAGE_DYNAMIC);

		// 랜더 플레이어를 만듭니다.
		// 랜더플레이어는 어떤기능을 하죠?
		// 매쉬1개 재질1개를 사용해서
		// 랜더링을 할수 있죠?
		// 이녀석은 매쉬가 4개인 녀석이다.
		m_AllRenderPlayer[_Fbx] = m_Render->CreateRenderPlayerToFbx(_Fbx, _MatName, L"LWSMP");

		// 랜더러에서 자동으로 세팅된다.
		// 랜더링 되는 순간 자동으로 세팅된다.
		for (auto& _RD : m_AllRenderPlayer[_Fbx])
		{
			_RD->TEXTURE(L"FrameAniTex", m_AniBoneTex[_Fbx]);
		}

		// 랜더 플레이어 리스트입니다.
		FindRenderPlayer = m_AllRenderPlayer.find(_Fbx);
	} ;

	NewAni->m_BoneTex = m_AniBoneTex[_Fbx];
	NewAni->m_MeshAni = this;
	NewAni->m_RenderPlayerList = FindRenderPlayer->second;

	m_AllAni[_AniName] = NewAni;

	return std::list<Game_Ptr<HRENDERPLAYER>>();
}

void HBoneAnimationCom::Init() 
{
	m_Render = ACTOR()->FindComType<Game_Renderer>();
	m_IsPlay = true;
	if (nullptr == m_Render)
	{
		AMSG(L"랜더러가 없으면 3D애니메이션을 만들수 없습니다.");
	}
}
void HBoneAnimationCom::Update() 
{
	if (nullptr == m_CurAni)
	{
		return;
	}

	if (false == m_IsPlay)
	{
		return;
	}

	m_CurAni->Update();
}
void HBoneAnimationCom::UpdateNext() 
{
	if (nullptr == m_CurAni)
	{
		return;
	}

	m_CurAni->UpdateNext();
}