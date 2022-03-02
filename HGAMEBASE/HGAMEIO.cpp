#include "HGAMEIO.h"
#include <Windows.h>
#include <assert.h>
#include <iostream> 
#include <io.h> 
#include "HGAMEFILE.h"

//////////////////////////////////////////////////////////// File;

Game_String HGAMEFILE::Ext() const
{
	size_t Start = m_Path.m_Str.FindLast(L".");
	size_t End = m_Path.m_Str.StrCount();
	// 마지막 폴더 경로죠?
	return m_Path.m_Str.RangeToStr(Start, End);
}


//////////////////////////////////////////////////////////// Dir;
HGAMEDIRECTORY::HGAMEDIRECTORY()
{
	// 현재 실행되는 경오를 알려준다.
	// LP -> 포인터
	// STR -> 문자열
	// W -> 와이드 바이트 문자열
	// LPWSTR typedef

	WCHAR Str[256];
	// D:\AR28\AR28API\AR28API\HGAMEBASE\BIN\Debug\x64
	// D:\AR28\AR28API\AR28API\HGAMEBASE\HGAMECLIENT
	GetCurrentDirectory(256, Str);

	m_Path.m_Str = Str;


}
HGAMEDIRECTORY::~HGAMEDIRECTORY()
{

}

void HGAMEDIRECTORY::MoveParent()
{
	// 사실 내가 for문 돌려서 할수도 있는데
	// 기능 이용해서 만들겠다.
	// 함수 찾기 
	// 어떻게 바꾸느냐?
	// 경로의 구분은? \로 이루어져 있다.
	size_t Start = m_Path.m_Str.FindLast(L"\\");
	size_t End = m_Path.m_Str.StrCount();
	m_Path.m_Str.EraseStr(Start, End);
}

void HGAMEDIRECTORY::MoveParent(const Game_String& _Folder)
{
	// ~~~~까지
	// 반복문중 뭘 써야 합니까?

	while (true)
	{

		Game_String Folder = FolderName();

		size_t S1 = Folder.StrCount();
		size_t S2 = _Folder.StrCount();

		if (Folder != _Folder)
		{
			MoveParent();
		}
		else {
			break;
		}
	}
}

// 폴더 이름 알려주는 것
Game_String HGAMEDIRECTORY::FolderName()
{
	return IOName();
}

// 자신의 아래쪽에 어떤 경로가 있을때만 거기로 이동하는 기능이다.
void HGAMEDIRECTORY::Move(const Game_String& _Folder)
{
	Game_String Path = m_Path.m_Str;

	Path += L"\\" + _Folder;
	// 이 경로가 정말 존재하는 경로인지 확인해봐야 한다.
	if (false == HGAMEPATH::IsExits(Path))
	{
		// 그냥 터트린다.
		assert(false);
	}

	m_Path.m_Str = Path;
}

std::list<HGAMEFILE> HGAMEDIRECTORY::DirAllFile(const wchar_t* _Ext, const wchar_t* _Name)
{
	std::list<HGAMEFILE> AllFileList;

	int checkDirFile = 0;
	// + L"\\*.*" 무슨 기호냐?
	// C://*.*
	// C:// 드라이브의 * 모든 파일명 . * 모든 확장자를 찾아라.
	Game_String dirPath;

	Game_String Ext = _Ext;
	Game_String Name = _Name;

	if (Name != L"")
	{
		// c:\\AAA*
		dirPath = m_Path.m_Str + L"\\" + Name + L"*";
	}
	else {
		dirPath = m_Path.m_Str + L"\\*";
	}

	if (Ext != L"")
	{
		dirPath += L"." + Ext;
	}
	else {
		dirPath += L".*";
	}

	_wfinddata64i32_t fd;//디렉토리 내 파일 및 폴더 정보 저장 객체
	intptr_t handle;

	// 최초의 파일을 찾는다.
	// 파일도 핸들이다.
	if ((handle = _wfindfirst(dirPath, &fd)) == -1L) //fd 구조체 초기화.
	{
		return AllFileList;
	}

	do //폴더 탐색 반복 시작
	{
		// 요녀석이면 
		if (fd.attrib & _A_SUBDIR)
		{
			// 디렉토리다
			checkDirFile = 0;
		}
		else 
		{
			// 파일이다.
			checkDirFile = 1;
		}


		// 나는 디렉토리의 경우가 필요 없다.
		//if (checkDirFile == 0 && fd.name[0] != '.') {
		//	// cout << "Dir  : " << path << "\\" << fd.name << endl;
		//	// searchingDir(path + "\\" + fd.name);//재귀적 호출
		//}
		// 1이 파일일때만 

		if (fd.name[0] == L'.')
		{
			continue;
		}

		if (checkDirFile == 1) 
		{
			Game_String FilePath = m_Path.m_Str + L"\\" + fd.name;

			HGAMEFILE NewFile = HGAMEFILE(FilePath);

			AllFileList.push_back(NewFile);
		}

		// 다음 파일이 없으면 -1인가를 리턴할 것이다.
	} while (_wfindnext(handle, &fd) == 0);

	_findclose(handle);


	return AllFileList;
}

bool HGAMEDIRECTORY::IsFileExits(const Game_String& _FileName)
{
	return HGAMEPATH::IsExits(PlusFileName(_FileName));
}