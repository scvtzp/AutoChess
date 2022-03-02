#include "HBoneAnimationComEx.h"
#include "HRENDERER.h"
#include <HGAMETIME.h>
#include "HGAMEACTOR.h"
#include "HFbxEx.h"

bool Game_BoneAnimationCom_Ex::MESHANI::IsEnd()
{
	return m_CurFrame == m_End || m_CurFrame == m_End + 1;
}

void Game_BoneAnimationCom_Ex::MESHANI::Reset()
{
	m_CurTime = m_StartTime;
	m_CurFrame = m_Start;
	m_NextFrame = m_CurFrame + 1;
}

void Game_BoneAnimationCom_Ex::MESHANI::SetStartFrame(int _Frame)
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
	m_StartTime = m_Start / (float)m_AniData->FbxModeRate;
	// ���� �ϰ�.
	Reset();
	// NewAni->m_End = _End;
	//                    600
	//NewAni->m_EndTime = NewAni->m_End / (float)NewAni->m_AniData->FrameModeCount();

}

void Game_BoneAnimationCom_Ex::MESHANI::SetEndFrame(int _Frame)
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
	m_EndTime = m_End / (float)m_AniData->FbxModeRate;
	// ���� �ϰ�.
	Reset();
	// NewAni->m_End = _End;
	//                    600
	//NewAni->m_EndTime = NewAni->m_End / (float)NewAni->m_AniData->FrameModeCount();

}


void Game_BoneAnimationCom_Ex::MESHANI::Update()
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
			m_NextFrame = m_CurFrame + 1;
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

void Game_BoneAnimationCom_Ex::MESHANI::UpdateNext()
{
	// Update ��������.
	// 100 101
	// �ؽ�ó�� �������� ���� ���?
	// m_UserBoneData <== ���� ������ ��� �ִ� �ڷᱸ��
	// == ���� 134���ϱ� 
	// m_UserBoneData 
	for (int i = 0; i < (int)FrameMatData.size(); ++i)
	{
		Bone* Ptr = m_MeshAni->m_MainFbx->FindBone(i);

		if (true == m_AniData->m_AniFrameData[i].m_Data.empty())
		{
			// �۷ι� ����Դϴ�.
			// FrameMatData[i] = Ptr->BonePos.Global;

			FrameMatData[i] = HMATRIX::Affine(Ptr->BonePos.GlobalScale, Ptr->BonePos.GlobalRotation, Ptr->BonePos.GlobalTranslation);
			continue;
		}


		// 1434���� ���� ������ �������̴�.
		HFbxExBoneFrameData& CurFrameData = m_AniData->m_AniFrameData[i].m_Data[m_CurFrame];
		// 1434���� ���� ������ ������ + 1�̴�.
		HFbxExBoneFrameData& NextFrameData = m_AniData->m_AniFrameData[i].m_Data[m_NextFrame];

		float fCurFrameTime = (float)CurFrameData.Time;
		float fNextFrameTime = (float)NextFrameData.Time;
		// ��� �ణ ���� �ִ°� ���ƿ�
		float Percent = (float)(fNextFrameTime - m_CurTime) / (1.0f / (float)m_AniData->FbxModeRate);

		Game_Vector S = Game_Vector::Lerp(CurFrameData.S, NextFrameData.S, 0.0F);
		Game_Vector Q = Game_Vector::QSLerp(CurFrameData.Q, NextFrameData.Q, 0.0F);
		Game_Vector T = Game_Vector::Lerp(CurFrameData.T, NextFrameData.T, 0.0F);
		FrameBoneData[i].AniScale = S;
		FrameBoneData[i].AniQ = Q;
		FrameBoneData[i].AniPos = T;
		FrameMatData[i] = HMATRIX::Affine(S, Q, T);
		FrameBoneData[i].AniMat = FrameMatData[i];

		//Bone* Ptr1 = m_MeshAni->m_MainFbx->FindBone((int)i);
		//HMATRIX Mat = m_MeshAni->m_MainFbx->FindBone((int)i)->BonePos.Offset;


		FrameMatData[i] = Ptr->BonePos.Offset * FrameMatData[i];



	}

	//for (size_t i = 0; i < FrameMatData.size(); i++)
	//{
	//	FrameMatData[i].IDEN();
	//}

	// �ؽ�ó�� �����ǰ�
	// �׳༮�� �������Ҷ� FrameAniTex�� ���õ���.
	m_BoneTex->SetPixel(&FrameMatData[0], (int)(sizeof(HMATRIX) * FrameMatData.size()));
}
/// //////////////////////////////////////HBoneAnimationCom

Game_Ptr<HRENDERPLAYER> Game_BoneAnimationCom_Ex::FbxToPartRenderPlayer(const Game_String& _Fbx, const Game_String& _Mat, int _DrawSetIndex, int _PushListIndex, int _MainIndex, int _SubIndex)
{
	// ���� �Ѻθ� ����Ϸ��� ��찡 �ִٸ�

	if (nullptr != m_MainFbx)
	{
		Game_Ptr<HRENDERPLAYER> Rp = m_Render->CreateRenderPlayerToFbxPartEx(_Fbx, _Mat, _DrawSetIndex, _MainIndex, _SubIndex);
		m_RenderPlayerList[_PushListIndex].push_back(Rp);
		Rp->TEXTURE(L"FrameAniTex", m_BoneTex);
		return Rp;
	}

	AMSG(L"FBX�� �Ѻ����� ����Ϸ��� �߽��ϴ�.");

	// ���� ������ ä���� �Ѵ�.

	return nullptr;
}

std::vector<Game_Ptr<HRENDERPLAYER>> Game_BoneAnimationCom_Ex::MainFbx(const Game_String& _Fbx, const Game_String& _Mat, int _Index)
{
	if (nullptr != m_MainFbx)
	{
		m_RenderPlayerList.push_back(m_Render->CreateRenderPlayerToFbxEx(_Fbx, _Mat, _Index));

		for (auto& _RD : m_RenderPlayerList[m_RenderPlayerList.size() - 1])
		{
			_RD->TEXTURE(L"FrameAniTex", m_BoneTex);
		}

		return m_RenderPlayerList[m_RenderPlayerList.size() - 1];
	}

	m_MainFbx = Game_Fbx_Ex::Find(_Fbx);

	if (nullptr == m_MainFbx)
	{
		AMSG(L"�������� �ʴ� FBX�� �ִϸ��̼� ������Ʈ�� ������� �߽��ϴ�.");
	}

	if (0 == m_MainFbx->GetBoneCount())
	{
		AMSG(L"��ο���� �������� �ʴ� fbx�� main Fbx�� �ɼ� �����ϴ�..");
	}

	if (0 == m_MainFbx->GetDrawSetCount())
	{
		AMSG(L"��ο���� �������� �ʴ� fbx�� main Fbx�� �ɼ� �����ϴ�..");
	}

	DRAWSET* DrawSetPtr = m_MainFbx->GetDrawSet(_Index);

	if (0 == DrawSetPtr->m_Mesh.size())
	{
		AMSG(L"�Ž��� �������� �ʴ� ��ο� ������ ���� �÷��̾ ������� �����ϴ�.");
	}


	std::vector<Game_Ptr<HRENDERPLAYER>> Players = m_Render->CreateRenderPlayerToFbxEx(_Fbx, _Mat, _Index);

	m_RenderPlayerList.push_back(Players);

	for (auto& RenderPlayer : Players)
	{
		RenderPlayer->RenderOption.IsAni = true;
	}


	// 134���� ���� ����� ��� �ֱ� ���� �༮�̴�.
	m_BoneTex = new HTEXTURE();
	m_BoneTex->Create(
		Game_Vector((float)(m_MainFbx->GetBoneCount() * 4.0F), 1.0F),  // ũ��
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D11_BIND_SHADER_RESOURCE,
		D3D11_USAGE_DYNAMIC);

	// ���̷��� ������
	// ���õ� fbx�� ���̷��氳���� 0�� �ƴ϶��

	for (auto& _RD : m_RenderPlayerList[m_RenderPlayerList.size() - 1])
	{
		_RD->TEXTURE(L"FrameAniTex", m_BoneTex);
	}

	return m_RenderPlayerList[m_RenderPlayerList.size() - 1];
}

Game_Ptr<Game_BoneAnimationCom_Ex::MESHANI> Game_BoneAnimationCom_Ex::FindAni(const Game_String& _AniName)
{
	if (m_AllAni.end() == m_AllAni.find(_AniName))
	{
		return nullptr;
	}

	return m_AllAni[_AniName];
}

void Game_BoneAnimationCom_Ex::SetCurFrame(int _Frame)
{
	if (nullptr != m_CurAni)
	{
		m_CurAni->m_CurFrame = _Frame; // 500
		m_CurAni->m_NextFrame = _Frame + 1;

		m_CurAni->m_CurTime = m_CurAni->m_AniData->FrameTime(m_CurAni->m_CurFrame);
	}


}

void Game_BoneAnimationCom_Ex::ChangeAni(const Game_String& _AniName)
{
	// ���� �ؾ��� �ִϸ��̼�
	m_CurAni = FindAni(_AniName);

	if (nullptr == m_CurAni)
	{
		AMSG(_AniName + L"�������� �ʴ� �ִϸ��̼����� ü���� �Ϸ��� �߽��ϴ�.");
	}

	m_CurAni->Reset();
}


void Game_BoneAnimationCom_Ex::CreateAni(
	const Game_String& _AniFbx, 		 // const HGAMESTRING& _Fbx				Monster03.fbx ����
	const Game_String& _AniName,	 // const HGAMESTRING& _AniName 		Attack�� ������.
	int _Start,						 // 500
	int _End,						 // 580
	int _TakeIndex,	 // const HGAMESTRING& _TakeName 		Take 001�� 1434
	float _Speed,					// �ӷ�����
	bool _Loop 				// �ӷ�����
)
{
	Game_Ptr<Game_Fbx_Ex> FbxPtr = Game_Fbx_Ex::Find(_AniFbx);

	if (nullptr == FbxPtr)
	{
		AMSG(L"�������� �ʴ� FBX���� �ִϸ��̼��� ������� �߽��ϴ�.");
	}

	HFbxExAniData* AniPtr = FbxPtr->FindAniData(_TakeIndex);

	if (nullptr == AniPtr)
	{
		AMSG(L"�������� �ʴ� FBX���� �ִϸ��̼ǿ��� �ִϸ��̼��� ������� �߽��ϴ�.");
	}

	if (nullptr != FindAni(_AniName))
	{
		AMSG(_AniName + L"�̹� �����ϴ� �ִϸ��̼��� �� ������� �߽��ϴ�.");
	}


	if (_Start == -1)
	{
		_Start = (int)AniPtr->TimeStartCount;
	}

	if (_End == -1)
	{
		_End = (int)AniPtr->TimeEndCount;
	}

	// �ִϸ��̼��� �ִ����� ȣ���ؾ� �Ѵ�.
	FbxPtr->CalHFbxExBoneFrameTransMatrix(m_MainFbx, _AniFbx);

	Game_Ptr<MESHANI> NewAni = new MESHANI();

	NewAni->m_AniFbx = FbxPtr;
	NewAni->m_AniData = AniPtr;
	NewAni->m_Name = _AniName;
	NewAni->m_Loop = _Loop;
	NewAni->m_Speed = _Speed;
	NewAni->m_Start = _Start;
	NewAni->m_End = _End;
	NewAni->m_StartTime = NewAni->m_Start / (float)NewAni->m_AniData->FbxModeRate;
	NewAni->m_EndTime = NewAni->m_End / (float)NewAni->m_AniData->FbxModeRate;
	NewAni->m_CurTime = NewAni->m_StartTime;
	NewAni->FrameBoneData.resize(m_MainFbx->GetBoneCount());
	NewAni->FrameMatData.resize(m_MainFbx->GetBoneCount());
	NewAni->m_BoneTex = m_BoneTex.PTR;
	NewAni->m_MeshAni = this;

	m_AllAni[_AniName] = NewAni;

	return;
}

void Game_BoneAnimationCom_Ex::Init()
{
	m_Render = ACTOR()->FindComType<Game_Renderer>();
	m_IsPlay = true;
	if (nullptr == m_Render)
	{
		AMSG(L"�������� ������ 3D�ִϸ��̼��� ����� �����ϴ�.");
	}
}
void Game_BoneAnimationCom_Ex::Update()
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
void Game_BoneAnimationCom_Ex::UpdateNext()
{
	if (nullptr == m_CurAni)
	{
		return;
	}

	m_CurAni->UpdateNext();
}