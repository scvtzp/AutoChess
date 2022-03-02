#pragma once
#include <map>
#include <HPTR.h>
#include <HGAMESTRING.h>
#include <HGAMEACTOR.h>
#include <HRENDERER.h>
#pragma once
class Game_Actor;
class HACTORCOM;
class Game_2DAnimation : public HACTORCOM
{
public:
	Game_Ptr<Game_Renderer> m_Render123;

public:
	void Init(Game_Ptr<Game_Renderer> _Render = nullptr);
	void Update() override;

private:
	class Game_2DFrameAnimation : public Game_Ref
	{
	public:
		Game_Actor* Actor;
		Game_String m_AniName;
		Game_String m_SpriteName;
		unsigned int m_Start;
		unsigned int m_End;
		float m_FrameTime;
		bool m_Loop;

		unsigned int m_CurFrame;
		float m_CurFrameTime;

		//std::map<unsigned int, std::list<std::function<void(Game_Actor* _Actor)>>> m_StartFunc;
	//	std::map<unsigned int, std::list<std::function<void(Game_Actor* _Actor)>>> m_EndFunc;

	public:
		//void CheckStartFunc(unsigned int _Frame)
		//{
		//	if (m_StartFunc.end() == m_StartFunc.find(_Frame))
		//	{
		//		return;
		//	}

		//	for (auto& _Var : m_StartFunc[_Frame])
		//	{
		//		_Var(Actor);
		//	}
		//}

		//void CheckEndFunc(unsigned int _Frame)
		//{
		//	if (m_EndFunc.end() == m_EndFunc.find(_Frame))
		//	{
		//		return;
		//	}

		//	for (auto& _Var : m_EndFunc[_Frame])
		//	{
		//		_Var(Actor);
		//	}
		//}


	public:
		void Reset() {
			m_CurFrame = m_Start;
			m_CurFrameTime = m_FrameTime;
		}
	};

private:
	std::map<Game_String, Game_Ptr<Game_2DFrameAnimation>> m_Ani;
	Game_Ptr<Game_2DFrameAnimation> m_CurAni;

public:
	bool CurAniEnd() {
		return m_CurAni->m_CurFrame == m_CurAni->m_End;
	}

	unsigned int CurAniFrame()
	{
		return m_CurAni->m_CurFrame;
	}

private:
	Game_Ptr<Game_2DFrameAnimation> FindAni(const Game_String& _AniName)
	{
		if (m_Ani.end() == m_Ani.find(_AniName))
		{
			return nullptr;
		}

		return m_Ani[_AniName];
	}

public:
	void CreateAni(
		const Game_String& _AniName,
		const Game_String& _SpriteName,
		unsigned int _Start,
		unsigned int _End,
		float _FrameTime,
		bool _Loop);


	void ChangeAni(const Game_String& _AniName);

};

