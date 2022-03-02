#pragma once
#include "Game_Device.h"
#include <HMGR.h>

class HRASTERIZER : public HMGR<HRASTERIZER>
{
public:
	static void Create(const Game_String& _Name, const D3D11_RASTERIZER_DESC& Desc)
	{
		CreateToInsert(_Name, Desc);
	}

private:
	D3D11_RASTERIZER_DESC m_Desc;
	ID3D11RasterizerState* m_State;

public:
	void Create();
	void Setting();

public:
	HRASTERIZER(const D3D11_RASTERIZER_DESC& _Desc);
	~HRASTERIZER();

};

