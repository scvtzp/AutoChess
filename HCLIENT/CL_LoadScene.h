#pragma once
#include <HGAMECOM.h>
#include <mutex>

class CL_LoadScene : public HSCENECOM
{
public:
	CL_LoadScene();
	~CL_LoadScene();
	std::mutex Mut;
	
	void SceneChangeStart() override;
	void SceneChangeEnd() override;
	void Init() override;
	void Update() override;
	void Load(int _Load_Num);

	void Load_Functions();

	int LoadCount = 0;
};