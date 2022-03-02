#include "HMATERIAL.h"
#include <HGAMEDEBUG.h>

Game_Material::Game_Material() : m_AllCheck(true)
{
	m_ALLSHADER.resize((int)SHADERTYPE::SHADER_END);
}
Game_Material::~Game_Material() 
{

}

RENDERPATHTYPE Game_Material::PathType() 
{
	return PIXSHADER()->RPType();
}

void Game_Material::VTXSHADER(const Game_String& _Name) 
{
	m_VTXSHADER = HVTXSHADER::Find(_Name);

	if (nullptr == m_VTXSHADER)
	{
		AMSG(L"존재하지 않는 버텍스 쉐이더를 세팅하려고 했습니다." + _Name);
	}

	m_ALLSHADER[(int)SHADERTYPE::SHADER_VS] = m_VTXSHADER;

}
void Game_Material::PIXSHADER(const Game_String& _Name) 
{
	m_PIXSHADER = HPIXSHADER::Find(_Name);

	if (nullptr == m_PIXSHADER)
	{
		AMSG(L"존재하지 않는 픽셀 쉐이더를 세팅하려고 했습니다." + _Name);
	}

	m_ALLSHADER[(int)SHADERTYPE::SHADER_PS] = m_PIXSHADER;
}
void Game_Material::RASTERIZER(const Game_String& _Name) 
{
	m_RASTERIZER = HRASTERIZER::Find(_Name);

	if (nullptr == m_RASTERIZER)
	{
		AMSG(L"존재하지 않는 버텍스 쉐이더를 세팅하려고 했습니다.");
	}
}
void Game_Material::DEPTHSTENCIL(const Game_String& _Name) 
{
	m_DEPTHSTENCIL = HDEPTHSTENCIL::Find(_Name);

	if (nullptr == m_DEPTHSTENCIL)
	{
		AMSG(L"존재하지 않는 버텍스 쉐이더를 세팅하려고 했습니다.");
	}
}
void Game_Material::BLEND(const Game_String& _Name) 
{
	m_BLEND = HBLEND::Find(_Name);

	if (nullptr == m_BLEND)
	{
		AMSG(L"존재하지 않는 버텍스 쉐이더를 세팅하려고 했습니다.");
	}
}

void Game_Material::Setting() 
{
	if (true == m_AllCheck)
	{
		if (	nullptr == m_VTXSHADER ||
				nullptr == m_PIXSHADER ||
				nullptr == m_RASTERIZER ||
				nullptr == m_DEPTHSTENCIL ||
				nullptr == m_BLEND)
		{
			AMSG(L"세팅중 nullptr이 있습니다.");
		}
	}

	// 라스트 세팅이라는걸 만들어야 한다고 나중에 말해주세요.
	m_VTXSHADER->Setting();
	m_PIXSHADER->Setting();
	m_RASTERIZER->Setting();
	m_DEPTHSTENCIL->Setting();
	m_BLEND->Setting();
}