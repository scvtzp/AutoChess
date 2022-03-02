#pragma once
#include <HGAMEMATH.h>
#include <vector>
#include "HRENDERER.h"
#include <map>

//class NaviTri 
//{
//public:
//	Game_Vector Arr[3];
//	std::vector<NaviTri*> m_Tri;
//
//};

struct MAPOPTION
{
	float XCount;
	float ZCount;
	float YRatio;
	float YScale;
	Game_Vector PixelUv;
	Game_Vector PixelScale;
};

struct YTri 
{
	Game_Vector Arr[3];
};

struct YTile
{
	YTri Up;
	YTri Down;
};

class HHeightMap : public HACTORCOM
{
private:
	int X;
	int Z;

	MAPOPTION MapOption;

	// std::vector<NaviTri> m_Tri;
	Game_Ptr<HMESH> m_Mesh;
	Game_Ptr<Game_Renderer> m_Render;
	Game_Ptr<HRENDERPLAYER> m_RenderPlayer;
	std::vector<VTX3DMESHIN> m_VecVtx;
	std::map<__int64, YTile> m_YTileMap;
	Game_Ptr<HTEXTURE> m_HTex;

	float Min;

public:
	Game_Ptr<Game_Renderer> Render()
	{
		return m_Render;
	}


public:
	float GetMin();
	int2 Index(float4 _Pos);
	float YHeight(float4 _Pos);
	Game_Vector CalTexelPos(const Game_Vector& _Pos);
	Game_Vector CalPosToWorld(const Game_Vector& _Pos);

public:
	void Init(int& _X, int& _Z, const Game_String _MapHeightTex, int _Order = 0);

	void BaseTexture(const Game_String _TextureName);
	void HTex(const Game_String _TextureName);;
};

