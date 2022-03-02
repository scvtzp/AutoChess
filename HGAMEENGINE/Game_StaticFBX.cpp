//#include "LejFbxLoader.h"
//#include "LejEngineDebug.h"
//#include "LejMesh.h"
//#include "LejRenderer.h"
#include "Game_StaticFBX.h"
#include "HMESH.h"

Game_StaticFBX::Game_StaticFBX() : MeshCount(0)
{
}

Game_StaticFBX::~Game_StaticFBX()
{
	if (nullptr != m_Imp)
	{
		m_Imp->Destroy();
		m_Imp = nullptr;
	}

	if (nullptr != m_Scene)
	{
		m_Scene->Destroy();
		m_Scene = nullptr;
	}

	if (nullptr != m_Ios)
	{
		m_Ios->Destroy();
		m_Ios = nullptr;
	}

	if (nullptr != m_Mgr)
	{
		m_Mgr->Destroy();
		m_Mgr = nullptr;
	}

}

/* MeshInit()에서 하는 일을 여기서 그대로 합니다.  */
void Game_StaticFBX::CreateMesh()
{
	if (m_FbxVtx.size() != m_FbxIdx.size())
	{
		AMSG(L"인덱스 버퍼 버텍스 버퍼 매칭 오류");
		return;
	}

	m_VB.resize(m_FbxVtx.size());
	m_IB.resize(m_FbxIdx.size());

	/* 프레임워크 방식으로 관리합니다. */
	for (size_t i = 0; i < m_FbxVtx.size(); i++)
	{
		Game_Ptr<HVBBUFFER> NewVB = new HVBBUFFER();
		NewVB->Create(m_FbxVtx[i].size(), sizeof(VTX3DMESHIN), &m_FbxVtx[i][0], D3D11_USAGE::D3D11_USAGE_DEFAULT);
		m_VB.push_back(NewVB);

		for (size_t j = 0; j < m_FbxIdx[i].size(); j++)
		{
			Game_Ptr<HIBBUFFER> NewIB = new HIBBUFFER();
			NewIB->Create(m_FbxIdx[i][j].size(), sizeof(UINT), &m_FbxIdx[i][j][0], DXGI_FORMAT::DXGI_FORMAT_R32_UINT, D3D11_USAGE::D3D11_USAGE_DEFAULT);
			m_IB[i].push_back(NewIB);

			HMESH* NewMesh = new HMESH();

			NewMesh->VB(NewVB);
			NewMesh->IB(NewIB);

			m_Mesh.push_back(NewMesh);
		}
	}

	/* 프레임워크의 구조는 Mesh 1 : Material 1 입니다. */
	if (m_Mesh.size() != m_UserMaterialData.size())
	{
		AMSG(L"매쉬와 메테리얼의 크기가 다릅니다.");
		return;
	}

	// LejGameDebug::ConsolePrintText(L"버텍스 및 인덱스 버퍼 버퍼 매쉬생성 완료");
}



// 어떤 게임의 메쉬구조가 ....
// 어떤 게임의 FBX파일이 배치된 디렉터리 구조가 
// 이렇게 되어있을 수 있다. 
// PLAYER.FBX
// PLAYER_ATTACK.FBX
// PLAYER_IDLE.FBX
// 
// 혹은
// MOSTER.FBX
// 통으로 이렇게 되어있고
// 그 안에 ANIDATA가 여러개 있는 식으로 존재 할 가능성이 있다. 
// 
// FBX란 것은 본 과 메쉬로 되어있다. 
// 프레임워크의 분류에 맞게 되어있지 않다면, LOAD를 하나 더 만들면 된다. 


// 지금은
// 애니메이션 부분  
// 메쉬 부분 
// 각각 만들어서 로드한다. 


void Game_StaticFBX::Load()
{
	// LejGameDebug::ConsolePrintText(L"LoadStart");
	ReMat.SetRow(0, FbxVector4{ 1.0, 0.0, 0.0, 0.0 });
	ReMat.SetRow(1, FbxVector4{ 0.0, 0.0, 1.0, 0.0 });
	ReMat.SetRow(2, FbxVector4{ 0.0, 1.0, 0.0, 0.0 });
	ReMat.SetRow(3, FbxVector4{ 0.0, 0.0, 0.0, 1.0 });

	m_Mgr = fbxsdk::FbxManager::Create();
	m_Ios = fbxsdk::FbxIOSettings::Create(m_Mgr, IOSROOT);
	m_Mgr->SetIOSettings(m_Ios);
	m_Scene = fbxsdk::FbxScene::Create(m_Mgr, "");
	m_Imp = fbxsdk::FbxImporter::Create(m_Mgr, "");

	if (false == m_Imp->Initialize(CW2A(GameFile.FullPath(), CP_UTF8).m_psz, -1, m_Mgr->GetIOSettings()))
	{
		AMSG(GameFile.FullPath() + " FBX 파일 이니셜라이즈 실패");
	}

	/* 기본 환경 구성입니다. */
	m_Imp->Import(m_Scene);

	eLocalSpaceType = m_Scene->GetGlobalSettings().GetAxisSystem();

	if (eLocalSpaceType == fbxsdk::FbxAxisSystem::eDirectX)
	{
		m_Scene->GetGlobalSettings().SetAxisSystem(fbxsdk::FbxAxisSystem::eMax);
	}
	else if (eLocalSpaceType != fbxsdk::FbxAxisSystem::eMax)
	{
		m_Scene->GetGlobalSettings().SetAxisSystem(fbxsdk::FbxAxisSystem::eMax);
	}

	/* 애니메이션 체크를 합니다. 로드 한 것의 구성을 알 수 있습니다. 애니메이션 파일의 갯수를 알 수 있습니다.  */
	AniCheck();
	// LejGameDebug::ConsolePrintText(L"애니메이션 체크 완료 애니메이션 개수 : %d", m_UserAniData.size());

	int MatCount = 0;
	/* 삼각화는 ROOT를 기준으로 버텍스들을 재조정 하는 것입니다. 각각 분할된 이미지들을 줄을 세워 정상적으로 보이게 합니다. */
	Trangulate(m_Scene->GetRootNode(), MatCount);
	// LejGameDebug::ConsolePrintText(L"삼각화 완료");
	m_UserMaterialData.resize(MatCount);
	MatCount = 0;
	MaterialCheck(m_Scene->GetRootNode(), MatCount);
	// LejGameDebug::ConsolePrintText(L"머티리얼 체크 완료 %d", MatCount);

	/* 경로를 커스컴 경로로 바꾸어줍니다. */
	for (size_t i = 0; i < m_UserMaterialData.size(); i++)
	{
		Game_String DirPath = GameFile.DirPath();
		m_UserMaterialData[i].DifTexture.EraseStr(DirPath);
	}

	MeshVtxData(m_Scene->GetRootNode());

	/* 본을 체크합니다. 본의 구조를 알 수 있습니다. */
	BoneCheck();

	/* 메쉬 형식이 달라야 하는 경우, 예를들면 static 매쉬에 해당한다면, 기존과는 다르게 메쉬를 생성해야 합니다.
	m_UserAniData를 계산할 필요가 없습니다. */
	if (0 >= m_UserAniData.size())
	{
		CreateMesh();
		// LejGameDebug::ConsolePrintText(L" 매쉬 완료");
		// LejGameDebug::ConsolePrintText(L" static 매쉬 완료");
		return;
	}

	size_t I = m_UserAniData.size();
	//LejGameDebug::ConsolePrintText(L"본 체크 완료 본 개수 : %d\n", m_UserBoneData.size());

/* 애니메이션은 1개입니다. 본은 134개입니다. */
	for (size_t AniIndex = 0; AniIndex < m_UserAniData.size(); AniIndex++)
	{
		m_UserAniData[AniIndex].m_AniFrameData.resize(m_UserBoneData.size());

		for (size_t BoneCount = 0; BoneCount < m_UserBoneData.size(); BoneCount++)
		{
			size_t I = m_UserBoneData.size();
			/* 1434프레임 만큼 확장합니다. */
			m_UserAniData[AniIndex].m_AniFrameData[BoneCount].m_Data.resize(m_UserAniData[AniIndex].FrameCount + 1);
		}
	}

	/* 스키닝을 안하는 static mesh라도 가중치와 인덱스가 없는 것이 아닙니다. 주의하세요 */
	m_MapWI.resize(m_FbxVtx.size());
	/* ROOT가 여러개 일수 있습니다. 조심합니다. */
	MeshCount = 0;

	/* 연산이 많은 함수 입니다. 체크할 요소들이 단순히 많기 때문입니다. for문을 많이 있고, 재귀도 있습니다. 본x본프레임(134 * 1434)도 있습니다. */
	/* 따라서 자체포맷을 합니다. 쓰레드에서 로딩을 합니다. */
	AnimationClipCheck(m_Scene->GetRootNode());
	MeshCount = 0;

	/* 인덱스와 가중치를 사용했습니다. 지워줍니다. */
	CalWeightAndIndex();
	m_MapWI.clear();

	/* 이름으로 관리할 수 있게 준비합니다. */
	for (size_t i = 0; i < m_UserAniData.size(); i++)
	{
		m_UserAniDataMap[m_UserAniData[i].AniName] = &m_UserAniData[i];
	}
	//LejGameDebug::ConsolePrintText(L" Ani 매쉬 완료");

/* 최종적으로 메쉬를 세팅합니다. */
	CreateMesh();
}


void Game_StaticFBX::MeshVtxData(fbxsdk::FbxNode* _Node)
{
	fbxsdk::FbxNodeAttribute* _Att = _Node->GetNodeAttribute();

	/* 재귀를 합니다. 버텍스가 여러개라서가 아닙니다. 어느노드를 가지고 있는지 알 수 없기 때문입니다. */
	if (nullptr != _Att &&
		(_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh ||
			_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbs ||
			_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbsSurface)
		)
	{
		if ((_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbs ||
			_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbsSurface))
		{
			int a = 0;
		}

		FbxMesh* _Mesh = _Node->GetMesh();

		if (nullptr == _Mesh)
		{
			AMSG(L"if (nullptr == _Mesh)");
		}

		VtxData(_Mesh);
	}

	int ChildCount = _Node->GetChildCount();

	for (int i = 0; i < ChildCount; i++)
	{
		MeshVtxData(_Node->GetChild(i));
	}
}


void Game_StaticFBX::VtxData(fbxsdk::FbxMesh* _Mesh)
{
	m_FbxVtx.push_back(std::vector<VTX3DMESHIN>());
	m_FbxIdx.push_back(std::vector<std::vector<UINT>>());

	std::vector<VTX3DMESHIN>& VtxData = m_FbxVtx[m_FbxVtx.size() - 1];
	std::vector<std::vector<UINT>>& IdxData = m_FbxIdx[m_FbxIdx.size() - 1];

	int iVtxCount = _Mesh->GetControlPointsCount();

	// 익히고 알고 있는 LocalSpace에 존재하는 Postion정보의 첫번째 포인터
	// 위치정보의 첫번째 포인터이다.
	FbxVector4* pVtxPos = _Mesh->GetControlPoints();

	VtxData.resize(iVtxCount);

	// 맥스식으로 삼각화 했기 때문에
	for (int i = 0; i < VtxData.size(); i++)
	{
		// 약간 이해하기 힘든게 0 2 1로 받아오는건데
		// 맥스 공간에 뒤집어진 z와 y를 표현하기 위해서 처리.
		// 정점==점의 위치정보
		VtxData[i].Pos.x = (float)pVtxPos[i].mData[0];
		VtxData[i].Pos.y = (float)pVtxPos[i].mData[2];
		VtxData[i].Pos.z = (float)pVtxPos[i].mData[1];
		VtxData[i].Pos.w = 1.0f;
		// HGAMEDEBUG3D::ConsoleText(L"%d = %f, %f, %f", i, VtxData[i].Pos.x, VtxData[i].Pos.y, VtxData[i].Pos.z);
	}


	UINT VtxId = 0;

	// 이 매쉬를 그리는데 사용한 재질 정보
	FbxGeometryElementMaterial* MatData = _Mesh->GetElementMaterial();
	// 그리는데 사용한 재질개수
	// 지금 로드하고 있는 데이터를 몇개의 재질로 그렸느냐?
	int MatCount = _Mesh->GetNode()->GetMaterialCount();

	IdxData.resize(MatCount);

	// 삼각형
	int iPolygonCount = _Mesh->GetPolygonCount();

	// std::vector<std::list<UINT>> m_List;
	// m_List.resize(MatCount);

	// 삼각형 개수만큼 돌면서
	for (int PolgonIndex = 0; PolgonIndex < iPolygonCount; PolgonIndex++)
	{
		// 대부분 3일것이다.
		int iPolyGonSize = _Mesh->GetPolygonSize(PolgonIndex);
		if (3 != iPolyGonSize)
		{
			AMSG(L"삼각형이 아닌 면이 발견됐습니다.");
		}

		int iIDX[3] = {};
		// 그 삼각형에 필요한 
		for (int TriIndex = 0; TriIndex < iPolyGonSize; TriIndex++)
		{
			//                                      0번째 삼각형의    0~2번째 점을 저에게 주세요.
			// 그녀석은 17641개의 정점중 5번째 정점이야.
			// +그리는 순서이기도 하다.
			int iControlIndex = _Mesh->GetPolygonVertex(PolgonIndex, TriIndex);

			iIDX[TriIndex] = iControlIndex;

			LoadNormal(_Mesh, VtxData, VtxId, iControlIndex);
			// LoadUv(_Mesh, VtxData, VtxId_Mesh->GetTextureUVIndex(PolgonIndex, TriIndex), iControlIndex);
			LoadUv(_Mesh, VtxData, VtxId, iControlIndex);
			LoadTangent(_Mesh, VtxData, VtxId, iControlIndex);
			LoadBinormal(_Mesh, VtxData, VtxId, iControlIndex);

			++VtxId;
		}

		int iMtrID = MatData->GetIndexArray().GetAt(PolgonIndex);


		// 인덱스 버퍼 
		IdxData[iMtrID].push_back(iIDX[0]);
		IdxData[iMtrID].push_back(iIDX[2]);
		IdxData[iMtrID].push_back(iIDX[1]);
	}

	//for (size_t i = 0; i < m_List.size(); i++)
	//{
	//	for (auto& _Index : m_List[i])
	//	{
	//		IdxData[i].push_back(_Index);
	//	}
	//}

	int a = 0;
}

//        x    y    z   w
// weight 0.5  0.5  
// index  6    5




// ANIDATA 하나 
// 본은 134개 
// 본프레임은 1434개 (본마다 프레임애니메이션을 의미함)
// 프레임 동작이 1434개임 
// FPS = 30

// 행렬
// M1 M2 M3 M4 M5 M6 M7 M8 M9 M10
// □ □ □ □ □ □ □ □ □ □ 
// 이것하나하나는 애니메이션 본 프레임이다. 
// EX) 플레이어 팔이 회전할 떄 
// 본
// 본프레임
// resize



// 노드 
// 재귀
void Game_StaticFBX::AnimationClipCheck(fbxsdk::FbxNode* _Node)
{
	fbxsdk::FbxNodeAttribute* _Att = _Node->GetNodeAttribute();

	// 정점정보가 여러개일거라서 이렇게 하는게 아니고.
	// 어느 노드가 가지고 있는지를 모르기 때문에 이렇게 하는것이다.
	if (nullptr != _Att &&
		(_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh ||
			_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbs ||
			_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbsSurface)
		)
	{
		if ((_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbs ||
			_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbsSurface))
		{
			int a = 0;
		}

		FbxMesh* _Mesh = _Node->GetMesh();

		if (nullptr == _Mesh)
		{
			AMSG(L"if (nullptr == _Mesh)");
		}


		// 1434(개) x 133(개) 의 정보를 채워넣는 곳 
		// 시간 대비 0,1,2,3,4,5,6......
		// 행렬을 채워주고 있다. 

		CalAnimationClip_Static(_Mesh);
	}

	int ChildCount = _Node->GetChildCount();

	// 재귀 
	for (int i = 0; i < ChildCount; i++)
	{
		AnimationClipCheck(_Node->GetChild(i));
	}
}





void Game_StaticFBX::CalAnimationClip_Static(fbxsdk::FbxMesh* _Mesh)
{
	// 이녀석이 없다고 애니메이션이 없는건 아니다.
	int iSkinCount = _Mesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
	// 이게 0이라고 애니메이션이 없는건 아니다.

	if (iSkinCount <= 0)
	{
		return;
	}

	int VtxCount = (int)m_FbxVtx[MeshCount].size();

	// 애니메이션중에 

	// 매쉬의 기본 매트릭스를 가져온다.
	// 소스 그대로 가져오는 
	fbxsdk::FbxNode* Node = _Mesh->GetNode();
	const FbxVector4 vS = Node->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot);
	const FbxVector4 vR = Node->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot);
	const FbxVector4 vT = Node->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot);

	// 회전 요소는 쿼터니온임을 기억하자  
	FbxAMatrix matTrans = FbxAMatrix(vT, vR, vS);

	for (int i = 0; i < iSkinCount; i++)
	{
		FbxSkin* pSkin = (FbxSkin*)_Mesh->GetDeformer(i, fbxsdk::FbxDeformer::eSkin);

		if (!pSkin)
		{
			continue;
		}

		FbxSkin::EType eSkinningType = pSkin->GetSkinningType();

		// eNerb
		// eSkinningType == FbxSkin::eRigid
		if (eSkinningType == FbxSkin::eRigid ||
			eSkinningType == FbxSkin::eLinear)
		{
			//if (eSkinningType == FbxSkin::eRigid)
			//{
			//	AMSG(L"물리 기반 애니메이션 입니다.");
			//}

			// 관절
			int iClusterCount = pSkin->GetClusterCount();

			for (int j = 0; j < iClusterCount; j++)
			{
				fbxsdk::FbxCluster* pCluster = pSkin->GetCluster(j);

				if (nullptr == pCluster->GetLink())
				{
					continue;
				}

				Game_String StrBoneName = CA2W(pCluster->GetLink()->GetName()).m_psz;

				LejFbxBoneData* pBone = FindBone(StrBoneName);

				if (nullptr == pBone)
				{
					continue;
				}

				// 인덱스와 가중치를 모으기만 한다. 재계산은 하지 않은 상태이다. 
				// 애니메이션이 있어야 가중치와 인덱스가 의미가 있다. 
				// 스키닝이  있다. 없다.를 판별하자. 

				LoadWeightAndIndex(pCluster, pBone->iIndex, m_FbxVtx[MeshCount]);

				// 변경해서 주의해야함
				// 좌표계를 바꾸는 함수 
				LoadOffsetMatrix(pCluster, matTrans, pBone);

				// 본프레임이 가지고 있는 시간-속성을 채워준다. 
				LoadTimeTransForm(Node, pCluster, matTrans, pBone);
			}

		}

	}

	++MeshCount;

}





void Game_StaticFBX::LoadUv(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
{
	int iCount = _Mesh->GetElementUVCount();

	if (0 == iCount)
	{
		AMSG(L"UV가 여러개 입니다.");
	}

	FbxGeometryElementUV* pElement = _Mesh->GetElementUV();
	int iDataIndex = VtxId;
	// 폴리곤 기반으로 로드한 녀석이냐.
	// 이 방식으로 만든것이다. 
	if (pElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(VtxId);
		}
		else if (FbxGeometryElement::eDirect == pElement->GetReferenceMode())
		{
			iDataIndex = _Index;
		}
		else
		{
			iDataIndex = VtxId;
			// AMSG(L"맵핑 방식중 처리할수 없는 방식입니다.");
		}
	}
	// 17641으로 됐느냐.
	else if (pElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (FbxGeometryElement::eDirect == pElement->GetReferenceMode())
		{
			iDataIndex = _Index;
		}
		else if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(_Index);
		}
		else {
			AMSG(L"맵핑 방식중 처리할수 없는 방식입니다.");
		}
	}

	// 노말은 가져왔고.
	FbxVector2 vUV = pElement->GetDirectArray().GetAt(iDataIndex);

	_ArrVtx[_Index].Uv.x = (float)(vUV.mData[0] - (int)vUV.mData[0]);
	_ArrVtx[_Index].Uv.y = (float)(1.0f - (vUV.mData[1] - (int)vUV.mData[1]));


	// _ArrVtx[_Index].Uv.x = (float)vUV.mData[0];
	// _ArrVtx[_Index].Uv.y = 1.0f - (float)vUV.mData[1];
	// Y는 뒤집는다.

	// _ArrVtx[_Index].BiNormal.z = (float)BiNormal.mData[1];

}




void Game_StaticFBX::LoadBinormal(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
{
	int iCount = _Mesh->GetElementBinormalCount();

	if (0 == iCount)
	{
		return;
	}


	// FbxGeometryElementUserData/* pElement = _Mesh->GetElementTangent();
	// _Mesh->GetElementBinormalCount();
	FbxGeometryElementBinormal* pElement = _Mesh->GetElementBinormal();
	int iDataIndex = VtxId;

	// 폴리곤 기반으로 로드한 녀석이냐.
	// 이 방식으로 만든것이다. 
	if (pElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(VtxId);
		}
		else
		{
			iDataIndex = VtxId;
			// AMSG(L"맵핑 방식중 처리할수 없는 방식입니다.");
		}
	}
	// 17641으로 됐느냐.
	else if (pElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (FbxGeometryElement::eDirect == pElement->GetReferenceMode())
		{
			iDataIndex = _Index;
		}
		else if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(_Index);
		}
		else {
			AMSG(L"맵핑 방식중 처리할수 없는 방식입니다.");
		}
	}

	// 노말은 가져왔고.
	FbxVector4 BiNormal = pElement->GetDirectArray().GetAt(iDataIndex);

	_ArrVtx[_Index].BiNormal.x = (float)BiNormal.mData[0];
	_ArrVtx[_Index].BiNormal.y = (float)BiNormal.mData[2];
	_ArrVtx[_Index].BiNormal.z = (float)BiNormal.mData[1];
	_ArrVtx[_Index].BiNormal.NORMAL3D();
}





void Game_StaticFBX::LoadTangent(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
{
	int iCount = _Mesh->GetElementTangentCount();

	if (0 == iCount)
	{
		return;
	}
	// FbxGeometryElementUserData/* pElement = _Mesh->GetElementTangent();
	FbxGeometryElementTangent* pElement = _Mesh->GetElementTangent();
	int iDataIndex = VtxId;

	// 폴리곤 기반으로 로드한 녀석이냐.
	// 이 방식으로 만든것이다. 
	if (pElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(VtxId);
		}
		else
		{
			iDataIndex = VtxId;
			// AMSG(L"맵핑 방식중 처리할수 없는 방식입니다.");
		}
	}
	// 17641으로 됐느냐.
	else if (pElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (FbxGeometryElement::eDirect == pElement->GetReferenceMode())
		{
			iDataIndex = _Index;
		}
		else if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(_Index);
		}
		else {
			AMSG(L"맵핑 방식중 처리할수 없는 방식입니다.");
		}
	}

	// 노말은 가져왔고.
	FbxVector4 Tangent = pElement->GetDirectArray().GetAt(iDataIndex);

	// 길이가 1이 아닐수 있다.
	_ArrVtx[_Index].Tangent.x = (float)Tangent.mData[0];
	_ArrVtx[_Index].Tangent.y = (float)Tangent.mData[2];
	_ArrVtx[_Index].Tangent.z = (float)Tangent.mData[1];
	_ArrVtx[_Index].Tangent.NORMAL3D();
}





void Game_StaticFBX::LoadNormal(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
{

	int iCount = _Mesh->GetElementNormalCount();

	if (0 == iCount)
	{
		AMSG(L"GetElementNormalCount가 여러개 입니다.");
	}


	FbxGeometryElementNormal* pElement = _Mesh->GetElementNormal();
	int iDataIndex = VtxId;

	// 폴리곤 기반으로 로드한 녀석이냐.
	// 이 방식으로 만든것이다. 
	if (pElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(VtxId);
		}
		else
		{
			iDataIndex = VtxId;
			// AMSG(L"맵핑 방식중 처리할수 없는 방식입니다.");
		}
	}
	// 17641으로 됐느냐.
	else if (pElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (FbxGeometryElement::eDirect == pElement->GetReferenceMode())
		{
			iDataIndex = _Index;
		}
		else if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(_Index);
		}
		else {
			AMSG(L"맵핑 방식중 처리할수 없는 방식입니다.");
		}
	}

	// 노말은 가져왔고.
	FbxVector4 Normal = pElement->GetDirectArray().GetAt(iDataIndex);

	_ArrVtx[_Index].Normal.x = (float)Normal.mData[0];
	_ArrVtx[_Index].Normal.y = (float)Normal.mData[2];
	_ArrVtx[_Index].Normal.z = (float)Normal.mData[1];
	_ArrVtx[_Index].Normal.NORMAL3D();
}
// 





void Game_StaticFBX::MaterialCheck(fbxsdk::FbxNode* _Node, int& _Count)
{
	fbxsdk::FbxNodeAttribute* _Att = _Node->GetNodeAttribute();

	int MtrlCount = _Node->GetMaterialCount();

	if (MtrlCount > 0)
	{
		for (int i = 0; i < MtrlCount; i++)
		{
			FbxSurfaceMaterial* pMtrl = _Node->GetMaterial(i);

			if (nullptr == pMtrl)
			{
				AMSG(L"if (nullptr == pMtrl) 머티리얼 정보 에러");
			}


			m_UserMaterialData[_Count].DifColor = MaterialColor(pMtrl, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
			m_UserMaterialData[_Count].AmbColor = MaterialColor(pMtrl, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
			m_UserMaterialData[_Count].SpcColor = MaterialColor(pMtrl, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
			m_UserMaterialData[_Count].EmvColor = MaterialColor(pMtrl, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);
			m_UserMaterialData[_Count].SpecularPower = MaterialFactor(pMtrl, FbxSurfaceMaterial::sSpecularFactor);
			m_UserMaterialData[_Count].Shininess = MaterialFactor(pMtrl, FbxSurfaceMaterial::sShininess);
			m_UserMaterialData[_Count].TransparencyFactor = MaterialFactor(pMtrl, FbxSurfaceMaterial::sTransparencyFactor);

			m_UserMaterialData[_Count].DifTexture = MaterialTex(pMtrl, FbxSurfaceMaterial::sDiffuse);
			m_UserMaterialData[_Count].BmpTexture = MaterialTex(pMtrl, FbxSurfaceMaterial::sNormalMap);
			m_UserMaterialData[_Count].SpcTexture = MaterialTex(pMtrl, FbxSurfaceMaterial::sSpecular);


			++_Count;
		}

		// m_UserMaterialData.push_back();
	}

	int ChildCount = _Node->GetChildCount();

	for (int i = 0; i < ChildCount; i++)
	{
		MaterialCheck(_Node->GetChild(i), _Count);
	}

}




Game_Vector Game_StaticFBX::MaterialColor(FbxSurfaceMaterial* pMtrl, const char* _ColorName, const char* _FactorName)
{
	FbxDouble3 vResult(0, 0, 0);
	double dFactor = 0;
	FbxProperty ColorPro = pMtrl->FindProperty(_ColorName);
	FbxProperty FactorPro = pMtrl->FindProperty(_FactorName);

	// 유효체크 이 색상이 3d맥스에서 정말 사용하는 값이야?
	if (true == ColorPro.IsValid() && true == FactorPro.IsValid())
	{
		vResult = ColorPro.Get<FbxDouble3>();
		dFactor = FactorPro.Get<FbxDouble>();

		if (dFactor != 1)
		{
			vResult[0] *= dFactor;
			vResult[1] *= dFactor;
			vResult[2] *= dFactor;
		}
	}

	return Game_Vector((float)vResult[0], (float)vResult[1], (float)vResult[2]);
}




float Game_StaticFBX::MaterialFactor(FbxSurfaceMaterial* pMtrl, const char* _FactorName)
{
	double dFactor = 0;
	FbxProperty FactorPro = pMtrl->FindProperty(_FactorName);

	// 유효체크 이 색상이 3d맥스에서 정말 사용하는 값이야?
	if (true == FactorPro.IsValid())
	{
		dFactor = FactorPro.Get<FbxDouble>();
	}

	return (float)dFactor;
}





Game_String Game_StaticFBX::MaterialTex(FbxSurfaceMaterial* pMtrl, const char* _FactorName)
{
	FbxProperty TexturePro = pMtrl->FindProperty(_FactorName);
	Game_String Str;
	if (true == TexturePro.IsValid())
	{
		int iTexCount = TexturePro.GetSrcObjectCount<FbxFileTexture>();

		// 보면 알겠지만 여러개 세트가 들어있을수 있지만
		// 일단 1개만 있다고 가정하고 처리하겠다.
		if (iTexCount > 0)
		{
			FbxFileTexture* pFileTex = TexturePro.GetSrcObject<FbxFileTexture>(0);

			if (nullptr != pFileTex)
			{
				Str = pFileTex->GetFileName();
			}
		}
		else {
			return L"";
		}

	}
	else {
			return L"";
	}

	return Str;
}





void Game_StaticFBX::Trangulate(fbxsdk::FbxNode* _Node, int& _MatCount)
{
	fbxsdk::FbxNodeAttribute* _Att = _Node->GetNodeAttribute();

	if (nullptr != _Att &&
		(_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh ||
			_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbs ||
			_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbsSurface)
		)
	{
		if ((_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbs ||
			_Att->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbsSurface))
		{
			int a = 0;
		}
		FbxGeometryConverter Con(m_Mgr);
		Con.Triangulate(_Att, true);
	}

	_MatCount += _Node->GetMaterialCount();

	int ChildCount = _Node->GetChildCount();

	for (int i = 0; i < ChildCount; i++)
	{
		Trangulate(_Node->GetChild(i), _MatCount);
	}

}



/* 본이 있는지만 확인하는 함수 입니다. */
void Game_StaticFBX::BoneCheck()
{
	// 마치 트리구조와 비슷하므로
	// 재귀가 아주 유용하다.
	int BoneCount = 0;

	// 본은 재귀를 돌면서 
	LoadBoneCount(m_Scene->GetRootNode(), BoneCount);

	if (0 < BoneCount)
	{
		// 리사이즈 한다. 
		m_UserBoneData.resize(BoneCount);
	}

	// 이미 다 리사이즈를 했기 때문에.
	BoneCount = 0;


	LoadBone(m_Scene->GetRootNode(), BoneCount);
}




void Game_StaticFBX::LoadBoneCount(fbxsdk::FbxNode* _Node, int& _Count)
{
	fbxsdk::FbxNodeAttribute* _Att = _Node->GetNodeAttribute();

	// 스켈레톤 == 본 
	// 본임을 확인하고 
	// 재귀를 돈다. 
	// 본의 부모-자식 관계들을 돌면서 노드를 계산한다. 


	if (nullptr != _Att
		&& _Att->GetAttributeType()// Attribute : 속성 
		== fbxsdk::FbxNodeAttribute::eSkeleton)
	{
		++_Count;
		// m_UserBoneData.resize
	}

	int ChildCount = _Node->GetChildCount();

	// 재귀 본의 부모-자식관계들을 확인하면서 
	for (int i = 0; i < ChildCount; i++)
	{
		LoadBoneCount(_Node->GetChild(i), _Count);
	}
}





void Game_StaticFBX::LoadBone(fbxsdk::FbxNode* _Node, int& _iIndex, int _iDepth, int _iParent) {

	fbxsdk::FbxNodeAttribute* _Att = _Node->GetNodeAttribute();

	int ParentIndex = _iIndex;

	if (nullptr != _Att
		&& _Att->GetAttributeType()
		== fbxsdk::FbxNodeAttribute::eSkeleton)
	{
		m_UserBoneData[_iIndex].Name = CA2W(_Node->GetName()).m_psz;
		m_UserBoneData[_iIndex].iIndex = _iIndex;
		m_UserBoneData[_iIndex].iParentIndex = ParentIndex;
		m_UserBoneData[_iIndex].iDepth = _iDepth;
		// 맵에도 넣어주는 이유는 나중에 본을 이름으로 쉽게 찾기 위해서 이다.
		m_BoneDataMap[m_UserBoneData[_iIndex].Name] = &m_UserBoneData[_iIndex];
		++_iIndex;
	}

	int ChildCount = _Node->GetChildCount();
	for (int i = 0; i < ChildCount; i++)
	{
		LoadBone(_Node->GetChild(i), _iIndex, _iDepth + 1, _iIndex);
	}

}



// STATIC 매쉬를 위해 커스텀한 랜더러를 만들 었다. 
void Game_StaticFBX::CreateStaticMesh(Game_Ptr<Game_Renderer> _NewRenderer, const Game_String& _Fbx)
{

	Game_Ptr<Game_StaticFBX> TestFbx = Game_StaticFBX::Find(_Fbx);

	if (TestFbx->m_UserMaterialData.size() != TestFbx->m_Mesh.size())
	{
		AMSG(L"개수가 다릅니다.");
	}

	for (size_t i = 0; i < TestFbx->m_Mesh.size(); i++)
	{
		// 어거지로라도 m_UserMatData내부의 개수를 맞출것이다.
		// m_UserMaterialData == m_Mesh 선생님이 로드할때 같은 수를 만들게

		// 경로를 편집한다. 
		HGAMEFILE File = TestFbx->GameFile.DirPath() + TestFbx->m_UserMaterialData[i].DifTexture;

		if (nullptr == HTEXTURE::Find(File.FileName()))
		{
			// 경로를 편집한다. 
			HTEXTURE::Load(TestFbx->GameFile.DirPath() + TestFbx->m_UserMaterialData[i].DifTexture);
		}

		// 랜더플레이어는 매쉬를 만드는 것인데 
		// 여기에 따로 커스텀한 샘플러와 텍스쳐를 세팅한다. 
		Game_Ptr<HRENDERPLAYER> Player = _NewRenderer->CreateRenderPlayer(TestFbx->m_Mesh[i], L"3DStaticTex");
		Player->TEXTURE(L"Tex", File.FileName());
		Player->SAMPLER(L"Smp", L"LWSMP");
	}
}






// AniCheck()가 하는일 
// m_UserAniData안에 LejFbxAniData가 얼마나 쌓였는지 알고 싶다.
// LejFbxAniData 하나하나의 정보체의 속성값을 채워준다. 
// 애니메이션데이터는 여러개의 프레임정보를 가진다. 

void Game_StaticFBX::AniCheck()
{
	fbxsdk::FbxArray<FbxString*> AniNameArray;
	// 애니메이션의 이름의 리스트를 얻어온다.
	m_Scene->FillAnimStackNameArray(AniNameArray);


	if (0 == AniNameArray.Size())
	{
		return;
	}

	// 프로젝트 세팅현재 상황.
	// 코드생성 -> 런타임 라이브러리 -> 
	// 디버그 일대 다중스레드 디버그 MTD
	// 릴리즈 일대 다중스레드 MD
	// HTOOL은 정적 라이브러리로 MFC 사용

	// 정적라이브러리에서 mfc사용으로 변경
	// 유니티 빌드로 모두 변환

	// 벡터는 무조건 미리 사이즈를 정해주는 RESIZE나
	// reserve를 사용하는것이 좋다.
	// 본 134개 
	// 본프레임 1434개 
	// 이렇게 많이 도는데 resize 안하면 어떻게 할거냐 ??
	// 미리 재계산한다. 



	m_UserAniData.resize(AniNameArray.Size());



	for (int i = 0; i < AniNameArray.GetCount(); i++)
	{
		// pAniStatic 애니메이션 하나하나의 세부적인 정보를 담고 있다.
		FbxAnimStack* pAniStatic = m_Scene->FindMember<FbxAnimStack>(AniNameArray[i]->Buffer());

		if (nullptr == pAniStatic)
		{
			AMSG(L"if (nullptr == pAniStatic)");
			continue;
		}

		// std::wcout << m_UserAniData[i].AniName << std::endl;



		m_UserAniData[i].AniName = CA2W(pAniStatic->GetName(), CP_UTF8).m_psz;

		// HGAMEDEBUG3D::ConsoleText(L"애니메이션 : %s 탐색", pAniStatic->GetName());
		FbxTakeInfo* TakeInfo = m_Scene->GetTakeInfo(pAniStatic->GetName());

		// 
		m_UserAniData[i].StartTime = TakeInfo->mLocalTimeSpan.GetStart();
		m_UserAniData[i].EndTime = TakeInfo->mLocalTimeSpan.GetStop();
		m_UserAniData[i].TimeMode = m_Scene->GetGlobalSettings().GetTimeMode();
		m_UserAniData[i].TimeStartCount = m_UserAniData[i].StartTime.GetFrameCount(m_UserAniData[i].TimeMode);
		m_UserAniData[i].TimeEndCount = m_UserAniData[i].EndTime.GetFrameCount(m_UserAniData[i].TimeMode);
		m_UserAniData[i].FrameCount = m_UserAniData[i].TimeEndCount - m_UserAniData[i].TimeStartCount;
		m_UserAniData[i].FbxModeCount = m_UserAniData[i].FrameModeCount();
	}

	// 
	for (int i = 0; i < AniNameArray.GetCount(); i++)
	{
		delete AniNameArray[i];
	}

}




LejFbxBoneData* Game_StaticFBX::FindBone(const Game_String& _Name)
{
	if (m_BoneDataMap.end() == m_BoneDataMap.find(_Name))
	{
		AMSG(_Name + L" 이름의 본은 존재하지 않습니다.");
	}

	return m_BoneDataMap[_Name];
}




void Game_StaticFBX::LoadWeightAndIndex(fbxsdk::FbxCluster* _Cluster, int _BoneIndex, std::vector<VTX3DMESHIN>& _ArrVtx)
{
	// fbxsdk::FbxCluster 클러스터는 스키닝의 정보를 담고 있는 구조체이다. 

	int iIndexCount = _Cluster->GetControlPointIndicesCount();

	for (size_t i = 0; i < iIndexCount; i++)
	{
		//// _BoneIndex 인덱스
		LejFbxIndexWeight IW;

		IW.Index = _BoneIndex;
		IW.Weight = _Cluster->GetControlPointWeights()[i];

		int iClusterIndex = _Cluster->GetControlPointIndices()[i];

		//if (m_MapWI[MeshCount].end() != m_MapWI[MeshCount].find(iClusterIndex))
		//{
		//	AMSG(L"인덱스와 가중치가 겹칩니다.");
		//}

		m_MapWI[MeshCount][iClusterIndex].push_back(IW);
		//m_MapWI[MeshCount][iClusterIndex].Index = _BoneIndex;
		//m_MapWI[MeshCount][iClusterIndex].Weight = _Cluster->GetControlPointWeights()[i];
	}

	//_Cluster->GetControlPointWeights();
	// int iIndex = _Cluster->GetControlPointWeights();

}








// 본마다 가지고 있는 오프셋행렬의 값을 채워준다. 

void Game_StaticFBX::LoadOffsetMatrix(fbxsdk::FbxCluster* _Cluster, const fbxsdk::FbxAMatrix& _BaseTrans, LejFbxBoneData* _Bone)
{
	// 애니메이션을 위한 가장 기본적인 축 행렬을 만들어주는 것이다. 


	// 3D맥스 좌표계
	// OPENGL의 좌표계
	// DX의 좌표계
	// 3D맥스 좌표계 -> DX의 좌표계 로 바꾸고 싶다. 

	/*

		1 0 0 0
		0 0 1 0
		0 1 0 0
		0 0 0 1

		10  0 0
		0c  s 0
		0-c c 0
		00  0 1

		1 0 0 0			1 2 3 4 	   1 0 0 0
		0 0 1 0    *	5 6 7 8    *   0 0 1 0
		0 1 0 0			9 0 1 2		   0 1 0 0
		0 0 0 1			3 4 5 6		   0 0 0 1

		1 2 3 4			1 0 0 0
		9 0 1 2	   *	0 0 1 0
		5 6 7 8			0 1 0 0
		3 4 5 6			0 0 0 1

		1 3 2 4
		9 1 0 2
		5 7 6 8
		3 5 4 6

	*/
	FbxAMatrix ClusterMat;
	FbxAMatrix LinkClusterMat;

	_Cluster->GetTransformMatrix(ClusterMat);
	_Cluster->GetTransformLinkMatrix(LinkClusterMat);

	FbxAMatrix Offset;
	Offset = LinkClusterMat.Inverse() * ClusterMat * _BaseTrans;

	// DX 좌표계로 바꾸려고 하는것이다.
	// 좌표계는 DX좌표계 기준으로 
	// Z축과 Y축이 반대로 되어있다. 
	// 그러므로 뒤집는다. 

	Offset = ReMat * Offset * ReMat;


	_Bone->Offset = FbxMatrixToLejMatrix(Offset); // 
	_Bone->Bone = FbxMatrixToLejMatrix(_BaseTrans);// BONE은 항등행렬 만들어짐 

}





HMATRIX Game_StaticFBX::FbxMatrixToLejMatrix(const fbxsdk::FbxAMatrix& _BaseTrans)
{
	HMATRIX Mat;

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			Mat.Arr2D[y][x] = (float)_BaseTrans.Get(y, x);
		}
	}

	return Mat;
}





Game_Vector Game_StaticFBX::FbxVectorToLejVector(const fbxsdk::FbxVector4& _BaseVector)
{
	Game_Vector Vec;
	for (int i = 0; i < 4; i++)
	{
		Vec.Arr[i] = (float)_BaseVector.mData[i];
	}
	return Vec;

}


// ->CreateRenderPlayer



Game_Vector Game_StaticFBX::FbxQuaterToLejVector(const fbxsdk::FbxQuaternion& _BaseQ)
{
	Game_Vector Vec;
	for (int i = 0; i < 4; i++)
	{
		Vec.Arr[i] = (float)_BaseQ.mData[i];
	}
	return Vec;
}








void Game_StaticFBX::LoadTimeTransForm(fbxsdk::FbxNode* _Node, fbxsdk::FbxCluster* _Cluster, const fbxsdk::FbxAMatrix& _BaseTrans, LejFbxBoneData* _Bone)
{
	// 			LoadTimeTransForm(Node, pCluster, matTrans, pBone);

	//			m_UserAniData[i].m_AniFrameData.re

	for (size_t i = 0; i < m_UserAniData.size(); i++)
	{
		if (0 == _Bone->iIndex)
		{
			int a = 0;
		}

		FbxLongLong Start = m_UserAniData[i].TimeStartCount;
		FbxLongLong End = m_UserAniData[i].TimeEndCount;

		m_UserAniData[i].m_AniFrameData[_Bone->iIndex].BoneIndex = _Bone->iIndex; // 본정보 그자체 

		// 1초당 30프레임이니까
		// 300 10초시간대의 애니메이션
		// 1434
		for (FbxLongLong j = Start; j <= End; j++)
		{
			FbxTime tTime = {};
			// 타임모드를 기반으로한 시간을 알아낼수 있다.
			tTime.SetFrame(j, m_UserAniData[i].TimeMode);

			// 시간대비 0 1 2 3 4 5 6 7 8 9  ~~~~~~~ 1434   애니메이션-델타타임의 보간 
			// 애니메이션을 만든다 .

			// EvaluateGlobalTransform   노드에서 글로벌 및 로컬 변환 행렬을 얻을 수 있다. 
			FbxAMatrix matOff = _Node->EvaluateGlobalTransform(tTime) * _BaseTrans;

			// EvaluateGlobalTransform   노드에서 글로벌 및 로컬 변환 행렬을 얻을 수 있다.
			FbxAMatrix matCur = matOff.Inverse() * _Cluster->GetLink()->EvaluateGlobalTransform(tTime);
			// FbxAMatrix matCur = matOff.Inverse() * _Cluster->GetLink()->EvaluateLocalTransform(tTime);


			matCur = ReMat * matCur * ReMat;

			// 어쩄든 ... 이것으로 값을 채워준다. 
			m_UserAniData[i].m_AniFrameData[_Bone->iIndex].m_Data[j].Time = tTime.GetSecondDouble();
			m_UserAniData[i].m_AniFrameData[_Bone->iIndex].m_Data[j].FrameMat = FbxMatrixToLejMatrix(matCur);
			m_UserAniData[i].m_AniFrameData[_Bone->iIndex].m_Data[j].S = FbxVectorToLejVector(matCur.GetS());
			m_UserAniData[i].m_AniFrameData[_Bone->iIndex].m_Data[j].Q = FbxQuaterToLejVector(matCur.GetQ());
			m_UserAniData[i].m_AniFrameData[_Bone->iIndex].m_Data[j].T = FbxVectorToLejVector(matCur.GetT());
		}
	}
}










void Game_StaticFBX::CalWeightAndIndex()
{
	for (auto& MapWI : m_MapWI[MeshCount])
	{
		if (MapWI.second.size() > 4)
		{
			AMSG(L"가중치 계산에 4개 이상의 값이 들어왔습니다.");

			// 
			// 람다 문법 언네임드 펑션.
			std::sort(MapWI.second.begin(), MapWI.second.end(),
				[](const LejFbxIndexWeight& _Left, const LejFbxIndexWeight& _Right) {
					return _Left.Weight > _Right.Weight;
				}
			);

			double dSum = 0.0;

			for (int i = 0; i < MapWI.second.size(); i++)
			{
				dSum += MapWI.second[i].Weight;
			}

			double dInterPolate = 1.0f - dSum;

			std::vector<LejFbxIndexWeight>::iterator IterErase = MapWI.second.begin() + 4;

			MapWI.second.erase(IterErase, MapWI.second.end());
			MapWI.second[0].Weight += dInterPolate;
		}
		;
		float Weight[4] = { 0.0f };
		int Index[4] = { 0 };

		for (size_t i = 0; i < MapWI.second.size(); i++)
		{
			Weight[i] = (float)MapWI.second[i].Weight;
			Index[i] = MapWI.second[i].Index;
		}

		//if (1 < MapWI.second.size())
		//{
		//	HGAMEDEBUG3D::ConsoleText(L"%d, %d, %d, %d\n", Index[0], Index[1], Index[2], Index[3]);
		//	HGAMEDEBUG3D::ConsoleText(L"%f, %f, %f, %f\n", Weight[0], Weight[1], Weight[2], Weight[3]);
		//	//Weight[i] = (float)MapWI.second[i].Weight;
		//	//Index[i] = MapWI.second[i].Index;
		//}

		memcpy_s(m_FbxVtx[MeshCount][MapWI.first].Weight.Arr, sizeof(Game_Vector), Weight, sizeof(Game_Vector));
		memcpy_s(m_FbxVtx[MeshCount][MapWI.first].Index, sizeof(Game_Vector), Index, sizeof(Game_Vector));
	}
	++MeshCount;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 자체 포맷이다. 


////////////////////////////////////////////////////////////////////////////////////////////// 한번만 LOAD 쓰레드  


void Game_StaticFBX::LoadUserFormat(const Game_String& _Path)
{
	HGAMEFILE LoadFile = HGAMEFILE(_Path, L"rb");

	// std::vector<std::vector<VTX3DMESHIN>> m_FbxVtx; // 버텍스

	int Size;
	LoadFile >> Size;
	m_FbxVtx.resize(Size);
	for (size_t i = 0; i < m_FbxVtx.size(); i++)
	{
		LoadFile >> Size;
		m_FbxVtx[i].resize(Size);
		for (size_t j = 0; j < m_FbxVtx[i].size(); j++)
		{
			LoadFile >> m_FbxVtx[i][j];
		}
	}


	// std::vector<std::vector<std::vector<UINT>>> m_FbxIdx; // 인덱스
	LoadFile >> Size;
	m_FbxIdx.resize(Size);
	for (size_t i = 0; i < m_FbxIdx.size(); i++)
	{
		LoadFile >> Size;
		m_FbxIdx[i].resize(Size);
		for (size_t j = 0; j < m_FbxIdx[i].size(); j++)
		{
			LoadFile >> Size;
			m_FbxIdx[i][j].resize(Size);
			for (size_t k = 0; k < m_FbxIdx[i][j].size(); k++)
			{
				LoadFile >> m_FbxIdx[i][j][k];
			}
		}
	}

	///////////////////////////////////////////////// 머테리얼 

	LoadFile >> Size;
	m_UserMaterialData.resize(Size);
	for (size_t i = 0; i < m_UserMaterialData.size(); i++)
	{
		LoadFile >> m_UserMaterialData[i].DifColor; //HVECTOR DifColor;
		LoadFile >> m_UserMaterialData[i].AmbColor; //HVECTOR AmbColor; // 빛
		LoadFile >> m_UserMaterialData[i].SpcColor; //HVECTOR SpcColor; // 빛
		LoadFile >> m_UserMaterialData[i].EmvColor; //HVECTOR EmvColor; // 빛
		LoadFile >> m_UserMaterialData[i].SpecularPower;//float SpecularPower;		// 빛의 강도
		LoadFile >> m_UserMaterialData[i].TransparencyFactor;//float TransparencyFactor;	// 빛의 강도
		LoadFile >> m_UserMaterialData[i].Shininess;//float Shininess;			// 빛의 강도
		LoadFile >> m_UserMaterialData[i].DifTexture;//HGAMESTRING DifTexture;	// 텍스처경로 
		LoadFile >> m_UserMaterialData[i].BmpTexture;//HGAMESTRING BmpTexture; // 텍스처경로
		LoadFile >> m_UserMaterialData[i].SpcTexture;//HGAMESTRING SpcTexture; // 텍스처경로
	}

	LoadFile >> Size;
	if (-1 == Size)
	{
		// 애니메이션이 없다는 이야기.
		return;
	}

	////////////////////////////////////////////////////////////// 본->애니메이션 
	// 애니메이션을 만들기 위한 프레임간의 관계를 정의하는 델타타임, 애니메이션 속성, 본프레임의 로컬정보를 가지고 있다. 

	m_UserAniData.resize(Size);
	for (size_t i = 0; i < m_UserAniData.size(); i++)
	{
		LoadFile >> m_UserAniData[i].AniName;
		LoadFile >> m_UserAniData[i].StartTime;
		LoadFile >> m_UserAniData[i].EndTime;
		LoadFile >> m_UserAniData[i].TimeStartCount;
		LoadFile >> m_UserAniData[i].TimeEndCount;
		LoadFile >> m_UserAniData[i].FrameCount;
		LoadFile >> m_UserAniData[i].TimeMode;
		LoadFile >> m_UserAniData[i].FbxModeCount;

		LoadFile >> Size;
		m_UserAniData[i].m_AniFrameData.resize(Size);
		for (size_t j = 0; j < m_UserAniData[i].m_AniFrameData.size(); j++)
		{
			LoadFile >> m_UserAniData[i].m_AniFrameData[j].BoneIndex;
			LoadFile >> Size;
			m_UserAniData[i].m_AniFrameData[j].m_Data.resize(Size);
			for (size_t k = 0; k < m_UserAniData[i].m_AniFrameData[j].m_Data.size(); k++)
			{
				LoadFile >> m_UserAniData[i].m_AniFrameData[j].m_Data[k];
			}
		}
	}


	// std::vector<HFBXBONEDATA> m_UserBoneData; // 본
	LoadFile >> Size;
	m_UserBoneData.resize(Size);
	for (size_t i = 0; i < m_UserBoneData.size(); i++)
	{
		LoadFile >> m_UserBoneData[i].Name;
		LoadFile >> m_UserBoneData[i].iDepth;
		LoadFile >> m_UserBoneData[i].iIndex;
		LoadFile >> m_UserBoneData[i].iParentIndex;
		LoadFile >> m_UserBoneData[i].Offset;
		LoadFile >> m_UserBoneData[i].Bone;
	}


	for (size_t i = 0; i < m_UserAniData.size(); i++)
	{
		m_UserAniDataMap[m_UserAniData[i].AniName] = &m_UserAniData[i];
	}

	for (size_t i = 0; i < m_UserBoneData.size(); i++)
	{
		m_BoneDataMap[m_UserBoneData[i].Name] = &m_UserBoneData[i];
	}

	CreateMesh();
}









// 메쉬 본 머테리얼 애니메이션

/// 자체 포맷은 : 세이브 한번 해 놓으면 굳이 기존 코드를 로드 할 필요가 없다. 
/// 또한 저장한다는 것은 필요없는 것을 저장하는 것이 아닌 랜더링에 필수적인 데이터를 저장하는 행위이다. 

// 근데 사실 상용게임들은 본 하나하나 134개를 쓰레드로 만들어서 각자 저장, 로드 하게 만든다. 이렇게 하면 빠르다. 최적화 


////////////////////////////////////////////////////////////////////////////////////////////// 한번만 SAVE 쓰레드  
void Game_StaticFBX::SaveUserFormat(const Game_String& _Path)
{
	HGAMEFILE SaveFile = HGAMEFILE(_Path, L"wb");


	// std::vector<std::vector<VTX3DMESHIN>> m_FbxVtx; // 버텍스
	SaveFile << (int)m_FbxVtx.size();
	for (size_t i = 0; i < m_FbxVtx.size(); i++)
	{
		SaveFile << (int)m_FbxVtx[i].size();
		for (size_t j = 0; j < m_FbxVtx[i].size(); j++)
		{
			SaveFile << m_FbxVtx[i][j];
		}
	}

	// std::vector<std::vector<std::vector<UINT>>> m_FbxIdx; // 인덱스
	SaveFile << (int)m_FbxIdx.size();
	for (size_t i = 0; i < m_FbxIdx.size(); i++)
	{
		SaveFile << (int)m_FbxIdx[i].size();
		for (size_t j = 0; j < m_FbxIdx[i].size(); j++)
		{
			SaveFile << (int)m_FbxIdx[i][j].size();
			for (size_t k = 0; k < m_FbxIdx[i][j].size(); k++)
			{
				SaveFile << m_FbxIdx[i][j][k];
			}
		}
	}


	///////////////////////////////////////////////////////////// 머테리얼 

	// std::vector<HFBXMATDATA> m_UserMaterialData;
	SaveFile << (int)m_UserMaterialData.size();
	for (size_t i = 0; i < m_UserMaterialData.size(); i++)
	{
		// 이렇게 값형으로 저장하는 이유는 저장할 때 연산이 단순하기 때문이다. 
		// 이것이 결과가 값형이 나오게 구조를 만든 이유다. 
		SaveFile << m_UserMaterialData[i].DifColor; //HVECTOR DifColor;
		SaveFile << m_UserMaterialData[i].AmbColor; //HVECTOR AmbColor; // 빛
		SaveFile << m_UserMaterialData[i].SpcColor; //HVECTOR SpcColor; // 빛
		SaveFile << m_UserMaterialData[i].EmvColor; //HVECTOR EmvColor; // 빛
		SaveFile << m_UserMaterialData[i].SpecularPower;//float SpecularPower;		// 빛의 강도
		SaveFile << m_UserMaterialData[i].TransparencyFactor;//float TransparencyFactor;	// 빛의 강도
		SaveFile << m_UserMaterialData[i].Shininess;//float Shininess;			// 빛의 강도
		SaveFile << m_UserMaterialData[i].DifTexture;//HGAMESTRING DifTexture;	// 텍스처경로 
		SaveFile << m_UserMaterialData[i].BmpTexture;//HGAMESTRING BmpTexture; // 텍스처경로
		SaveFile << m_UserMaterialData[i].SpcTexture;//HGAMESTRING SpcTexture; // 텍스처경로
	}

	if (0 >= m_UserAniData.size())
	{
		// 애니메이션이 없다는 이야기죠?
		SaveFile << (int)-1;
		return;
	}

	std::wcout << L"ANIDATA SAVE" << std::endl; // 콘솔 창에 띄울것이다. 
	// std::vector<HFBXANIDATA> m_UserAniData; // 애니메이션 

	SaveFile << (int)m_UserAniData.size();
	for (size_t i = 0; i < m_UserAniData.size(); i++)
	{
		SaveFile << m_UserAniData[i].AniName;
		SaveFile << m_UserAniData[i].StartTime;
		SaveFile << m_UserAniData[i].EndTime;
		SaveFile << m_UserAniData[i].TimeStartCount;
		SaveFile << m_UserAniData[i].TimeEndCount;
		SaveFile << m_UserAniData[i].FrameCount;
		SaveFile << m_UserAniData[i].TimeMode;
		SaveFile << m_UserAniData[i].FbxModeCount;

		SaveFile << (int)m_UserAniData[i].m_AniFrameData.size();
		for (size_t j = 0; j < m_UserAniData[i].m_AniFrameData.size(); j++)
		{
			SaveFile << m_UserAniData[i].m_AniFrameData[j].BoneIndex;
			SaveFile << (int)m_UserAniData[i].m_AniFrameData[j].m_Data.size();
			for (size_t k = 0; k < m_UserAniData[i].m_AniFrameData[j].m_Data.size(); k++)
			{
				SaveFile << m_UserAniData[i].m_AniFrameData[j].m_Data[k];
			}
		}
	}


	// std::vector<HFBXBONEDATA> m_UserBoneData; // 본
	SaveFile << (int)m_UserBoneData.size();
	for (size_t i = 0; i < m_UserBoneData.size(); i++)
	{
		SaveFile << m_UserBoneData[i].Name;
		SaveFile << m_UserBoneData[i].iDepth;
		SaveFile << m_UserBoneData[i].iIndex;
		SaveFile << m_UserBoneData[i].iParentIndex;
		SaveFile << m_UserBoneData[i].Offset;
		SaveFile << m_UserBoneData[i].Bone;
	}

}