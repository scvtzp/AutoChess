#include "HCLIENTSCENECOM.h"
#include <HGAMESCENE.h>
#include <HGAMEACTOR.h>
#include <HCAM.h>
#include <Game_Trans.h>
#include <Game_SpriteRenderer.h>
#include <HGAMESPRITE.h>
#include <FreeCam.h>
#include <HGAMETIME.h>


HCLIENTSCENECOM::HCLIENTSCENECOM() {

}
HCLIENTSCENECOM::~HCLIENTSCENECOM() {

}

enum class RENDERGROUP
{
	PLAY,
	UI,
};

void HCLIENTSCENECOM::Init() 
{
	////±âº» Ä· »ý¼º
	//{
	//	Game_Ptr<Game_Actor> NewActor = SCENE()->CreateActor();
	//	CamActor = NewActor;
	//	NewActor->TRANS()->LPOS({ 0, 0, -10 });
	//	Game_Ptr<HCAM> NewCam = NewActor->CreateCom<HCAM>(0, (int)RenderType::Default);
	//	NewCam->MODE(CAMMODE::PERS);
	//	NewCam->CAMSIZE({ 12.8f, 7.2f });
	//	m_LineFilter = NewCam->AddFilter<HOutLineFilter>(10);
	//	m_BloomFilter = NewCam->AddFilter<HBloomFilter>(10);
	//	NewActor->CreateCom<FreeCam>(10.0f);

	//	CamActor->TRANS()->LPOS({ 3.5f, 9.f, 0.5f });
	//	CamActor->TRANS()->LROT({ 80, 0.f, 0.f });
	//}
}

void HCLIENTSCENECOM::Update() 
{


}