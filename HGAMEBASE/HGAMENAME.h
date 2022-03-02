#pragma once
#include "HGAMESTRING.h"

class HGAMENAME
{
private:
	Game_String m_Name;

public:
	Game_String NAME() 
	{
		return m_Name;
	}

	void NAME(const Game_String& _Name) 
	{
		m_Name = _Name;
	}

public:
	HGAMENAME() {}
	virtual ~HGAMENAME() {}
};

