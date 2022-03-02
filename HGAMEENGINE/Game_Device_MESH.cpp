#include "Game_Device.h"
#include "HVBBUFFER.h"
#include "HIBBUFFER.h"
#include "DEFVTXHEADER.h"
#include "HMESH.h"

void Game_Device::MESHINIT() 
{
	// 박스 

	{
		std::vector<VTX2DIMGIN> m_Color;
		m_Color.resize(4);
		// 기본 매쉬를 만들때는 크기를 1로 만들어 놓는 경우가 대부분이다.
		m_Color[0].Pos = { -0.5f, 0.5f, 0.0f, 1.0f };
		m_Color[1].Pos = { 0.5f, 0.5f, 0.0f, 1.0f };
		m_Color[2].Pos = { 0.5f, -0.5f, 0.0f, 1.0f };
		m_Color[3].Pos = { -0.5f, -0.5f, 0.0f, 1.0f };

		m_Color[0].Uv = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_Color[1].Uv = { 1.0f, 0.0f, 0.0f, 1.0f };
		m_Color[2].Uv = { 1.0f, 1.0f, 0.0f, 1.0f };
		m_Color[3].Uv = { 0.0f, 1.0f, 0.0f, 1.0f };

		m_Color[0].Color = m_Color[1].Color = m_Color[2].Color = m_Color[3].Color = Game_Vector::WHITE;
		HVBBUFFER::Create<VTX2DIMGIN>(L"2DCOLORRECT", m_Color);

		// 시계방향으로 그린다는게 중요하다.
		// 뒷면은 안그리게 하는게 이 그려지는 방향에 의해서 결정된다.
		std::vector<UINT> m_IDX = {0, 1, 2, 0, 2, 3};
		HIBBUFFER::Create<UINT>(L"2DCOLORRECT", m_IDX);

		HMESH::Create(L"2DCOLORRECT");
		// HMESH::Create(L"2DCOLORRECT", L"2DCOLORRECT", L"2DCOLORRECT");
	}

	{
		std::vector<VTX3DMESHIN> m_Color;
		m_Color.resize(5);
		// 기본 매쉬를 만들때는 크기를 1로 만들어 놓는 경우가 대부분이다.
		m_Color[0].Pos = { -0.5f, 0.5f, -0.5f, 1.0f };
		m_Color[1].Pos = { 0.5f, 0.5f, -0.5f, 1.0f };
		m_Color[2].Pos = { 0.5f, -0.5f, -0.5f, 1.0f };
		m_Color[3].Pos = { -0.5f, -0.5f, -0.5f, 1.0f };
		m_Color[4].Pos = { 0.0f, 0.0f, 1.0f, 1.0f };

		m_Color[0].Normal = m_Color[0].Pos.NORMAL3DTORETURN();
		m_Color[1].Normal = m_Color[1].Pos.NORMAL3DTORETURN();
		m_Color[2].Normal = m_Color[2].Pos.NORMAL3DTORETURN();
		m_Color[3].Normal = m_Color[3].Pos.NORMAL3DTORETURN();
		m_Color[4].Normal = m_Color[4].Pos.NORMAL3DTORETURN();

		m_Color[0].Color = m_Color[1].Color = m_Color[2].Color = m_Color[3].Color = Game_Vector::WHITE;
		m_Color[4].Color = Game_Vector::RED;
		HVBBUFFER::Create<VTX3DMESHIN>(L"DIRMESH", m_Color);

		// 시계방향으로 그린다는게 중요하다.
		// 뒷면은 안그리게 하는게 이 그려지는 방향에 의해서 결정된다.
		std::vector<UINT> m_IDX = { 0, 1, 2, 0, 2, 3,  4, 1, 0 , 4, 2, 1, 4, 3, 2, 4, 0 , 3  };
		HIBBUFFER::Create<UINT>(L"DIRMESH", m_IDX);

		HMESH::Create(L"DIRMESH");
		// HMESH::Create(L"2DCOLORRECT", L"2DCOLORRECT", L"2DCOLORRECT");
	}

	{
		std::vector<VTX2DIMGIN> m_Color;
		m_Color.resize(4);
		// 기본 매쉬를 만들때는 크기를 1로 만들어 놓는 경우가 대부분이다.
		m_Color[0].Pos = { -0.5f, 0.0f, 0.5f, 1.0f };
		m_Color[1].Pos = { 0.5f, 0.0f, 0.5f, 1.0f };
		m_Color[2].Pos = { 0.5f, 0.0f, -0.5f, 1.0f };
		m_Color[3].Pos = { -0.5f, 0.0f, -0.5f, 1.0f };

		m_Color[0].Color = m_Color[1].Color = m_Color[2].Color = m_Color[3].Color = Game_Vector::WHITE;

		HVBBUFFER::Create<VTX2DIMGIN>(L"GRIDMESH", m_Color);
		// 시계방향으로 그린다는게 중요하다.
		// 뒷면은 안그리게 하는게 이 그려지는 방향에 의해서 결정된다.
		std::vector<UINT> m_IDX = { 0, 1, 2, 0, 2, 3 };
		HIBBUFFER::Create<UINT>(L"GRIDMESH", m_IDX);

		HMESH::Create(L"GRIDMESH");
		// HMESH::Create(L"2DCOLORRECT", L"2DCOLORRECT", L"2DCOLORRECT");
	}

	{
		std::vector<TARGETVTX> m_TVTX;
		m_TVTX.resize(4);
		// 기본 매쉬를 만들때는 크기를 1로 만들어 놓는 경우가 대부분이다.
		m_TVTX[0].Pos = { -1.0f, 1.0f, 0.0f, 1.0f };
		m_TVTX[1].Pos = { 1.0f, 1.0f, 0.0f, 1.0f };
		m_TVTX[2].Pos = { 1.0f, -1.0f, 0.0f, 1.0f };
		m_TVTX[3].Pos = { -1.0f, -1.0f, 0.0f, 1.0f };

		m_TVTX[0].Uv = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_TVTX[1].Uv = { 1.0f, 0.0f, 0.0f, 1.0f };
		m_TVTX[2].Uv = { 1.0f, 1.0f, 0.0f, 1.0f };
		m_TVTX[3].Uv = { 0.0f, 1.0f, 0.0f, 1.0f };

		HVBBUFFER::Create<TARGETVTX>(L"TargetFullRect", m_TVTX);
		// 시계방향으로 그린다는게 중요하다.
		// 뒷면은 안그리게 하는게 이 그려지는 방향에 의해서 결정된다.
		std::vector<UINT> m_IDX = { 0, 1, 2, 0, 2, 3 };
		HIBBUFFER::Create<UINT>(L"TargetFullRect", m_IDX);

		HMESH::Create(L"TargetFullRect");
	}

	{
		std::vector<TARGETVTX> m_TVTX;
		m_TVTX.resize(4);
		// 기본 매쉬를 만들때는 크기를 1로 만들어 놓는 경우가 대부분이다.
		m_TVTX[0].Pos = { -0.5f, 0.5f, 0.0f, 1.0f };
		m_TVTX[1].Pos = { 0.5f, 0.5f, 0.0f, 1.0f };
		m_TVTX[2].Pos = { 0.5f, -0.5f, 0.0f, 1.0f };
		m_TVTX[3].Pos = { -0.5f, -0.5f, 0.0f, 1.0f };

		m_TVTX[0].Uv = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_TVTX[1].Uv = { 1.0f, 0.0f, 0.0f, 1.0f };
		m_TVTX[2].Uv = { 1.0f, 1.0f, 0.0f, 1.0f };
		m_TVTX[3].Uv = { 0.0f, 1.0f, 0.0f, 1.0f };

		HVBBUFFER::Create<TARGETVTX>(L"TargetDebugRect", m_TVTX);
		// 시계방향으로 그린다는게 중요하다.
		// 뒷면은 안그리게 하는게 이 그려지는 방향에 의해서 결정된다.
		std::vector<UINT> m_IDX = { 0, 1, 2, 0, 2, 3 };
		HIBBUFFER::Create<UINT>(L"TargetDebugRect", m_IDX);

		HMESH::Create(L"TargetDebugRect");
	}

	

	{
		VTX3DMESHIN V;
		std::vector<VTX3DMESHIN> SphereVtx;
		std::vector<DWORD> SphereIdx;

		float fRadius = 0.5f;

		// 북극점
		V.Pos = Game_Vector{ 0.0f, fRadius, 0.0f, 1.0f };
		V.Uv = Game_Vector{ 0.5f, 0.0f };
		V.Color = Game_Vector{ 1.0f, 1.0f, 1.0f, 1.0f };
		V.Normal = V.Pos;
		V.Normal.NORMAL3D();
		V.Normal.w = 0.0f;
		V.Tangent = Game_Vector{ 1.0f, 0.0f, 0.0f, 0.0f };
		V.BiNormal = Game_Vector{ 0.0f, 0.0f, 1.0f, 0.0f };

		SphereVtx.push_back(V);

		UINT iStackCount = 40; // 가로 분할 개수
		UINT iSliceCount = 40; // 세로 분할 개수

		float yRotAngle = DirectX::XM_PI / (float)iStackCount;
		float zRotAngle = DirectX::XM_2PI / (float)iSliceCount;

		float yUvRatio = 1.0f / (float)iSliceCount;
		float zUvRatio = 1.0f / (float)iStackCount;

		// PointToDir

		for (UINT y = 1; y < iStackCount; ++y)
		{
			float phi = y * yRotAngle;

			for (UINT z = 0; z < iSliceCount + 1; ++z)
			{
				float theta = z * zRotAngle;

				V.Pos = Game_Vector
				{
					fRadius * sinf(y * yRotAngle) * cosf(z * zRotAngle),
					fRadius * cosf(y * yRotAngle),
					fRadius * sinf(y * yRotAngle) * sinf(z * zRotAngle),
					1.0f
				};


				V.Pos.w = 1.0f;
				V.Uv = Game_Vector{ yUvRatio * z, zUvRatio * y };
				V.Color = Game_Vector{ 1.0f, 1.0f, 1.0f, 1.0f };
				V.Normal = V.Pos;
				V.Normal.NORMAL3D();
				V.Normal.w = 0.0f;

				V.Tangent.x = -fRadius * sinf(phi) * sinf(theta);
				V.Tangent.y = 0.0f;
				V.Tangent.z = fRadius * sinf(phi) * cosf(theta);
				V.Tangent.NORMAL3D();
				V.Tangent.w = 0.0f;

				V.BiNormal = Game_Vector::Corss3D(V.Tangent, V.Normal);
				V.BiNormal.NORMAL3D();
				V.BiNormal.w = 0.0f;

				SphereVtx.push_back(V);
			}
		}

		// 남극점
		V.Pos = Game_Vector{ 0.0f, -fRadius, 0.0f, 1.0f };
		V.Uv = Game_Vector{ 0.5f, 1.0f };
		V.Color = Game_Vector{ 1.0f, 1.0f, 1.0f, 1.0f };
		V.Normal = V.Pos;
		V.Normal.NORMAL3D();
		V.Normal.w = 0.0f;
		SphereVtx.push_back(V);

		// 인덱스 버퍼 만들기
		SphereIdx.clear();
		for (UINT i = 0; i < iSliceCount; i++)
		{
			SphereIdx.push_back(0);
			SphereIdx.push_back(i + 2);
			SphereIdx.push_back(i + 1);
		}


		for (UINT y = 0; y < iStackCount - 2; ++y)
		{
			for (UINT z = 0; z < iSliceCount; ++z)
			{
				SphereIdx.push_back((iSliceCount + 1) * y + z + 1);
				SphereIdx.push_back((iSliceCount + 1) * (y + 1) + (z + 1) + 1);
				SphereIdx.push_back((iSliceCount + 1) * (y + 1) + z + 1);

				SphereIdx.push_back((iSliceCount + 1) * y + z + 1);
				SphereIdx.push_back((iSliceCount + 1) * y + (z + 1) + 1);
				SphereIdx.push_back((iSliceCount + 1) * (y + 1) + (z + 1) + 1);
			}
		}

		// 북극점 인덱스
		UINT iBotStart = (UINT)SphereVtx.size() - 1;

		for (UINT i = 0; i < iSliceCount; ++i)
		{
			SphereIdx.push_back(iBotStart);
			SphereIdx.push_back(iBotStart - (i + 2));
			SphereIdx.push_back(iBotStart - (i + 1));
		}

		HVBBUFFER::Create(L"SPHERE", SphereVtx);
		HIBBUFFER::Create(L"SPHERE", SphereIdx);
		HMESH::Create(L"SPHERE");
	}


}