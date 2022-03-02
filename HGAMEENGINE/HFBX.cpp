#include "HFBX.h"
// #include "HGAMEDEBUG3D.h"
#include "HMESH.h"

// std::map<string, void*> 
HFBX::HFBX() : MeshCount(0)
{

}
HFBX::~HFBX()
{
	if (nullptr != m_Imp) {
		m_Imp->Destroy();
		m_Imp = nullptr;
	}
	if (nullptr != m_Scene) {
		m_Scene->Destroy();
		m_Scene = nullptr;
	}
	if (nullptr != m_Ios) {
		m_Ios->Destroy();
		m_Ios = nullptr;
	}
	if (nullptr != m_Mgr) {
		m_Mgr->Destroy();
		m_Mgr = nullptr;
	}

}

void HFBX::CreateMesh()
{
	//std::vector<HVTX2D> m_VTX;
	//m_VTX.push_back({ { -0.5f, 0.5f, 0.0f, 1.0F },	{ 0.0f, 0.0f, 0.0f, 1.0F }, Game_Vector::WHITE });
	//m_VTX.push_back({ { 0.5f, 0.5f, 0.0f, 1.0F },	{ 1.0f, 0.0f, 0.0f, 1.0F },Game_Vector::WHITE });
	//m_VTX.push_back({ { 0.5f, -0.5f, 0.0f, 1.0F },	{ 1.0f, 1.0f, 0.0f, 1.0F },Game_Vector::WHITE });
	//m_VTX.push_back({ { -0.5f, -0.5f, 0.0f, 1.0F }, { 0.0f, 1.0f, 0.0f, 1.0F },Game_Vector::WHITE });
	//HVBBUFFER::Create(L"RECT", m_VTX);

	if (m_FbxVtx.size() != m_FbxIdx.size())
	{
		AMSG(L"인덱스 버퍼 버텍스 버퍼 매칭 오류");
		return;
	}


	m_VB.resize(m_FbxVtx.size());
	m_IB.resize(m_FbxIdx.size());
	for (size_t i = 0; i < m_FbxVtx.size(); i++)
	{
		Game_Ptr<HVBBUFFER> NewVB = new HVBBUFFER();
		NewVB->Create(m_FbxVtx[i].size(), sizeof(VTX3DMESHIN), &m_FbxVtx[i][0], D3D11_USAGE::D3D11_USAGE_DEFAULT);
		m_VB.push_back(NewVB);

		// m_IB.resize(m_FbxIdx[i].size());
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

	if (m_Mesh.size() != m_UserMatData.size())
	{
		// 0 1 번 그릴때 메테리얼 2번써줘야 하고
		// 0 2 번 그릴때 메테리얼 5번 써줘야하고 이런 상황이 왔다.
		AMSG(L"매쉬와 메테리얼의 크기가 다릅니다.");
		return;
	}


	// HGAMEDEBUG3D::ConsoleText(L"버텍스 및 인덱스 버퍼 버퍼 매쉬생성 완료");


	//std::vector<UINT> m_IDX = { 0, 1, 2, 0, 2, 3 };
	//HIBBUFFER::Create(L"RECT", m_IDX);
	//HMESH::Create(L"RECT");

}

void HFBX::Load()
{
	// HGAMEDEBUG3D::ConsoleText(m_Path + " LoadStart");

	ReMat.SetRow(0, FbxVector4{ 1.0, 0.0, 0.0, 0.0 });
	ReMat.SetRow(1, FbxVector4{ 0.0, 0.0, 1.0, 0.0 });
	ReMat.SetRow(2, FbxVector4{ 0.0, 1.0, 0.0, 0.0 });
	ReMat.SetRow(3, FbxVector4{ 0.0, 0.0, 0.0, 1.0 });

	m_Mgr = fbxsdk::FbxManager::Create();
	m_Ios = fbxsdk::FbxIOSettings::Create(m_Mgr, IOSROOT);
	m_Mgr->SetIOSettings(m_Ios);
	m_Scene = fbxsdk::FbxScene::Create(m_Mgr, "");
	m_Imp = fbxsdk::FbxImporter::Create(m_Mgr, "");

	// 파일과 임포터를 연결한다.
	if (false == m_Imp->Initialize(CW2A(GameFile.FullPath(), CP_UTF8).m_psz, -1, m_Mgr->GetIOSettings()))
	{
		AMSG(GameFile.FullPath() + " FBX 파일 이니셜라이즈 실패");
	}

	// 기본 구성환경을 읽어온다.
	// 그 기본축을 이단계에서 설정할수 있는데.
	// 그런데 그대로 읽는다.
	m_Imp->Import(m_Scene);
	eLocalSpaceType = m_Scene->GetGlobalSettings().GetAxisSystem();



	if (eLocalSpaceType == fbxsdk::FbxAxisSystem::eDirectX)
	{
		m_Scene->GetGlobalSettings().SetAxisSystem(fbxsdk::FbxAxisSystem::eMax);
	}
	else if (eLocalSpaceType != fbxsdk::FbxAxisSystem::eMax)
	{
		// 기저 벡터의 기준이 다르다.
		m_Scene->GetGlobalSettings().SetAxisSystem(fbxsdk::FbxAxisSystem::eMax);
	}


	AniCheck();
	// HGAMEDEBUG3D::ConsoleText(L"애니메이션 체크 완료 애니메이션 개수 : %d", m_UserAniData.size());

	int MatCount = 0;
	Trangulate(m_Scene->GetRootNode(), MatCount);
	// HGAMEDEBUG3D::ConsoleText(L"삼각화 완료");

	m_UserMatData.resize(MatCount);
	MatCount = 0;
	MaterialCheck(m_Scene->GetRootNode(), MatCount);
	// HGAMEDEBUG3D::ConsoleText(L"머티리얼 체크 완료 %d", MatCount);

	for (size_t i = 0; i < m_UserMatData.size(); i++)
	{
		Game_String DirPath = GameFile.DirPath();
		m_UserMatData[i].DifTexture.EraseStr(DirPath);
	}

	MeshVtxData(m_Scene->GetRootNode());

	BoneCheck();

	// STATIC 매쉬
	if (0 >= m_UserAniData.size())
	{
		CreateMesh();
		// HGAMEDEBUG3D::ConsoleText(m_Path + L" static 매쉬 완료");
		return;
	}

	//// HGAMEDEBUG3D::ConsoleText(L"본 체크 완료 본 개수 : %d\n", m_UserBoneData.size());
	size_t I = m_UserAniData.size();

	for (size_t AniIndex = 0; AniIndex < m_UserAniData.size(); AniIndex++)
	{
		// 애니메이션은 1개인데 본은 134개입니다.
		// 134리사이즈 한다.
		m_UserAniData[AniIndex].m_AniFrameData.resize(m_UserBoneData.size());

		for (size_t BoneCount = 0; BoneCount < m_UserBoneData.size(); BoneCount++)
		{
			size_t I = m_UserBoneData.size();
			// 1434게만금
			m_UserAniData[AniIndex].m_AniFrameData[BoneCount].m_Data.resize(m_UserAniData[AniIndex].FrameCount + 1);
		}
	}

	// 이건 가중치와 index를 재계산할때 사용할 녀석인데 
	// 이녀석을 쓰는곳이
	// Game_Vector Weight; // 얼마나 영향을 받아야 하는가? 0.5 영향을 받는다.
	// int Index[4]; // 21번째 본에
	m_MapWI.resize(m_FbxVtx.size());

	// 이녀석은 혹시나 루트나 
	MeshCount = 0;

	// 왤까요? 134 * 1434
	// 애니메이션이 1434프레임인데.
	// 본은 134개가 있고.
	// 안할수는 없다.
	// 중간을 잘라먹을수도 없다.
	// 애니메이션을 
	// 자체포맷과 쓰레드 로딩을 할것이다.
	// 쓰레드 로딩부터 볼건데.
	// 쓰레드 이론부터 알아야 한다.
	AnimationClipCheck(m_Scene->GetRootNode());

	MeshCount = 0;
	CalWeightAndIndex();
	// 인덱스와 가중치를 다 계산했으므로 날려버린다.
	m_MapWI.clear();

	for (size_t i = 0; i < m_UserAniData.size(); i++)
	{
		m_UserAniDataMap[m_UserAniData[i].AniName] = &m_UserAniData[i];
	}

	CreateMesh();
}

void HFBX::MeshVtxData(fbxsdk::FbxNode* _Node)
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

		VtxData(_Mesh);
	}

	int ChildCount = _Node->GetChildCount();

	for (int i = 0; i < ChildCount; i++)
	{
		MeshVtxData(_Node->GetChild(i));
	}
}

void HFBX::VtxData(fbxsdk::FbxMesh* _Mesh) {

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
		// 약간 이해하기 힘든게 021로 받아오는건데
		// 맥스 공간에 뒤집어진 z와 y를 표현하기 위해서 처리.
		VtxData[i].Pos.x = (float)pVtxPos[i].mData[0];
		VtxData[i].Pos.y = (float)pVtxPos[i].mData[2];
		VtxData[i].Pos.z = (float)pVtxPos[i].mData[1];
		VtxData[i].Pos.w = 1.0f;
		// HGAMEDEBUG3D::ConsoleText(L"%d = %f, %f, %f", i, VtxData[i].Pos.x, VtxData[i].Pos.y, VtxData[i].Pos.z);
	}


	UINT VtxId = 0;

	// 이 매쉬를 그리는데 사용한 재질 정보
	FbxGeometryElementMaterial* MatData = _Mesh->GetElementMaterial();
	// 그리는데 사용한 재질재수
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

void HFBX::AnimationClipCheck(fbxsdk::FbxNode* _Node)
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

		CalAnimationClip(_Mesh);
	}

	int ChildCount = _Node->GetChildCount();

	for (int i = 0; i < ChildCount; i++)
	{
		AnimationClipCheck(_Node->GetChild(i));
	}
}

void HFBX::CalAnimationClip(fbxsdk::FbxMesh* _Mesh)
{
	int iSkinCount = _Mesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
	if (iSkinCount <= 0)
	{
		AMSG(L"스키닝 정보가 존재하지 않습니다.");
	}

	int VtxCount = (int)m_FbxVtx[MeshCount].size();

	// 애니메이션중에 

	// 매쉬의 기본 매트릭스를 가져온다.
	// 소스 그대로 가져오는 
	fbxsdk::FbxNode* Node = _Mesh->GetNode();
	const FbxVector4 vS = Node->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot);
	const FbxVector4 vR = Node->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot);
	const FbxVector4 vT = Node->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot);

	// 쿼터니온으로 얻어올수 있다.

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

				HFBXBONEDATA* pBone = FindBone(StrBoneName);

				if (nullptr == pBone)
				{
					continue;
				}

				// 인덱스와 가중치를 모으기만 한다. 재계산은 하지 않은 상태이다.
				LoadWeightAndIndex(pCluster, pBone->iIndex, m_FbxVtx[MeshCount]);
				// 변경해서 주의해야함
				LoadOffsetMatrix(pCluster, matTrans, pBone);
				LoadTimeTransForm(Node, pCluster, matTrans, pBone);
			}

		}

	}

	// 이거때문에 가중치와 


	++MeshCount;

}

void HFBX::LoadUv(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
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

void HFBX::LoadBinormal(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
{
	int iCount = _Mesh->GetElementBinormalCount();

	if (0 == iCount)
	{
		AMSG(L"GetElementBinormalCount가 여러개 입니다.");
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

void HFBX::LoadTangent(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
{
	int iCount = _Mesh->GetElementTangentCount();

	if (0 == iCount)
	{
		AMSG(L"GetElementTangentCount가 여러개 입니다.");
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

void HFBX::LoadNormal(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
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
void HFBX::MaterialCheck(fbxsdk::FbxNode* _Node, int& _Count)
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


			m_UserMatData[_Count].DifColor = MaterialColor(pMtrl, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
			m_UserMatData[_Count].AmbColor = MaterialColor(pMtrl, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
			m_UserMatData[_Count].SpcColor = MaterialColor(pMtrl, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
			m_UserMatData[_Count].EmvColor = MaterialColor(pMtrl, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);
			m_UserMatData[_Count].SpecularPower = MaterialFactor(pMtrl, FbxSurfaceMaterial::sSpecularFactor);
			m_UserMatData[_Count].Shininess = MaterialFactor(pMtrl, FbxSurfaceMaterial::sShininess);
			m_UserMatData[_Count].TransparencyFactor = MaterialFactor(pMtrl, FbxSurfaceMaterial::sTransparencyFactor);


			m_UserMatData[_Count].DifTexture = MaterialTex(pMtrl, FbxSurfaceMaterial::sDiffuse);
			m_UserMatData[_Count].BmpTexture = MaterialTex(pMtrl, FbxSurfaceMaterial::sNormalMap);
			m_UserMatData[_Count].SpcTexture = MaterialTex(pMtrl, FbxSurfaceMaterial::sSpecular);


			++_Count;
		}

		// m_UserMatData.push_back();
	}

	int ChildCount = _Node->GetChildCount();

	for (int i = 0; i < ChildCount; i++)
	{
		MaterialCheck(_Node->GetChild(i), _Count);
	}

}

Game_Vector HFBX::MaterialColor(FbxSurfaceMaterial* pMtrl, const char* _ColorName, const char* _FactorName)
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

float HFBX::MaterialFactor(FbxSurfaceMaterial* pMtrl, const char* _FactorName) {
	double dFactor = 0;
	FbxProperty FactorPro = pMtrl->FindProperty(_FactorName);

	// 유효체크 이 색상이 3d맥스에서 정말 사용하는 값이야?
	if (true == FactorPro.IsValid())
	{
		dFactor = FactorPro.Get<FbxDouble>();
	}

	return (float)dFactor;
}

Game_String HFBX::MaterialTex(FbxSurfaceMaterial* pMtrl, const char* _FactorName)
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
			AMSG(L"if (iTexCount > 0)");
		}

	}
	else {
		AMSG(L"if (true != TexturePro.IsValid())");
	}

	return Str;
}

void HFBX::Trangulate(fbxsdk::FbxNode* _Node, int& _MatCount)
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

void HFBX::BoneCheck()
{
	// 마치 트리구조와 비슷하므로
	// 재귀가 아주 유용하다.
	int BoneCount = 0;
	LoadBoneCount(m_Scene->GetRootNode(), BoneCount);

	if (0 < BoneCount)
	{
		m_UserBoneData.resize(BoneCount);
	}

	// 이미 다 리사이즈를 했기 때문에.
	BoneCount = 0;
	LoadBone(m_Scene->GetRootNode(), BoneCount);
}


void HFBX::LoadBoneCount(fbxsdk::FbxNode* _Node, int& _Count)
{
	fbxsdk::FbxNodeAttribute* _Att = _Node->GetNodeAttribute();

	if (nullptr != _Att
		&& _Att->GetAttributeType()
		== fbxsdk::FbxNodeAttribute::eSkeleton)
	{
		++_Count;
		// m_UserBoneData.resize
	}

	int ChildCount = _Node->GetChildCount();

	for (int i = 0; i < ChildCount; i++)
	{
		LoadBoneCount(_Node->GetChild(i), _Count);
	}
}

void HFBX::LoadBone(fbxsdk::FbxNode* _Node, int& _iIndex, int _iDepth, int _iParent) {

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
		m_MapBoneData[m_UserBoneData[_iIndex].Name] = &m_UserBoneData[_iIndex];
		++_iIndex;
	}

	int ChildCount = _Node->GetChildCount();
	for (int i = 0; i < ChildCount; i++)
	{
		LoadBone(_Node->GetChild(i), _iIndex, _iDepth + 1, _iIndex);
	}

}

void HFBX::AniCheck()
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
	m_UserAniData.resize(AniNameArray.Size());

	for (int i = 0; i < AniNameArray.GetCount(); i++)
	{
		FbxAnimStack* pAniStatic = m_Scene->FindMember<FbxAnimStack>(AniNameArray[i]->Buffer());

		// pAniStatic 애니메이션 하나하나의 세부적인 정보를 담고 있다.
		if (nullptr == pAniStatic)
		{
			AMSG(L"if (nullptr == pAniStatic)");
			continue;
		}

		// std::wcout << m_UserAniData[i].AniName << std::endl;



		m_UserAniData[i].AniName = CA2W(pAniStatic->GetName(), CP_UTF8).m_psz;

		// HGAMEDEBUG3D::ConsoleText(L"애니메이션 : %s 탐색", pAniStatic->GetName());
		FbxTakeInfo* TakeInfo = m_Scene->GetTakeInfo(pAniStatic->GetName());

		m_UserAniData[i].StartTime = TakeInfo->mLocalTimeSpan.GetStart();
		m_UserAniData[i].EndTime = TakeInfo->mLocalTimeSpan.GetStop();
		m_UserAniData[i].TimeMode = m_Scene->GetGlobalSettings().GetTimeMode();
		m_UserAniData[i].TimeStartCount = m_UserAniData[i].StartTime.GetFrameCount(m_UserAniData[i].TimeMode);
		m_UserAniData[i].TimeEndCount = m_UserAniData[i].EndTime.GetFrameCount(m_UserAniData[i].TimeMode);
		m_UserAniData[i].FrameCount = m_UserAniData[i].TimeEndCount - m_UserAniData[i].TimeStartCount;
		m_UserAniData[i].FbxModeCount = m_UserAniData[i].FrameModeCount();
	}

	for (int i = 0; i < AniNameArray.GetCount(); i++)
	{
		delete AniNameArray[i];
	}

}

HFBXBONEDATA* HFBX::FindBone(const Game_String& _Name)
{
	if (m_MapBoneData.end() == m_MapBoneData.find(_Name))
	{
		AMSG(_Name + L" 이름의 본은 존재하지 않습니다.");
	}

	return m_MapBoneData[_Name];
}

void HFBX::LoadWeightAndIndex(fbxsdk::FbxCluster* _Cluster, int _BoneIndex, std::vector<VTX3DMESHIN>& _ArrVtx)
{
	// fbxsdk::FbxCluster은 스키닝 정보를 담고 있는 정보체이다.
	int iIndexCount = _Cluster->GetControlPointIndicesCount();

	for (size_t i = 0; i < iIndexCount; i++)
	{
		//// _BoneIndex 인덱스
		HFBXIW IW;

		IW.Index = _BoneIndex;
		IW.Weight = _Cluster->GetControlPointWeights()[i];

		int iClusterIndex = _Cluster->GetControlPointIndices()[i];
		m_MapWI[MeshCount][iClusterIndex].push_back(IW);
	}
}


// 본마다 가지고 있는 
void HFBX::LoadOffsetMatrix(fbxsdk::FbxCluster* _Cluster, const fbxsdk::FbxAMatrix& _BaseTrans, HFBXBONEDATA* _Bone)
{
	// 애니메이션을 위한 가장 기본적인 축 행렬을 만들어주는 것인데.

	// 3D맥스의 기본적인 좌표축이 어긋나 있다. 
	/*

	1 0 0 0
	0 0 1 0
	0 1 0 0
	0 0 0 1

	10  0 0
	0c  s 0
	0-c c 0
	00  0 1

	//
	1 0 0 0    1 0 0 0 	   1 0 0 0
	0 0 1 0    0 1 0 0 	   0 0 1 0
	0 1 0 0	   0 0 -10	   0 1 0 0
	0 0 0 1	   0 0 0 1	   0 0 0 1
	
	0	1	0	0
	1	0	0	0
	* + * + * + *

	1 0 0 0    1 0 0 0
	0 0-1 0	   0 0 1 0
	0 1 0 0	   0 1 0 0
	0 0 0 1	   0 0 0 1

	0 0-1 0
	0 1 0 0
	* + * + * + *

	1 0 0 0
	0-1 0 0
	0 0 1 0
	0 0 0 1

	*/
	FbxAMatrix ClusterMat;
	FbxAMatrix LinkClusterMat;

	_Cluster->GetTransformMatrix(ClusterMat);
	_Cluster->GetTransformLinkMatrix(LinkClusterMat);

	FbxAMatrix Offset;
	Offset = LinkClusterMat.Inverse() * ClusterMat * _BaseTrans;
	// 다이렉트 좌표계로 바꾸려고 하는것이다.
	Offset = ReMat * Offset * ReMat;
	_Bone->Offset = FbxMatToHMAT(Offset);
	_Bone->Bone = FbxMatToHMAT(_BaseTrans);

}

HMATRIX HFBX::FbxMatToHMAT(const fbxsdk::FbxAMatrix& _BaseTrans)
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

Game_Vector HFBX::FbxVecToHVEC(const fbxsdk::FbxVector4& _BaseVector)
{
	Game_Vector Vec;
	for (int i = 0; i < 4; i++)
	{
		Vec.Arr[i] = (float)_BaseVector.mData[i];
	}
	return Vec;

}

Game_Vector HFBX::FbxQToHVEC(const fbxsdk::FbxQuaternion& _BaseQ)
{
	Game_Vector Vec;
	for (int i = 0; i < 4; i++)
	{
		Vec.Arr[i] = (float)_BaseQ.mData[i];
	}
	return Vec;
}

void HFBX::LoadTimeTransForm(fbxsdk::FbxNode* _Node, fbxsdk::FbxCluster* _Cluster, const fbxsdk::FbxAMatrix& _BaseTrans, HFBXBONEDATA* _Bone)
{
	for (size_t i = 0; i < m_UserAniData.size(); i++)
	{
		if (0 == _Bone->iIndex)
		{
			int a = 0;
		}

		FbxLongLong Start = m_UserAniData[i].TimeStartCount;
		FbxLongLong End = m_UserAniData[i].TimeEndCount;

		m_UserAniData[i].m_AniFrameData[_Bone->iIndex].BoneIndex = _Bone->iIndex;

		// 1초당 30프레임이니까
		// 300 10초시간대의 애니메이션
		// 1434
		for (FbxLongLong j = Start; j <= End; j++)
		{
			FbxTime tTime = {};
			// 타임모드를 기반으로한 시간을 알아낼수 있다.
			tTime.SetFrame(j, m_UserAniData[i].TimeMode);

			// 시간대비 0 1 2 3 4 5 6 7 8 9  ~~~~~~~ 1434
			FbxAMatrix matOff = _Node->EvaluateGlobalTransform(tTime) * _BaseTrans;
			FbxAMatrix matCur = matOff.Inverse() * _Cluster->GetLink()->EvaluateGlobalTransform(tTime);
			// FbxAMatrix matCur = matOff.Inverse() * _Cluster->GetLink()->EvaluateLocalTransform(tTime);

			matCur = ReMat * matCur * ReMat;

			m_UserAniData[i].m_AniFrameData[_Bone->iIndex].m_Data[j].Time = tTime.GetSecondDouble();
			m_UserAniData[i].m_AniFrameData[_Bone->iIndex].m_Data[j].FrameMat = FbxMatToHMAT(matCur);
			m_UserAniData[i].m_AniFrameData[_Bone->iIndex].m_Data[j].S = FbxVecToHVEC(matCur.GetS());
			m_UserAniData[i].m_AniFrameData[_Bone->iIndex].m_Data[j].Q = FbxQToHVEC(matCur.GetQ());
			m_UserAniData[i].m_AniFrameData[_Bone->iIndex].m_Data[j].T = FbxVecToHVEC(matCur.GetT());
		}
	}
}



void HFBX::CalWeightAndIndex()
{
	for (auto& MapWI : m_MapWI[MeshCount])
	{
		if (MapWI.second.size() > 4)
		{
			AMSG(L"가중치 계산에 4개 이상의 값이 들어왔습니다.");

			// 
			// 람다 문법 언네임드 펑션.
			std::sort(MapWI.second.begin(), MapWI.second.end(),
				[](const HFBXIW& _Left, const HFBXIW& _Right) {
					return _Left.Weight > _Right.Weight;
				}
			);

			double dSum = 0.0;

			for (int i = 0; i < MapWI.second.size(); i++)
			{
				dSum += MapWI.second[i].Weight;
			}

			double dInterPolate = 1.0f - dSum;

			std::vector<HFBXIW>::iterator IterErase = MapWI.second.begin() + 4;

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

		memcpy_s(m_FbxVtx[MeshCount][MapWI.first].Weight.Arr, sizeof(Game_Vector), Weight, sizeof(Game_Vector));
		memcpy_s(m_FbxVtx[MeshCount][MapWI.first].Index, sizeof(Game_Vector), Index, sizeof(Game_Vector));
	}
	++MeshCount;
}

void HFBX::LoadUserFormat(const Game_String& _Path)
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

	LoadFile >> Size;
	m_UserMatData.resize(Size);
	for (size_t i = 0; i < m_UserMatData.size(); i++)
	{
		LoadFile >> m_UserMatData[i].DifColor; //Game_Vector DifColor;
		LoadFile >> m_UserMatData[i].AmbColor; //Game_Vector AmbColor; // 빛
		LoadFile >> m_UserMatData[i].SpcColor; //Game_Vector SpcColor; // 빛
		LoadFile >> m_UserMatData[i].EmvColor; //Game_Vector EmvColor; // 빛
		LoadFile >> m_UserMatData[i].SpecularPower;//float SpecularPower;		// 빛의 강도
		LoadFile >> m_UserMatData[i].TransparencyFactor;//float TransparencyFactor;	// 빛의 강도
		LoadFile >> m_UserMatData[i].Shininess;//float Shininess;			// 빛의 강도
		LoadFile >> m_UserMatData[i].DifTexture;//Game_String DifTexture;	// 텍스처경로 
		LoadFile >> m_UserMatData[i].BmpTexture;//Game_String BmpTexture; // 텍스처경로
		LoadFile >> m_UserMatData[i].SpcTexture;//Game_String SpcTexture; // 텍스처경로
	}

	LoadFile >> Size;
	if (-1 == Size)
	{
		// 애니메이션이 없다는 이야기.
		return;
	}

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
		m_MapBoneData[m_UserBoneData[i].Name] = &m_UserBoneData[i];
	}

	CreateMesh();
}

void HFBX::SaveUserFormat(const Game_String& _Path)
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

	// std::vector<HFBXMATDATA> m_UserMatData;
	SaveFile << (int)m_UserMatData.size();
	for (size_t i = 0; i < m_UserMatData.size(); i++)
	{
		SaveFile << m_UserMatData[i].DifColor; //Game_Vector DifColor;
		SaveFile << m_UserMatData[i].AmbColor; //Game_Vector AmbColor; // 빛
		SaveFile << m_UserMatData[i].SpcColor; //Game_Vector SpcColor; // 빛
		SaveFile << m_UserMatData[i].EmvColor; //Game_Vector EmvColor; // 빛
		SaveFile << m_UserMatData[i].SpecularPower;//float SpecularPower;		// 빛의 강도
		SaveFile << m_UserMatData[i].TransparencyFactor;//float TransparencyFactor;	// 빛의 강도
		SaveFile << m_UserMatData[i].Shininess;//float Shininess;			// 빛의 강도
		SaveFile << m_UserMatData[i].DifTexture;//Game_String DifTexture;	// 텍스처경로 
		SaveFile << m_UserMatData[i].BmpTexture;//Game_String BmpTexture; // 텍스처경로
		SaveFile << m_UserMatData[i].SpcTexture;//Game_String SpcTexture; // 텍스처경로
	}

	if (0 >= m_UserAniData.size())
	{
		// 애니메이션이 없다는 이야기죠?
		SaveFile << (int)-1;
		return;
	}


	// std::vector<HFBXANIDATA> m_UserAniData; // 애니메이션 

	std::wcout << L"ANIDATA SAVE" << std::endl;

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