#pragma once
#include <Windows.h>
#include "HGAMEDEBUG.h"
#include <map>
#include <string>
#include "HGAMEMATH.h"
#include "HMGR.h"
#include "HPTR.h"

// 함수포인터를 TYPEDEF
//typedef int(__stdcall* DEADTIME)(); 
//typedef int(__stdcall* INITFUNC)();
// 두가지는 형식은 똑같은데
// typedef 이름만 DEADTIME INITFUNC

typedef int(__stdcall* DEADTIME)();
typedef DEADTIME INITFUNC; // 

class HGAMEMOVIE;
class HGAMEWINDOW : public HFOBJMGR<HGAMEWINDOW>
{
	////////////////////////////////////////////////////// static
private:
	// 프로그래머들의 트릭이다.
	// 메모리영역의 특징을 이용해서
	// 프로그램이 켜지자마자 Main보다도
	// 먼저 실행되는 함수를 만들고 싶을때 사용할수 있는 방식이다.
	static void HRegDef();
	

	class HGAMEWINDOWSTART 
	{
	public:
		HGAMEWINDOWSTART() {
			HRegDef();
		}
	};
	// 배웠던 문법의 응용이다.
	static HGAMEWINDOWSTART Starter;

	// 이녀석은 아무나 건들면 안된다.
	static bool IsProcess;
	static LRESULT CALLBACK HWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	static void Create(const Game_String& _WindowName);
	static void Create(const Game_String& _WindowName, HWND _HWnd);


	// HGAMEWINDOW*과 이름하나 엮인다.
public:
	static void HWINMESSAGE(INITFUNC _Init, DEADTIME _CallBack);

	////////////////////////////////////////////////////// member

private:
	HWND m_HWND;
	HDC m_HDC;
	int Style;
	Game_Vector m_Size;

	// 윈도우 하나당 누구나 영상을 재생할수 있는것이다.
	HGAMEMOVIE* MoviePlayer;

public:
	HGAMEMOVIE* GetMoviePlayer() 
	{
		return MoviePlayer;
	}

public:
	HWND WINHWND() {
		return m_HWND;
	}

	HDC WINHDC() {
		return m_HDC;
	}

	Game_Vector Size() {
		return m_Size;
	}

	Game_Vector HSize() {
		return m_Size.HalfVector();
	}

	Game_Vector MousePos();
	HMATRIX ViewPort();

	Game_Vector MousePosTo3D();

	void Size(const Game_Vector& _Size);
	Game_Vector WindowPosTo3DPos(Game_Vector _WindowPos);

	// 마음대로 만드는것을 별로 좋아하지 않는다.
public:
	HGAMEWINDOW(const Game_String& _Name);
	HGAMEWINDOW(HWND _HWnd);
	~HGAMEWINDOW();
};
