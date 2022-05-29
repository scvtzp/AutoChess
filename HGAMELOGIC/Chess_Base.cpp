#include "Chess_Base.h"
#include "Chess_player.h"
#include <HGAMETIME.h>

void Chess_Base::AStar()
{
	if (Info.MyUnit)
	{

		//0. Board_Array �ʱ�ȭ
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				Board_Array[i][j] = false; //���� �� ���� �޴� ��. true = ����
			}
		}

		//1. �� ��ġ ����.
		AStar_Struct* Start = new AStar_Struct;
		Start->X = Info.Position_X;
		Start->Y= Info.Position_Y;
		Start->GScore = 0;
		OpenList.emplace_back(Start);

		//2. �� ���� ã��.
		AStar_Struct Enemy = Find_Enemy();
		if (Enemy.X == -1 && Enemy.Y == -1) //���� ������ �� ����.
			return;
		float EnemyX = Enemy.X;
		float EnemyY = Enemy.Y;

		//�Ʊ� ���� �ִ�ĭ �������.
		for (auto iter : Chess_player::Piece_Board)
		{
			Board_Array[(int)iter->Info.Position_X][(int)iter->Info.Position_Y] = true;
		}

		//3. ����
		bool BWhile = true;
		while (BWhile)
		{
			//1. G�� ����.
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
							if (i == 0 || j == 0) //���� �ϳ��� 0�̶�� == �����¿�
							{
								if (Find_Close_Astar(itor->X + i, itor->Y + j)->GScore + 10 < G)
								{
									G = Find_Close_Astar(itor->X + i, itor->Y + j)->GScore + 10;
									itor->Origin_X = Find_Close_Astar(itor->X + i, itor->Y + j)->X;
									itor->Origin_Y = Find_Close_Astar(itor->X + i, itor->Y + j)->Y;
								}
							}
							else //�밢��
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

			//2. F�ּҰ� ������ ã��.
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
				else if (itor->FScore == F && itor->HScore < Fh) //�Ȱ��ٸ� H��������.
				{
					F = itor->FScore;
					Fx = itor->X;
					Fy = itor->Y;
					Fh = itor->HScore;
				}
			}
			CloseList.emplace_back(Find_Open_Astar(Fx, Fy)); //CL�� �ְ�
			OpenList.remove(Find_Open_Astar(Fx, Fy)); //OL���� ����.

			//3. �ֺ� ��� ����
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
						if (Fx + i == EnemyX && Fy + j == EnemyY) //Ż�� ����. �������� ����.
						{
							if (i == 0 || j == 0)
							{
								//CloseList.emplace_back(Find_Open_Astar(EnemyX, EnemyY)); ������ �����¿� �� ĭ�� ������ �ǵ��� ��.
								//OpenList.remove(Find_Open_Astar(EnemyX, EnemyY));
								BWhile = false;
								// 4. �������� Ž���Ͽ� ��ã�� �Ϸ�.
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

			//Ż��
			if (OpenList.empty()) //1.���� ��� ������ = �ٰ��� �� �ִ� ��� ��� Ž���Ϸ��ص� ��������.
				return;
		}


		//5. open, close, board ����.
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
		//0. Board_Array �ʱ�ȭ
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				Board_Array[i][j] = false; //���� �� ���� �޴� ��. true = ����
			}
		}
	
		//1. �� ��ġ ����.
		AStar_Struct* Start = new AStar_Struct;
		Start->X = Info.Position_X;
		Start->Y = Info.Position_Y;
		Start->GScore = 0;
		OpenList.emplace_back(Start);
	
		//2. �� ���� ã��.
		AStar_Struct Enemy = Find_Enemy();
		if (Enemy.X == -1 && Enemy.Y == -1) //���� ������ �� ����.
			return;
		float EnemyX = Enemy.X;
		float EnemyY = Enemy.Y;
	
		for (auto iter : Chess_player::Piece_Enemy_ChessBoard)
		{
			Board_Array[(int)iter->Info.Position_X][(int)iter->Info.Position_Y] = true;
		}

		//3. ����
		bool BWhile = true;
		while (BWhile)
		{
			//1. G�� ����.
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
							if (i == 0 || j == 0) //���� �ϳ��� 0�̶�� == �����¿�
							{
								if (Find_Close_Astar(itor->X + i, itor->Y + j)->GScore + 10 < G)
								{
									G = Find_Close_Astar(itor->X + i, itor->Y + j)->GScore + 10;
									itor->Origin_X = Find_Close_Astar(itor->X + i, itor->Y + j)->X;
									itor->Origin_Y = Find_Close_Astar(itor->X + i, itor->Y + j)->Y;
								}
							}
							else //�밢��
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
	
			//2. F�ּҰ� ������ ã��.
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
				else if (itor->FScore == F && itor->HScore < Fh) //�Ȱ��ٸ� H��������.
				{
					F = itor->FScore;
					Fx = itor->X;
					Fy = itor->Y;
					Fh = itor->HScore;
				}
			}
			CloseList.emplace_back(Find_Open_Astar(Fx, Fy)); //CL�� �ְ�
			OpenList.remove(Find_Open_Astar(Fx, Fy)); //OL���� ����.
	
			//3. �ֺ� ��� ����
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
						if (Fx + i == EnemyX && Fy + j == EnemyY) //Ż�� ����. �������� ����.
						{
							if (i == 0 || j == 0)
							{
								//CloseList.emplace_back(Find_Open_Astar(EnemyX, EnemyY)); ������ �����¿� �� ĭ�� ������ �ǵ��� ��.
								//OpenList.remove(Find_Open_Astar(EnemyX, EnemyY));
								BWhile = false;
								// 4. �������� Ž���Ͽ� ��ã�� �Ϸ�.
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
	
			//Ż��
			if (OpenList.empty()) //1.���� ��� ������ = �ٰ��� �� �ִ� ��� ��� Ž���Ϸ��ص� ��������.
				return;
		}
	
	
		//5. open, close, board ����.
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
	//��ġ�� �ִٸ�(�ʵ忡 ���ٸ�) ����
	if (Info.Banch)
		return;

	//�̵� ��
	else if (IsMove)
	{
		Info.State = Chess_State::Run;

		//Info.Position_X = Info.Move_X;
		//Info.Position_Y = Info.Move_Y;
		Info.Real_X += (Info.Move_X - Info.Position_X) * Game_Time::DeltaTime();
		Info.Real_Y += (Info.Move_Y - Info.Position_Y) * Game_Time::DeltaTime();

		Set_Rot(Info.Move_X, Info.Move_Y);

		if (Info.Real_X == Info.Move_X && Info.Real_Y == Info.Move_Y) //+-���� �������ϳ�?
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

	//��ų ���� ����
	else if (Info.Mana >= Info.MaxMana)
	{
		Info.Mana = 0;
		Info.Play_Skill = true;
		Skill_Init();
		return;
	}

	//��ų ���� ��
	else if (Info.Play_Skill)
	{
		Info.Mana = 0;
		if(Info.State != Chess_State::Skill02)
			Info.State = Chess_State::Skill01;
	}

	//���� ��
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
					if (itor->Info.Position_X == Enemy.X && itor->Info.Position_Y == Enemy.Y) //���� �ʿ�
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
					if (itor->Info.Position_X == Enemy.X && itor->Info.Position_Y == Enemy.Y) //���� �ʿ�
					{
						itor->Info.Hp -= Info.Atk;
						itor->Info.Mana += 5;
						Set_Rot(itor->Info.Position_X, itor->Info.Position_Y);
					}
				}
			}
		}
	}
	//�̵� ��� ����
	else
	{
		AStar();
		Info.Real_X = Info.Position_X;
		Info.Real_Y = Info.Position_Y;
		IsMove = true;
	}
}

float Chess_Base::Me_CloseEnemy_distance() //���� ����� ������ �Ÿ� ��ȯ.
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
				fY *= -1; //Ȥ�� ���� �ϴ� ��� �ʱ�ȭ.

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
				fY *= -1; //Ȥ�� ���� �ϴ� ��� �ʱ�ȭ.

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

AStar_Struct Chess_Base::Find_Enemy() //���� ����� �� ����.
{
	if (Info.MyUnit)
	{
		//���� ���ٸ�?
		if (Chess_player::Piece_Enemy_ChessBoard.empty())
		{
			//�� ��ġ ����.
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
				fY *= -1; //Ȥ�� ���� �ϴ� ��� �ʱ�ȭ.

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

		Board_Array[forReturn.X][forReturn.Y] = false; //�������� block Ǯ����. (���������� ��忡 �����ؼ�.)

		////Test��.
		//forReturn.X = 7;
		//forReturn.Y = 6;
		//Board_Array[forReturn.X][forReturn.Y] = false; //�������� block Ǯ����. (���������� ��忡 �����ؼ�.)

		return forReturn;
	}

	else //��
	{
		//���� ���ٸ�?
		if (Chess_player::Piece_Board.empty())
		{
			//�� ��ġ ����.
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
				fY *= -1; //Ȥ�� ���� �ϴ� ��� �ʱ�ȭ.

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

		Board_Array[forReturn.X][forReturn.Y] = false; //�������� block Ǯ����. (���������� ��忡 �����ؼ�.)
		return forReturn;
	}
}

void Chess_Base::Find_Route(int Start_X, int Start_Y, int Route_X, int Route_Y)
{
	if (Start_X == Find_Close_Astar(Route_X, Route_Y)->Origin_X 
		&& Start_Y == Find_Close_Astar(Route_X, Route_Y)->Origin_Y) //Route_XY�� ��������� �ٷ� �� ���� ����.
	{
		Info.Move_X = Route_X;
		Info.Move_Y = Route_Y;
		return;
	}
	if (Find_Close_Astar(Route_X, Route_Y) != nullptr) //�ƴ϶�� ���
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

void Chess_Base::Update()
{
	if (HP_Bar == nullptr) //init�� �ȵ��� ��, �ε����϶�
		return;

	MeshActor->TRANS()->LPOS({ Info.Real_X, 0.f , Info.Real_Y });
	MeshActor->TRANS()->LROT({ -90.f , TRANS()->LROT().y,  TRANS()->LROT().z });
	
	if(EffectActor != nullptr) //����Ʈ���ʹ� ���¾ֵ� ����.
		EffectActor->TRANS()->LPOS({ Info.Real_X, 0.02f, Info.Real_Y });

	Death_Check();
	Set_Ani();

	float Hp_Scale = Info.Hp / Info.MaxHp * 0.8f;
	HP_Bar->TRANS()->LSCALE({ Hp_Scale , 0.15f, 1.f });

	IsBanch();

	//�̵� (��ġ�� ������ �н�)
	if (!Info.Banch)
	{
		HPBar_Act->On();
		Move(); //����Ե� move�� �̵� �̿ܿ� �����Ǻ������̶� playskill�� �ٲ���.
		HP_Bar->TRANS()->LPOS({ Info.Real_X - (0.8f - Hp_Scale) / 2, 1.f, Info.Real_Y });
		HP_BlackBar->TRANS()->LPOS({ Info.Real_X, 1.f, Info.Real_Y });
	}
	else
	{
		HPBar_Act->Off();
	}

	if (Info.Play_Skill)
		Skill_Update(); //init�� ��ų���·� �������ٶ� MOve���� ���������.
}

void Chess_Base::IsBanch()
{
	if (Info.Position_Y != -2)
	{
		HP_Bar->On();
		HP_BlackBar->On();

		Info.Banch = false;
	}
	else
	{
		HP_Bar->Off();
		HP_BlackBar->Off();

		Info.Banch = true;
	}
}

void Chess_Base::Death_Check()
{
	if (Info.Hp <= 0)
	{
		if (Chess_player::CanDeath)
		{
			if(MeshActor != nullptr)
				MeshActor->Death();
			if (EffectActor != nullptr)
				EffectActor->Death();
			if(HPBar_Act != nullptr)
				HPBar_Act->Death();
			ACTOR()->Death();
			Info.Death = true;

			if (Info.MyUnit)
			{
				Chess_player::Piece_Board.remove(this);
			}
			else
			{
				Chess_player::Piece_Enemy_ChessBoard.remove(this);
			}
		}
		else
		{
			Info.Hp = Info.MaxHp; //Ǯ�Ƿ� ȸ�� (�׽�Ʈ��)
		}
	}
}

void Chess_Base::Skill()
{
}

void Chess_Base::Skill_Init()
{
}

void Chess_Base::Skill_Update()
{
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
	case Chess_State::Skill02:
		NewPtr->ChangeAni(L"Skill02");
		break;
	case Chess_State::Victory:
		NewPtr->ChangeAni(L"Victory");
		break;
	default:
		NewPtr->ChangeAni(L"Idle");
		break;
	}
}

void Chess_Base::Make_HpBar()
{
	float4 CUTDATA = { 0,0,1,1 };
	float4 DRAWCOLOR = { 1,1,1,0.5f }; //50%������
	Game_Vector Scale = { 0.7f, 0.12f, 1.f };

	HPBar_Act = SCENE()->CreateActor();

	//����/�ʷ� ü�¹� ����
	{
		//Game_Ptr<Game_Actor> Act = SCENE()->CreateActor();
		HP_Bar = HPBar_Act->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Default);
		HP_Bar->TRANS()->LSCALE(Scale);
		HP_Bar->TRANS()->LROT({ 90.f, 0.f, 0.f });

		HP_Bar->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
		HP_Bar->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);
		if (Info.MyUnit)
			HP_Bar->TEXTURE(L"Tex", L"Green.png");
		else
			HP_Bar->TEXTURE(L"Tex", L"Red.png");
		HP_Bar->SAMPLER(L"Smp", "LWSMP");
	}
	//ü�¹� ���� �޹�� ����
	{
		//Game_Ptr<Game_Actor> Act1 = SCENE()->CreateActor();
		HP_BlackBar = HPBar_Act->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Default);
		HP_BlackBar->TRANS()->LSCALE(Scale);
		HP_BlackBar->TRANS()->LROT({ 90.f, 0.f, 0.f });

		HP_BlackBar->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
		HP_BlackBar->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);
		HP_BlackBar->TEXTURE(L"Tex", L"Black.png");
		HP_BlackBar->SAMPLER(L"Smp", "LWSMP");
	}
}