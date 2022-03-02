#pragma once
#include "HRENDERER.h"
#include "HGAMESPRITE.h"

class Game_SpriteRenderer : public Game_Renderer
{
public:
	Game_Vector m_Color;

private:
	Game_Vector CutData;
	Game_Ptr<Game_Sprite> m_SPRITE;
	unsigned int m_SpriteIndex;

public:
	void SPRITE(const Game_String& _Name, unsigned int _SpriteIndex = 0)
	{
		m_SPRITE = Game_Sprite::Find(_Name);
		SpriteIndex(_SpriteIndex);
	}

	void SpriteIndex(unsigned int _SpriteIndex)
	{
		m_SpriteIndex = _SpriteIndex;
	}

public:
	void Alpha(float _A) 
	{
		m_Color.w = _A;
	}
	void Color(Game_Vector _Color) 
	{
		m_Color = _Color;
	}

public:
	void Init(int _Order = 0);
	void Render(Game_Ptr<HCAM> _Cam) override;

};

