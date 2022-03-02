#include "HHeightMap.h"
#include "HGAMEACTOR.h"
#include "DEFVTXHEADER.h"


void HHeightMap::Init(int& _X, int& _Z, const Game_String _MapHeightTex, int _Order)
{
	// Game_Vector Test = Game_Vector::Corss3D(Game_Vector::RIGHT, Game_Vector::FORWARD);

	Min = 0.0f;
	X = _X;
	Z = _Z;
	

	MapOption.XCount = (float)X;
	MapOption.ZCount = (float)Z;
	MapOption.YRatio = (float)1.0f;
	MapOption.YRatio = (float)10.0f;
	MapOption.YScale = TRANS()->WSCALE().y;

	m_Render = ACTOR()->CreateCom<Game_Renderer>(_Order);

	// 100, 100 500, 500

	m_Mesh = new HMESH();

	// 이건 미리 지정 되어야 한다.
	m_HTex = HTEXTURE::Find(_MapHeightTex);
	
	MapOption.PixelUv = Game_Vector(1.0f / m_HTex->Size().x, 1.0f / m_HTex->Size().y);
	MapOption.PixelScale = Game_Vector(TRANS()->WSCALE().x / m_HTex->Size().x, TRANS()->WSCALE().z / m_HTex->Size().y);
	Game_Ptr<HVBBUFFER> VB = new HVBBUFFER();
	Game_Ptr<HIBBUFFER> IB = new HIBBUFFER();

	m_VecVtx.resize((X + 1) * (Z + 1));
	// 기본 매쉬를 만들때는 크기를 1로 만들어 놓는 경우가 대부분이다.
	// 얼마씩 떨어져야 하나요?

	float StartX = -0.5f;
	float StartZ = 0.5f;

	for (int z = 0; z < Z + 1; z++)
	{
		for (int x = 0; x < X + 1; x++)
		{
			float PosZ = StartZ - (z * ((float)1 / Z));
			float PosX = StartX + (x * ((float)1 / X));

			int Index = z * (X + 1) + x;

			m_VecVtx[Index].Pos.x = PosX;
			m_VecVtx[Index].Pos.y = 0.0f;
			m_VecVtx[Index].Pos.z = PosZ;

			//Game_Vector Pos;
			//Pos = CalTexelPos(m_VecVtx[z * (X + 1) + x].Pos);
			//m_VecVtx[Index].Pos.y = m_HTex->GetPixel({ (int)Pos.x, (int)Pos.y }).x;

			// 여기서 계산해서 넣어줄 생각입니다
			m_VecVtx[Index].Uv.x = (float)x;
			m_VecVtx[Index].Uv.y = (float)z;
			m_VecVtx[Index].Normal = Game_Vector::UP;
		}
	}

	//for (int z = 0; z < Z; z++)
	//{
	//	for (int x = 0; x < X; x++)
	//	{
	//		int Index = z * (X + 1) + x;

	//		Game_Vector UpPos0;
	//		Game_Vector UpPos1;
	//		Game_Vector UpPos2;

	//		Game_Vector DownPos0;
	//		Game_Vector DownPos1;
	//		Game_Vector DownPos2;

	//		// 한번에 2개 삼각형 구합시다.

	//		// x가 홀수일때와
	//		// 짝수일때가 다르다.
	//		UpPos0 = m_VecVtx[Index].Pos;
	//		UpPos1 = m_VecVtx[Index + 1].Pos;
	//		UpPos2 = m_VecVtx[Index + (X + 1) + 1].Pos;

	//		Game_Vector Normal = Game_Vector::Corss3D((UpPos0 - UpPos1) , (UpPos0 - UpPos2));
	//		Normal.NORMAL3D();
	//		Normal.w = 0.0f;
	//		m_VecVtx[Index].Normal += Normal;
	//		m_VecVtx[Index].Normal = m_VecVtx[Index].Normal / 2.0f;
	//		m_VecVtx[Index].Normal.NORMAL3D();
	//		m_VecVtx[Index + 1].Normal += Normal;
	//		m_VecVtx[Index + 1].Normal = m_VecVtx[Index].Normal / 2.0f;
	//		m_VecVtx[Index + 1].Normal.NORMAL3D();
	//		m_VecVtx[Index + (X + 1) + 1].Normal += Normal;
	//		m_VecVtx[Index + (X + 1) + 1].Normal = m_VecVtx[Index + (X + 1) + 1].Normal / 2.0f;
	//		m_VecVtx[Index + (X + 1) + 1].Normal.NORMAL3D();

	//		DownPos0 = m_VecVtx[Index].Pos;
	//		DownPos1 = m_VecVtx[Index + (X + 1) + 1].Pos;
	//		DownPos2 = m_VecVtx[Index + (X + 1) - 1].Pos;

	//		Normal = Game_Vector::Corss3D((DownPos0 - DownPos1), (DownPos0 - DownPos2));
	//		Normal.NORMAL3D();
	//		Normal.w = 0.0f;
	//		m_VecVtx[Index].Normal += Normal;
	//		m_VecVtx[Index].Normal = m_VecVtx[Index].Normal / 2.0f;
	//		m_VecVtx[Index].Normal.NORMAL3D();
	//		m_VecVtx[Index + (X + 1) + 1].Normal += Normal;
	//		m_VecVtx[Index + (X + 1) + 1].Normal = m_VecVtx[Index + (X + 1) + 1].Normal / 2.0f;
	//		m_VecVtx[Index + (X + 1) + 1].Normal.NORMAL3D();
	//		m_VecVtx[Index + (X + 1) - 1].Normal += Normal;
	//		m_VecVtx[Index + (X + 1) - 1].Normal = m_VecVtx[Index + (X + 1) - 1].Normal / 2.0f;
	//		m_VecVtx[Index + (X + 1) - 1].Normal.NORMAL3D();
	//	}
	//}

	VB->Create(m_VecVtx.size(), sizeof(VTX3DMESHIN), &m_VecVtx[0]);



	std::vector<UINT> m_MapIdx;

	//     일단 면 개수죠?
	// m_MapIdx.resize(X * Z * 2 * 3);

	// int Pivot = 0;

	for (int z = 0; z < Z; z++)
	{
		for (int x = 0; x < X; x++)
		{
			int Pivot = z* (X + 1) + x;
			m_MapIdx.push_back(Pivot); // 0
			m_MapIdx.push_back(Pivot + 1); // 1
			m_MapIdx.push_back(Pivot + ( X + 1 ) + 1); // 2

			m_MapIdx.push_back(Pivot); // 0 
			m_MapIdx.push_back(Pivot + (X + 1) + 1); // 2
			m_MapIdx.push_back(Pivot + (X + 1)); // 3
		}
	}

	// 시계방향으로 그린다는게 중요하다.
	// 뒷면은 안그리게 하는게 이 그려지는 방향에 의해서 결정된다.
	IB->Create(m_MapIdx.size(), sizeof(UINT), &m_MapIdx[0], DXGI_FORMAT::DXGI_FORMAT_R32_UINT);
	m_Mesh->VB(VB);
	m_Mesh->IB(IB);

	m_Render->CreateRenderPlayer(m_Mesh, L"HeightMap");
	HTex(_MapHeightTex);
	m_Render->CBUFFER(L"MapOption", &MapOption, CBUFFERMODE::CB_LINK);
}

void HHeightMap::BaseTexture(const Game_String _TextureName) 
{
	m_Render->TEXTURE(L"BaseTex", _TextureName);
}

Game_Vector HHeightMap::CalTexelPos(const Game_Vector& _Pos)
{
	Game_Vector Size = m_HTex->Size();

	Game_Vector Vec;

	Vec.x = _Pos.x + 0.5f;
	Vec.y = (_Pos.z * - 1.0f) + 0.5f;

	Vec.x *= Size.x;
	Vec.y *= Size.y;
	return Vec;
}

void HHeightMap::HTex(const Game_String _TextureName) 
{
	m_Render->TEXTURE(L"HTex", _TextureName);

	m_YTileMap.clear();



	int HX = (X / 2);
	int HZ = (Z / 2);


	// PTR->SetPixel();

	// 몇 칸인지 알고 있죠?

	float InterX = (1.0f / (float)X);
	float InterZ = (1.0f / (float)Z);
	float HInterX = InterX * 0.5F;
	float HInterZ = InterZ * 0.5F;

	for (int z = -HZ; z < (HZ + 1); z++)
	{
		for (int x = -HX; x < (HX + 1); x++)
		{
			int2 Index;
			Index.x = x;
			Index.y = z;

			if (Index.x == 0 && Index.y == -10)
			{
				int a = 0;
			}

			float CenterX = (x * InterX);
			float CenterZ = (z * InterZ);

			m_YTileMap[Index.Key].Up.Arr[0] = { CenterX + -HInterX, 0.0f, CenterZ + HInterZ };
			m_YTileMap[Index.Key].Up.Arr[1] = { CenterX + HInterX, 0.0f, CenterZ + HInterZ };
			m_YTileMap[Index.Key].Up.Arr[2] = { CenterX + HInterX, 0.0f, CenterZ + -HInterZ };


			m_YTileMap[Index.Key].Down.Arr[0] = m_YTileMap[Index.Key].Up.Arr[0];
			m_YTileMap[Index.Key].Down.Arr[1] = m_YTileMap[Index.Key].Up.Arr[2];
			m_YTileMap[Index.Key].Down.Arr[2] = { CenterX + -HInterX, 0.0f, CenterZ + -HInterZ };

			Game_Vector Pos;
			Pos = CalTexelPos(m_YTileMap[Index.Key].Up.Arr[0]);
			m_YTileMap[Index.Key].Up.Arr[0].y = m_HTex->GetPixel({ (int)Pos.x, (int)Pos.y }).x * TRANS()->WSCALE().y;
			Pos = CalTexelPos(m_YTileMap[Index.Key].Up.Arr[1]);
			m_YTileMap[Index.Key].Up.Arr[1].y = m_HTex->GetPixel({ (int)Pos.x, (int)Pos.y }).x * TRANS()->WSCALE().y;
			Pos = CalTexelPos(m_YTileMap[Index.Key].Up.Arr[2]);
			m_YTileMap[Index.Key].Up.Arr[2].y = m_HTex->GetPixel({ (int)Pos.x, (int)Pos.y }).x * TRANS()->WSCALE().y;

			// 이 둘을 외적하면 하면
			//Game_Vector Dir1 = m_YTileMap[Index.Key].Up.Arr[0] - m_YTileMap[Index.Key].Up.Arr[1];
			//Game_Vector Dir2 = m_YTileMap[Index.Key].Up.Arr[0] - m_YTileMap[Index.Key].Up.Arr[2];


			Pos = CalTexelPos(m_YTileMap[Index.Key].Down.Arr[0]);
			m_YTileMap[Index.Key].Down.Arr[0].y = m_HTex->GetPixel({ (int)Pos.x, (int)Pos.y }).x * TRANS()->WSCALE().y;
			Pos = CalTexelPos(m_YTileMap[Index.Key].Down.Arr[1]);
			m_YTileMap[Index.Key].Down.Arr[1].y = m_HTex->GetPixel({ (int)Pos.x, (int)Pos.y }).x * TRANS()->WSCALE().y;
			Pos = CalTexelPos(m_YTileMap[Index.Key].Down.Arr[2]);
			m_YTileMap[Index.Key].Down.Arr[2].y = m_HTex->GetPixel({ (int)Pos.x, (int)Pos.y }).x * TRANS()->WSCALE().y;

			for (size_t i = 0; i < 3; i++)
			{
				if (Min > m_YTileMap[Index.Key].Up.Arr[i].y)
				{
					Min = m_YTileMap[Index.Key].Up.Arr[i].y;
				}
			}

			for (size_t i = 0; i < 3; i++)
			{
				if (Min > m_YTileMap[Index.Key].Down.Arr[i].y)
				{
					Min = m_YTileMap[Index.Key].Down.Arr[i].y;
				}
			}
		}
	}


}

int2 HHeightMap::Index(float4 _Pos)
{
	float4 Scale = TRANS()->WSCALE();

	float XSize = Scale.x / (float)X;
	float ZSize = Scale.z / (float)Z;

	int2 ReturnIndex;

	float IndexX = _Pos.x / XSize;
	float IndexZ = _Pos.z / ZSize;

	IndexX = roundf(IndexX);
	IndexZ = roundf(IndexZ);

	ReturnIndex.x = (int)IndexX;
	ReturnIndex.y = (int)IndexZ;

	return ReturnIndex;
}

Game_Vector HHeightMap::CalPosToWorld(const Game_Vector& _Pos) 
{
	Game_Vector ReturnX;
	Game_Vector Scale;
	Scale = TRANS()->WSCALE();

	ReturnX.x = _Pos.x * Scale.x;
	ReturnX.z = _Pos.z * Scale.z;
	ReturnX.y = _Pos.y;

	return ReturnX;
}

float HHeightMap::YHeight(float4 _Pos) 
{
	float4 Scale = TRANS()->WSCALE();

	// 위치도 중요한데.
	Scale = Scale.HalfVector();

	if (
		_Pos.z > Scale.z || 
		_Pos.z < -Scale.z ||
		_Pos.x > Scale.x ||
		_Pos.x < -Scale.x
		)
	{
		return Min;
	}

	Game_Vector Ori = _Pos;
	Ori.y = 0.0f;
	Game_Vector Dir = Game_Vector::UP;


	int2 Key = Index(_Pos);
	float YDis = Min;

	Game_Vector UpTri0 = CalPosToWorld(m_YTileMap[Key.Key].Up.Arr[0]);
	Game_Vector UpTri1 = CalPosToWorld(m_YTileMap[Key.Key].Up.Arr[1]);
	Game_Vector UpTri2 = CalPosToWorld(m_YTileMap[Key.Key].Up.Arr[2]);
	DirectX::TriangleTests::Intersects(Ori, Dir, UpTri0, UpTri1, UpTri2, YDis);
	if (YDis != Min)
	{
		return YDis;
	}

	Game_Vector DownTri0 = CalPosToWorld(m_YTileMap[Key.Key].Down.Arr[0]);
	Game_Vector DownTri1 = CalPosToWorld(m_YTileMap[Key.Key].Down.Arr[1]);
	Game_Vector DownTri2 = CalPosToWorld(m_YTileMap[Key.Key].Down.Arr[2]);
	DirectX::TriangleTests::Intersects(Ori, Dir, DownTri0, DownTri1, DownTri2, YDis);
	if (YDis != Min)
	{
		return YDis;
	}

	return Min;

}

float HHeightMap::GetMin() 
{
	return Min;
}