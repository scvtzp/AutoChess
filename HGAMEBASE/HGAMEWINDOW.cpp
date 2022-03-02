#include "HGAMEWINDOW.h"
#include <Windows.h>
#include <assert.h>
#include "HGAMESOUND.h"
#include "HGAMEMOVIE.h"

HGAMEWINDOW::HGAMEWINDOWSTART HGAMEWINDOW::Starter = HGAMEWINDOW::HGAMEWINDOWSTART();

bool HGAMEWINDOW::IsProcess = true;

void CALLBACK OnGraphEvent(HWND hwnd, long evCode, LONG_PTR param1, LONG_PTR param2)
{
	switch (evCode)
	{
	case EC_COMPLETE:
	case EC_USERABORT:
		{
			wchar_t ArrName[256];
			GetWindowText(hwnd, ArrName, 256);
			Game_Ptr<HGAMEWINDOW> PTR = HGAMEWINDOW::Find(ArrName);
			PTR->GetMoviePlayer()->MovieStop();
			// g_pPlayer->Stop();
			break;
		}
	case EC_ERRORABORT:
		// NotifyError(hwnd, L"Playback error.");
		{
			AMSG(L"영상 재생에 문제가 있습니다.");
			wchar_t ArrName[256];
			GetWindowText(hwnd, ArrName, 256);
			Game_Ptr<HGAMEWINDOW> PTR = HGAMEWINDOW::Find(ArrName);
			PTR->GetMoviePlayer()->MovieStop();
			break;
		}
	}
}

LRESULT CALLBACK HGAMEWINDOW::HWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_PAINT:
	{
		// 윈도우에 등록되지 않았을때도 한번 들어옵니다.

		wchar_t ArrName[256];
		GetWindowText(hWnd, ArrName, 256);
		Game_Ptr<HGAMEWINDOW> PTR = HGAMEWINDOW::Find(ArrName);

		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);

		if (nullptr != PTR)
		{
			if (PTR->MoviePlayer->State() != STATE_NO_GRAPH && PTR->MoviePlayer->HasVideo())
			{
				RECT rc;
				GetClientRect(hWnd, &rc);

				PTR->MoviePlayer->UpdateVideoWindow(&rc);

				// 이게 아마 동영상을 그려주는애이다.
				// The player has video, so ask the player to repaint. 
				PTR->MoviePlayer->Repaint(hdc);
			}
			else
			{
				FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			}
		}

		EndPaint(hWnd, &ps);

	}
	break;
	case WM_SIZE:
	{
		wchar_t ArrName[256];
		GetWindowText(hWnd, ArrName, 256);
		Game_Ptr<HGAMEWINDOW> PTR = HGAMEWINDOW::Find(ArrName);
		if (nullptr != PTR)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);

			PTR->MoviePlayer->UpdateVideoWindow(&rc);

		}
		return 0;
	}

	case WM_CREATE:
	{
		wchar_t ArrName[256] = L"";
		//// mfc라서 HWndProc가 실행이 안됩니다.
		GetWindowText(hWnd, ArrName, 256);
		break;
	}
	case WM_DESTROY:
		// 현재 실행되는 메세지함수가 0을 리턴하게 하는 함수다.
		IsProcess = false;
		break;
	case WM_DISPLAYCHANGE:
		// 무비플레이어만 사용할수 있는 함수다.
		// 무비플레이어는 누가 들고 있죠?
		// 진짜 만들어진 무비플레이어는 누가 들고 있습니까?
		// 윈도우가 들고 있다.
		// 나를 실행한 윈도우를 알아내야 한다.
		// 여기는 전역함수라 알수가 없거든.
		// 윈도우를 하나 띄운다고 하면 쉬워지지만
		// 안그럴꺼다.
	{
		wchar_t ArrName[256];
		GetWindowText(hWnd, ArrName, 256);
		Game_Ptr<HGAMEWINDOW> PTR = HGAMEWINDOW::Find(ArrName);
		PTR->MoviePlayer->DisplayModeChanged();
		break;
	}
	case WM_GRAPH_EVENT:
	{
		wchar_t ArrName[256];
		GetWindowTextW(hWnd, ArrName, 256);
		Game_Ptr<HGAMEWINDOW> PTR = HGAMEWINDOW::Find(ArrName);
		PTR->MoviePlayer->HandleGraphEvent(OnGraphEvent);
		// g_pPlayer->HandleGraphEvent(OnGraphEvent);
		return 0;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void HGAMEWINDOW::HRegDef()
{
	HGAMEDEBUG::LeckCheck();

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &HGAMEWINDOW::HWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = nullptr;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"DEF";
	wcex.hIconSm = nullptr;

	RegisterClassExW(&wcex);
}


void HGAMEWINDOW::HWINMESSAGE(INITFUNC _Init, DEADTIME _CallBack)
{
	if (nullptr != _Init)
	{
		_Init();
	}

	MSG msg;
	while (true == IsProcess)
	{
		if (0 != PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (nullptr != _CallBack)
			{
				// HGAMESOUND::Update();
				HGAMEDEBUG::DebugFrameUpdate();
				_CallBack();
			}
		}
	}
}

void HGAMEWINDOW::Create(const Game_String& _WindowName)
{
	Game_Ptr<HGAMEWINDOW> NewWindow = CreateToInsert(_WindowName, _WindowName);
}

void HGAMEWINDOW::Create(const Game_String& _WindowName, HWND _HWnd)
{
	Game_Ptr<HGAMEWINDOW> NewWindow = CreateToInsert(_WindowName, _HWnd);
}

HGAMEWINDOW::HGAMEWINDOW(const Game_String& _Name) : MoviePlayer(nullptr)
{
	// 만들어지는 부분에서 m_HWND이 있다.
	// 윈도우 핸들이 있어야.
	// HDC를 얻어올수 있다.

	// 어떤 스타일로 만들어졌는지 WS_OVERLAPPEDWINDOW

	Style = WS_OVERLAPPEDWINDOW;
	// 이러한 한번만 된다.
	m_HWND = CreateWindowW(L"DEF", _Name, Style,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, nullptr, nullptr);

	if (!m_HWND)
	{
		assert(false);
	}

	// 윈도우에 무언가를 그릴수 있는 권한이므로
	// 윈도우를 통해서 얻어올수 있다.
	m_HDC = GetDC(m_HWND);

	ShowWindow(m_HWND, SW_SHOW);
	UpdateWindow(m_HWND);

	// HGAMEMOVIE 재생하려면 
	MoviePlayer = new HGAMEMOVIE(m_HWND);
}

HGAMEWINDOW::HGAMEWINDOW(HWND _HWnd) : Style(0), MoviePlayer(nullptr)
{
	m_HWND = _HWnd;

	if (!m_HWND)
	{
		assert(false);
	}

	m_HDC = GetDC(m_HWND);
	UpdateWindow(m_HWND);
	MoviePlayer = new HGAMEMOVIE(m_HWND);
}

HGAMEWINDOW::~HGAMEWINDOW()
{
	if (nullptr != MoviePlayer)
	{
		delete MoviePlayer;
	}
}


void HGAMEWINDOW::Size(const Game_Vector& _Size)
{
	// 윈도우 바와 좌우에도 바가 있는데 그 사이즈가 무시된 것이다.
	// 그걸 고려하도록 짜야 한다.

	// 현재 윈도우의 크기를 얻어온다.
	// GetClientRect(m_HWND, )

	// 800 600
	// 804 620 => 요거 바가 합쳐진 사이즈를 나에게 주니까
	// 기존 윈도우의 크기에서 바를 포함시킨 사이즈로 알아온다.

	m_Size = _Size;

	RECT Rect = { 0, 0, _Size.IX(), _Size.IY() };

	// Rect들어갔다 나오면 수정될수 있는것.
	AdjustWindowRect(&Rect, Style, FALSE);

	SetWindowPos(m_HWND, nullptr, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top, SWP_NOMOVE | SWP_NOZORDER);
}

HMATRIX HGAMEWINDOW::ViewPort()
{
	//m_VIEWPORT = { 0, };
	//// 윈도우 좌표계 기반이다.
	//m_VIEWPORT.TopLeftX = 0.0f;
	//m_VIEWPORT.TopLeftY = 0.0f;
	//m_VIEWPORT.Width = m_WINDOW->Size().X;
	//m_VIEWPORT.Height = m_WINDOW->Size().Y;
	//m_VIEWPORT.MinDepth = 0.0f;
	//m_VIEWPORT.MaxDepth = 1.0f;

	HMATRIX Mat;

	Mat.VIEWPORT(0.0f, 0.0f, Size().x, Size().y, 0.0f, 1.0f);

	return Mat;

	// Mat.VIEWPORT()

	 //Size().X;
}

// 윈도우 마다 다르다. 그러므로 윈도우에 구현해야 한다.
Game_Vector HGAMEWINDOW::MousePos()
{
	// 커서의 위치
	// 상식적으로 이름이 다 되어있다.
	// LP가 붙어있으면 LP이름
	POINT P;
	// 모니터에서의 마우스 위치
	GetCursorPos(&P);
	// m_HWND 윈도우 기준으로 
	ScreenToClient(m_HWND, &P);

	return { (float)P.x, (float)P.y };
}

// SOLID 객체지향의 다섯가지 권장 설계중
// 인터페이스 분리원칙
// 자잘한함수를 만들고 그것들을 조합해서 새로운 기능을 만들어라.

Game_Vector HGAMEWINDOW::MousePosTo3D()
{
	Game_Vector Pos = MousePos();

	Pos.x -= m_Size.HX();
	Pos.y -= m_Size.HY();
	Pos.y *= -1.0f;

	return Pos;
}

Game_Vector HGAMEWINDOW::WindowPosTo3DPos(Game_Vector _WindowPos)
{
	return Game_Vector::WindowPosTo3DPos(_WindowPos, m_Size);
}