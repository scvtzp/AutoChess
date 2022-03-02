#pragma once
#include <HGAMESTRING.h>
#include "HGAMEFONT.h"
#include <vector>
#include <HGAMEDEBUG.h>

class HTEXTURE;
class HRENDERPLAYER;
class Game_3D_Debug
{
private:
	static Game_Ptr<HGAMEFONT> m_DebugFont;
	static float m_BasicFontSize;
	static Game_Vector m_BasicStartPos;
	static Game_Vector m_CurTextSize;
	static Game_Vector m_BasicColor;
	static Game_Ptr<HRENDERPLAYER> m_TargetDebugTextureRenderPlayer;
	static Game_Ptr<HRENDERPLAYER> m_TargetDebugTextureBackRenderPlayer;
	static HMATRIX m_VPMAT;

public:
	static void Init(bool _IsOpenConsole = false);

private:
	class DebugTextInfo 
	{
	public:
		Game_String Text;
		float Size;
		Game_Vector Pos;
		Game_Vector Color;
		
	};

	class DebugTextureInfo
	{
	public:
		Game_Ptr<HTEXTURE> m_Tex;
		Game_Vector BackColor;
		HMATRIX WVP;
	};

	static std::vector<DebugTextInfo> m_AllDebugText;
	static std::vector<DebugTextureInfo> m_AllDebugTexture;



public:

	static void DrawDebugText(const Game_String& _Text, float _Size, Game_Vector _Pos, Game_Vector _Color);

	static void DrawDebugTexture(const Game_Ptr<HTEXTURE>& _Texture, Game_Vector _Size, Game_Vector _Pos, Game_Vector _BackColor);

	template<typename ... REST>
	static void DrawDebugText(const Game_String& _Text, REST ... _Arg)
	{
		wchar_t ArrText[256];
		swprintf_s(ArrText, _Text, _Arg...);
		Game_3D_Debug::DrawDebugText(ArrText, m_BasicFontSize, m_CurTextSize, m_BasicColor);
		m_CurTextSize.y += m_BasicFontSize;
	}

	template<typename ... REST>
	static void ConsolePrintText(const Game_String& _Text, REST ... _Arg)
	{
		HGAMEDEBUG::ConsolePrintText(_Text, _Arg...);
	}

	// HGAMEDEBUG BASE FUNC
public:

	static void OpenConsole() 
	{
		HGAMEDEBUG::OpenConsole();
	}

public:
	static void DebugRender();

};