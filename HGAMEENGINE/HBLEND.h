#pragma once
#pragma once
#include "Game_Device.h"
#include <HMGR.h>

class HBLEND : public HMGR<HBLEND>
{
public:
	static void Create(const Game_String& _Name, const D3D11_BLEND_DESC& Desc)
	{
		CreateToInsert(_Name, Desc);
	}

private:
	D3D11_BLEND_DESC m_Desc;
	ID3D11BlendState* m_State;

public:
	void Create();
	void Setting();

public:
	HBLEND(const D3D11_BLEND_DESC& _Desc);
	~HBLEND();
};

