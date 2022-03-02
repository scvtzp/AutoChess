#pragma once
#include "HGAMECOM.h"
#include "HFBX.h"

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
class HBoneAnimationCom : public HACTORCOM
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
		HBoneAnimationCom* m_MeshAni;
		HFBX* m_Fbx;
		HFBXANIDATA* m_AniData;
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
		std::vector<BONEFRAMEDATA> FrameBoneData;

		// ������ �Ŵ°��� �����Ѵ�.
		// 134
		// �ظ��� ��쿡�� 134
		// �����ִ� �� �ؽ�ó�� �������� ��.
		Game_Ptr<HTEXTURE> m_BoneTex; // = new CreateTEXTURE; �̷��� ���Ѵ�.

		// ���� ���
		std::vector<HMATRIX> FrameMatData;
		std::vector<Game_Ptr<HRENDERPLAYER>> m_RenderPlayerList;

	public:
		bool IsEnd();

	public:
		// ���ӳ����� �̰� ������ 
		// �������� ����� ����̴�
		void SetStartFrame(int _Frame);
		void SetEndFrame(int _Frame);

	public:
		// �ִϸ��̼��� ���� �÷��̾ ��� ���� ���
		void RDOFF();
		void RDON();

	public:
		// �ִϸ��̼��� ���� �ʱ� ���·� ������ �Լ�.
		void Reset();
		void Update();  // �����Ӱ�� 502 ����ϰ�
		void UpdateNext(); // ����� 502�������� ������ ���⼭ ����Ѵ�.
	};
	friend MESHANI;

private:
	Game_Ptr<Game_Renderer> m_Render;
	Game_Ptr<MESHANI> m_CurAni;
	bool m_IsPlay;

	// ���� �����ٰ��̴�.
	//       FBX 134
	std::map<Game_String, Game_Ptr<HTEXTURE>> m_AniBoneTex;
	//       ATTECT
	//       DEF
	std::map<Game_String, std::vector<Game_Ptr<HRENDERPLAYER> >> m_AllRenderPlayer;

	//       ATTECT
	//       DEF
	std::map<Game_String, Game_Ptr<MESHANI>> m_AllAni;

public:
	Game_Ptr<MESHANI> FindAni(const Game_String& _Ani);

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

public:
	// �������� �ٲ�
	void ChangeAni(const Game_String& _AniName);

	std::list<Game_Ptr<HRENDERPLAYER>> CreateAni(
		const Game_String& _Fbx, 		 // const Game_String& _Fbx				Monster03.fbx ����
		const Game_String& _TakeName,	 // const Game_String& _TakeName 		Take 001�� 1434
		const Game_String& _AniName,	 // const Game_String& _AniName 		Attack�� ������.
		int _Start,						 // 500
		int _End,						 // 580
		const Game_String& _MatName = L"3DANIFORWARD",	 // �� � ���׸���� ������ �ض�.
		float Speed = 1.0f,					// �ӷ�����
		bool Loop = false					// �ӷ�����
	);

public:
	void Init() override;
	void Update() override;
	void UpdateNext() override;

};

