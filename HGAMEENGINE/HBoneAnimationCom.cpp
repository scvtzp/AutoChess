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
	// ���� �ϰ�.
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
	// ���� �ϰ�.
	Reset();
	// NewAni->m_End = _End;
	//                    600
	//NewAni->m_EndTime = NewAni->m_End / (float)NewAni->m_AniData->FrameModeCount();

}


void HBoneAnimationCom::MESHANI::Update()
{
	// �ð� �缭
	// �������� ������Ų��.
	// I + 1
	// n 100 + 1

	// 0���� ��� �ð��� �ƴϴ�.
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

	// m_AniData->StartTime == 0 �ϰ��̴�.
	float m_CalTime = ((float)m_AniData->StartTime.GetSecondDouble()) + m_CurTime;

	// ���� ������
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
	// Update ��������.
	// 100 101
	// �ؽ�ó�� �������� ���� ���?
	// m_UserBoneData <== ���� ������ ��� �ִ� �ڷᱸ��
	// == ���� 134���ϱ� 
	// m_UserBoneData 
	for (int i = 0; i < (int)m_Fbx->m_UserBoneData.size(); ++i)
	{

		// 134
		// std::vector<BoneFrame> m_AniFrameData;

		//m_AniData->
		// Ȥ���� 
		// empty��� ����?
		if (true == m_AniData->m_AniFrameData[i].m_Data.empty())
		{
			// Ȥ�ó� Ű������ �����Ͱ� ������
			// �ִϸ��̼ǿ��� �������� �ʴ³�. 
			// ���� ��Ʈ�ų� �������� �ʴ� �������� �ش��ϴ� ���̴�.
			// �⺻ ��� �������� �����̴�.
			FrameMatData[i] = m_Fbx->m_UserBoneData[i].Bone;
			continue;
		}


		// 1434���� ���� ������ �������̴�.
		BoneFrameData& CurFrameData = m_AniData->m_AniFrameData[i].m_Data[m_CurFrame];
		// 1434���� ���� ������ ������ + 1�̴�.
		BoneFrameData& NextFrameData = m_AniData->m_AniFrameData[i].m_Data[m_NextFrame];

		// �̰� ����ȯ ���ϴ°� ������ ����.
		// ��ŸŸ�� 1��?
		// 1��
		// 10
		// 300 * 1
		// 301
		// 10
		// 10.0333333333 ��
		float fCurFrameTime = (float)CurFrameData.Time;
		float fNextFrameTime = (float)NextFrameData.Time;

		//                      10 ~ 10.3333333 ���� �ð��̰���?
		//                      0~ 0.03333333333 ���� � ��     1/ 30 0.033333333
		//                      0.0123458
		//                      0~0.033333333 ������   
		float Percent = (float)(fNextFrameTime - m_CurTime) / (1.0f / (float)m_AniData->FbxModeCount);

		// ��������.
		// Lerp( 100, 200, 0.5) => 150
		// Lerp( 0, 90, 0.5) => 45
		// Lerp( 10, 110, 0.5) => 60
		// �����̴�.
		// ���ʹ� ������ �����ϰ�
		// ������ ��������?
		// float lerp(float p1, float p2, float d1) {
		// return (1 - d1)p1 + d1p2;
		// }
		// float lerp(hvector p1, hvector p2, hvector d1) {
		// return (1 - d1)p1 + d1p2;
		// }

		// 0 0 0 // 10 10 10 0.5 == 5 5 5
		// �� �����ӻ����� �������� ���ٴ� ��.
		Game_Vector S = Game_Vector::Lerp(CurFrameData.S, NextFrameData.S, Percent);
		// Q�� ���ʹϿ�
		// ������ü�� �𸣰� ���ʹϿ��� ���丸 �ľ��ϰ� �ִ�.
		// ���ʹϿ��� ���ο� �������ڸ� ���ο� XYZ�� ȸ������ �� ������ �ְ�
		// �װ��� float 4���� ǥ���Ҽ� �ִ� �༮�̴�.
		// ��׸���
		// �̰� ��� ������?
		// xȸ�� �� ����.
		// �׶��� �ܼ�����
		// 0 0 0 => 40 0 0   0.5 20 0 0
		// ȸ���� �߰������� �����ǰ�
		// ���鼱�������� ����Ѵ�.
		Game_Vector Q = Game_Vector::QSLerp(CurFrameData.Q, NextFrameData.Q, Percent);
		Game_Vector T = Game_Vector::Lerp(CurFrameData.T, NextFrameData.T, Percent);

		// ���Ŀ� ������ �־ �����س������̱�� �ѵ�
		// �ִϸ��̼��̶��� ���� ���� �����.
		FrameBoneData[i].AniScale = S;
		FrameBoneData[i].AniQ = Q;
		FrameBoneData[i].AniPos = T;

		// Affine == LWorld
		// LWorld��� ũ����
		// �̳༮�� Ư���� ȸ�� ���������� ���带 �ǹ��ϴµ�.
		FrameMatData[i] = HMATRIX::Affine(S, Q, T);
		FrameBoneData[i].AniMat = FrameMatData[i];
		// ���� ��İ� ���ؼ� FrameMatData[i]
		// FrameMatData[i] 134���� ���� �ȴ�.
		// �̰� �ִϸ��̼��� ����̴�.
		HMATRIX Mat;
		Mat.Scale3D({ 2.0f, 2.0f, 2.0f });
		FrameMatData[i] = Mat * m_Fbx->m_UserBoneData[i].Offset * FrameMatData[i];
	}

	for (size_t i = 0; i < FrameMatData.size(); i++)
	{
		FrameMatData[i].IDEN();
	}

	// �ؽ�ó�� �����ǰ�
	// �׳༮�� �������Ҷ� FrameAniTex�� ���õ���.
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
	// �ִϸ��̼��� �����÷��̾ ������ �ִ�.
	if (nullptr != m_CurAni)
	{
		m_CurAni->RDOFF();
	}

	// ���� �ؾ��� �ִϸ��̼�
	m_CurAni = FindAni(_AniName);

	if (nullptr == m_CurAni)
	{
		AMSG(_AniName + L"�������� �ʴ� �ִϸ��̼����� ü���� �Ϸ��� �߽��ϴ�.");
	}

	m_CurAni->Reset();
}


std::list<Game_Ptr<HRENDERPLAYER>> HBoneAnimationCom::CreateAni(
	const Game_String& _Fbx, 		 // const Game_String& _Fbx				Monster03.fbx ����
	const Game_String& _TakeName,	 // const Game_String& _TakeName 		Take 001�� 1434
	const Game_String& _AniName,	 // const Game_String& _AniName 		Attack�� ������.
	int _Start,						 // 500
	int _End,						 // 580
	const Game_String& _MatName ,	 // �� � ���׸���� ������ �ض�.
	float _Speed ,					// �ӷ�����
	bool _Loop 				// �ӷ�����
)
{
	Game_Ptr<HFBX> FbxPtr = HFBX::Find(_Fbx);

	if (nullptr == FbxPtr)
	{
		AMSG(L"�������� �ʴ� FBX���� �ִϸ��̼��� ������� �߽��ϴ�.");
	}

	HFBXANIDATA* AniPtr = FbxPtr->FindAniData(_TakeName);

	if (nullptr == AniPtr)
	{
		AMSG(L"�������� �ʴ� FBX���� �ִϸ��̼ǿ��� �ִϸ��̼��� ������� �߽��ϴ�.");
	}

	if (nullptr != FindAni(_AniName))
	{
		AMSG(_AniName + L"�̹� �����ϴ� �ִϸ��̼��� �� ������� �߽��ϴ�.");
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

	// _Fbx�� �Ž����� �ִϸ��̼��� ����� ������ ����.
	std::map<Game_String, std::vector<Game_Ptr<HRENDERPLAYER>>>::iterator FindRenderPlayer = m_AllRenderPlayer.find(_Fbx);
	if (m_AllRenderPlayer.end() == m_AllRenderPlayer.find(_Fbx))
	{
		// 134���� ���� ����� ��� �ֱ� ���� �༮�̴�.
		m_AniBoneTex[_Fbx] = new HTEXTURE();

		// �ȼ��ϳ��� ���Ͱ� �Ѱ��� ���µ�.
		// ����� �ȼ� 4����� �̾߱� �̹Ƿ�
		// FbxPtr->m_UserBoneData.size() ����? 134 * 4
		// �ȼ��ϳ��� �����ϳ��� ��ƾ� �ϴϱ� 
		// DXGI_FORMAT_R32G32B32A32_FLOAT 16����Ʈ ������ ����.
		// RED�� 32��Ʈ�� 
		// 1����Ʈ �� ��Ʈ�Դϱ�?
		// 32��Ʈ��? 4����Ʈ
		// R 4 G 4 B 4 A 4 == ���ļ� 16����Ʈ
		// ���� �����Ʈ? float X float Y float Z float W
		// �޸� �޸� => ��Ʈ�� ����Ʈ�� �̷���� �ִ�.
		// �ñ������� ���� �ؽ�ó�� float int divice�� ���δ�.
		// ��Ʈ�� ����Ʈ�� �̷�����ְ� ũ�Ⱑ ���ٸ�.
		// �־��ټ��� �ִ�.
		// memset memcpy

		// �־��� ���̴�..
		// Texture2D FrameAniTex : register(t0);
		m_AniBoneTex[_Fbx]->Create(
			Game_Vector((float)(FbxPtr->m_UserBoneData.size() * 4.0F), 1.0F),  // ũ��
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			D3D11_BIND_SHADER_RESOURCE,
			D3D11_USAGE_DYNAMIC);

		// ���� �÷��̾ ����ϴ�.
		// �����÷��̾�� ������ ����?
		// �Ž�1�� ����1���� ����ؼ�
		// �������� �Ҽ� ����?
		// �̳༮�� �Ž��� 4���� �༮�̴�.
		m_AllRenderPlayer[_Fbx] = m_Render->CreateRenderPlayerToFbx(_Fbx, _MatName, L"LWSMP");

		// ���������� �ڵ����� ���õȴ�.
		// ������ �Ǵ� ���� �ڵ����� ���õȴ�.
		for (auto& _RD : m_AllRenderPlayer[_Fbx])
		{
			_RD->TEXTURE(L"FrameAniTex", m_AniBoneTex[_Fbx]);
		}

		// ���� �÷��̾� ����Ʈ�Դϴ�.
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
		AMSG(L"�������� ������ 3D�ִϸ��̼��� ����� �����ϴ�.");
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