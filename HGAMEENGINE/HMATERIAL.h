#pragma once
#include <HMGR.h>
#include "HVTXSHADER.h"
#include "HPIXSHADER.h"
#include "HRASTERIZER.h"
#include "HDEPTHSTENCIL.h"
#include "HBLEND.h"

class HRENDERPLAYER;
class Game_Material : public HMGR<Game_Material>
{
public:
	friend HRENDERPLAYER;

public:
	static Game_Ptr<Game_Material> Create(const Game_String& _Name)
	{
		return CreateToInsert(_Name);
	}

private:
	bool m_AllCheck;
	Game_Ptr<HVTXSHADER>			m_VTXSHADER;
	Game_Ptr<HPIXSHADER>			m_PIXSHADER;
	std::vector<HSHADER*>		m_ALLSHADER;

	Game_Ptr<HRASTERIZER>	m_RASTERIZER;
	Game_Ptr<HDEPTHSTENCIL> m_DEPTHSTENCIL;
	Game_Ptr<HBLEND>		m_BLEND;

public:
	Game_Ptr<HVTXSHADER> VTXSHADER()		{ return m_VTXSHADER; }
	Game_Ptr<HPIXSHADER> PIXSHADER()		{ return m_PIXSHADER; }
	Game_Ptr<HRASTERIZER> RASTERIZER()		{ return m_RASTERIZER; }
	Game_Ptr<HDEPTHSTENCIL> DEPTHSTENCIL()	{ return m_DEPTHSTENCIL; }
	Game_Ptr<HBLEND>		BLEND()			{ return m_BLEND; }


	RENDERPATHTYPE PathType();

	void VTXSHADER(const Game_String& _Name);
	void PIXSHADER(const Game_String& _Name);
	void RASTERIZER(const Game_String& _Name);
	void DEPTHSTENCIL(const Game_String& _Name);
	void BLEND(const Game_String& _Name);

public:
	Game_Material();
	~Game_Material();

public:
	void Setting();
};

