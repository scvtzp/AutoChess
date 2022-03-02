#pragma once
#include "HSHADER.h"
#include <HMGR.h>

class HSAMPLER : public HMGR<HSAMPLER>
{
public:
	static void Create(const Game_String& _Name, const D3D11_SAMPLER_DESC& Desc)
	{
		CreateToInsert(_Name, Desc);
	}

public:
	D3D11_SAMPLER_DESC m_Desc;
	ID3D11SamplerState* m_State;

public:
	void Create();
	void Setting(SHADERTYPE _Type, unsigned int _Reg);

public:
	HSAMPLER(const D3D11_SAMPLER_DESC& _Desc);
	~HSAMPLER();
};

