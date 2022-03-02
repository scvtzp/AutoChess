#pragma once
#include "HGAMECOM.h"
#include "HFbxEx.h"

// �ؽ�ó�� ����?
// FLOAT���� �̷���� 2���� �迭�̴�.

// 512 512 / 16

// ������ �ƴϴ�.
// �������� ���ο��� �������
// ���� �÷��̾ ������̴�.
// �����÷��̾�� �Ž��� ���׸���
// �ִϸ��̼� �Ҷ� � �ִϸ��̼ǿ��� � ȿ���� �ַ���
// ���׸����� �ٲ��� �Ѵ�.
// �����÷��̾ �ִϸ��̼Ǹ��� �޶��
// �Ѵٴ� �̾߱�ų�
// �� �ִϸ��̼��� �����Ҷ�
// ���׸����� �ٲ�
// ���׸����� ������ �ִ� �÷��̾ ������ ���� �����̴�.
class HTEXTURE;
class Game_Renderer;
class Game_BoneAnimationCom_Ex : public HACTORCOM
{
public:
	// 500 �����ӿ��� 501 ������ �� 0.3������ ���� ������ ��³༮.
	// 550 ~~~~~~�������~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 551
	// 0            0.2                                 1

	// ũ�� 
	// ���

	// ���� �����ӿ����� ���� �� ���¸� ������ �༮.
	class BONEFRAMEDATA
	{
	public:
		HMATRIX AniMat;
		HMATRIX AniWorldMat;
		Game_Vector AniScale;
		Game_Vector AniPos;
		Game_Vector AniEuler;
		Game_Vector AniQ;

		Game_Vector Scale;
		Game_Vector Pos;
		Game_Vector Euler;
		Game_Vector Q;
	};

	// 2D�� �ִϸ��̼��̶� ������ �ٸ��� ����.
	// ���� ��� �ȴ´�.
	// 1434 500 550
public:
	class MESHANI : public Game_Ref
	{
	public:
		Game_BoneAnimationCom_Ex* m_MeshAni;
		Game_Fbx_Ex* m_AniFbx;
		HFbxExAniData* m_AniData;
		int m_Start;
		int m_End;

		Game_String m_Name;

		float m_CurAniTime;
		float m_StartTime;
		float m_EndTime;

		float m_Speed;
		float m_CurTime;
		bool m_Loop;

		int m_CurFrame;
		int m_NextFrame;

		// ������ �Ŵ°��� �����Ѵ�.
		// 134
		// �ظ��� ��쿡�� 134
		// �����ִ� �� �ؽ�ó�� �������� ��.
		HTEXTURE* m_BoneTex; // = new CreateTEXTURE; �̷��� ���Ѵ�.

		// ���� ���
		std::vector<BONEFRAMEDATA> FrameBoneData;
		std::vector<HMATRIX> FrameMatData;
		// std::vector<Game_Ptr<HRENDERPLAYER>> m_RenderPlayerList;

	public:
		bool IsEnd();

	public:
		// ���ӳ����� �̰� ������ 
		// �������� ����� ����̴�
		void SetStartFrame(int _Frame);
		void SetEndFrame(int _Frame);

	public:
		// �ִϸ��̼��� ���� �ʱ� ���·� ������ �Լ�.
		void Reset();
		void Update();  // �����Ӱ�� 502 ����ϰ�
		void UpdateNext(); // ����� 502�������� ������ ���⼭ ����Ѵ�.
	};
	friend MESHANI;

private:
	std::vector<BONEFRAMEDATA> FrameBoneData;
	std::vector<HMATRIX> FrameMatData;
	std::vector<std::vector<Game_Ptr<HRENDERPLAYER>>> m_RenderPlayerList;
	Game_Ptr<HTEXTURE> m_BoneTex; // ��� �ֵ鿡�� �����ִ� �ؽ�ó

	Game_Ptr<Game_Renderer> m_Render;
	Game_Ptr<MESHANI> m_CurAni;
	bool m_IsPlay;
	Game_Fbx_Ex* m_MainFbx;

	std::map<Game_String, Game_Ptr<MESHANI>> m_AllAni;

public:
	Game_Ptr<MESHANI> FindAni(const Game_String& _AniName);

	bool IsPlay() {
		return m_IsPlay;
	}

	void Play()
	{
		m_IsPlay = true;
	}

	void Stop()
	{
		m_IsPlay = false;
	}

	void PlayChange()
	{
		m_IsPlay = !m_IsPlay;
	}

public:
	Game_Ptr<MESHANI> CurAni()
	{
		return m_CurAni;
	}

	void SetCurFrame(int _Frame);

	//������ �߰�
	public:
		int Get_Num()
		{
			return m_CurAni->m_CurFrame;
		}

public:
	// �������� �ٲ�
	void ChangeAni(const Game_String& _AniName);

	std::vector<Game_Ptr<HRENDERPLAYER>> MainFbx(const Game_String& _Fbx, const Game_String& _Mat, int _DrawSetIndex);
	std::vector<Game_Ptr<HRENDERPLAYER>> MainFbx_Static(const Game_String& _Fbx, const Game_String& _Mat, int _DrawSetIndex);
	Game_Ptr<HRENDERPLAYER> FbxToPartRenderPlayer(const Game_String& _Fbx, const Game_String& _Mat, int _DrawSetIndex, int _PushListIndex, int _MainIndex, int _SubIndex);

	void CreateAni
	(
		const Game_String& _AniFbx, 	// Monster_Run.Fbx
		const Game_String& _AniName,	// Run
		int _Start = -1,						// ������
		int _End = -1,						// ������
		int _TakeIndex = 0,					// 0
		float _Speed = 1.0f,				// �ӷ�����
		bool _Loop = true					// ����
	);

public:
	void Init() override;
	void Update() override;
	void UpdateNext() override;

};

