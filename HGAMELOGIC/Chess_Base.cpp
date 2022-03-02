#include "Chess_Base.h"
#include "Chess_player.h"
#include <HGAMETIME.h>

void Chess_Base::AStar()
{
	if (Info.MyUnit)
	{

		//0. Board_Array 초기화
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				Board_Array[i][j] = false; //막힌 길 판정 받는 곳. true = 막힘
			}
		}

		//1. 내 위치 받음.
		AStar_Struct* Start = new AStar_Struct;
		Start->X = Info.Position_X;
		Start->Y= Info.Position_Y;
		Start->GScore = 0;
		OpenList.emplace_back(Start);

		//2. 적 정보 찾음.
		AStar_Struct Enemy = Find_Enemy();
		if (Enemy.X == -1 && Enemy.Y == -1) //적이 없으면 걍 리턴.
			return;
		float EnemyX = Enemy.X;
		float EnemyY = Enemy.Y;

		//아군 유닛 있는칸 막힘취급.
		for (auto iter : Chess_player::Piece_Board)
		{
			Board_Array[(int)iter->Info.Position_X][(int)iter->Info.Position_Y] = true;
		}

		//3. 루프
		bool BWhile = true;
		while (BWhile)
		{
			//1. G값 갱신.
			for (auto itor : OpenList)
			{
				int G = itor->GScore;
				for (int i = -1; i <= 1; i++)
				{
					for (int j = -1; j <= 1; j++)
					{
						if (i == 0 && j == 0)
							;
						else if (Find_Close_Astar(itor->X + i, itor->Y + j) != nullptr)
						{
							if (i == 0 || j == 0) //둘중 하나가 0이라면 == 상하좌우
							{
								if (Find_Close_Astar(itor->X + i, itor->Y + j)->GScore + 10 < G)
								{
									G = Find_Close_Astar(itor->X + i, itor->Y + j)->GScore + 10;
									itor->Origin_X = Find_Close_Astar(itor->X + i, itor->Y + j)->X;
									itor->Origin_Y = Find_Close_Astar(itor->X + i, itor->Y + j)->Y;
								}
							}
							else //대각선
							{
								if (Find_Close_Astar(itor->X + i, itor->Y + j)->GScore + 14 < G)
								{
									G = Find_Close_Astar(itor->X + i, itor->Y + j)->GScore + 14;
									itor->Origin_X = Find_Close_Astar(itor->X + i, itor->Y + j)->X;
									itor->Origin_Y = Find_Close_Astar(itor->X + i, itor->Y + j)->Y;
								}
							}
						}
					}
				}
				itor->GScore = G;
				itor->FScore = itor->GScore + itor->HScore;

			}

			//2. F최소값 낮은거 찾음.
			int F = 1000, Fx = 999, Fy = 999, Fh = 1000;
			for (auto itor : OpenList)
			{
				if (itor->FScore < F)
				{
					F = itor->FScore;
					Fx = itor->X;
					Fy = itor->Y;
					Fh = itor->HScore;
				}
				else if (itor->FScore == F && itor->HScore < Fh) //똑같다면 H기준으로.
				{
					F = itor->FScore;
					Fx = itor->X;
					Fy = itor->Y;
					Fh = itor->HScore;
				}
			}
			CloseList.emplace_back(Find_Open_Astar(Fx, Fy)); //CL에 넣고
			OpenList.remove(Find_Open_Astar(Fx, Fy)); //OL에서 삭제.

			//3. 주변 노드 오픈
			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					if (i == 0 && j == 0)
						;
					else if (Fx + i < 0 || Fy + j < 0 || Fx + i > 7 || Fy + j > 7)
						;
					else if (IsBlock(Fx + i, Fy + j) == false &&
						Find_Open_Astar(Fx + i, Fy + j) == nullptr &&
						Find_Close_Astar(Fx + i, Fy + j) == nullptr)
					{
						if (Fx + i == EnemyX && Fy + j == EnemyY) //탈출 조건. 목적지를 닫음.
						{
							if (i == 0 || j == 0)
							{
								//CloseList.emplace_back(Find_Open_Astar(EnemyX, EnemyY)); 목적지 상하좌우 앞 칸이 최종이 되도록 함.
								//OpenList.remove(Find_Open_Astar(EnemyX, EnemyY));
								BWhile = false;
								// 4. 역순으로 탐색하여 길찾기 완료.
								Find_Route(Start->X, Start->Y, Fx, Fy);

								break;
							}
						}
						else
						{
							AStar_Struct* Node = new AStar_Struct;
							Node->X = Fx + i;
							Node->Y = Fy + j;
							Node->Origin_X = Fx;
							Node->Origin_Y = Fy;
							Node->HScore = Get_HScore(Node->X, Node->Y, EnemyX, EnemyY);
							OpenList.emplace_back(Node);
						}
					}
				}
			}

			//탈출
			if (OpenList.empty()) //1.열린 노드 없을때 = 다가갈 수 있는 모든 노드 탐색완료해도 닫지않음.
				return;
		}


		//5. open, close, board 비우기.
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				Board_Array[i][j] = false;
			}
		}
		for (auto itor : OpenList)
		{
			delete itor;
		}
		for (auto itor : CloseList)
		{
			delete itor;
		}

		OpenList.clear();
		CloseList.clear();
	}

	else
	{
		//0. Board_Array 초기화
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				Board_Array[i][j] = false; //막힌 길 판정 받는 곳. true = 막힘
			}
		}
	
		//1. 내 위치 받음.
		AStar_Struct* Start = new AStar_Struct;
		Start->X = Info.Position_X;
		Start->Y = Info.Position_Y;
		Start->GScore = 0;
		OpenList.emplace_back(Start);
	
		//2. 적 정보 찾음.
		AStar_Struct Enemy = Find_Enemy();
		if (Enemy.X == -1 && Enemy.Y == -1) //적이 없으면 걍 리턴.
			return;
		float EnemyX = Enemy.X;
		float EnemyY = Enemy.Y;
	
		for (auto iter : Chess_player::Piece_Enemy_ChessBoard)
		{
			Board_Array[(int)iter->Info.Position_X][(int)iter->Info.Position_Y] = true;
		}

		//3. 루프
		bool BWhile = true;
		while (BWhile)
		{
			//1. G값 갱신.
			for (auto itor : OpenList)
			{
				int G = itor->GScore;
				for (int i = -1; i <= 1; i++)
				{
					for (int j = -1; j <= 1; j++)
					{
						if (i == 0 && j == 0)
							;
						else if (Find_Close_Astar(itor->X + i, itor->Y + j) != nullptr)
						{
							if (i == 0 || j == 0) //둘중 하나가 0이라면 == 상하좌우
							{
								if (Find_Close_Astar(itor->X + i, itor->Y + j)->GScore + 10 < G)
								{
									G = Find_Close_Astar(itor->X + i, itor->Y + j)->GScore + 10;
									itor->Origin_X = Find_Close_Astar(itor->X + i, itor->Y + j)->X;
									itor->Origin_Y = Find_Close_Astar(itor->X + i, itor->Y + j)->Y;
								}
							}
							else //대각선
							{
								if (Find_Close_Astar(itor->X + i, itor->Y + j)->GScore + 14 < G)
								{
									G = Find_Close_Astar(itor->X + i, itor->Y + j)->GScore + 14;
									itor->Origin_X = Find_Close_Astar(itor->X + i, itor->Y + j)->X;
									itor->Origin_Y = Find_Close_Astar(itor->X + i, itor->Y + j)->Y;
								}
							}
						}
					}
				}
				itor->GScore = G;
				itor->FScore = itor->GScore + itor->HScore;
	
			}
	
			//2. F최소값 낮은거 찾음.
			int F = 1000, Fx = 999, Fy = 999, Fh = 1000;
			for (auto itor : OpenList)
			{
				if (itor->FScore < F)
				{
					F = itor->FScore;
					Fx = itor->X;
					Fy = itor->Y;
					Fh = itor->HScore;
				}
				else if (itor->FScore == F && itor->HScore < Fh) //똑같다면 H기준으로.
				{
					F = itor->FScore;
					Fx = itor->X;
					Fy = itor->Y;
					Fh = itor->HScore;
				}
			}
			CloseList.emplace_back(Find_Open_Astar(Fx, Fy)); //CL에 넣고
			OpenList.remove(Find_Open_Astar(Fx, Fy)); //OL에서 삭제.
	
			//3. 주변 노드 오픈
			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					if (i == 0 && j == 0)
						;
					else if (Fx + i < 0 || Fy + j < 0 || Fx + i > 7 || Fy + j > 7)
						;
					else if (IsBlock(Fx + i, Fy + j) == false &&
						Find_Open_Astar(Fx + i, Fy + j) == nullptr &&
						Find_Close_Astar(Fx + i, Fy + j) == nullptr)
					{
						if (Fx + i == EnemyX && Fy + j == EnemyY) //탈출 조건. 목적지를 닫음.
						{
							if (i == 0 || j == 0)
							{
								//CloseList.emplace_back(Find_Open_Astar(EnemyX, EnemyY)); 목적지 상하좌우 앞 칸이 최종이 되도록 함.
								//OpenList.remove(Find_Open_Astar(EnemyX, EnemyY));
								BWhile = false;
								// 4. 역순으로 탐색하여 길찾기 완료.
								Find_Route(Start->X, Start->Y, Fx, Fy);
	
								break;
							}
						}
						else
						{
							AStar_Struct* Node = new AStar_Struct;
							Node->X = Fx + i;
							Node->Y = Fy + j;
							Node->Origin_X = Fx;
							Node->Origin_Y = Fy;
							Node->HScore = Get_HScore(Node->X, Node->Y, EnemyX, EnemyY);
							OpenList.emplace_back(Node);
						}
					}
				}
			}
	
			//탈출
			if (OpenList.empty()) //1.열린 노드 없을때 = 다가갈 수 있는 모든 노드 탐색완료해도 닫지않음.
				return;
		}
	
	
		//5. open, close, board 비우기.
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				Board_Array[i][j] = false;
			}
		}
		for (auto itor : OpenList)
		{
			delete itor;
		}
		for (auto itor : CloseList)
		{
			delete itor;
		}
	
		OpenList.clear();
		CloseList.clear();
	}
}

void Chess_Base::Move()
{
	if (Info.Banch)
	{
		return;
	}
	else if (IsMove)
	{
		Info.State = Chess_State::Run;

		//Info.Position_X = Info.Move_X;
		//Info.Position_Y = Info.Move_Y;
		Info.Real_X += (Info.Move_X - Info.Position_X) * Game_Time::DeltaTime();
		Info.Real_Y += (Info.Move_Y - Info.Position_Y) * Game_Time::DeltaTime();

		Set_Rot(Info.Move_X, Info.Move_Y);

		if (Info.Real_X == Info.Move_X && Info.Real_Y == Info.Move_Y) //+-오차 잡아줘야하나?
		{
			Info.Position_X = Info.Move_X;
			Info.Position_Y = Info.Move_Y;
			Info.Real_X = Info.Move_X;
			Info.Real_Y = Info.Move_Y;
			IsMove = false;
		}
		else if (Info.Move_X - Info.Position_X > 0 && Info.Real_X > Info.Move_X)
		{
			Info.Position_X = Info.Move_X;
			Info.Position_Y = Info.Move_Y;
			Info.Real_X = Info.Move_X;
			Info.Real_Y = Info.Move_Y;
			IsMove = false;
		}
		else if (Info.Move_X - Info.Position_X < 0 && Info.Real_X < Info.Move_X
			|| Info.Move_Y - Info.Position_Y < 0 && Info.Real_Y < Info.Move_Y
			|| Info.Move_Y - Info.Position_Y > 0 && Info.Real_Y > Info.Move_Y)
		{
			Info.Position_X = Info.Move_X;
			Info.Position_Y = Info.Move_Y;
			Info.Real_X = Info.Move_X;
			Info.Real_Y = Info.Move_Y;
			IsMove = false;
		}
	}

	else if (Info.Mana >= 100)
	{
		Info.Mana = 0;
		Info.Play_Skile = true;
		return;
	}
	else if (Info.Play_Skile)
	{
		Info.Mana = 0;
		if(Info.State != Chess_State::Skill02)
			Info.State = Chess_State::Skill01;
	}

	else if (Me_CloseEnemy_distance() <= Info.Range)
	{
		Info.State = Chess_State::Attack_1;

		AtkTime += Game_Time::DeltaTime();

		 if (AtkTime >= Info.Atk_Rate)
		{
			 if(!HGAMESOUND::IsPlay(L"Blade"))
				HGAMESOUND::Play(L"Blade", L"Blade.wav");

			Info.Mana += 10;
			AtkTime = 0.f;
			AStar_Struct Enemy = Find_Enemy();
			if (Info.MyUnit)
			{
				for (auto itor : Chess_player::Piece_Enemy_ChessBoard)
				{
					if (itor->Info.Position_X == Enemy.X && itor->Info.Position_Y == Enemy.Y) //개선 필요
					{
						itor->Info.Hp -= Info.Atk;
						itor->Info.Mana += 5;
						Set_Rot(itor->Info.Position_X, itor->Info.Position_Y);
					}
				}
			}

			else
			{
				for (auto itor : Chess_player::Piece_Board)
				{
					if (itor->Info.Position_X == Enemy.X && itor->Info.Position_Y == Enemy.Y) //개선 필요
					{
						itor->Info.Hp -= Info.Atk;
						itor->Info.Mana += 5;
						Set_Rot(itor->Info.Position_X, itor->Info.Position_Y);
					}
				}
			}
		}
	}
	else
	{
		AStar();
		Info.Real_X = Info.Position_X;
		Info.Real_Y = Info.Position_Y;
		IsMove = true;
	}
}

float Chess_Base::Me_CloseEnemy_distance() //가장 가까운 적과의 거리 반환.
{
	if (Info.MyUnit)
	{
		float EnemyX, EnemyY, EnemyDistance = 999.f;
		for (auto itor : Chess_player::Piece_Enemy_ChessBoard)
		{
			float fX = (itor->Info.Position_X - Info.Position_X);
			float fY = (itor->Info.Position_Y - Info.Position_Y);
			if (fX < 0)
				fX *= -1;
			if (fY < 0)
				fY *= -1; //혹시 몰라서 하는 양수 초기화.

			float Dis = sqrt((fX * fX) + (fY * fY));
			if (Dis < EnemyDistance)
				EnemyDistance = Dis;
		}
		return EnemyDistance;
	}
	else
	{
		float EnemyX, EnemyY, EnemyDistance = 999.f;
		for (auto itor : Chess_player::Piece_Board)
		{
			float fX = (itor->Info.Position_X - Info.Position_X);
			float fY = (itor->Info.Position_Y - Info.Position_Y);
			if (fX < 0)
				fX *= -1;
			if (fY < 0)
				fY *= -1; //혹시 몰라서 하는 양수 초기화.

			float Dis = sqrt((fX * fX) + (fY * fY));
			if (Dis < EnemyDistance)
				EnemyDistance = Dis;
		}
		return EnemyDistance;
	}
}

int Chess_Base::Get_HScore(int x1, int y1, int x2, int y2)
{
	int X = x1 - x2;
	int Y = y1 - y2;
	if (X < 0)
		X *= -1;
	if (Y < 0)
		Y *= -1;
	
	if (X < Y)
		return (X * 14) + (Y - X) * 10;
	else
		return (Y * 14) + (X - Y) * 10;
}

bool Chess_Base::IsBlock(int x, int y)
{
	if (Board_Array[x][y])
		return true;
	else
		return false;
}

AStar_Struct* Chess_Base::Find_Close_Astar(int x, int y)
{
	for (auto itor : CloseList)
	{
		if (itor->X == x && itor->Y == y)
			return itor;
	}
	return nullptr;
}

AStar_Struct* Chess_Base::Find_Open_Astar(int x, int y)
{
	for (auto itor : OpenList)
	{
		if (itor->X == x && itor->Y == y)
			return itor;
	}
	return nullptr;
}

AStar_Struct Chess_Base::Find_Enemy() //가장 가까운 적 뱉음.
{
	if (Info.MyUnit)
	{
		//적이 없다면?
		if (Chess_player::Piece_Enemy_ChessBoard.empty())
		{
			//내 위치 뱉음.
			AStar_Struct forReturn; 
			forReturn.X = Info.Position_X;
			forReturn.Y = Info.Position_Y;

			return forReturn;
		}
		float EnemyX, EnemyY, EnemyDistance = 999.f;
		for (auto itor : Chess_player::Piece_Enemy_ChessBoard)
		{
			float fX = (itor->Info.Position_X - Info.Position_X);
			float fY = (itor->Info.Position_Y - Info.Position_Y);
			if (fX < 0)
				fX *= -1;
			if (fY < 0)
				fY *= -1; //혹시 몰라서 하는 양수 초기화.

			float Dis = sqrt((fX * fX) + (fY * fY));
			if (Dis < EnemyDistance)
			{
				EnemyDistance = Dis;
				EnemyX = itor->Info.Position_X;
				EnemyY = itor->Info.Position_Y;
			}


			//Board_Array[(int)itor->Info.Position_X][(int)itor->Info.Position_Y] = true;
			Board_Array[(int)itor->Info.Move_X][(int)itor->Info.Move_Y] = true;

		}

		AStar_Struct forReturn;
		forReturn.X = (int)EnemyX;
		forReturn.Y = (int)EnemyY;

		Board_Array[forReturn.X][forReturn.Y] = false; //목적지는 block 풀어줌. (마지막에는 노드에 들어가야해서.)

		////Test용.
		//forReturn.X = 7;
		//forReturn.Y = 6;
		//Board_Array[forReturn.X][forReturn.Y] = false; //목적지는 block 풀어줌. (마지막에는 노드에 들어가야해서.)

		return forReturn;
	}

	else //적
	{
		//적이 없다면?
		if (Chess_player::Piece_Board.empty())
		{
			//내 위치 뱉음.
			AStar_Struct forReturn;
			forReturn.X = Info.Position_X;
			forReturn.Y = Info.Position_Y;

			return forReturn;
		}
		float EnemyX, EnemyY, EnemyDistance = 999.f;
		for (auto itor : Chess_player::Piece_Board)
		{
			float fX = (itor->Info.Position_X - Info.Position_X);
			float fY = (itor->Info.Position_Y - Info.Position_Y);
			if (fX < 0)
				fX *= -1;
			if (fY < 0)
				fY *= -1; //혹시 몰라서 하는 양수 초기화.

			float Dis = sqrt((fX * fX) + (fY * fY));
			if (Dis < EnemyDistance)
			{
				EnemyDistance = Dis;
				EnemyX = itor->Info.Position_X;
				EnemyY = itor->Info.Position_Y;
			}


			//Board_Array[(int)itor->Info.Position_X][(int)itor->Info.Position_Y] = true;
			Board_Array[(int)itor->Info.Move_X][(int)itor->Info.Move_Y] = true;

		}

		AStar_Struct forReturn;
		forReturn.X = (int)EnemyX;
		forReturn.Y = (int)EnemyY;

		Board_Array[forReturn.X][forReturn.Y] = false; //목적지는 block 풀어줌. (마지막에는 노드에 들어가야해서.)
		return forReturn;
	}
}

void Chess_Base::Find_Route(int Start_X, int Start_Y, int Route_X, int Route_Y)
{
	if (Start_X == Find_Close_Astar(Route_X, Route_Y)->Origin_X 
		&& Start_Y == Find_Close_Astar(Route_X, Route_Y)->Origin_Y) //Route_XY가 출발지점의 바로 앞 노드면 스톱.
	{
		Info.Move_X = Route_X;
		Info.Move_Y = Route_Y;
		return;
	}
	if (Find_Close_Astar(Route_X, Route_Y) != nullptr) //아니라면 재귀
	{
		Find_Route(Start_X, Start_Y, Find_Close_Astar(Route_X, Route_Y)->Origin_X, Find_Close_Astar(Route_X, Route_Y)->Origin_Y);
	}
}

void Chess_Base::Set_Rot(int _x, int _y)
{
	int X = _x - Info.Position_X;
	int Y = _y - Info.Position_Y;
	
	bool bX = true, bY= true;
	if (X < 0)
	{
		//X *= -1;
		bX = false;
	}
	if (Y < 0)
	{
		//Y *= -1;
		bY = false;
	}
	float Dig = (float)(atan2(Y, X) * 180 / 3.141592);
	if (Dig > 175 || Dig < -175)
	{
		Dig = -180;
	}

	TRANS()->LROT({ 0.f, 0, 90.f-Dig});

	//if (bX && bY)
	//	TRANS()->LROT({ 0.f, 0, (float)(atan2(X, Y) * 180 / 3.141592) });
	//else if (bX && !bY)
	//	TRANS()->LROT({ 0.f, 0, (float)(atan2(X, Y) * 180 / 3.141592) +90.f});
	//else if (!bX && bY)
	//	TRANS()->LROT({ 0.f, 0, (float)(atan2(X, Y) * 180 / 3.141592)  + 270.f});
	//else if (!bX && !bY)
	//	TRANS()->LROT({ 0.f, 0, (float)(atan2(X, Y) * 180 / 3.141592) +180.f });

}

void Chess_Base::IsBanch()
{
	if (Info.Position_Y != -2)
		Info.Banch = false;
	else
		Info.Banch = true;
}

void Chess_Base::Check_Death(bool Check)
{
	if (Info.Hp <= 0 || Check)
	{
		ACTOR()->Death();
		Info.Death = true;
		HP_Bar->Death();
		HP_BlackBar->Death();
		//MeshActor->Death();
	}
}

void Chess_Base::Death_Check()
{
	if (Info.Hp <= 0)
	{
		MeshActor->Death();
		ACTOR()->Death();
		Info.Death = true;
	}
}

void Chess_Base::Base_Update()
{
	Set_Ani();
}

void Chess_Base::Set_Ani()
{
	switch (Info.State)
	{
	case Chess_State::Attack_1:
		NewPtr->ChangeAni(L"Attack01");
		break;
	case Chess_State::Attack_2:
		NewPtr->ChangeAni(L"Attack02");
		break;
	case Chess_State::Born:
		NewPtr->ChangeAni(L"Born");
		break;
	case Chess_State::Death:
		NewPtr->ChangeAni(L"Death");
		break;
	case Chess_State::Dizzy:
		NewPtr->ChangeAni(L"Dizzy");
		break;
	case Chess_State::Idle:
		NewPtr->ChangeAni(L"Idle");
		break;
	case Chess_State::Jump:
		NewPtr->ChangeAni(L"Jump");
		break;
	case Chess_State::Run:
		NewPtr->ChangeAni(L"Run");
		break;
	case Chess_State::Skill01:
		NewPtr->ChangeAni(L"Skill01");
		break;
	case Chess_State::Victory:
		NewPtr->ChangeAni(L"Victory");
		break;
	default:
		NewPtr->ChangeAni(L"Idle");
		break;
	}
}