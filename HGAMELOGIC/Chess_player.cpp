#include "Chess_player.h"
#include <random>
#include <Game_3D_Debug.h>
#include "Game_2DAnimation.h"

//유닛
#include "WindRanger.h"
#include "SwordMaster.h"
#include "Warewolf.h"
#include "SoulBreaker.h"

#define Store_Unit_Scale { 1.8f, 1.8f , 1.f }
#define Mouse_Scale { 0.4f, 0.4f , 1.f } //100x100 픽셀

ChessPiece_Supply Chess_player::ChessPiece_Supply = {};
list<Game_Ptr<Chess_Base>> Chess_player::Piece_Board;
list<Game_Ptr<Chess_Base>> Chess_player::Piece_Enemy_ChessBoard;
bool Chess_player::Round = true;
bool Chess_player::PickCheck = false;
Game_Vector Chess_player::PickPos;

Chess_player::Chess_player() {}
Chess_player::~Chess_player() {}

void Chess_player::Init()
{
	if (false == HGAMEINPUT::IsKey(L"Mouse_L"))
		HGAMEINPUT::CreateKey(L"Mouse_L", VK_LBUTTON);

	//마우스
	{
		Game_Ptr<Game_Actor> Act = SCENE()->CreateActor();
		Act->TRANS()->LSCALE(Mouse_Scale);
		Mouse_Col= Act->CreateCom<Game_Collision>((int)Col_Name::Mouse);
		Mouse_Col->ColType(COLTYPE::AABB2D);
		//Mouse_Col->TRANS()->LSCALE(Mouse_Scale);

		Mouse_ColRenderer = Act->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Ui);
		float4 CUTDATA = { 0,0,1,1 };
		Mouse_ColRenderer->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
		float4 DRAWCOLOR = { 1,1,1,1 };
		Mouse_ColRenderer->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);
		Mouse_ColRenderer->TEXTURE(L"Tex", L"ColTest.png");
		Mouse_ColRenderer->SAMPLER(L"Smp", "LWSMP");
		//Mouse_ColRenderer->TRANS()->LSCALE(Mouse_Scale);
	}

	//시작/정지 버튼
	{
		Game_Ptr<Game_Actor> Act = SCENE()->CreateActor();
		PauseButton_Col = Act->CreateCom<Game_Collision>((int)Col_Name::Button);
		PauseButton_Col->ColType(COLTYPE::AABB2D);
		PauseButton_Col->TRANS()->LSCALE({ 1.f, 1.f , 1.f });
		PauseButton_Col->PushStayFunc(this, &Chess_player::Round_Col);


		PauseButton_ColRenderer = Act->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Ui);
		float4 CUTDATA = { 0,0,1,1 };
		PauseButton_ColRenderer->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
		float4 DRAWCOLOR = { 1,1,1,1 };
		PauseButton_ColRenderer->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);
		PauseButton_ColRenderer->TEXTURE(L"Tex", L"Red.png");
		PauseButton_ColRenderer->SAMPLER(L"Smp", "LWSMP");
		PauseButton_ColRenderer->TRANS()->LSCALE({ 1.f, 1.f , 1.f });

		PauseButton_ColRenderer->TRANS()->LPOS({5.0f, -0.5f ,0.f});
	}

	//테스트
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			Game_Ptr<Game_Collision> Col;
			Game_Ptr<Game_Renderer> ColRenderer;

			Game_Ptr<Game_Actor> Act = SCENE()->CreateActor();
			Act->TRANS()->LSCALE({ 0.6f, 0.6f , 0.6f });
			Act->TRANS()->LPOS({ -2.45f + i * 0.7f, -3.10f + j * 0.7f, 0.f });

			Col = Act->CreateCom<Game_Collision>((int)Col_Name::Board);
			Col->ColType(COLTYPE::AABB2D);
			//Col->TRANS()->LSCALE({ 0.5f, 0.5f , 0.5f });
			Col->PushStayFunc(this, &Chess_player::Board_Col);
			Col->Set_intNote(i*10+j); //5,4번이면 54로 저장.

			ColRenderer = Act->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Ui);
			float4 CUTDATA = { 0,0,1,1 };
			ColRenderer->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
			float4 DRAWCOLOR = { 1,1,1,1 };
			ColRenderer->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);
			ColRenderer->TEXTURE(L"Tex", L"ColTest.png");
			ColRenderer->SAMPLER(L"Smp", "LWSMP");
			ColRenderer->Off();
			//ColRenderer->TRANS()->LSCALE({ 0.6f, 0.6f , 0.5f });

			//ColRenderer->TRANS()->LPOS({ -2.45f + i * 0.7f, -1.70f + j * 0.7f, 0.f });
			//ColRenderer->TRANS()->LPOS({ (float)i ,0.f, (float)j-2 });
			//ColRenderer->TRANS()->LROT({ 90.f ,0.f, 0.f});
		}
	}

	//테스트용 유닛 초기 추가
	Test();

	UI_Setting();

	Supply_Input<WindRanger>(1);
	Supply_Input<SwordMaster>(1);

	ReRoll();
	Set_ShopAni();


	//ui 꺼버림
	Ui_Black->Off();
	for (size_t i = 0; i < 5; i++)
	{
		if (Store_Actor[i] != nullptr)
		{
			Store_Actor[i]->Off();
		}
	}
}

void Chess_player::Update()
{
	//마우스 좌표 업데이트
	Mouse_Col->TRANS()->LPOS({ HGAMEINPUT::MousePos3D().x / 100.f, HGAMEINPUT::MousePos3D().y / 100.f, HGAMEINPUT::MousePos3D().z / 100.f });
	Mouse_ColRenderer->TRANS()->LPOS({ HGAMEINPUT::MousePos3D().x / 100, HGAMEINPUT::MousePos3D().y / 100, HGAMEINPUT::MousePos3D().z / 100 });
	Game_3D_Debug::DrawDebugText(L"Mouse_Col W위치 %f %f %f", Mouse_Col->TRANS()->WPOS().x, Mouse_Col->TRANS()->WPOS().y, Mouse_Col->TRANS()->WPOS().z);
	Game_3D_Debug::DrawDebugText(L"Mouse_ColRenderer W위치 %f %f %f", Mouse_ColRenderer->TRANS()->WPOS().x, Mouse_ColRenderer->TRANS()->WPOS().y, Mouse_ColRenderer->TRANS()->WPOS().z);


	//////적 유닛들 사망 시 삭제시킴.
	for (auto itor : Piece_Enemy_ChessBoard)
	{
		if (itor->Info.Death)
		{
			Piece_Enemy_ChessBoard.remove(itor);
			break; //개선하고싶음. 야매코드
		}
	}

	//리롤
	if (HGAMEINPUT::Down("D"))
	{
		ReRoll();
		for (size_t i = 0; i < 5; i++)
		{
			if (Store_Actor[i] != nullptr)
			{
				Store_Actor[i]->Death();
			}
		}
		Set_ShopAni();
	}

	Ui_Switch();
}

void Chess_player::Set_ShopTex(Game_String Name, int ShopNum, float X, float Y, float Z, int Start, int End)
{
		Game_Vector Pos = { X, Y , (float)-ShopNum };
		float4 DRAWCOLOR = { 1,1,1,1 };

		Store_Actor[ShopNum] = SCENE()->CreateActor();
		Store_Actor[ShopNum]->TRANS()->LPOS(Pos);
		Store_Actor[ShopNum]->TRANS()->WSCALE(Store_Unit_Scale);

		Store_ColRenderer[ShopNum] = Store_Actor[ShopNum]->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Ui);
		float4 CUTDATA = { 0,0,1,1 };
		Store_ColRenderer[ShopNum]->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
		//float4 DRAWCOLOR = { 1,1,1,1 };
		Store_ColRenderer[ShopNum]->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);
		Store_ColRenderer[ShopNum]->TEXTURE(L"Tex", L"ColTest.png");
		Store_ColRenderer[ShopNum]->SAMPLER(L"Smp", "LWSMP");
		Store_ColRenderer[ShopNum]->LSCALE({0.3f, 0.5f, 1.f});

		Store_Renderer[ShopNum] = Store_Actor[ShopNum]->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Ui);
		Store_Ani[ShopNum] = Store_Actor[ShopNum]->CreateCom<Game_2DAnimation>(Store_Renderer[ShopNum]);
		Store_Ani[ShopNum]->CreateAni(L"Shop", Name, 0, 39, 0.1f, true);
		Store_Ani[ShopNum]->ChangeAni(L"Shop");
		Store_Renderer[ShopNum]->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);
		Store_Renderer[ShopNum]->TEXTURE(L"Tex", Name);
		Store_Renderer[ShopNum]->SAMPLER(L"Smp", "LWSMP");

		//2d AABB 충돌
		Store_Col[ShopNum] = Store_Actor[ShopNum]->CreateCom<Game_Collision>((int)Col_Name::Shop_Unit);
		Store_Col[ShopNum]->ColType(COLTYPE::AABB2D);
		Store_Col[ShopNum]->PushStayFunc(this, &Chess_player::Store_Unit_Col);
		Store_Col[ShopNum]->Set_Note(Name);
		Store_Col[ShopNum]->LSCALE({ 0.3f, 0.5f, 1.f });

}

void Chess_player::ReRoll()
{
	switch (Level)
	{
	case 1:	
		for (int i = 0; i < 5; ++i)
		{
			Chess_Store[i] = ChessPiece_Supply.Lv1_Supply.back();
			ChessPiece_Supply.Lv1_Supply.pop_back();
			random_shuffle(ChessPiece_Supply.Lv1_Supply.begin(), ChessPiece_Supply.Lv1_Supply.end());
		}
		break;

	case 2:		
		for (int i = 0; i < 5; ++i)
		{
			if (rand() % 3 == 0)
			{
				Chess_Store[i] = ChessPiece_Supply.Lv1_Supply.back();
				ChessPiece_Supply.Lv1_Supply.pop_back();
				random_shuffle(ChessPiece_Supply.Lv1_Supply.begin(), ChessPiece_Supply.Lv1_Supply.end());
			}
			else
			{
				Chess_Store[i] = ChessPiece_Supply.Lv2_Supply.back();
				ChessPiece_Supply.Lv2_Supply.pop_back();
				random_shuffle(ChessPiece_Supply.Lv2_Supply.begin(), ChessPiece_Supply.Lv2_Supply.end());
			}
		}
		break;

	case 3:		
		break;

	case 4:		
		break;

	case 5:		
		break;

	case 6:		
		break;

	case 7:		
		break;

	case 8:		
		break;

	case 9:		
		break;

	case 10:		
		break;
	default:
		break;
	}
}

void Chess_player::Set_ShopAni()
{
	for (int i = 0; i < 5; ++i)
	{
		if (Chess_Store[i].Get_Name() == "WindRanger")
			Set_ShopTex(L"WindRanger.png", i, i - 2, 2.f, 1.f, 0, 49);
		else if (Chess_Store[i].Get_Name() == "SwordMaster")
			Set_ShopTex(L"SwordMaster.png", i, i - 2, 2.f, 1.f, 0, 39);
		else if (Chess_Store[i].Get_Name() == "WareWolf")
			Set_ShopTex(L"WareWolf.png", i, i - 2, 2.f, 1.f, 0, 49);
	}
}

void Chess_player::UI_Setting()
{
	//ui검정 뒷배경 생성
	{
		Game_Ptr<Game_Actor> Act = SCENE()->CreateActor();
		Ui_Black = Act->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Ui);
		Ui_Black->TRANS()->LPOS({0.f, 1.8f, 0.f});
		Ui_Black->TRANS()->LSCALE({12.8f, 3.6f, 1.f});
		float4 CUTDATA;
		CUTDATA = { 0,0,1,1 };
		Ui_Black->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
		float4 DRAWCOLOR;
		DRAWCOLOR = { 1,1,1,0.5f }; //50%반투명
		Ui_Black->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);

		Ui_Black->TEXTURE(L"Tex", L"Black.png");
		Ui_Black->SAMPLER(L"Smp", "LWSMP");
	}
}

void Chess_player::Round_Col(Game_Collision* _This, Game_Collision* _Other)
{
	if (HGAMEINPUT::Down(L"Mouse_L"))
	{
		if (Round)
			Round = false;
		else
			Round = true;
	}
}

void Chess_player::Board_Col(Game_Collision* _This, Game_Collision* _Other)
{
	if (Round)
		return;

	if (HGAMEINPUT::Down(L"Mouse_L"))
	{
		int x, y;
		x = _This->Get_intNote() / 10;
		y = _This->Get_intNote() % 10 - 2; //해당 col의 좌표
		for (auto iter : Piece_Enemy_ChessBoard)
		{
			if (iter->Info.Position_X == x && iter->Info.Position_Y == y)
			{
				PickCheck = true;
				PickPos.x = x;
				PickPos.y = y;
				return;
			}
		}
		for (auto iter : Piece_Board)
		{
			if (iter->Info.Position_X == x && iter->Info.Position_Y == y)
			{
				PickCheck = true;
				PickPos.x = x;
				PickPos.y = y;
				return;
			}
		}
		for (auto iter : Piece_Banch)
		{
			if (iter->Info.Position_X == x && iter->Info.Position_Y == y)
			{
				PickCheck = true;
				PickPos.x = x;
				PickPos.y = y;
				return;
			}
		}
	}
	else if (HGAMEINPUT::Up(L"Mouse_L") && PickCheck)
	{
		int x, y;
		x = _This->Get_intNote() / 10;
		y = _This->Get_intNote() % 10-2; //해당 col의 좌표
		//이동시킴.
		for (auto iter : Piece_Enemy_ChessBoard)
		{
			if (iter->Info.Position_X == PickPos.x && iter->Info.Position_Y == PickPos.y)
			{
				iter->Info.Position_X = x;
				iter->Info.Position_Y = y;
				iter->Info.Real_X = iter->Info.Position_X;
				iter->Info.Real_Y = iter->Info.Position_Y;
				PickCheck = false;
				return;
			}
		}
		for (auto iter : Piece_Board)
		{
			if (iter->Info.Position_X == PickPos.x && iter->Info.Position_Y == PickPos.y)
			{
				iter->Info.Position_X = x;
				iter->Info.Position_Y = y;
				iter->Info.Real_X = iter->Info.Position_X;
				iter->Info.Real_Y = iter->Info.Position_Y;
				PickCheck = false;
				return;
			}
		}
		for (auto iter : Piece_Banch)
		{
			if (iter->Info.Position_X == PickPos.x && iter->Info.Position_Y == PickPos.y)
			{
				iter->Info.Position_X = x;
				iter->Info.Position_Y = y;
				iter->Info.Real_X = iter->Info.Position_X;
				iter->Info.Real_Y = iter->Info.Position_Y;
				PickCheck = false;
				return;
			}
		}

	}
}

template<class T>
void Chess_player::Supply_Input(int Level)
{
	int num = 5;
		for (int i = 0; i < num; ++i)
		{
			T a;
			switch (Level)
			{
			case 1:
				num = 25;
				//a.Init();
				ChessPiece_Supply.Lv1_Supply.emplace_back(a);
				break;
			case 2:
				num = 30;
				a.Init();
				ChessPiece_Supply.Lv2_Supply.emplace_back(a);
				break;
			case 3:
				num = 25;
				a.Init();
				ChessPiece_Supply.Lv3_Supply.emplace_back(a);
				break;
			case 4:
				num = 15;
				a.Init();
				ChessPiece_Supply.Lv4_Supply.emplace_back(a);
				break;
			case 5:
				num = 10;
				a.Init();
				ChessPiece_Supply.Lv5_Supply.emplace_back(a);
				break;
			default:
				break;
			}
		}
}

//상점에서의 유닛 생성.
void Chess_player::Store_Unit_Col(Game_Collision* _This, Game_Collision* _Other)
{
	if (HGAMEINPUT::Down(L"Mouse_L"))
	{
		Make_Piece(_This->Get_Note());

		_This->ACTOR()->Death();
	}
}

void Chess_player::Make_Piece(Game_String _Name, int _Star)
{
	if (L"WindRanger.png" == _Name)
		Make_Piece_T<WindRanger>(_Star);
	else if (L"SwordMaster.png" == _Name)
		Make_Piece_T<SwordMaster>(_Star);
	else if (L"WareWolf.png" == _Name)
		Make_Piece_T<WareWolf>(_Star);

	//	Set_Banch_Sequence(); //자리바꾸고 -> --돌면서 빈공간에 넣어.

	Check_StarUP(_Name.RangeToStr(-1, _Name.Size()-4)); //레벨업 체크함.
}

template<class T>
void Chess_player::Make_Piece_T(int _Star)
{
	Game_Ptr<Game_Actor> PTR1 = SCENE()->CreateActor();
	Game_Ptr<T> TestEnemy = PTR1->CreateCom<T>();
	TestEnemy->Info.Banch = false;
	TestEnemy->Info.Star = _Star;
	for (int i = 0; i < 8; ++i) //자리 찾아줌
	{
		if (!Find_Piece(i, -2))
		{
			TestEnemy->Info.Position_X = i;
			TestEnemy->Info.Position_Y = -2;
			TestEnemy->Info.Real_X = TestEnemy->Info.Position_X;
			TestEnemy->Info.Real_Y = TestEnemy->Info.Position_Y;
			
			break;
		}
	}
	Piece_Banch.emplace_back(TestEnemy);
}

void Chess_player::Check_StarUP(Game_String _Name, int _Star)
{
	int Count = 0;

	for (auto i : Piece_Banch)
		if (i->Info.Name == _Name && i->Info.Star == _Star)
			Count++;
	for (auto i : Piece_Board)
		if (i->Info.Name == _Name && i->Info.Star == _Star)
			Count++;

	if (Count >= 3)
	{
		//삭제.
		//for (auto &i : Piece_Banch)
		//	if (i->Info.Name == _Name && i->Info.Star == _Star)
		//	{
		//		i->ACTOR()->Death();
		//	}

		for (vector<Game_Ptr<Chess_Base>>::iterator iter = Piece_Banch.begin(); iter != Piece_Banch.end(); )
		{
			if (iter->PTR->Info.Name == _Name && iter->PTR->Info.Star == _Star)
			{
				iter->PTR->Check_Death(true);
				iter = Piece_Banch.erase(iter);
			}
			else
				iter++;
		}
 
		//내부에 메쉬act는 어떻게죽이지?
		for (list<Game_Ptr<Chess_Base>>::iterator iter = Piece_Board.begin(); iter != Piece_Board.end(); )
		{
			if (iter->PTR->Info.Name == _Name && iter->PTR->Info.Star == _Star)
			{
				iter->PTR->Check_Death(true);
				iter = Piece_Board.erase(iter);
			}
			else
				iter++;
		}
		//생성.
		Make_Piece(_Name+=".png", _Star+1);

		if(_Star == 1)
			Check_StarUP(_Name, 2); //연쇄작용 위하여 2성 체크.
	}
}

bool Chess_player::Find_Piece(int x, int y)
{
	if (y == -2) //벤치라면
	{
		for (auto i : Piece_Banch)
		{
			if (i->Info.Position_X == x)
				return true;
		}
	}
	else
	{
		for (auto i : Piece_Board)
		{
			if (i->Info.Position_X == x && i->Info.Position_Y == y)
				return true;
		}
	}

	return false;
}

void Chess_player::Set_Banch_Sequence()
{
	int num= 0;
	for (auto iter : Piece_Banch)
	{
		iter->Info.Position_X = num;
		iter->Info.Position_Y = -2;
		iter->Info.Real_X = iter->Info.Position_X;
		iter->Info.Real_Y = iter->Info.Position_Y;

		num++;
	}
}

void Chess_player::Test()
{
	//테스트용 아군 유닛 생성//////////////////////////////////////
	//{
	//	Game_Ptr<Game_Actor> PTR1 = SCENE()->CreateActor();
	//	Game_Ptr<WindRanger> TestEnemy = PTR1->CreateCom<WindRanger>();
	//	TestEnemy->Info.Position_X = 1;
	//	TestEnemy->Info.Position_Y = 0;
	//	TestEnemy->Info.Real_X = 1;
	//	TestEnemy->Info.Real_Y = 0;
	//	Piece_Board.emplace_back(TestEnemy);
	//}
	//{
	//	Game_Ptr<Game_Actor> PTR1 = SCENE()->CreateActor();
	//	Game_Ptr<WareWolf> TestEnemy = PTR1->CreateCom<WareWolf>();
	//	TestEnemy->Info.Position_X = 1;
	//	TestEnemy->Info.Position_Y = 1;
	//	TestEnemy->Info.Real_X = 1;
	//	TestEnemy->Info.Real_Y = 1;
	//	Piece_Board.emplace_back(TestEnemy);
	//}
	//{
	//	Game_Ptr<Game_Actor> PTR1 = SCENE()->CreateActor();
	//	Game_Ptr<SwordMaster> TestEnemy = PTR1->CreateCom<SwordMaster>();
	//	TestEnemy->Info.Position_X = 1;
	//	TestEnemy->Info.Position_Y = 4;
	//	TestEnemy->Info.Real_X = 1;
	//	TestEnemy->Info.Real_Y = 4;
	//	Piece_Board.emplace_back(TestEnemy);
	//}
	{
		Game_Ptr<Game_Actor> PTR1 = SCENE()->CreateActor();
		Game_Ptr<SoulBreaker> TestEnemy = PTR1->CreateCom<SoulBreaker>();
		TestEnemy->Info.Position_X = 1;
		TestEnemy->Info.Position_Y = 6;
		TestEnemy->Info.Real_X = 1;
		TestEnemy->Info.Real_Y = 6;
		Piece_Board.emplace_back(TestEnemy);
	}

	//테스트용 적 유닛 생성///////////////////////////////////////////////
	/*{
		Game_Ptr<Game_Actor> PTR1 = SCENE()->CreateActor();
		Game_Ptr<WindRanger> TestEnemy = PTR1->CreateCom<WindRanger>();
		TestEnemy->Info.Position_X = 7;
		TestEnemy->Info.Position_Y = 6;
		TestEnemy->Info.Real_X = 7;
		TestEnemy->Info.Real_Y = 6;
		TestEnemy->Info.MyUnit = false;
		Piece_Enemy_ChessBoard.emplace_back(TestEnemy);
	}
	{
		Game_Ptr<Game_Actor> PTR1 = SCENE()->CreateActor();
		Game_Ptr<SwordMaster> TestEnemy = PTR1->CreateCom<SwordMaster>();
		TestEnemy->Info.Position_X = 6;
		TestEnemy->Info.Position_Y = 7;
		TestEnemy->Info.Real_X = 6;
		TestEnemy->Info.Real_Y = 7;
		Piece_Enemy_ChessBoard.emplace_back(TestEnemy);
	}*/
	//{
	//	Game_Ptr<Game_Actor> PTR1 = SCENE()->CreateActor();
	//	Game_Ptr<WareWolf> TestEnemy = PTR1->CreateCom<WareWolf>();
	//	TestEnemy->Info.Position_X = 5;
	//	TestEnemy->Info.Position_Y = 7;
	//	TestEnemy->Info.Real_X = 5;
	//	TestEnemy->Info.Real_Y = 7;
	//	TestEnemy->Info.MyUnit = false;
	//	Piece_Enemy_ChessBoard.emplace_back(TestEnemy);
	//}
}

void Chess_player::Ui_Switch()
{
	if (HGAMEINPUT::Down("S"))
	{
		if (Ui_Black->IsUpdate())
		{
			Ui_Black->Off();
			for (size_t i = 0; i < 5; i++)
			{
				if (Store_Actor[i] != nullptr)
				{
					Store_Actor[i]->Off();
				}
			}
		}

		else
		{
			Ui_Black->On();
			for (size_t i = 0; i < 5; i++)
			{
				if (Store_Actor[i] != nullptr)
				{
					Store_Actor[i]->On();
				}
			}
		}
	}

	if (HGAMEINPUT::Down("Test"))
	{
		if (Ui_Black->IsUpdate())
		{
			Ui_Black->Off();
			for (size_t i = 0; i < 5; i++)
			{
				if (Store_ColRenderer[i] != nullptr)
				{
					Store_ColRenderer[i]->Off();
				}
			}
		}

		else
		{
			Ui_Black->On();
			for (size_t i = 0; i < 5; i++)
			{
				if (Store_ColRenderer[i] != nullptr)
				{
					Store_ColRenderer[i]->On();
				}
			}
		}
	}
}
