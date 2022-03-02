#include "HCOL.h"
#include "HGAMESCENE.h"
#include "HGAMEACTOR.h"

void Game_Collision::Init(int _Order /*= 0*/)
{
	PARENT(ACTOR()->TRANS());
	// 나자신이 트랜스 폼이니까

	// 이걸 가지고 있다.
	// m_TRANSDATA
	Order(_Order);
	ACTOR()->SCENE()->PushCol(this);
}

void Game_Collision::ColCheck(Game_Ptr<Game_Collision> _Other) 
{
	// 콜 Enter // 처음충돌했을때
	// 콜 exit // 충돌이 끝났을때
	// 콜 Stay // 계속 충돌중일때

	
	if (true == Col(m_Type, _Other.PTR, _Other.PTR->m_Type))
	{
	// 충돌 했다.
		if (m_ColSet.end() == m_ColSet.find(_Other.PTR))
		{
			m_ColSet.insert(_Other.PTR);
			_Other.PTR->m_ColSet.insert(this);

			CallEnter(_Other);
			_Other->CallEnter(this);
		}
		else 
		{
			CallStay(_Other);
			_Other->CallStay(this);
		}
		

	}
	else
	{
		if (m_ColSet.end() != m_ColSet.find(_Other.PTR))
		{
			// 충돌을 하지 않았는데 애가 내안에 있다.
			// 처음 충돌했다.

			CallExit(_Other);
			_Other->CallExit(this);

			m_ColSet.erase(_Other.PTR);
			_Other.PTR->m_ColSet.erase(this);
		}
	}

}

void Game_Collision::CallEnter(Game_Collision* _Other)
{
	for (auto& _Value : m_EnterFunc)
	{
		// this == Game_Collision* const
		// Game_Ptr<Game_Collision>
		_Value(this, _Other);
	}
}
void Game_Collision::CallStay(Game_Collision* _Other) 
{
	for (auto& _Value : m_StayFunc)
	{
		_Value(this, _Other);
	}
}
void Game_Collision::CallExit(Game_Collision* _Other)
{
	for (auto& _Value : m_ExitFunc)
	{
		_Value(this, _Other);
	}
}

void Game_Collision::Release()
{
	// 내부 B this == B

	//         A야
	for (auto& _OtherCol : m_ColSet)
	{

		// 알아서 자기가 생각기에 EXIT라면

		// A  ====== 야 나를 빼.   B
		_OtherCol->m_ColSet.erase(this);
	}

	
}