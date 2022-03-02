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
			AMSG(L"���� ����� ������ �ֽ��ϴ�.");
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
		// �����쿡 ��ϵ��� �ʾ������� �ѹ� ���ɴϴ�.

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

				// �̰� �Ƹ� �������� �׷��ִ¾��̴�.
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
		//// mfc�� HWndProc�� ������ �ȵ˴ϴ�.
		GetWindowText(hWnd, ArrName, 256);
		break;
	}
	case WM_DESTROY:
		// ���� ����Ǵ� �޼����Լ��� 0�� �����ϰ� �ϴ� �Լ���.
		IsProcess = false;
		break;
	case WM_DISPLAYCHANGE:
		// �����÷��̾ ����Ҽ� �ִ� �Լ���.
		// �����÷��̾�� ���� ��� ����?
		// ��¥ ������� �����÷��̾�� ���� ��� �ֽ��ϱ�?
		// �����찡 ��� �ִ�.
		// ���� ������ �����츦 �˾Ƴ��� �Ѵ�.
		// ����� �����Լ��� �˼��� ���ŵ�.
		// �����츦 �ϳ� ���ٰ� �ϸ� ����������
		// �ȱ׷�����.
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
	// ��������� �κп��� m_HWND�� �ִ�.
	// ������ �ڵ��� �־��.
	// HDC�� ���ü� �ִ�.

	// � ��Ÿ�Ϸ� ����������� WS_OVERLAPPEDWINDOW

	Style = WS_OVERLAPPEDWINDOW;
	// �̷��� �ѹ��� �ȴ�.
	m_HWND = CreateWindowW(L"DEF", _Name, Style,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, nullptr, nullptr);

	if (!m_HWND)
	{
		assert(false);
	}

	// �����쿡 ���𰡸� �׸��� �ִ� �����̹Ƿ�
	// �����츦 ���ؼ� ���ü� �ִ�.
	m_HDC = GetDC(m_HWND);

	ShowWindow(m_HWND, SW_SHOW);
	UpdateWindow(m_HWND);

	// HGAMEMOVIE ����Ϸ��� 
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
	// ������ �ٿ� �¿쿡�� �ٰ� �ִµ� �� ����� ���õ� ���̴�.
	// �װ� ����ϵ��� ¥�� �Ѵ�.

	// ���� �������� ũ�⸦ ���´�.
	// GetClientRect(m_HWND, )

	// 800 600
	// 804 620 => ��� �ٰ� ������ ����� ������ �ִϱ�
	// ���� �������� ũ�⿡�� �ٸ� ���Խ�Ų ������� �˾ƿ´�.

	m_Size = _Size;

	RECT Rect = { 0, 0, _Size.IX(), _Size.IY() };

	// Rect���� ������ �����ɼ� �ִ°�.
	AdjustWindowRect(&Rect, Style, FALSE);

	SetWindowPos(m_HWND, nullptr, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top, SWP_NOMOVE | SWP_NOZORDER);
}

HMATRIX HGAMEWINDOW::ViewPort()
{
	//m_VIEWPORT = { 0, };
	//// ������ ��ǥ�� ����̴�.
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

// ������ ���� �ٸ���. �׷��Ƿ� �����쿡 �����ؾ� �Ѵ�.
Game_Vector HGAMEWINDOW::MousePos()
{
	// Ŀ���� ��ġ
	// ��������� �̸��� �� �Ǿ��ִ�.
	// LP�� �پ������� LP�̸�
	POINT P;
	// ����Ϳ����� ���콺 ��ġ
	GetCursorPos(&P);
	// m_HWND ������ �������� 
	ScreenToClient(m_HWND, &P);

	return { (float)P.x, (float)P.y };
}

// SOLID ��ü������ �ټ����� ���� ������
// �������̽� �и���Ģ
// �������Լ��� ����� �װ͵��� �����ؼ� ���ο� ����� ������.

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