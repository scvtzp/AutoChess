#pragma once
#include <HMGR.h>
#include "HVBBUFFER.h"
#include "HIBBUFFER.h"

// class HVBBUFFER;
// class HIBBUFFER;
class HMESH : public HMGR<HMESH>
{
public:
	static Game_Ptr<HMESH> Create(const Game_String& _Name)
	{
		Game_Ptr<HMESH> Mesh = CreateToInsert(_Name);
		Mesh->VB(_Name);
		Mesh->IB(_Name);

		return Mesh;
	}

	static Game_Ptr<HMESH> Create(const Game_String& _Name, const Game_String& _VB, const Game_String& _IB)
	{
		Game_Ptr<HMESH> Mesh = CreateToInsert(_Name);
		Mesh->VB(_VB);
		Mesh->IB(_IB);
		return Mesh;
	}


private:
	Game_Ptr<HVBBUFFER> m_VB;
	Game_Ptr<HIBBUFFER> m_IB;

public:
	Game_Ptr<HVBBUFFER> VB() { return m_VB; }
	Game_Ptr<HIBBUFFER> IB() { return m_IB; }

public:
	void VB(Game_Ptr<HVBBUFFER> _VB);
	void IB(Game_Ptr<HIBBUFFER> _IB);

public:
	void VB(const Game_String& _Name);
	void IB(const Game_String& _Name);

public:
	void Render();
};

