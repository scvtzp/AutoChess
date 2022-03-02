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

/* MeshInit()���� �ϴ� ���� ���⼭ �״�� �մϴ�.  */
void Game_StaticFBX::CreateMesh()
{
	if (m_FbxVtx.size() != m_FbxIdx.size())
	{
		AMSG(L"�ε��� ���� ���ؽ� ���� ��Ī ����");
		return;
	}

	m_VB.resize(m_FbxVtx.size());
	m_IB.resize(m_FbxIdx.size());

	/* �����ӿ�ũ ������� �����մϴ�. */
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

	/* �����ӿ�ũ�� ������ Mesh 1 : Material 1 �Դϴ�. */
	if (m_Mesh.size() != m_UserMaterialData.size())
	{
		AMSG(L"�Ž��� ���׸����� ũ�Ⱑ �ٸ��ϴ�.");
		return;
	}

	// LejGameDebug::ConsolePrintText(L"���ؽ� �� �ε��� ���� ���� �Ž����� �Ϸ�");
}



// � ������ �޽������� ....
// � ������ FBX������ ��ġ�� ���͸� ������ 
// �̷��� �Ǿ����� �� �ִ�. 
// PLAYER.FBX
// PLAYER_ATTACK.FBX
// PLAYER_IDLE.FBX
// 
// Ȥ��
// MOSTER.FBX
// ������ �̷��� �Ǿ��ְ�
// �� �ȿ� ANIDATA�� ������ �ִ� ������ ���� �� ���ɼ��� �ִ�. 
// 
// FBX�� ���� �� �� �޽��� �Ǿ��ִ�. 
// �����ӿ�ũ�� �з��� �°� �Ǿ����� �ʴٸ�, LOAD�� �ϳ� �� ����� �ȴ�. 


// ������
// �ִϸ��̼� �κ�  
// �޽� �κ� 
// ���� ���� �ε��Ѵ�. 


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
		AMSG(GameFile.FullPath() + " FBX ���� �̴ϼȶ����� ����");
	}

	/* �⺻ ȯ�� �����Դϴ�. */
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

	/* �ִϸ��̼� üũ�� �մϴ�. �ε� �� ���� ������ �� �� �ֽ��ϴ�. �ִϸ��̼� ������ ������ �� �� �ֽ��ϴ�.  */
	AniCheck();
	// LejGameDebug::ConsolePrintText(L"�ִϸ��̼� üũ �Ϸ� �ִϸ��̼� ���� : %d", m_UserAniData.size());

	int MatCount = 0;
	/* �ﰢȭ�� ROOT�� �������� ���ؽ����� ������ �ϴ� ���Դϴ�. ���� ���ҵ� �̹������� ���� ���� ���������� ���̰� �մϴ�. */
	Trangulate(m_Scene->GetRootNode(), MatCount);
	// LejGameDebug::ConsolePrintText(L"�ﰢȭ �Ϸ�");
	m_UserMaterialData.resize(MatCount);
	MatCount = 0;
	MaterialCheck(m_Scene->GetRootNode(), MatCount);
	// LejGameDebug::ConsolePrintText(L"��Ƽ���� üũ �Ϸ� %d", MatCount);

	/* ��θ� Ŀ���� ��η� �ٲپ��ݴϴ�. */
	for (size_t i = 0; i < m_UserMaterialData.size(); i++)
	{
		Game_String DirPath = GameFile.DirPath();
		m_UserMaterialData[i].DifTexture.EraseStr(DirPath);
	}

	MeshVtxData(m_Scene->GetRootNode());

	/* ���� üũ�մϴ�. ���� ������ �� �� �ֽ��ϴ�. */
	BoneCheck();

	/* �޽� ������ �޶�� �ϴ� ���, ������� static �Ž��� �ش��Ѵٸ�, �������� �ٸ��� �޽��� �����ؾ� �մϴ�.
	m_UserAniData�� ����� �ʿ䰡 �����ϴ�. */
	if (0 >= m_UserAniData.size())
	{
		CreateMesh();
		// LejGameDebug::ConsolePrintText(L" �Ž� �Ϸ�");
		// LejGameDebug::ConsolePrintText(L" static �Ž� �Ϸ�");
		return;
	}

	size_t I = m_UserAniData.size();
	//LejGameDebug::ConsolePrintText(L"�� üũ �Ϸ� �� ���� : %d\n", m_UserBoneData.size());

/* �ִϸ��̼��� 1���Դϴ�. ���� 134���Դϴ�. */
	for (size_t AniIndex = 0; AniIndex < m_UserAniData.size(); AniIndex++)
	{
		m_UserAniData[AniIndex].m_AniFrameData.resize(m_UserBoneData.size());

		for (size_t BoneCount = 0; BoneCount < m_UserBoneData.size(); BoneCount++)
		{
			size_t I = m_UserBoneData.size();
			/* 1434������ ��ŭ Ȯ���մϴ�. */
			m_UserAniData[AniIndex].m_AniFrameData[BoneCount].m_Data.resize(m_UserAniData[AniIndex].FrameCount + 1);
		}
	}

	/* ��Ű���� ���ϴ� static mesh�� ����ġ�� �ε����� ���� ���� �ƴմϴ�. �����ϼ��� */
	m_MapWI.resize(m_FbxVtx.size());
	/* ROOT�� ������ �ϼ� �ֽ��ϴ�. �����մϴ�. */
	MeshCount = 0;

	/* ������ ���� �Լ� �Դϴ�. üũ�� ��ҵ��� �ܼ��� ���� �����Դϴ�. for���� ���� �ְ�, ��͵� �ֽ��ϴ�. ��x��������(134 * 1434)�� �ֽ��ϴ�. */
	/* ���� ��ü������ �մϴ�. �����忡�� �ε��� �մϴ�. */
	AnimationClipCheck(m_Scene->GetRootNode());
	MeshCount = 0;

	/* �ε����� ����ġ�� ����߽��ϴ�. �����ݴϴ�. */
	CalWeightAndIndex();
	m_MapWI.clear();

	/* �̸����� ������ �� �ְ� �غ��մϴ�. */
	for (size_t i = 0; i < m_UserAniData.size(); i++)
	{
		m_UserAniDataMap[m_UserAniData[i].AniName] = &m_UserAniData[i];
	}
	//LejGameDebug::ConsolePrintText(L" Ani �Ž� �Ϸ�");

/* ���������� �޽��� �����մϴ�. */
	CreateMesh();
}


void Game_StaticFBX::MeshVtxData(fbxsdk::FbxNode* _Node)
{
	fbxsdk::FbxNodeAttribute* _Att = _Node->GetNodeAttribute();

	/* ��͸� �մϴ�. ���ؽ��� �������󼭰� �ƴմϴ�. �����带 ������ �ִ��� �� �� ���� �����Դϴ�. */
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

	// ������ �˰� �ִ� LocalSpace�� �����ϴ� Postion������ ù��° ������
	// ��ġ������ ù��° �������̴�.
	FbxVector4* pVtxPos = _Mesh->GetControlPoints();

	VtxData.resize(iVtxCount);

	// �ƽ������� �ﰢȭ �߱� ������
	for (int i = 0; i < VtxData.size(); i++)
	{
		// �ణ �����ϱ� ����� 0 2 1�� �޾ƿ��°ǵ�
		// �ƽ� ������ �������� z�� y�� ǥ���ϱ� ���ؼ� ó��.
		// ����==���� ��ġ����
		VtxData[i].Pos.x = (float)pVtxPos[i].mData[0];
		VtxData[i].Pos.y = (float)pVtxPos[i].mData[2];
		VtxData[i].Pos.z = (float)pVtxPos[i].mData[1];
		VtxData[i].Pos.w = 1.0f;
		// HGAMEDEBUG3D::ConsoleText(L"%d = %f, %f, %f", i, VtxData[i].Pos.x, VtxData[i].Pos.y, VtxData[i].Pos.z);
	}


	UINT VtxId = 0;

	// �� �Ž��� �׸��µ� ����� ���� ����
	FbxGeometryElementMaterial* MatData = _Mesh->GetElementMaterial();
	// �׸��µ� ����� ��������
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




// ANIDATA �ϳ� 
// ���� 134�� 
// ���������� 1434�� (������ �����Ӿִϸ��̼��� �ǹ���)
// ������ ������ 1434���� 
// FPS = 30

// ���
// M1 M2 M3 M4 M5 M6 M7 M8 M9 M10
// �� �� �� �� �� �� �� �� �� �� 
// �̰��ϳ��ϳ��� �ִϸ��̼� �� �������̴�. 
// EX) �÷��̾� ���� ȸ���� �� 
// ��
// ��������
// resize



// ��� 
// ���
void Game_StaticFBX::AnimationClipCheck(fbxsdk::FbxNode* _Node)
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


		// 1434(��) x 133(��) �� ������ ä���ִ� �� 
		// �ð� ��� 0,1,2,3,4,5,6......
		// ����� ä���ְ� �ִ�. 

		CalAnimationClip_Static(_Mesh);
	}

	int ChildCount = _Node->GetChildCount();

	// ��� 
	for (int i = 0; i < ChildCount; i++)
	{
		AnimationClipCheck(_Node->GetChild(i));
	}
}





void Game_StaticFBX::CalAnimationClip_Static(fbxsdk::FbxMesh* _Mesh)
{
	// �̳༮�� ���ٰ� �ִϸ��̼��� ���°� �ƴϴ�.
	int iSkinCount = _Mesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
	// �̰� 0�̶�� �ִϸ��̼��� ���°� �ƴϴ�.

	if (iSkinCount <= 0)
	{
		return;
	}

	int VtxCount = (int)m_FbxVtx[MeshCount].size();

	// �ִϸ��̼��߿� 

	// �Ž��� �⺻ ��Ʈ������ �����´�.
	// �ҽ� �״�� �������� 
	fbxsdk::FbxNode* Node = _Mesh->GetNode();
	const FbxVector4 vS = Node->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot);
	const FbxVector4 vR = Node->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot);
	const FbxVector4 vT = Node->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot);

	// ȸ�� ��Ҵ� ���ʹϿ����� �������  
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

				LejFbxBoneData* pBone = FindBone(StrBoneName);

				if (nullptr == pBone)
				{
					continue;
				}

				// �ε����� ����ġ�� �����⸸ �Ѵ�. ������ ���� ���� �����̴�. 
				// �ִϸ��̼��� �־�� ����ġ�� �ε����� �ǹ̰� �ִ�. 
				// ��Ű����  �ִ�. ����.�� �Ǻ�����. 

				LoadWeightAndIndex(pCluster, pBone->iIndex, m_FbxVtx[MeshCount]);

				// �����ؼ� �����ؾ���
				// ��ǥ�踦 �ٲٴ� �Լ� 
				LoadOffsetMatrix(pCluster, matTrans, pBone);

				// ���������� ������ �ִ� �ð�-�Ӽ��� ä���ش�. 
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





void Game_StaticFBX::LoadNormal(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
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
				AMSG(L"if (nullptr == pMtrl) ��Ƽ���� ���� ����");
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




float Game_StaticFBX::MaterialFactor(FbxSurfaceMaterial* pMtrl, const char* _FactorName)
{
	double dFactor = 0;
	FbxProperty FactorPro = pMtrl->FindProperty(_FactorName);

	// ��ȿüũ �� ������ 3d�ƽ����� ���� ����ϴ� ���̾�?
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



/* ���� �ִ����� Ȯ���ϴ� �Լ� �Դϴ�. */
void Game_StaticFBX::BoneCheck()
{
	// ��ġ Ʈ�������� ����ϹǷ�
	// ��Ͱ� ���� �����ϴ�.
	int BoneCount = 0;

	// ���� ��͸� ���鼭 
	LoadBoneCount(m_Scene->GetRootNode(), BoneCount);

	if (0 < BoneCount)
	{
		// �������� �Ѵ�. 
		m_UserBoneData.resize(BoneCount);
	}

	// �̹� �� ������� �߱� ������.
	BoneCount = 0;


	LoadBone(m_Scene->GetRootNode(), BoneCount);
}




void Game_StaticFBX::LoadBoneCount(fbxsdk::FbxNode* _Node, int& _Count)
{
	fbxsdk::FbxNodeAttribute* _Att = _Node->GetNodeAttribute();

	// ���̷��� == �� 
	// ������ Ȯ���ϰ� 
	// ��͸� ����. 
	// ���� �θ�-�ڽ� ������� ���鼭 ��带 ����Ѵ�. 


	if (nullptr != _Att
		&& _Att->GetAttributeType()// Attribute : �Ӽ� 
		== fbxsdk::FbxNodeAttribute::eSkeleton)
	{
		++_Count;
		// m_UserBoneData.resize
	}

	int ChildCount = _Node->GetChildCount();

	// ��� ���� �θ�-�ڽİ������ Ȯ���ϸ鼭 
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
		// �ʿ��� �־��ִ� ������ ���߿� ���� �̸����� ���� ã�� ���ؼ� �̴�.
		m_BoneDataMap[m_UserBoneData[_iIndex].Name] = &m_UserBoneData[_iIndex];
		++_iIndex;
	}

	int ChildCount = _Node->GetChildCount();
	for (int i = 0; i < ChildCount; i++)
	{
		LoadBone(_Node->GetChild(i), _iIndex, _iDepth + 1, _iIndex);
	}

}



// STATIC �Ž��� ���� Ŀ������ �������� ���� ����. 
void Game_StaticFBX::CreateStaticMesh(Game_Ptr<Game_Renderer> _NewRenderer, const Game_String& _Fbx)
{

	Game_Ptr<Game_StaticFBX> TestFbx = Game_StaticFBX::Find(_Fbx);

	if (TestFbx->m_UserMaterialData.size() != TestFbx->m_Mesh.size())
	{
		AMSG(L"������ �ٸ��ϴ�.");
	}

	for (size_t i = 0; i < TestFbx->m_Mesh.size(); i++)
	{
		// ������ζ� m_UserMatData������ ������ ������̴�.
		// m_UserMaterialData == m_Mesh �������� �ε��Ҷ� ���� ���� �����

		// ��θ� �����Ѵ�. 
		HGAMEFILE File = TestFbx->GameFile.DirPath() + TestFbx->m_UserMaterialData[i].DifTexture;

		if (nullptr == HTEXTURE::Find(File.FileName()))
		{
			// ��θ� �����Ѵ�. 
			HTEXTURE::Load(TestFbx->GameFile.DirPath() + TestFbx->m_UserMaterialData[i].DifTexture);
		}

		// �����÷��̾�� �Ž��� ����� ���ε� 
		// ���⿡ ���� Ŀ������ ���÷��� �ؽ��ĸ� �����Ѵ�. 
		Game_Ptr<HRENDERPLAYER> Player = _NewRenderer->CreateRenderPlayer(TestFbx->m_Mesh[i], L"3DStaticTex");
		Player->TEXTURE(L"Tex", File.FileName());
		Player->SAMPLER(L"Smp", L"LWSMP");
	}
}






// AniCheck()�� �ϴ��� 
// m_UserAniData�ȿ� LejFbxAniData�� �󸶳� �׿����� �˰� �ʹ�.
// LejFbxAniData �ϳ��ϳ��� ����ü�� �Ӽ����� ä���ش�. 
// �ִϸ��̼ǵ����ʹ� �������� ������������ ������. 

void Game_StaticFBX::AniCheck()
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
	// �� 134�� 
	// �������� 1434�� 
	// �̷��� ���� ���µ� resize ���ϸ� ��� �Ұų� ??
	// �̸� �����Ѵ�. 



	m_UserAniData.resize(AniNameArray.Size());



	for (int i = 0; i < AniNameArray.GetCount(); i++)
	{
		// pAniStatic �ִϸ��̼� �ϳ��ϳ��� �������� ������ ��� �ִ�.
		FbxAnimStack* pAniStatic = m_Scene->FindMember<FbxAnimStack>(AniNameArray[i]->Buffer());

		if (nullptr == pAniStatic)
		{
			AMSG(L"if (nullptr == pAniStatic)");
			continue;
		}

		// std::wcout << m_UserAniData[i].AniName << std::endl;



		m_UserAniData[i].AniName = CA2W(pAniStatic->GetName(), CP_UTF8).m_psz;

		// HGAMEDEBUG3D::ConsoleText(L"�ִϸ��̼� : %s Ž��", pAniStatic->GetName());
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
		AMSG(_Name + L" �̸��� ���� �������� �ʽ��ϴ�.");
	}

	return m_BoneDataMap[_Name];
}




void Game_StaticFBX::LoadWeightAndIndex(fbxsdk::FbxCluster* _Cluster, int _BoneIndex, std::vector<VTX3DMESHIN>& _ArrVtx)
{
	// fbxsdk::FbxCluster Ŭ�����ʹ� ��Ű���� ������ ��� �ִ� ����ü�̴�. 

	int iIndexCount = _Cluster->GetControlPointIndicesCount();

	for (size_t i = 0; i < iIndexCount; i++)
	{
		//// _BoneIndex �ε���
		LejFbxIndexWeight IW;

		IW.Index = _BoneIndex;
		IW.Weight = _Cluster->GetControlPointWeights()[i];

		int iClusterIndex = _Cluster->GetControlPointIndices()[i];

		//if (m_MapWI[MeshCount].end() != m_MapWI[MeshCount].find(iClusterIndex))
		//{
		//	AMSG(L"�ε����� ����ġ�� ��Ĩ�ϴ�.");
		//}

		m_MapWI[MeshCount][iClusterIndex].push_back(IW);
		//m_MapWI[MeshCount][iClusterIndex].Index = _BoneIndex;
		//m_MapWI[MeshCount][iClusterIndex].Weight = _Cluster->GetControlPointWeights()[i];
	}

	//_Cluster->GetControlPointWeights();
	// int iIndex = _Cluster->GetControlPointWeights();

}








// ������ ������ �ִ� ����������� ���� ä���ش�. 

void Game_StaticFBX::LoadOffsetMatrix(fbxsdk::FbxCluster* _Cluster, const fbxsdk::FbxAMatrix& _BaseTrans, LejFbxBoneData* _Bone)
{
	// �ִϸ��̼��� ���� ���� �⺻���� �� ����� ������ִ� ���̴�. 


	// 3D�ƽ� ��ǥ��
	// OPENGL�� ��ǥ��
	// DX�� ��ǥ��
	// 3D�ƽ� ��ǥ�� -> DX�� ��ǥ�� �� �ٲٰ� �ʹ�. 

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

	// DX ��ǥ��� �ٲٷ��� �ϴ°��̴�.
	// ��ǥ��� DX��ǥ�� �������� 
	// Z��� Y���� �ݴ�� �Ǿ��ִ�. 
	// �׷��Ƿ� �����´�. 

	Offset = ReMat * Offset * ReMat;


	_Bone->Offset = FbxMatrixToLejMatrix(Offset); // 
	_Bone->Bone = FbxMatrixToLejMatrix(_BaseTrans);// BONE�� �׵���� ������� 

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

		m_UserAniData[i].m_AniFrameData[_Bone->iIndex].BoneIndex = _Bone->iIndex; // ������ ����ü 

		// 1�ʴ� 30�������̴ϱ�
		// 300 10�ʽð����� �ִϸ��̼�
		// 1434
		for (FbxLongLong j = Start; j <= End; j++)
		{
			FbxTime tTime = {};
			// Ÿ�Ӹ�带 ��������� �ð��� �˾Ƴ��� �ִ�.
			tTime.SetFrame(j, m_UserAniData[i].TimeMode);

			// �ð���� 0 1 2 3 4 5 6 7 8 9  ~~~~~~~ 1434   �ִϸ��̼�-��ŸŸ���� ���� 
			// �ִϸ��̼��� ����� .

			// EvaluateGlobalTransform   ��忡�� �۷ι� �� ���� ��ȯ ����� ���� �� �ִ�. 
			FbxAMatrix matOff = _Node->EvaluateGlobalTransform(tTime) * _BaseTrans;

			// EvaluateGlobalTransform   ��忡�� �۷ι� �� ���� ��ȯ ����� ���� �� �ִ�.
			FbxAMatrix matCur = matOff.Inverse() * _Cluster->GetLink()->EvaluateGlobalTransform(tTime);
			// FbxAMatrix matCur = matOff.Inverse() * _Cluster->GetLink()->EvaluateLocalTransform(tTime);


			matCur = ReMat * matCur * ReMat;

			// ��� ... �̰����� ���� ä���ش�. 
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
			AMSG(L"����ġ ��꿡 4�� �̻��� ���� ���Խ��ϴ�.");

			// 
			// ���� ���� ����ӵ� ���.
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



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ��ü �����̴�. 


////////////////////////////////////////////////////////////////////////////////////////////// �ѹ��� LOAD ������  


void Game_StaticFBX::LoadUserFormat(const Game_String& _Path)
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

	///////////////////////////////////////////////// ���׸��� 

	LoadFile >> Size;
	m_UserMaterialData.resize(Size);
	for (size_t i = 0; i < m_UserMaterialData.size(); i++)
	{
		LoadFile >> m_UserMaterialData[i].DifColor; //HVECTOR DifColor;
		LoadFile >> m_UserMaterialData[i].AmbColor; //HVECTOR AmbColor; // ��
		LoadFile >> m_UserMaterialData[i].SpcColor; //HVECTOR SpcColor; // ��
		LoadFile >> m_UserMaterialData[i].EmvColor; //HVECTOR EmvColor; // ��
		LoadFile >> m_UserMaterialData[i].SpecularPower;//float SpecularPower;		// ���� ����
		LoadFile >> m_UserMaterialData[i].TransparencyFactor;//float TransparencyFactor;	// ���� ����
		LoadFile >> m_UserMaterialData[i].Shininess;//float Shininess;			// ���� ����
		LoadFile >> m_UserMaterialData[i].DifTexture;//HGAMESTRING DifTexture;	// �ؽ�ó��� 
		LoadFile >> m_UserMaterialData[i].BmpTexture;//HGAMESTRING BmpTexture; // �ؽ�ó���
		LoadFile >> m_UserMaterialData[i].SpcTexture;//HGAMESTRING SpcTexture; // �ؽ�ó���
	}

	LoadFile >> Size;
	if (-1 == Size)
	{
		// �ִϸ��̼��� ���ٴ� �̾߱�.
		return;
	}

	////////////////////////////////////////////////////////////// ��->�ִϸ��̼� 
	// �ִϸ��̼��� ����� ���� �����Ӱ��� ���踦 �����ϴ� ��ŸŸ��, �ִϸ��̼� �Ӽ�, ���������� ���������� ������ �ִ�. 

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
		m_BoneDataMap[m_UserBoneData[i].Name] = &m_UserBoneData[i];
	}

	CreateMesh();
}









// �޽� �� ���׸��� �ִϸ��̼�

/// ��ü ������ : ���̺� �ѹ� �� ������ ���� ���� �ڵ带 �ε� �� �ʿ䰡 ����. 
/// ���� �����Ѵٴ� ���� �ʿ���� ���� �����ϴ� ���� �ƴ� �������� �ʼ����� �����͸� �����ϴ� �����̴�. 

// �ٵ� ��� �����ӵ��� �� �ϳ��ϳ� 134���� ������� ���� ���� ����, �ε� �ϰ� �����. �̷��� �ϸ� ������. ����ȭ 


////////////////////////////////////////////////////////////////////////////////////////////// �ѹ��� SAVE ������  
void Game_StaticFBX::SaveUserFormat(const Game_String& _Path)
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


	///////////////////////////////////////////////////////////// ���׸��� 

	// std::vector<HFBXMATDATA> m_UserMaterialData;
	SaveFile << (int)m_UserMaterialData.size();
	for (size_t i = 0; i < m_UserMaterialData.size(); i++)
	{
		// �̷��� �������� �����ϴ� ������ ������ �� ������ �ܼ��ϱ� �����̴�. 
		// �̰��� ����� ������ ������ ������ ���� ������. 
		SaveFile << m_UserMaterialData[i].DifColor; //HVECTOR DifColor;
		SaveFile << m_UserMaterialData[i].AmbColor; //HVECTOR AmbColor; // ��
		SaveFile << m_UserMaterialData[i].SpcColor; //HVECTOR SpcColor; // ��
		SaveFile << m_UserMaterialData[i].EmvColor; //HVECTOR EmvColor; // ��
		SaveFile << m_UserMaterialData[i].SpecularPower;//float SpecularPower;		// ���� ����
		SaveFile << m_UserMaterialData[i].TransparencyFactor;//float TransparencyFactor;	// ���� ����
		SaveFile << m_UserMaterialData[i].Shininess;//float Shininess;			// ���� ����
		SaveFile << m_UserMaterialData[i].DifTexture;//HGAMESTRING DifTexture;	// �ؽ�ó��� 
		SaveFile << m_UserMaterialData[i].BmpTexture;//HGAMESTRING BmpTexture; // �ؽ�ó���
		SaveFile << m_UserMaterialData[i].SpcTexture;//HGAMESTRING SpcTexture; // �ؽ�ó���
	}

	if (0 >= m_UserAniData.size())
	{
		// �ִϸ��̼��� ���ٴ� �̾߱���?
		SaveFile << (int)-1;
		return;
	}

	std::wcout << L"ANIDATA SAVE" << std::endl; // �ܼ� â�� �����̴�. 
	// std::vector<HFBXANIDATA> m_UserAniData; // �ִϸ��̼� 

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