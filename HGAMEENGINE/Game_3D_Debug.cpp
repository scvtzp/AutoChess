#include "Game_3D_Debug.h"
#include "HTEXTURE.h"
#include "HRENDERER.h"
#include "HRenderTarget.h"

std::vector<Game_3D_Debug::DebugTextInfo> Game_3D_Debug::m_AllDebugText;
Game_Ptr<HGAMEFONT> Game_3D_Debug::m_DebugFont;

float Game_3D_Debug::m_BasicFontSize = 20.0f;
Game_Vector Game_3D_Debug::m_BasicStartPos = Game_Vector(0.0F, 0.0F, 0.0F, 0.0F);
Game_Vector Game_3D_Debug::m_CurTextSize = Game_Vector(0.0F, 0.0F, 0.0F, 0.0F);
Game_Vector Game_3D_Debug::m_BasicColor = Game_Vector::RED;
Game_Ptr<HRENDERPLAYER> Game_3D_Debug::m_TargetDebugTextureRenderPlayer;
Game_Ptr<HRENDERPLAYER> Game_3D_Debug::m_TargetDebugTextureBackRenderPlayer;
std::vector<Game_3D_Debug::DebugTextureInfo> Game_3D_Debug::m_AllDebugTexture;
HMATRIX Game_3D_Debug::m_VPMAT;


void Game_3D_Debug::Init(bool _IsOpenConsole)
{
	HMATRIX P;
	HMATRIX V;
	V.VIEWLH(Game_Vector(0.0F,0.0F, -10.0F), Game_Vector::FORWARD, Game_Vector::UP);
	P.ORTHLH(Game_Device::MAINOBJ()->BackBufferRenderTarget()->Texture(0)->Size(), 0.1f, 1000.0f);
	m_VPMAT = V * P;


	HGAMEFONT::Init();

	m_DebugFont = new HGAMEFONT();
	m_DebugFont->CreateGameFont(L"±¼¸²");

	m_TargetDebugTextureRenderPlayer = new HRENDERPLAYER();

	m_TargetDebugTextureRenderPlayer->MESH(L"TargetDebugRect");
	m_TargetDebugTextureRenderPlayer->MATERIAL(L"TARGETDEBUG");
	m_TargetDebugTextureRenderPlayer->SAMPLER(L"Smp", L"LWSMP");

	m_TargetDebugTextureBackRenderPlayer = new HRENDERPLAYER();

	m_TargetDebugTextureBackRenderPlayer->MESH(L"TargetDebugRect");
	m_TargetDebugTextureBackRenderPlayer->MATERIAL(L"TARGETDEBUGBACK");

	

	m_AllDebugText.reserve(100);
	m_AllDebugTexture.reserve(100);

	if (true == _IsOpenConsole)
	{
		OpenConsole();
	}
}


void Game_3D_Debug::DrawDebugText(const Game_String& _Text, float _Size, Game_Vector _Pos, Game_Vector _Color) 
{
	m_AllDebugText.push_back({ _Text, _Size, _Pos, _Color });
}

void Game_3D_Debug::DrawDebugTexture(const Game_Ptr<HTEXTURE>& _Texture, Game_Vector _Size, Game_Vector _Pos, Game_Vector _BackColor)
{
	HMATRIX MAT;
	_Size.z = 1.0F;

	_Pos.x += _Size.HX();
	_Pos.y -= _Size.HY();

	MAT.World(_Size, {0.0f, 0.0f, 0.0f, 1.0f}, _Pos);
	MAT = MAT * m_VPMAT;
	m_AllDebugTexture.push_back({ _Texture, _BackColor, MAT});
}


void Game_3D_Debug::DebugRender() 
{
	for (size_t i = 0; i < m_AllDebugTexture.size(); i++)
	{
		m_TargetDebugTextureBackRenderPlayer->CBUFFER(L"WVPMATRIX", &m_AllDebugTexture[i].WVP, CBUFFERMODE::CB_LINK);
		m_TargetDebugTextureBackRenderPlayer->CBUFFER(L"BACKCOLOR", &m_AllDebugTexture[i].BackColor, CBUFFERMODE::CB_LINK);
		m_TargetDebugTextureBackRenderPlayer->Render();
		m_TargetDebugTextureBackRenderPlayer->Reset();

		m_TargetDebugTextureRenderPlayer->TEXTURE(L"Tex", m_AllDebugTexture[i].m_Tex);
		m_TargetDebugTextureRenderPlayer->CBUFFER(L"WVPMATRIX", &m_AllDebugTexture[i].WVP, CBUFFERMODE::CB_LINK);
		
		m_TargetDebugTextureRenderPlayer->Render();
		m_TargetDebugTextureRenderPlayer->Reset();

	}

	for (size_t i = 0; i < m_AllDebugText.size(); i++)
	{
		m_DebugFont->DrawFont(m_AllDebugText[i].Text, m_AllDebugText[i].Size, m_AllDebugText[i].Pos, m_AllDebugText[i].Color);
	}


	m_CurTextSize = m_BasicStartPos;

	m_AllDebugText.clear();
	m_AllDebugTexture.clear();
	Game_Device::Reset();
}