#pragma once
// 점 RECT
// 집합 다수 => 자료구조
#include <vector>
#include <HMGR.h>
#include "HDEVICEBUFFER.h"
#include <HPTR.h>
// 포지션



class HVBBUFFER : public HMGR<HVBBUFFER>, public HDEVICEBUFFER
{
public:
	template<typename T>
	static void Create(const Game_String& _Name, std::vector<T> _Vtx, D3D11_USAGE _USAGE = D3D11_USAGE::D3D11_USAGE_DEFAULT)
	{
		Game_Ptr<HVBBUFFER> Res = CreateToInsert(_Name);
		Res->Create(_Vtx.size(), sizeof(T), &_Vtx[0], _USAGE);
	}

public:
	unsigned int m_Size;
	unsigned int m_Count;
	unsigned int m_Offset;

	// size_t Count 점의 개수
	// size_t Size
	// const void* _Data 
	// D3D11_USAGE _USAGE 메모리가 어디 들어갈거냐?
	void Create(size_t _Count, size_t _Size, const void* _Data, D3D11_USAGE _USAGE = D3D11_USAGE::D3D11_USAGE_DEFAULT);

public:
	void Setting();

public:
	HVBBUFFER();
};

