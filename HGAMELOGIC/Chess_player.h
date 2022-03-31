#pragma once
#include "GAMELOGIC.h"
#include "Chess_Base.h"
#include "Game_2DAnimation.h"
#include <HCOL.h>

struct ChessPiece_Supply
{
	vector<Chess_Base> Lv1_Supply;
	vector<Chess_Base> Lv2_Supply;
	vector<Chess_Base> Lv3_Supply;
	vector<Chess_Base> Lv4_Supply;
	vector<Chess_Base> Lv5_Supply;

	//기물 갯수.
	//1코-45 2코-30 3코-25 4코-15 5코-10
};

enum class Chess_Team
{
	Ally,
	Enemy,
	End
};

class Chess_player : public HACTORCOM
{
public:
	Chess_player();
	~Chess_player();

private:
	int Hp = 100;
	int Level = 1;
	int Exp = 0;

	map<Chess_Race, Chess_Base> Race_synergy; //현재 종족 현황?(특성)
	map<Chess_Class, Chess_Base> Class_synergy; //현재 직업 현황(특성) //라운드 시작시마다 돌아감.

	//마우스
	Game_Ptr<Game_Collision> Mouse_Col;
	Game_Ptr<Game_Renderer> Mouse_ColRenderer;

	//버튼
	Game_Ptr<Game_Collision> PauseButton_Col;
	Game_Ptr<Game_Renderer> PauseButton_ColRenderer;

	//상점
	vector<Chess_Base> Chess_Store = std::vector<Chess_Base>(5);
	Game_Ptr<Game_Actor> Store_Actor[5];
	Game_Ptr<Game_Renderer> Store_Renderer[5];
	Game_Ptr<Game_2DAnimation> Store_Ani[5];
	Game_Ptr<Game_Collision> Store_Col[5];
	Game_Ptr<Game_Renderer> Store_ColRenderer[5];

	//필드
	vector<Game_Ptr<Chess_Base>> Piece_Banch; //벤치유닛
	void Set_Banch_Sequence();
	
	//필드 이동
	static bool PickCheck;
	static Game_Vector PickPos;

	//UI
	Game_Ptr<Game_Renderer> Ui_Black;

	//Test
	template<typename T>
	void Spawn_Unit(int _X, int _Y, Chess_Team _Team = Chess_Team::Ally); //_Team 0이라면 아군 1이라면 적군


public:
	static ChessPiece_Supply ChessPiece_Supply; //공급처
	static list<Game_Ptr<Chess_Base>> Piece_Board; //내 보드 유닛 목록 (토큰 포함)
	static list<Game_Ptr<Chess_Base>> Piece_Enemy_ChessBoard; //상대방 보드유닛 목록 (토큰 포함)

	static bool Round;

	void Init() override;
	void Update() override;

	//Store 관련
	template <class T>
	void Supply_Input(int Level);
	void ReRoll();

	void Set_ShopAni();
	void Set_ShopTex(Game_String Name, int ShopNum, float X, float Y, float Z, int AniStart = -1, int AniEnd = -1);
	void Store_Unit_Col(Game_Collision* _This, Game_Collision* _Other); //클릭했을때	
	void Make_Piece(Game_String _Name, int _Star = 1); //이름 넣어주면 유닛 벤치에 생산.
	template <class T>
	void Make_Piece_T(int _Star);

	//TEst
	void Test();
	void Ui_Switch();

	//init 세팅 관련.
	void UI_Setting();
	void Round_Col(Game_Collision* _This, Game_Collision* _Other); //라운드 시작/종료 버튼
	void Board_Col(Game_Collision* _This, Game_Collision* _Other); 

	//기타
	void Check_StarUP(Game_String _Name, int _Star = 1); //3개 모이면 업글해주는거 체크.
	bool Find_Piece(int x, int y);
};


//핸드 뽑는 순서
//1. 레벨별 출현 %에 따라서 5개의 등급 선정.
//2. 각 등급의 덱에서 하나 뽑음. x5swa3e2qw2