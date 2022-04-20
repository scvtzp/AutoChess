#include "CL_LoadScene.h"
#include "CL_Scene.h"

CL_LoadScene::CL_LoadScene()
{
}

CL_LoadScene::~CL_LoadScene()
{
}

void CL_LoadScene::SceneChangeStart()
{
}

void CL_LoadScene::SceneChangeEnd()
{
}

void CL_LoadScene::Init()
{
}

void CL_LoadScene::Update()
{
	if (HGAMEINPUT::Down(L"TEST1") || true)
		HGAMESCENE::ChangeScene(L"TestScene");
}