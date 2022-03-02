#pragma once
#include <Windows.h>
#include "HGAMEDEBUG.h"
#include <map>
#include <string>
#include "HGAMEMATH.h"
#include "HMGR.h"
#include "HPTR.h"

// �Լ������͸� TYPEDEF
//typedef int(__stdcall* DEADTIME)(); 
//typedef int(__stdcall* INITFUNC)();
// �ΰ����� ������ �Ȱ�����
// typedef �̸��� DEADTIME INITFUNC

typedef int(__stdcall* DEADTIME)();
typedef DEADTIME INITFUNC; // 

class HGAMEMOVIE;
class HGAMEWINDOW : public HFOBJMGR<HGAMEWINDOW>
{
	////////////////////////////////////////////////////// static
private:
	// ���α׷��ӵ��� Ʈ���̴�.
	// �޸𸮿����� Ư¡�� �̿��ؼ�
	// ���α׷��� �����ڸ��� Main���ٵ�
	// ���� ����Ǵ� �Լ��� ����� ������ ����Ҽ� �ִ� ����̴�.
	static void HRegDef();
	

	class HGAMEWINDOWSTART 
	{
	public:
		HGAMEWINDOWSTART() {
			HRegDef();
		}
	};
	// ����� ������ �����̴�.
	static HGAMEWINDOWSTART Starter;

	// �̳༮�� �ƹ��� �ǵ�� �ȵȴ�.
	static bool IsProcess;
	static LRESULT CALLBACK HWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	static void Create(const Game_String& _WindowName);
	static void Create(const Game_String& _WindowName, HWND _HWnd);


	// HGAMEWINDOW*�� �̸��ϳ� ���δ�.
public:
	static void HWINMESSAGE(INITFUNC _Init, DEADTIME _CallBack);

	////////////////////////////////////////////////////// member

private:
	HWND m_HWND;
	HDC m_HDC;
	int Style;
	Game_Vector m_Size;

	// ������ �ϳ��� ������ ������ ����Ҽ� �ִ°��̴�.
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

	// ������� ����°��� ���� �������� �ʴ´�.
public:
	HGAMEWINDOW(const Game_String& _Name);
	HGAMEWINDOW(HWND _HWnd);
	~HGAMEWINDOW();
};
