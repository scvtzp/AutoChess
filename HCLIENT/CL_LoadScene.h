#pragma once
#include <HGAMECOM.h>

class CL_LoadScene : public HSCENECOM
{
public:
	CL_LoadScene();
	~CL_LoadScene();
	void SceneChangeStart() override;
	void SceneChangeEnd() override;
	void Init() override;
	void Update() override;
};