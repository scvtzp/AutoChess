#pragma once
#include "HGAMECOM.h"
#include "HFbxEx.h"

// 텍스처는 뭐죠?
// FLOAT으로 이루어진 2차원 배열이다.

// 512 512 / 16

// 랜더러 아니다.
// 랜더러를 내부에서 만든다음
// 랜더 플레이어도 만들것이다.
// 랜더플레이어는 매쉬와 메테리얼
// 애니메이션 할때 어떤 애니메이션에서 어떤 효과를 주려면
// 메테리얼이 바뀌어야 한다.
// 랜더플레이어가 애니메이션마다 달라야
// 한다는 이야기거나
// 이 애니메이션이 시작할때
// 메테리얼을 바꿔
// 메테리얼을 가지고 있는 플레이어를 여러개 만들 생각이다.
class HTEXTURE;
class Game_Renderer;
class Game_BoneAnimationCom_Ex : public HACTORCOM
{
public:
	// 500 프레임에서 501 프레임 의 0.3지점의 보간 정보를 담는녀석.
	// 550 ~~~~~~어느지점~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 551
	// 0            0.2                                 1

	// 크기 
	// 행렬

	// 현재 프레임에서의 본의 각 상태를 저장할 녀석.
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

	// 2D때 애니메이션이랑 구조가 다를게 없다.
	// 공격 방어 걷는다.
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

		// 제한을 거는것을 좋아한다.
		// 134
		// 왠만한 경우에는 134
		// 여기있는 이 텍스처는 빌려오는 것.
		HTEXTURE* m_BoneTex; // = new CreateTEXTURE; 이렇게 안한다.

		// 많은 기능
		std::vector<BONEFRAMEDATA> FrameBoneData;
		std::vector<HMATRIX> FrameMatData;
		// std::vector<Game_Ptr<HRENDERPLAYER>> m_RenderPlayerList;

	public:
		bool IsEnd();

	public:
		// 게임내에서 이걸 쓸데가 
		// 툴에서만 사용할 기능이다
		void SetStartFrame(int _Frame);
		void SetEndFrame(int _Frame);

	public:
		// 애니메이션을 가장 초기 상태로 돌리는 함수.
		void Reset();
		void Update();  // 프레임계산 502 계산하고
		void UpdateNext(); // 본계산 502프레임의 본들을 여기서 계산한다.
	};
	friend MESHANI;

private:
	std::vector<BONEFRAMEDATA> FrameBoneData;
	std::vector<HMATRIX> FrameMatData;
	std::vector<std::vector<Game_Ptr<HRENDERPLAYER>>> m_RenderPlayerList;
	Game_Ptr<HTEXTURE> m_BoneTex; // 모든 애들에게 나눠주는 텍스처

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

	//남대현 추가
	public:
		int Get_Num()
		{
			return m_CurAni->m_CurFrame;
		}

public:
	// 어택으로 바꽈
	void ChangeAni(const Game_String& _AniName);

	std::vector<Game_Ptr<HRENDERPLAYER>> MainFbx(const Game_String& _Fbx, const Game_String& _Mat, int _DrawSetIndex);
	std::vector<Game_Ptr<HRENDERPLAYER>> MainFbx_Static(const Game_String& _Fbx, const Game_String& _Mat, int _DrawSetIndex);
	Game_Ptr<HRENDERPLAYER> FbxToPartRenderPlayer(const Game_String& _Fbx, const Game_String& _Mat, int _DrawSetIndex, int _PushListIndex, int _MainIndex, int _SubIndex);

	void CreateAni
	(
		const Game_String& _AniFbx, 	// Monster_Run.Fbx
		const Game_String& _AniName,	// Run
		int _Start = -1,						// 끝까지
		int _End = -1,						// 끝까지
		int _TakeIndex = 0,					// 0
		float _Speed = 1.0f,				// 속력으로
		bool _Loop = true					// 루프
	);

public:
	void Init() override;
	void Update() override;
	void UpdateNext() override;

};

