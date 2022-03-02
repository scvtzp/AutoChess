#pragma once
#include <iostream>
#include <assert.h>
#include "HGAMESTRING.h"

// Release������ ���� ����(� �������� ���� �˰� ����.)
// ������ _wassert�Լ��� ���� �Լ���� �ϴ°�.
#ifdef _DEBUG
#define AMSG(MSSAGE) _wassert(MSSAGE, _CRT_WIDE(__FILE__), (unsigned)(__LINE__))
#else
#define AMSG(MSSAGE) assert(false)
#endif

class HGAMEWINDOW;
class HGAMEDEBUG
{
	friend HGAMEWINDOW;

private:
	// �̳༮�� �������� ������.
	class HGAMEDEBUGSTART
	{
	public:
		void StartTest() {}

	public:
		HGAMEDEBUGSTART() 
		{
			// �ڵ����� ������ �� üũ�Ѵ�.
			// �ѹ��� ����ǵ��� �ߴ�.
			if (false == IsLeckCheck)
			{
				_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
				// _CrtSetBreakAlloc(349);
				IsLeckCheck = true;
			}
		}
		~HGAMEDEBUGSTART() 
		{
			if (true == bCon)
			{
				FreeConsole();
			}

			if (nullptr != stream)
			{
				fclose(stream);
			}
		}
	};

	//static void AssertMsg(const Game_String& _Msg) 
	//{
	//	_wassert(_Msg, _crt_wide()
	//}

	// ����� ������ �����̴�.
	static HGAMEDEBUGSTART Starter;
	static bool IsLeckCheck;
	// �Լ��� ����ϸ� �� Ŭ������ �ʿ������ϱ�.
	static void LeckCheck() 
	{
		Starter.StartTest();
	}

	static int m_FrameCount;
public:
	static bool bCon;
	static FILE* stream;
	// static Game_Ptr<HGAMEFONT> m_DebugFont;

	static int FrameCount() 
	{
		return m_FrameCount;
	}
	static void DebugFrameUpdate()
	{
		++m_FrameCount;
	}

public:

	static void OpenConsole()
	{
		if (0 == AllocConsole())
		{
			AMSG(L"�ܼ�â�� ���µ� �����߽��ϴ�.");
		}
		else 
		{
			freopen_s(&stream, "CONIN$", "rb", stdin);
			freopen_s(&stream, "CONOUT$", "wb", stdout);
			freopen_s(&stream, "CONOUT$", "wb", stderr);
			// GetLastError

			// wcout�� �ѱ��̶�� ���� �˷��ش�.
			std::wcout.imbue(std::locale("korean"));
			std::wcout << L"�� ��µǴ��� ���ڽ��ϴ�." << std::endl;
			bCon = true;
		}
	}

	template<typename ... REST>
	static void ConsolePrintText(const Game_String& _Text, REST ... _Arg)
	{
		if (false == bCon)
		{
			AMSG(L"�ܼ�â�� ������ �ʾҽ��ϴ�.");
		}

		wchar_t ArrText[256];
		swprintf_s(ArrText, _Text, _Arg...);
		std::wcout << ArrText << std::endl;
	}


};