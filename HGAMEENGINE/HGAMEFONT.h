#pragma once
#include <HMGR.h>
#include "HDEVICEBUFFER.h"
#include <HPTR.h>
#include <FW1FontWrapper.h>

#pragma comment(lib, "FW1FontWrapper.lib")

class HGAMEFONT : public HMGR<HGAMEFONT>
{
public:
	static void Create(const Game_String& _Name)
	{
		Game_Ptr<HGAMEFONT> Res = CreateToInsert(_Name);
		Res->CreateGameFont(_Name);
	}

private:
	class HGAMEFONTSTART
	{
	public:
		~HGAMEFONTSTART();
	};
	friend HGAMEFONTSTART;
	static HGAMEFONTSTART Starter;

public:
	static void Init();

private:
	static IFW1Factory* m_pFontFactory;
	IFW1FontWrapper* m_pFontWrapper;

public:
	// 글자크기를 특정하고 매쉬를 만들수 없기 대문에.
	// 지오메트릭 쉐이더를 쓴다. 
	// 기본적으로 윈도우 좌표계를 기반으로 하는 함수를 쓸것이다. 
	void DrawFont(const Game_String& _Text, float _Size, Game_Vector _Pos, Game_Vector _Color, FW1_TEXT_FLAG _Flag = FW1_TOP);



public:
	HGAMEFONT();
	~HGAMEFONT();

public:
	void CreateGameFont(const Game_String& _Name);
};

