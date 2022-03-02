#pragma once
#include <iostream>
#include <assert.h>
#include "HGAMESTRING.h"

// Release에서느 설정 변경(어떤 설정인지 내가 알게 뭐야.)
// 때문에 _wassert함수가 없는 함수라고 하는것.
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
	// 이녀석을 쓰는일이 없었다.
	class HGAMEDEBUGSTART
	{
	public:
		void StartTest() {}

	public:
		HGAMEDEBUGSTART() 
		{
			// 자동으로 무조건 릭 체크한다.
			// 한번만 실행되도록 했다.
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

	// 배웠던 문법의 응용이다.
	static HGAMEDEBUGSTART Starter;
	static bool IsLeckCheck;
	// 함수를 사용하면 이 클래스가 필요해지니까.
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
			AMSG(L"콘솔창을 여는데 실패했습니다.");
		}
		else 
		{
			freopen_s(&stream, "CONIN$", "rb", stdin);
			freopen_s(&stream, "CONOUT$", "wb", stdout);
			freopen_s(&stream, "CONOUT$", "wb", stderr);
			// GetLastError

			// wcout에 한국이라고 직접 알려준다.
			std::wcout.imbue(std::locale("korean"));
			std::wcout << L"잘 출력되는지 보겠습니다." << std::endl;
			bCon = true;
		}
	}

	template<typename ... REST>
	static void ConsolePrintText(const Game_String& _Text, REST ... _Arg)
	{
		if (false == bCon)
		{
			AMSG(L"콘솔창이 열리지 않았습니다.");
		}

		wchar_t ArrText[256];
		swprintf_s(ArrText, _Text, _Arg...);
		std::wcout << ArrText << std::endl;
	}


};