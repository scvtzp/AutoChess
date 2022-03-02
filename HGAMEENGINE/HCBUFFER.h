#pragma once
#include <vector>
#include <HMGR.h>
#include "HDEVICEBUFFER.h"
#include <HPTR.h>
#include "HSHADER.h"

enum class CBUFFERMODE
{
	CB_LINK, // CPU에 있는 메모리와 링크를 건다.
	CB_NEW,
	CB_NONE,
};

class HCBUFFER : public HMGR<HCBUFFER>, public HDEVICEBUFFER
{
public:
	template<typename T>
	static void Create(const Game_String& _Name/*, D3D11_USAGE _USAGE = D3D11_USAGE::D3D11_USAGE_DYNAMIC*/)
	{
		Game_Ptr<HCBUFFER> Res = CreateToInsert(_Name);
		Res->Create(sizeof(T));
	}

private:
	D3D11_MAPPED_SUBRESOURCE m_SubData;

public:
	void Create(unsigned int _Size);

	void Setting(SHADERTYPE _Type, unsigned int _Reg);

	template<typename T>
	void DataChange(const T& _Data)
	{
		DataChange((void*)&_Data, sizeof(T));
	}

	void DataChange(void* _Data, unsigned int _Size);
};