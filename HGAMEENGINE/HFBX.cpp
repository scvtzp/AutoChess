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
		AMSG(L"�ε��� ���� ���ؽ� ���� ��Ī ����");
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
		// 0 1 �� �׸��� ���׸��� 2������� �ϰ�
		// 0 2 �� �׸��� ���׸��� 5�� ������ϰ� �̷� ��Ȳ�� �Դ�.
		AMSG(L"�Ž��� ���׸����� ũ�Ⱑ �ٸ��ϴ�.");
		return;
	}


	// HGAMEDEBUG3D::ConsoleText(L"���ؽ� �� �ε��� ���� ���� �Ž����� �Ϸ�");


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

	// ���ϰ� �����͸� �����Ѵ�.
	if (false == m_Imp->Initialize(CW2A(GameFile.FullPath(), CP_UTF8).m_psz, -1, m_Mgr->GetIOSettings()))
	{
		AMSG(GameFile.FullPath() + " FBX ���� �̴ϼȶ����� ����");
	}

	// �⺻ ����ȯ���� �о�´�.
	// �� �⺻���� �̴ܰ迡�� �����Ҽ� �ִµ�.
	// �׷��� �״�� �д´�.
	m_Imp->Import(m_Scene);
	eLocalSpaceType = m_Scene->GetGlobalSettings().GetAxisSystem();



	if (eLocalSpaceType == fbxsdk::FbxAxisSystem::eDirectX)
	{
		m_Scene->GetGlobalSettings().SetAxisSystem(fbxsdk::FbxAxisSystem::eMax);
	}
	else if (eLocalSpaceType != fbxsdk::FbxAxisSystem::eMax)
	{
		// ���� ������ ������ �ٸ���.
		m_Scene->GetGlobalSettings().SetAxisSystem(fbxsdk::FbxAxisSystem::eMax);
	}


	AniCheck();
	// HGAMEDEBUG3D::ConsoleText(L"�ִϸ��̼� üũ �Ϸ� �ִϸ��̼� ���� : %d", m_UserAniData.size());

	int MatCount = 0;
	Trangulate(m_Scene->GetRootNode(), MatCount);
	// HGAMEDEBUG3D::ConsoleText(L"�ﰢȭ �Ϸ�");

	m_UserMatData.resize(MatCount);
	MatCount = 0;
	MaterialCheck(m_Scene->GetRootNode(), MatCount);
	// HGAMEDEBUG3D::ConsoleText(L"��Ƽ���� üũ �Ϸ� %d", MatCount);

	for (size_t i = 0; i < m_UserMatData.size(); i++)
	{
		Game_String DirPath = GameFile.DirPath();
		m_UserMatData[i].DifTexture.EraseStr(DirPath);
	}

	MeshVtxData(m_Scene->GetRootNode());

	BoneCheck();

	// STATIC �Ž�
	if (0 >= m_UserAniData.size())
	{
		CreateMesh();
		// HGAMEDEBUG3D::ConsoleText(m_Path + L" static �Ž� �Ϸ�");
		return;
	}

	//// HGAMEDEBUG3D::ConsoleText(L"�� üũ �Ϸ� �� ���� : %d\n", m_UserBoneData.size());
	size_t I = m_UserAniData.size();

	for (size_t AniIndex = 0; AniIndex < m_UserAniData.size(); AniIndex++)
	{
		// �ִϸ��̼��� 1���ε� ���� 134���Դϴ�.
		// 134�������� �Ѵ�.
		m_UserAniData[AniIndex].m_AniFrameData.resize(m_UserBoneData.size());

		for (size_t BoneCount = 0; BoneCount < m_UserBoneData.size(); BoneCount++)
		{
			size_t I = m_UserBoneData.size();
			// 1434�Ը���
			m_UserAniData[AniIndex].m_AniFrameData[BoneCount].m_Data.resize(m_UserAniData[AniIndex].FrameCount + 1);
		}
	}

	// �̰� ����ġ�� index�� �����Ҷ� ����� �༮�ε� 
	// �̳༮�� ���°���
	// Game_Vector Weight; // �󸶳� ������ �޾ƾ� �ϴ°�? 0.5 ������ �޴´�.
	// int Index[4]; // 21��° ����
	m_MapWI.resize(m_FbxVtx.size());

	// �̳༮�� Ȥ�ó� ��Ʈ�� 
	MeshCount = 0;

	// �ͱ��? 134 * 1434
	// �ִϸ��̼��� 1434�������ε�.
	// ���� 134���� �ְ�.
	// ���Ҽ��� ����.
	// �߰��� �߶�������� ����.
	// �ִϸ��̼��� 
	// ��ü���˰� ������ �ε��� �Ұ��̴�.
	// ������ �ε����� ���ǵ�.
	// ������ �̷к��� �˾ƾ� �Ѵ�.
	AnimationClipCheck(m_Scene->GetRootNode());

	MeshCount = 0;
	CalWeightAndIndex();
	// �ε����� ����ġ�� �� ��������Ƿ� ����������.
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

	// ���������� �������ϰŶ� �̷��� �ϴ°� �ƴϰ�.
	// ��� ��尡 ������ �ִ����� �𸣱� ������ �̷��� �ϴ°��̴�.
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

	// ������ �˰� �ִ� LocalSpace�� �����ϴ� Postion������ ù��° ������
	// ��ġ������ ù��° �������̴�.
	FbxVector4* pVtxPos = _Mesh->GetControlPoints();
	VtxData.resize(iVtxCount);

	// �ƽ������� �ﰢȭ �߱� ������
	for (int i = 0; i < VtxData.size(); i++)
	{
		// �ణ �����ϱ� ����� 021�� �޾ƿ��°ǵ�
		// �ƽ� ������ �������� z�� y�� ǥ���ϱ� ���ؼ� ó��.
		VtxData[i].Pos.x = (float)pVtxPos[i].mData[0];
		VtxData[i].Pos.y = (float)pVtxPos[i].mData[2];
		VtxData[i].Pos.z = (float)pVtxPos[i].mData[1];
		VtxData[i].Pos.w = 1.0f;
		// HGAMEDEBUG3D::ConsoleText(L"%d = %f, %f, %f", i, VtxData[i].Pos.x, VtxData[i].Pos.y, VtxData[i].Pos.z);
	}


	UINT VtxId = 0;

	// �� �Ž��� �׸��µ� ����� ���� ����
	FbxGeometryElementMaterial* MatData = _Mesh->GetElementMaterial();
	// �׸��µ� ����� �������
	// ���� �ε��ϰ� �ִ� �����͸� ��� ������ �׷ȴ���?
	int MatCount = _Mesh->GetNode()->GetMaterialCount();
	IdxData.resize(MatCount);

	// �ﰢ��
	int iPolygonCount = _Mesh->GetPolygonCount();

	// std::vector<std::list<UINT>> m_List;
	// m_List.resize(MatCount);

	// �ﰢ�� ������ŭ ���鼭
	for (int PolgonIndex = 0; PolgonIndex < iPolygonCount; PolgonIndex++)
	{
		// ��κ� 3�ϰ��̴�.
		int iPolyGonSize = _Mesh->GetPolygonSize(PolgonIndex);
		if (3 != iPolyGonSize)
		{
			AMSG(L"�ﰢ���� �ƴ� ���� �߰ߵƽ��ϴ�.");
		}

		int iIDX[3] = {};
		// �� �ﰢ���� �ʿ��� 
		for (int TriIndex = 0; TriIndex < iPolyGonSize; TriIndex++)
		{
			//                                      0��° �ﰢ����    0~2��° ���� ������ �ּ���.
			// �׳༮�� 17641���� ������ 5��° �����̾�.
			// +�׸��� �����̱⵵ �ϴ�.
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

		// �ε��� ����
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

	// ���������� �������ϰŶ� �̷��� �ϴ°� �ƴϰ�.
	// ��� ��尡 ������ �ִ����� �𸣱� ������ �̷��� �ϴ°��̴�.
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
		AMSG(L"��Ű�� ������ �������� �ʽ��ϴ�.");
	}

	int VtxCount = (int)m_FbxVtx[MeshCount].size();

	// �ִϸ��̼��߿� 

	// �Ž��� �⺻ ��Ʈ������ �����´�.
	// �ҽ� �״�� �������� 
	fbxsdk::FbxNode* Node = _Mesh->GetNode();
	const FbxVector4 vS = Node->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot);
	const FbxVector4 vR = Node->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot);
	const FbxVector4 vT = Node->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot);

	// ���ʹϿ����� ���ü� �ִ�.

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
			//	AMSG(L"���� ��� �ִϸ��̼� �Դϴ�.");
			//}

			// ����
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

				// �ε����� ����ġ�� �����⸸ �Ѵ�. ������ ���� ���� �����̴�.
				LoadWeightAndIndex(pCluster, pBone->iIndex, m_FbxVtx[MeshCount]);
				// �����ؼ� �����ؾ���
				LoadOffsetMatrix(pCluster, matTrans, pBone);
				LoadTimeTransForm(Node, pCluster, matTrans, pBone);
			}

		}

	}

	// �̰Ŷ����� ����ġ�� 


	++MeshCount;

}

void HFBX::LoadUv(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
{
	int iCount = _Mesh->GetElementUVCount();

	if (0 == iCount)
	{
		AMSG(L"UV�� ������ �Դϴ�.");
	}

	FbxGeometryElementUV* pElement = _Mesh->GetElementUV();
	int iDataIndex = VtxId;
	// ������ ������� �ε��� �༮�̳�.
	// �� ������� ������̴�. 
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
			// AMSG(L"���� ����� ó���Ҽ� ���� ����Դϴ�.");
		}
	}
	// 17641���� �ƴ���.
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
			AMSG(L"���� ����� ó���Ҽ� ���� ����Դϴ�.");
		}
	}

	// �븻�� �����԰�.
	FbxVector2 vUV = pElement->GetDirectArray().GetAt(iDataIndex);

	_ArrVtx[_Index].Uv.x = (float)(vUV.mData[0] - (int)vUV.mData[0]);
	_ArrVtx[_Index].Uv.y = (float)(1.0f - (vUV.mData[1] - (int)vUV.mData[1]));


	// _ArrVtx[_Index].Uv.x = (float)vUV.mData[0];
	// _ArrVtx[_Index].Uv.y = 1.0f - (float)vUV.mData[1];
	// Y�� �����´�.

	// _ArrVtx[_Index].BiNormal.z = (float)BiNormal.mData[1];
}

void HFBX::LoadBinormal(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
{
	int iCount = _Mesh->GetElementBinormalCount();

	if (0 == iCount)
	{
		AMSG(L"GetElementBinormalCount�� ������ �Դϴ�.");
	}


	// FbxGeometryElementUserData/* pElement = _Mesh->GetElementTangent();
	// _Mesh->GetElementBinormalCount();
	FbxGeometryElementBinormal* pElement = _Mesh->GetElementBinormal();
	int iDataIndex = VtxId;

	// ������ ������� �ε��� �༮�̳�.
	// �� ������� ������̴�. 
	if (pElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(VtxId);
		}
		else
		{
			iDataIndex = VtxId;
			// AMSG(L"���� ����� ó���Ҽ� ���� ����Դϴ�.");
		}
	}
	// 17641���� �ƴ���.
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
			AMSG(L"���� ����� ó���Ҽ� ���� ����Դϴ�.");
		}
	}

	// �븻�� �����԰�.
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
		AMSG(L"GetElementTangentCount�� ������ �Դϴ�.");
	}
	// FbxGeometryElementUserData/* pElement = _Mesh->GetElementTangent();
	FbxGeometryElementTangent* pElement = _Mesh->GetElementTangent();
	int iDataIndex = VtxId;

	// ������ ������� �ε��� �༮�̳�.
	// �� ������� ������̴�. 
	if (pElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(VtxId);
		}
		else
		{
			iDataIndex = VtxId;
			// AMSG(L"���� ����� ó���Ҽ� ���� ����Դϴ�.");
		}
	}
	// 17641���� �ƴ���.
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
			AMSG(L"���� ����� ó���Ҽ� ���� ����Դϴ�.");
		}
	}

	// �븻�� �����԰�.
	FbxVector4 Tangent = pElement->GetDirectArray().GetAt(iDataIndex);

	// ���̰� 1�� �ƴҼ� �ִ�.
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
		AMSG(L"GetElementNormalCount�� ������ �Դϴ�.");
	}


	FbxGeometryElementNormal* pElement = _Mesh->GetElementNormal();
	int iDataIndex = VtxId;

	// ������ ������� �ε��� �༮�̳�.
	// �� ������� ������̴�. 
	if (pElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(VtxId);
		}
		else
		{
			iDataIndex = VtxId;
			// AMSG(L"���� ����� ó���Ҽ� ���� ����Դϴ�.");
		}
	}
	// 17641���� �ƴ���.
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
			AMSG(L"���� ����� ó���Ҽ� ���� ����Դϴ�.");
		}
	}

	// �븻�� �����԰�.
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
				AMSG(L"if (nullptr == pMtrl) ��Ƽ���� ���� ����");
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

	// ��ȿüũ �� ������ 3d�ƽ����� ���� ����ϴ� ���̾�?
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

	// ��ȿüũ �� ������ 3d�ƽ����� ���� ����ϴ� ���̾�?
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

		// ���� �˰����� ������ ��Ʈ�� ��������� ������
		// �ϴ� 1���� �ִٰ� �����ϰ� ó���ϰڴ�.
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
	// ��ġ Ʈ�������� ����ϹǷ�
	// ��Ͱ� ���� �����ϴ�.
	int BoneCount = 0;
	LoadBoneCount(m_Scene->GetRootNode(), BoneCount);

	if (0 < BoneCount)
	{
		m_UserBoneData.resize(BoneCount);
	}

	// �̹� �� ������� �߱� ������.
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
		// �ʿ��� �־��ִ� ������ ���߿� ���� �̸����� ���� ã�� ���ؼ� �̴�.
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
	// �ִϸ��̼��� �̸��� ����Ʈ�� ���´�.
	m_Scene->FillAnimStackNameArray(AniNameArray);

	if (0 == AniNameArray.Size())
	{
		return;
	}

	// ������Ʈ �������� ��Ȳ.
	// �ڵ���� -> ��Ÿ�� ���̺귯�� -> 
	// ����� �ϴ� ���߽����� ����� MTD
	// ������ �ϴ� ���߽����� MD
	// HTOOL�� ���� ���̺귯���� MFC ���

	// �������̺귯������ mfc������� ����
	// ����Ƽ ����� ��� ��ȯ

	// ���ʹ� ������ �̸� ����� �����ִ� RESIZE��
	// reserve�� ����ϴ°��� ����.
	m_UserAniData.resize(AniNameArray.Size());

	for (int i = 0; i < AniNameArray.GetCount(); i++)
	{
		FbxAnimStack* pAniStatic = m_Scene->FindMember<FbxAnimStack>(AniNameArray[i]->Buffer());

		// pAniStatic �ִϸ��̼� �ϳ��ϳ��� �������� ������ ��� �ִ�.
		if (nullptr == pAniStatic)
		{
			AMSG(L"if (nullptr == pAniStatic)");
			continue;
		}

		// std::wcout << m_UserAniData[i].AniName << std::endl;



		m_UserAniData[i].AniName = CA2W(pAniStatic->GetName(), CP_UTF8).m_psz;

		// HGAMEDEBUG3D::ConsoleText(L"�ִϸ��̼� : %s Ž��", pAniStatic->GetName());
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
		AMSG(_Name + L" �̸��� ���� �������� �ʽ��ϴ�.");
	}

	return m_MapBoneData[_Name];
}

void HFBX::LoadWeightAndIndex(fbxsdk::FbxCluster* _Cluster, int _BoneIndex, std::vector<VTX3DMESHIN>& _ArrVtx)
{
	// fbxsdk::FbxCluster�� ��Ű�� ������ ��� �ִ� ����ü�̴�.
	int iIndexCount = _Cluster->GetControlPointIndicesCount();

	for (size_t i = 0; i < iIndexCount; i++)
	{
		//// _BoneIndex �ε���
		HFBXIW IW;

		IW.Index = _BoneIndex;
		IW.Weight = _Cluster->GetControlPointWeights()[i];

		int iClusterIndex = _Cluster->GetControlPointIndices()[i];
		m_MapWI[MeshCount][iClusterIndex].push_back(IW);
	}
}


// ������ ������ �ִ� 
void HFBX::LoadOffsetMatrix(fbxsdk::FbxCluster* _Cluster, const fbxsdk::FbxAMatrix& _BaseTrans, HFBXBONEDATA* _Bone)
{
	// �ִϸ��̼��� ���� ���� �⺻���� �� ����� ������ִ� ���ε�.

	// 3D�ƽ��� �⺻���� ��ǥ���� ��߳� �ִ�. 
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
	// ���̷�Ʈ ��ǥ��� �ٲٷ��� �ϴ°��̴�.
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

		// 1�ʴ� 30�������̴ϱ�
		// 300 10�ʽð����� �ִϸ��̼�
		// 1434
		for (FbxLongLong j = Start; j <= End; j++)
		{
			FbxTime tTime = {};
			// Ÿ�Ӹ�带 ��������� �ð��� �˾Ƴ��� �ִ�.
			tTime.SetFrame(j, m_UserAniData[i].TimeMode);

			// �ð���� 0 1 2 3 4 5 6 7 8 9  ~~~~~~~ 1434
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
			AMSG(L"����ġ ��꿡 4�� �̻��� ���� ���Խ��ϴ�.");

			// 
			// ���� ���� ����ӵ� ���.
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

	// std::vector<std::vector<VTX3DMESHIN>> m_FbxVtx; // ���ؽ�

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

	// std::vector<std::vector<std::vector<UINT>>> m_FbxIdx; // �ε���
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
		LoadFile >> m_UserMatData[i].AmbColor; //Game_Vector AmbColor; // ��
		LoadFile >> m_UserMatData[i].SpcColor; //Game_Vector SpcColor; // ��
		LoadFile >> m_UserMatData[i].EmvColor; //Game_Vector EmvColor; // ��
		LoadFile >> m_UserMatData[i].SpecularPower;//float SpecularPower;		// ���� ����
		LoadFile >> m_UserMatData[i].TransparencyFactor;//float TransparencyFactor;	// ���� ����
		LoadFile >> m_UserMatData[i].Shininess;//float Shininess;			// ���� ����
		LoadFile >> m_UserMatData[i].DifTexture;//Game_String DifTexture;	// �ؽ�ó��� 
		LoadFile >> m_UserMatData[i].BmpTexture;//Game_String BmpTexture; // �ؽ�ó���
		LoadFile >> m_UserMatData[i].SpcTexture;//Game_String SpcTexture; // �ؽ�ó���
	}

	LoadFile >> Size;
	if (-1 == Size)
	{
		// �ִϸ��̼��� ���ٴ� �̾߱�.
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


	// std::vector<HFBXBONEDATA> m_UserBoneData; // ��
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

	// std::vector<std::vector<VTX3DMESHIN>> m_FbxVtx; // ���ؽ�
	SaveFile << (int)m_FbxVtx.size();
	for (size_t i = 0; i < m_FbxVtx.size(); i++)
	{
		SaveFile << (int)m_FbxVtx[i].size();
		for (size_t j = 0; j < m_FbxVtx[i].size(); j++)
		{
			SaveFile << m_FbxVtx[i][j];
		}
	}

	// std::vector<std::vector<std::vector<UINT>>> m_FbxIdx; // �ε���
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
		SaveFile << m_UserMatData[i].AmbColor; //Game_Vector AmbColor; // ��
		SaveFile << m_UserMatData[i].SpcColor; //Game_Vector SpcColor; // ��
		SaveFile << m_UserMatData[i].EmvColor; //Game_Vector EmvColor; // ��
		SaveFile << m_UserMatData[i].SpecularPower;//float SpecularPower;		// ���� ����
		SaveFile << m_UserMatData[i].TransparencyFactor;//float TransparencyFactor;	// ���� ����
		SaveFile << m_UserMatData[i].Shininess;//float Shininess;			// ���� ����
		SaveFile << m_UserMatData[i].DifTexture;//Game_String DifTexture;	// �ؽ�ó��� 
		SaveFile << m_UserMatData[i].BmpTexture;//Game_String BmpTexture; // �ؽ�ó���
		SaveFile << m_UserMatData[i].SpcTexture;//Game_String SpcTexture; // �ؽ�ó���
	}

	if (0 >= m_UserAniData.size())
	{
		// �ִϸ��̼��� ���ٴ� �̾߱���?
		SaveFile << (int)-1;
		return;
	}


	// std::vector<HFBXANIDATA> m_UserAniData; // �ִϸ��̼� 

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


	// std::vector<HFBXBONEDATA> m_UserBoneData; // ��
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