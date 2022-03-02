#pragma once
#include <HGAMECOM.h>
#include <set>

#pragma comment(lib, "HGAMEBASE.lib")
#pragma comment(lib, "HGAMEENGINE.lib")

class HCAM;
class FreeCam : public HACTORCOM
{

public:
	Game_Ptr<HCAM> m_Cam;
	float m_Speed;
	float m_RotSpeed;

public:
	//FreeCam() 
	//{
	//  m_Cam->addref
	//}
	//~FreeCam();
	FreeCam();
	~FreeCam();

public:
	// Ä·ÀÇ È¸Àü
	void Init(float _Speed);
	void Update() override;


};

