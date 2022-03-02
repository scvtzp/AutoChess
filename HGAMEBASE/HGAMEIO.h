#pragma once
#include "HGAMESTRING.h"
#include <list>
// �����
// ���������
// ���ڿ� 
// ���� ����°� ���ڿ��� ���� ���϶��� ����.

// ��ο� �ʿ��� ��ɵ��� ����ϴ� Ŭ����

// �ε��Ҷ� �� ��� ã�ƾ� �Ұ� �ƴѰ�?
// �׷��� ������ ��ĥ�ϴ°��� �׻� ����?
// �޶����� �׶��׶� ������ ��θ� ã�ư��� �Ѵ�.
// D:\AR28\AR28API\AR28API\HGAMEBASE\BIN\RES
class HGAMEPATH
{
public:
	Game_String m_Str;

public:
	// �� ��ΰ� �����ϴ��� ���ϴ���?
	static bool IsExits(const Game_String& _Str)
	{
		// 0 �������� �ִ��� ������?
		// 2 �� ������ �б� �����̳� �ƴϳ�?
		if (0 == _waccess_s(_Str, 0))
		{
			return true;
		}

		return false;
	}

public:
	HGAMEPATH() {

	}


	HGAMEPATH(const wchar_t* _Path)  : m_Str(_Path)
	{

	}

	HGAMEPATH(const Game_String& _Path) : m_Str(_Path)
	{

	}
	~HGAMEPATH() {

	}
};

class HGAMEIO 
{
protected:
	HGAMEPATH m_Path;

public:
	Game_String PlusFileName(const Game_String& _Path)
	{
		return m_Path.m_Str + L"\\" + _Path;
	}

	static Game_String FileName(const Game_String& _Path) 
	{
		size_t Start1 = _Path.FindLast(L"\\");

		if (Start1 == std::string::npos)
		{
			Start1 = 0;
		}

		size_t Start2 = _Path.FindLast(L"/");

		if (Start2 == std::string::npos)
		{
			Start2 = 0;
		}

		size_t Start = std::string::npos;

		if (Start1 > Start2)
		{
			Start = Start1;
		}
		else {
			Start = Start2;
		}

		if (Start == std::string::npos)
		{
			Start = 0;
		}
		size_t End = _Path.StrCount();
		// ������ ���� �����?
		Game_String Name = _Path.RangeToStr(Start, End);
		return Name;
	}


	const wchar_t* PCONSTWCHAR() 
	{
		return m_Path.m_Str;
	}

	Game_String IOName() const
	{
		return HGAMEIO::FileName(m_Path.m_Str);
	}

	Game_String DirPath() const
	{
		// "c::aaa.text"
		// "aaa.text"

		Game_String Path = m_Path.m_Str;
		Path.EraseStr(HGAMEIO::FileName(m_Path.m_Str));
		return Path;
	}

	void SetPath(const Game_String& _Path)
	{
		m_Path = _Path;
	}

	HGAMEIO() {

	}
};


// �Ѵ� ��θ� �� ��� �ִ�.
// ���丮
class HGAMEFILE;
class HGAMEDIRECTORY : public HGAMEIO
{
public:
	void MoveParent();
	// ���̸��� ������ ���������� �ö󰡶�.
	void MoveParent(const Game_String& _Folder);
	void Move(const Game_String& _Folder);

	std::list<HGAMEFILE> DirAllFile(const wchar_t* _Ext = L"", const wchar_t* _Name = L"");


	Game_String FolderName();

	bool IsFileExits(const Game_String& _FileName);

	// �̳༮�� �����Ǹ�
	// ������ m_Path�� ���� ����Ǵ� ��θ� �ڽ��� ��η� ��� �����.
public:
	HGAMEDIRECTORY();
	~HGAMEDIRECTORY();
};
