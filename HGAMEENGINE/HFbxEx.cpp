#include "HFbxEx.h"
#include <HGAMEFILE.h>
#include "HVBBUFFER.h"
#include "HIBBUFFER.h"
// #include "JK3DEngineDebug.h"
#include "HMESH.h"
#include <algorithm>
#include "HTEXTURE.h"

map<Game_String, vector<HFbxExAniData>> Game_Fbx_Ex::LoadList;

DRAWSET::DRAWSET() :
	IsLod(false),
	IsLodLv(-1)
{
}
DRAWSET::~DRAWSET()
{
	for (size_t i = 0; i < m_FbxVtx.size(); i++)
	{
		delete m_FbxVtx[i];
	}

	for (size_t i = 0; i < m_FbxIdx.size(); i++)
	{
		delete m_FbxIdx[i];
	}
}

Game_Fbx_Ex::Game_Fbx_Ex() :
	m_pFbxManager(nullptr),
	m_pFbxScene(nullptr),
	m_pFbxRootNode(nullptr)
{
	m_AxisConversionMatrix.SetIdentity();
	m_JointPostConversionMatrix.SetIdentity();
}
Game_Fbx_Ex::~Game_Fbx_Ex()
{
	std::map<int, DRAWSET*>::iterator iter = m_mapDrawsets.begin();
	std::map<int, DRAWSET*>::iterator iterEnd = m_mapDrawsets.end();
	for (; iter != iterEnd; ++iter)
	{
		delete (*iter).second;
	}


	if (nullptr != m_pFbxScene) {
		m_pFbxScene->Destroy();
		m_pFbxScene = nullptr;
	}
	if (nullptr != m_pFbxManager) {
		m_pFbxManager->Destroy();
		m_pFbxManager = nullptr;
	}

}

void Game_Fbx_Ex::Load(float4 modifyRotationAxis)
{
	m_ModifyRotationAxis = { modifyRotationAxis.x, modifyRotationAxis.y, modifyRotationAxis.z, 1 };
	if (false == ImportScene())
	{
		return;
	}

	// 여기서 축변형을 하는데.
	// 문제가 생기는것.
	ConvertScene();

	AniCheck();

	ImportSceneInfo();
	ImportCluster();
	ImportControlPoints();
	ImportBone();
	LoadSkinAndCluster();
	DrawSetMeshesCreate();


	m_SceneInfo;
	m_mapDrawsets;
	m_UserAniData;
	m_vecRefBones;
}

bool Game_Fbx_Ex::ImportScene()
{
	m_pFbxManager = fbxsdk::FbxManager::Create();
	fbxsdk::FbxIOSettings* pIos = fbxsdk::FbxIOSettings::Create(m_pFbxManager, IOSROOT);
	fbxsdk::FbxImporter* pImporter = fbxsdk::FbxImporter::Create(m_pFbxManager, "");
	if (false == pImporter->Initialize(CW2A(GameFile.FullPath(), CP_UTF8).m_psz, -1, m_pFbxManager->GetIOSettings()))
	{
		//AMSG(L"FBX 파일 이니셜라이즈 실패");
		return false;
	}

	m_pFbxScene = fbxsdk::FbxScene::Create(m_pFbxManager, "");

	pImporter->Import(m_pFbxScene);

	pImporter->Destroy();

	return true;
}

void Game_Fbx_Ex::ConvertScene()
{
	fbxsdk::FbxAxisSystem::ECoordSystem CoordSystem = fbxsdk::FbxAxisSystem::ECoordSystem::eRightHanded;
	fbxsdk::FbxAxisSystem::EUpVector UpVector = fbxsdk::FbxAxisSystem::eYAxis;
	fbxsdk::FbxAxisSystem::EFrontVector FrontVector = (fbxsdk::FbxAxisSystem::EFrontVector)-fbxsdk::FbxAxisSystem::eParityOdd;

	fbxsdk::FbxAxisSystem JKEngineAxisSystem(UpVector, FrontVector, CoordSystem);

	fbxsdk::FbxAxisSystem SourceAxisSystem = m_pFbxScene->GetGlobalSettings().GetAxisSystem();

	if (SourceAxisSystem != JKEngineAxisSystem)
	{
		fbxsdk::FbxRootNodeUtility::RemoveAllFbxRoots(m_pFbxScene);
		JKEngineAxisSystem.ConvertScene(m_pFbxScene);

		fbxsdk::FbxAMatrix SourceMatrix;
		SourceAxisSystem.GetMatrix(SourceMatrix);

		fbxsdk::FbxAMatrix JKEngineMatrix;
		JKEngineAxisSystem.GetMatrix(JKEngineMatrix);

		m_AxisConversionMatrix = SourceMatrix.Inverse() * JKEngineMatrix;
	}

	int originUpSign = 0;
	int originFrontSign = 0;
	int engineUpSign = 0;
	int engineFrontSign = 0;

	fbxsdk::FbxAxisSystem::EFrontVector originFrontVector = SourceAxisSystem.GetFrontVector(originFrontSign);
	fbxsdk::FbxAxisSystem::EFrontVector engineFrontVector = JKEngineAxisSystem.GetFrontVector(engineFrontSign);
	fbxsdk::FbxAxisSystem::EUpVector originUpvector = SourceAxisSystem.GetUpVector(originUpSign);
	fbxsdk::FbxAxisSystem::EUpVector engineUpVector = JKEngineAxisSystem.GetUpVector(engineUpSign);
	fbxsdk::FbxAxisSystem::ECoordSystem originCoordSystem = SourceAxisSystem.GetCoorSystem();
	fbxsdk::FbxAxisSystem::ECoordSystem engineCoordSystem = JKEngineAxisSystem.GetCoorSystem();
	if (originUpvector == engineUpVector && originUpSign == engineUpSign)
	{
		if (originFrontSign != engineFrontSign && originCoordSystem == engineCoordSystem)
		{
			m_ModifyRotationAxis.mData[static_cast<int>(originUpvector) - 1] += 180;
		}
	}
	else if (originUpvector == engineUpVector && originUpSign != engineUpSign)
	{
		if (originUpvector == fbxsdk::FbxAxisSystem::EUpVector::eXAxis)
		{
			m_ModifyRotationAxis.mData[1] += 180;
		}
		else
		{
			m_ModifyRotationAxis.mData[0] += 180;
		}

		if (originFrontSign != engineFrontSign && originCoordSystem == engineCoordSystem)
		{
			m_ModifyRotationAxis.mData[static_cast<int>(originUpvector) - 1] += 180;
		}
	}
	else if (originUpvector == fbxsdk::FbxAxisSystem::EUpVector::eXAxis)
	{
		//origin up vector 가 x up vector 일때를 아직 만나보지를 못했다.

		m_ModifyRotationAxis.mData[1] += originUpSign * 90;
	}
	else
	{
		m_ModifyRotationAxis.mData[0] += originUpSign * 90;
		if (originUpSign != engineFrontSign)
		{
			m_ModifyRotationAxis.mData[static_cast<int>(originUpvector) - 1] += originUpSign * 180;
		}
	}


	m_JointPostConversionMatrix.SetR(m_ModifyRotationAxis);

	if (true == m_JointPostConversionMatrix.IsIdentity())
	{
		m_JointPostConversionMatrix = m_AxisConversionMatrix;
	}

	m_pFbxScene->GetAnimationEvaluator()->Reset();

	m_pFbxRootNode = m_pFbxScene->GetRootNode();
}

void Game_Fbx_Ex::AniCheck()
{
	fbxsdk::FbxArray<FbxString*> AniNameArray;
	m_pFbxScene->FillAnimStackNameArray(AniNameArray);

	if (0 == AniNameArray.Size())
	{
		return;
	}

	m_UserAniData.resize(AniNameArray.Size());

	for (int i = 0; i < AniNameArray.GetCount(); i++)
	{
		fbxsdk::FbxAnimStack* pAniStatic = m_pFbxScene->FindMember<fbxsdk::FbxAnimStack>(AniNameArray[i]->Buffer());

		if (nullptr == pAniStatic)
		{
			AMSG(L"if (nullptr == pAniStatic)");
			continue;
		}

		m_UserAniData[i].AniName = CA2W(pAniStatic->GetName(), CP_UTF8).m_psz;

		FbxTakeInfo* TakeInfo = m_pFbxScene->GetTakeInfo(pAniStatic->GetName());

		m_UserAniData[i].StartTime = TakeInfo->mLocalTimeSpan.GetStart();
		m_UserAniData[i].EndTime = TakeInfo->mLocalTimeSpan.GetStop();

		m_UserAniData[i].TimeMode = m_pFbxScene->GetGlobalSettings().GetTimeMode();
		m_UserAniData[i].TimeStartCount = m_UserAniData[i].StartTime.GetFrameCount(m_UserAniData[i].TimeMode);

		if (0 >= m_UserAniData[i].TimeStartCount)
		{
			m_UserAniData[i].TimeStartCount *= (FbxLongLong)-1;
		}

		m_UserAniData[i].TimeEndCount = m_UserAniData[i].EndTime.GetFrameCount(m_UserAniData[i].TimeMode);
		m_UserAniData[i].FrameCount = m_UserAniData[i].TimeEndCount - m_UserAniData[i].TimeStartCount;

		m_UserAniData[i].FbxModeCount = (long long)fbxsdk::FbxTime::GetFrameRate(m_UserAniData[i].TimeMode);
		m_UserAniData[i].FbxModeRate = (double)fbxsdk::FbxTime::GetFrameRate(m_UserAniData[i].TimeMode);

		double Test = fbxsdk::FbxTime::GetFrameRate(m_UserAniData[i].TimeMode);

	}

	for (int i = 0; i < AniNameArray.GetCount(); i++)
	{
		delete AniNameArray[i];
	}

}

void Game_Fbx_Ex::ImportSceneInfo()
{
	fbxsdk::FbxGeometryConverter Con(m_pFbxManager);
	Con.Triangulate(m_pFbxScene, true);

	fbxsdk::FbxTimeSpan GlobalTimeSpan(fbxsdk::FBXSDK_TIME_INFINITE, fbxsdk::FBXSDK_TIME_MINUS_INFINITE);

	m_SceneInfo.TotalMaterialNum = m_pFbxScene->GetMaterialCount();
	m_SceneInfo.TotalTextureNum = m_pFbxScene->GetTextureCount();
	int geometryCount = m_pFbxScene->GetGeometryCount();
	for (int geometryIndex = 0; geometryIndex < geometryCount; ++geometryIndex)
	{
		fbxsdk::FbxGeometry* geoMetry = m_pFbxScene->GetGeometry(geometryIndex);
		if (geoMetry->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh)
		{
			fbxsdk::FbxNode* geoMetryNode = geoMetry->GetNode();
			fbxsdk::FbxMesh* mesh = reinterpret_cast<fbxsdk::FbxMesh*>(geoMetry);
			if (nullptr != geoMetryNode &&
				0 >= mesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin))
			{
				fbxsdk::FbxNode* ParentNode = RecursiveFindParentLodGroup(geoMetryNode->GetParent());
				if (nullptr != ParentNode &&
					nullptr != ParentNode->GetNodeAttribute() &&
					fbxsdk::FbxNodeAttribute::eLODGroup == ParentNode->GetNodeAttribute()->GetAttributeType())
				{
					bool isLodRoot = false;
					for (int ChildIndex = 0; ChildIndex < ParentNode->GetChildCount(); ++ChildIndex)
					{
						fbxsdk::FbxNode* MeshNode = FindLODGroupNode(ParentNode, ChildIndex);
						if (geoMetryNode == MeshNode)
						{
							isLodRoot = true;
							break;
						}
					}
					if (false == isLodRoot)
					{
						continue;
					}
				}
			}

			++m_SceneInfo.TotalGeometryNum;

			m_SceneInfo.MeshInfo.push_back(HFbxExMeshInfo());
			HFbxExMeshInfo& meshInfo = m_SceneInfo.MeshInfo.at(m_SceneInfo.MeshInfo.size() - 1);
			if (geoMetry->GetName()[0] != '\0')
			{
				meshInfo.Name = geoMetry->GetName();
			}
			else
			{
				meshInfo.Name = nullptr != geoMetryNode ? geoMetryNode->GetName() : "None";
			}

			meshInfo.pMesh = mesh;
			meshInfo.bTriangulated = mesh->IsTriangleMesh();
			meshInfo.MaterialNum = geoMetryNode ? geoMetryNode->GetMaterialCount() : 0;
			meshInfo.FaceNum = mesh->GetPolygonCount();
			meshInfo.VertexNum = mesh->GetControlPointsCount();

			meshInfo.LODGroup = "";
			if (nullptr != geoMetryNode)
			{
				fbxsdk::FbxNode* ParentNode = RecursiveFindParentLodGroup(geoMetryNode->GetParent());
				if (ParentNode != nullptr && ParentNode->GetNodeAttribute() && ParentNode->GetNodeAttribute()->GetAttributeType() == fbxsdk::FbxNodeAttribute::eLODGroup)
				{
					meshInfo.LODGroup = ParentNode->GetName();
					meshInfo.bIsLodGroup = true;
					for (int LODIndex = 0; LODIndex < ParentNode->GetChildCount(); LODIndex++)
					{
						fbxsdk::FbxNode* MeshNode = FindLODGroupNode(ParentNode, LODIndex, geoMetryNode);
						if (geoMetryNode == MeshNode)
						{
							meshInfo.LODLevel = LODIndex;
							break;
						}
					}
				}
			}

			if (mesh->GetDeformerCount(FbxDeformer::eSkin) > 0)
			{
				m_SceneInfo.SkinnedMeshNum++;
				meshInfo.bIsSkelMesh = true;
				meshInfo.MorphNum = mesh->GetShapeCount();
				fbxsdk::FbxSkin* Skin = (fbxsdk::FbxSkin*)mesh->GetDeformer(0, FbxDeformer::eSkin);
				int ClusterCount = Skin->GetClusterCount();
				fbxsdk::FbxNode* Link = NULL;
				for (int ClusterId = 0; ClusterId < ClusterCount; ++ClusterId)
				{
					fbxsdk::FbxCluster* Cluster = Skin->GetCluster(ClusterId);
					Link = Cluster->GetLink();
					while (Link && Link->GetParent() && Link->GetParent()->GetSkeleton())
					{
						Link = Link->GetParent();
					}

					if (Link != NULL)
					{
						break;
					}
				}

				meshInfo.SkeletonRoot = nullptr != Link ? Link->GetName() : ("None");
				meshInfo.SkeletonElemNum = nullptr != Link ? Link->GetChildCount(true) : 0;

				if (nullptr != Link)
				{
					fbxsdk::FbxTimeSpan AnimTimeSpan(fbxsdk::FBXSDK_TIME_INFINITE, fbxsdk::FBXSDK_TIME_MINUS_INFINITE);
					Link->GetAnimationInterval(AnimTimeSpan);
					GlobalTimeSpan.UnionAssignment(AnimTimeSpan);
				}
			}
			else
			{
				m_SceneInfo.NonSkinnedMeshNum++;
				meshInfo.bIsSkelMesh = false;
				meshInfo.SkeletonRoot = "";
			}
			meshInfo.UniqueId = mesh->GetUniqueID();
		}
	}

	m_SceneInfo.bHasAnimation = false;
	int AnimCurveNodeCount = m_pFbxScene->GetSrcObjectCount<fbxsdk::FbxAnimCurveNode>();

	for (int AnimCurveNodeIndex = 2; AnimCurveNodeIndex < AnimCurveNodeCount; AnimCurveNodeIndex++)
	{
		fbxsdk::FbxAnimCurveNode* CurAnimCruveNode = m_pFbxScene->GetSrcObject<fbxsdk::FbxAnimCurveNode>(AnimCurveNodeIndex);
		if (CurAnimCruveNode->IsAnimated(true))
		{
			m_SceneInfo.bHasAnimation = true;
			break;
		}
	}

	m_SceneInfo.FrameRate = fbxsdk::FbxTime::GetFrameRate(m_pFbxScene->GetGlobalSettings().GetTimeMode());

	int dir = GlobalTimeSpan.GetDirection();
	if (GlobalTimeSpan.GetDirection() == FBXSDK_TIME_FORWARD)
	{
		m_SceneInfo.TotalTime = (GlobalTimeSpan.GetDuration().GetMilliSeconds()) / 1000.f * m_SceneInfo.FrameRate;
	}
	else
	{
		m_SceneInfo.TotalTime = 0;
	}

	fbxsdk::FbxNode* RootNode = m_pFbxScene->GetRootNode();
	HFbxExNodeInfo RootInfo;
	RootInfo.ObjectName = RootNode->GetName();
	RootInfo.UniqueId = RootNode->GetUniqueID();
	RootInfo.Transform = RootNode->EvaluateGlobalTransform();

	RootInfo.AttributeName = NULL;
	RootInfo.AttributeUniqueId = 0;
	RootInfo.AttributeType = NULL;

	RootInfo.ParentName = NULL;
	RootInfo.ParentUniqueId = 0;

	m_SceneInfo.HierarchyInfo.push_back(RootInfo);

	TraverseHierarchyNodeRecursively(m_SceneInfo, RootNode, RootInfo);

	if (m_SceneInfo.SkinnedMeshNum > 0)
	{
		m_ImportType = FbxImportType::Deformers;
	}
	else if (m_SceneInfo.TotalGeometryNum > 0)
	{
		m_ImportType = FbxImportType::Geometry;
	}

	if (true == m_SceneInfo.bHasAnimation)
	{
		if (FbxImportType::Failed == m_ImportType)
		{
			m_ImportType = FbxImportType::Otherwise;
		}
		else if (FbxImportType::Geometry == m_ImportType)
		{
			m_ImportType = FbxImportType::Deformers;
		}
	}
}

void Game_Fbx_Ex::ImportCluster()
{
	size_t meshCount = m_SceneInfo.MeshInfo.size();
	if (0 == meshCount)
	{
		return;
	}

	for (size_t n = 0; n < meshCount; ++n)
	{
		HFbxExMeshInfo& meshInfo = m_SceneInfo.MeshInfo.at(n);
		if (m_mapClusterData.end() == m_mapClusterData.find(meshInfo.LODLevel))
		{
			m_mapClusterData.insert(std::make_pair(meshInfo.LODLevel, std::vector<ClusterData>()));
		}

		std::vector<ClusterData>& vecClusterData = m_mapClusterData[meshInfo.LODLevel];

		fbxsdk::FbxNode* pNode = meshInfo.pMesh->GetNode();
		fbxsdk::FbxMesh* FbxMesh = meshInfo.pMesh;
		const int SkinDeformerCount = FbxMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
		for (int DeformerIndex = 0; DeformerIndex < SkinDeformerCount; DeformerIndex++)
		{
			fbxsdk::FbxSkin* Skin = (fbxsdk::FbxSkin*)FbxMesh->GetDeformer(DeformerIndex, fbxsdk::FbxDeformer::eSkin);
			for (int ClusterIndex = 0; ClusterIndex < Skin->GetClusterCount(); ClusterIndex++)
			{
				vecClusterData.push_back(ClusterData());
				ClusterData& clusterData = vecClusterData[vecClusterData.size() - 1];
				clusterData.m_Cluster = Skin->GetCluster(ClusterIndex);
				clusterData.m_Mesh = FbxMesh;
				clusterData.LinkName = clusterData.m_Cluster->GetLink()->GetName();
			}
		}
	}

}

void Game_Fbx_Ex::ImportControlPoints()
{
	size_t meshInfoSize = m_SceneInfo.MeshInfo.size();
	for (size_t meshInfoIndex = 0; meshInfoIndex < meshInfoSize; ++meshInfoIndex)
	{
		HFbxExMeshInfo& meshInfo = m_SceneInfo.MeshInfo.at(meshInfoIndex);
		fbxsdk::FbxNode* pMeshNode = meshInfo.pMesh->GetNode();
		fbxsdk::FbxMesh* pMesh = meshInfo.pMesh;

		if (m_mapDrawsets.end() == m_mapDrawsets.find(meshInfo.LODLevel))
		{
			m_mapDrawsets.insert(std::make_pair(meshInfo.LODLevel, new DRAWSET()));
		}

		DRAWSET* pDrawSet = m_mapDrawsets[meshInfo.LODLevel];

		if (pDrawSet->m_MapWI.end() == pDrawSet->m_MapWI.find(pMesh))
		{
			pDrawSet->m_MapWI.insert(std::make_pair(pMesh, std::map<int, std::vector<HFbxExIW>>()));
		}

		pDrawSet->IsLod = meshInfo.bIsLodGroup;
		pDrawSet->IsLodLv = meshInfo.LODLevel;
		pDrawSet->m_FbxVtx.push_back(new std::vector<VTX3DMESHIN>());
		pDrawSet->m_FbxIdx.push_back(new std::vector<std::vector<UINT>>());
		std::vector<VTX3DMESHIN>& VtxData = *(pDrawSet->m_FbxVtx[pDrawSet->m_FbxVtx.size() - 1]);
		std::vector<std::vector<UINT>>& IdxData = *(pDrawSet->m_FbxIdx[pDrawSet->m_FbxVtx.size() - 1]);

		int controlPointsCount = pMesh->GetControlPointsCount();
		fbxsdk::FbxVector4* pControlPoints = pMesh->GetControlPoints();

		VtxData.resize(controlPointsCount);

		fbxsdk::FbxAMatrix meshMatrix;
		meshMatrix = m_AxisConversionMatrix;

		meshMatrix = ComputeTotalMatrix(pMeshNode);

		if (false == meshInfo.bIsSkelMesh)
		{
			meshMatrix = m_JointPostConversionMatrix * meshMatrix;
		}

		bool isOddNegativeScale = IsOddNegativeScale(meshMatrix);

		for (int controlPointIndex = 0; controlPointIndex < controlPointsCount; ++controlPointIndex)
		{
			fbxsdk::FbxVector4 controlPoint = pControlPoints[controlPointIndex];
			// VtxData[controlPointIndex].Pos *= 2.0F;
			fbxsdk::FbxVector4 calculateControlPoint = meshMatrix.MultT(controlPoint);
			VtxData[controlPointIndex].Pos = FbxVecToTransform(calculateControlPoint);
			// 
		}

		DrawSetMaterialSetting(pMeshNode, pDrawSet);

		fbxsdk::FbxStringList UVSetNameList;
		pMesh->GetUVSetNames(UVSetNameList);
		int uvSetCount = UVSetNameList.GetCount();
		for (int uvSetIndex = 0; uvSetIndex < uvSetCount; ++uvSetIndex)
		{
			const char* uvSetName = UVSetNameList.GetStringAt(uvSetIndex);
			const fbxsdk::FbxGeometryElementUV* pUVElement = pMesh->GetElementUV(uvSetName);
			if (nullptr == pUVElement)
			{
				continue;
			}

			fbxsdk::FbxGeometryElement::EMappingMode eMappingMode = pUVElement->GetMappingMode();
			fbxsdk::FbxGeometryElement::EReferenceMode eReferenceMode = pUVElement->GetReferenceMode();
			bool useIndex = fbxsdk::FbxGeometryElement::EReferenceMode::eDirect != eReferenceMode;
			int IndexCount = true == useIndex ? pUVElement->GetIndexArray().GetCount() : 0;

			int nPolygonCount = pMesh->GetPolygonCount();
			for (int PolygonIndex = 0; PolygonIndex < nPolygonCount; ++PolygonIndex)
			{
				const int PolygonSize = pMesh->GetPolygonSize(PolygonIndex);
				if (3 != PolygonSize)
				{
					AMSG(L"삼각형이 아닌 면이 발견됬습니다.");
				}

				for (int PositionInPolygon = 0; PositionInPolygon < PolygonSize; ++PositionInPolygon)
				{
					int convertUvIndex = isOddNegativeScale ? 2 - PositionInPolygon : PositionInPolygon;
					int ControlPointIndex = pMesh->GetPolygonVertex(PolygonIndex, PositionInPolygon);
					int UVMapIndex = (fbxsdk::FbxGeometryElement::EMappingMode::eByControlPoint == eMappingMode) ?
						ControlPointIndex : PolygonIndex * 3 + PositionInPolygon;

					int UvIndex = useIndex ? pUVElement->GetIndexArray().GetAt(UVMapIndex) : UVMapIndex;
					fbxsdk::FbxVector2 uvValue = pUVElement->GetDirectArray().GetAt(UvIndex);

					VtxData[ControlPointIndex].Uv.x = static_cast<float>(uvValue.mData[0]);
					VtxData[ControlPointIndex].Uv.y = 1.f - static_cast<float>(uvValue.mData[1]);
				}
			}
		}

		pMesh->GetElementMaterialCount();
		fbxsdk::FbxGeometryElementMaterial* pGeometryElementMaterial = pMesh->GetElementMaterial();
		fbxsdk::FbxGeometryElementNormal* pGeometryElementNormal = pMesh->GetElementNormal();

		int materialCount = pMeshNode->GetMaterialCount();
		IdxData.resize(materialCount);

		UINT VtxId = 0;

		int nPolygonCount = pMesh->GetPolygonCount();
		for (int PolygonIndex = 0; PolygonIndex < nPolygonCount; ++PolygonIndex)
		{
			int PolygonSize = pMesh->GetPolygonSize(PolygonIndex);
			if (3 != PolygonSize)
			{
				AMSG(L"삼각형이 아닌 면이 발견됬습니다.");
			}

			int IndexArray[3] = { 0, };
			for (int PositionInPolygon = 0; PositionInPolygon < PolygonSize; ++PositionInPolygon)
			{
				int ControlPointIndex = pMesh->GetPolygonVertex(PolygonIndex, PositionInPolygon);
				IndexArray[PositionInPolygon] = ControlPointIndex;

				LoadNormal(pMesh, meshMatrix, VtxData, VtxId, ControlPointIndex);
				LoadTangent(pMesh, meshMatrix, VtxData, VtxId, ControlPointIndex);
				LoadBinormal(pMesh, meshMatrix, VtxData, VtxId, ControlPointIndex);

				++VtxId;
			}

			int materialId = pGeometryElementMaterial->GetIndexArray().GetAt(PolygonIndex);
			IdxData[materialId].push_back(IndexArray[0]);
			IdxData[materialId].push_back(IndexArray[2]);
			IdxData[materialId].push_back(IndexArray[1]);
		}

		pDrawSet->m_FbxVtxMap.insert(std::make_pair(pMesh, &VtxData));
	}


}

void Game_Fbx_Ex::ImportBone()
{
	size_t meshCount = m_SceneInfo.MeshInfo.size();
	if (0 == meshCount)
	{
		return;
	}

	std::vector<fbxsdk::FbxNode*> NodeArray;
	std::vector<fbxsdk::FbxNode*> SortedLinks;
	std::vector<fbxsdk::FbxCluster*> ClusterArray;
	fbxsdk::FbxNode* Link = nullptr;
	int SkelType = 0;
	fbxsdk::FbxNode* SkeletalMeshNode = nullptr;

	for (size_t n = 0; n < meshCount; ++n)
	{
		HFbxExMeshInfo& meshInfo = m_SceneInfo.MeshInfo.at(n);
		if (0 != meshInfo.LODLevel)
		{
			continue;
		}

		fbxsdk::FbxNode* pNode = meshInfo.pMesh->GetNode();
		fbxsdk::FbxMesh* FbxMesh = meshInfo.pMesh;

		NodeArray.push_back(pNode);
		const int SkinDeformerCount = FbxMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
		for (int DeformerIndex = 0; DeformerIndex < SkinDeformerCount; DeformerIndex++)
		{
			fbxsdk::FbxSkin* Skin = (fbxsdk::FbxSkin*)FbxMesh->GetDeformer(DeformerIndex, fbxsdk::FbxDeformer::eSkin);
			for (int ClusterIndex = 0; ClusterIndex < Skin->GetClusterCount(); ClusterIndex++)
			{
				ClusterArray.push_back(Skin->GetCluster(ClusterIndex));
			}
		}
	}


	if (0 == ClusterArray.size())
	{
		return;
	}
	SkeletalMeshNode = NodeArray[0];

	fbxsdk::PoseList PoseArray;

	if (RetrievePoseFromBindPose(m_pFbxScene, NodeArray, PoseArray) == false)
	{
		const int PoseCount = m_pFbxScene->GetPoseCount();
		for (int PoseIndex = PoseCount - 1; PoseIndex >= 0; --PoseIndex)
		{
			fbxsdk::FbxPose* CurrentPose = m_pFbxScene->GetPose(PoseIndex);

			if (CurrentPose && CurrentPose->IsBindPose())
			{
				m_pFbxScene->RemovePose(PoseIndex);
				CurrentPose->Destroy();
			}
		}

		m_pFbxManager->CreateMissingBindPoses(m_pFbxScene);
		if (RetrievePoseFromBindPose(m_pFbxScene, NodeArray, PoseArray) == false)
		{
			AMSG(L"Recreating bind pose failed.");
		}
		else
		{
		}
	}

	BuildSkeletonSystem(m_pFbxScene, ClusterArray, SortedLinks);

	if (SortedLinks.size() == 0)
	{
		AMSG(L"연결된 링크가 없다.");
	}

	size_t LinkIndex;

	for (LinkIndex = 0; LinkIndex < SortedLinks.size(); ++LinkIndex)
	{
		Link = SortedLinks[LinkIndex];

		for (size_t AltLinkIndex = LinkIndex + 1; AltLinkIndex < SortedLinks.size(); ++AltLinkIndex)
		{
			fbxsdk::FbxNode* AltLink = SortedLinks[AltLinkIndex];

			Game_String tempLinkName = Link->GetName();
			Game_String tempAltLinkName = AltLink->GetName();
			if (tempLinkName == tempAltLinkName)
			{
				AMSG(L"같은 링크");
			}
		}
	}

	fbxsdk::FbxArray<fbxsdk::FbxAMatrix> GlobalsPerLink;
	GlobalsPerLink.Grow(static_cast<int>(SortedLinks.size()));
	GlobalsPerLink[0] = m_AxisConversionMatrix;

	bool GlobalLinkFoundFlag;
	fbxsdk::FbxVector4 LocalLinkT;
	fbxsdk::FbxQuaternion LocalLinkQ;
	fbxsdk::FbxVector4	LocalLinkS;
	fbxsdk::FbxVector4 GlobalLinkT;
	fbxsdk::FbxQuaternion GlobalLinkQ;
	fbxsdk::FbxVector4	GlobalLinkS;

	bool bAnyLinksNotInBindPose = false;
	Game_String LinksWithoutBindPoses;
	int NumberOfRoot = 0;

	int RootIdx = -1;

	for (LinkIndex = 0; LinkIndex < SortedLinks.size(); LinkIndex++)
	{
		m_vecRefBones.push_back(Bone());
		Bone& tempBoneData = m_vecRefBones.at(m_vecRefBones.size() - 1);
		tempBoneData.Index = static_cast<int>(m_vecRefBones.size() - 1);

		Link = SortedLinks[LinkIndex];

		int ParentIndex = -1;
		fbxsdk::FbxNode* LinkParent = Link->GetParent();
		if (LinkIndex)
		{
			for (int ll = 0; ll < LinkIndex; ++ll)
			{
				fbxsdk::FbxNode* Otherlink = SortedLinks[ll];
				if (Otherlink == LinkParent)
				{
					ParentIndex = ll;
					break;
				}
			}
		}

		if (ParentIndex == -1)
		{
			++NumberOfRoot;
			RootIdx = static_cast<int>(LinkIndex);
			//	unreal 에서는 루트를 하나만 허용하지만 
			//	유니티에서는 여러개를 허용해서 밑의 코드를 제거함
			/*if (NumberOfRoot > 1)
			{
				AMSG(L"여러개의 루트");
			}*/
		}

		GlobalLinkFoundFlag = false;
		if (!SkelType)
		{
			if (PoseArray.GetCount())
			{
				for (int PoseIndex = 0; PoseIndex < PoseArray.GetCount(); PoseIndex++)
				{
					int PoseLinkIndex = PoseArray[PoseIndex]->Find(Link);
					if (PoseLinkIndex >= 0)
					{
						fbxsdk::FbxMatrix NoneAffineMatrix = PoseArray[PoseIndex]->GetMatrix(PoseLinkIndex);
						fbxsdk::FbxAMatrix Matrix = *(fbxsdk::FbxAMatrix*)(double*)&NoneAffineMatrix;
						GlobalsPerLink[static_cast<int>(LinkIndex)] = Matrix;
						GlobalLinkFoundFlag = true;
						break;
					}
				}
			}

			if (!GlobalLinkFoundFlag)
			{
				for (int ClusterIndex = 0; ClusterIndex < ClusterArray.size(); ClusterIndex++)
				{
					fbxsdk::FbxCluster* Cluster = ClusterArray[ClusterIndex];
					if (Link == Cluster->GetLink())
					{
						Cluster->GetTransformLinkMatrix(GlobalsPerLink[static_cast<int>(LinkIndex)]);
						GlobalLinkFoundFlag = true;
						break;
					}
				}
			}
		}

		if (!GlobalLinkFoundFlag)
		{
			GlobalsPerLink[static_cast<int>(LinkIndex)] = Link->EvaluateGlobalTransform();
		}

		GlobalsPerLink[static_cast<int>(LinkIndex)] = GlobalsPerLink[static_cast<int>(LinkIndex)];
		if (LinkIndex &&
			-1 != ParentIndex)
		{
			fbxsdk::FbxAMatrix	Matrix;
			Matrix = GlobalsPerLink[static_cast<int>(ParentIndex)].Inverse() * GlobalsPerLink[static_cast<int>(LinkIndex)];
			LocalLinkT = Matrix.GetT();
			LocalLinkQ = Matrix.GetQ();
			LocalLinkS = Matrix.GetS();
			GlobalLinkT = GlobalsPerLink[static_cast<int>(LinkIndex)].GetT();
			GlobalLinkQ = GlobalsPerLink[static_cast<int>(LinkIndex)].GetQ();
			GlobalLinkS = GlobalsPerLink[static_cast<int>(LinkIndex)].GetS();
		}
		else
		{
			GlobalLinkT = LocalLinkT = GlobalsPerLink[static_cast<int>(LinkIndex)].GetT();
			GlobalLinkQ = LocalLinkQ = GlobalsPerLink[static_cast<int>(LinkIndex)].GetQ();
			GlobalLinkS = LocalLinkS = GlobalsPerLink[static_cast<int>(LinkIndex)].GetS();
		}

		Bone& Bone = m_vecRefBones[static_cast<int>(LinkIndex)];

		Bone.Name = Link->GetName();

		JointPos& JointMatrix = Bone.BonePos;
		fbxsdk::FbxSkeleton* Skeleton = Link->GetSkeleton();
		if (Skeleton)
		{
			JointMatrix.Length = (float)Skeleton->LimbLength.Get();
			JointMatrix.XSize = (float)Skeleton->Size.Get();
			JointMatrix.YSize = (float)Skeleton->Size.Get();
			JointMatrix.ZSize = (float)Skeleton->Size.Get();
		}
		else
		{
			JointMatrix.Length = 1.;
			JointMatrix.XSize = 100.;
			JointMatrix.YSize = 100.;
			JointMatrix.ZSize = 100.;
		}

		Bone.ParentIndex = ParentIndex;
		Bone.NumChildren = 0;
		for (int ChildIndex = 0; ChildIndex < Link->GetChildCount(); ChildIndex++)
		{
			fbxsdk::FbxNode* Child = Link->GetChild(ChildIndex);
			if (IsBone(Child))
			{
				Bone.NumChildren++;
			}
		}

		JointMatrix.SetTranslation(LocalLinkT);
		JointMatrix.SetRotation(LocalLinkQ);
		JointMatrix.SetScale(LocalLinkS);
		JointMatrix.SetGlobalTranslation(GlobalLinkT);
		JointMatrix.SetGlobalRotation(GlobalLinkQ);
		JointMatrix.SetGlobalScale(GlobalLinkS);
		JointMatrix.BuildMatrix();
	}
}

void Game_Fbx_Ex::LoadSkinAndCluster()
{
	std::map<int, DRAWSET*>::iterator iter = m_mapDrawsets.begin();
	std::map<int, DRAWSET*>::iterator iterEnd = m_mapDrawsets.end();
	for (; iter != iterEnd; ++iter)
	{
		LoadAnimationVertexData((*iter).second, m_mapClusterData[(*iter).first]);
		CalAnimationVertexData((*iter).second);
	}
}

void Game_Fbx_Ex::DrawSetMeshesCreate()
{
	std::map<int, DRAWSET*>::iterator iter = m_mapDrawsets.begin();
	std::map<int, DRAWSET*>::iterator iterEnd = m_mapDrawsets.end();
	for (; iter != iterEnd; ++iter)
	{
		DrawSetMeshCreate((*iter).second);
	}
}

fbxsdk::FbxNode* Game_Fbx_Ex::RecursiveFindParentLodGroup(fbxsdk::FbxNode* parentNode)
{
	if (nullptr == parentNode)
	{
		return nullptr;
	}
	fbxsdk::FbxNodeAttribute* attribute = parentNode->GetNodeAttribute();
	if (nullptr != attribute && attribute->GetAttributeType() == fbxsdk::FbxNodeAttribute::eLODGroup)
	{
		return parentNode;
	}
	return RecursiveFindParentLodGroup(parentNode->GetParent());
}

fbxsdk::FbxNode* Game_Fbx_Ex::FindLODGroupNode(fbxsdk::FbxNode* NodeLodGroup, int LodIndex, fbxsdk::FbxNode* NodeToFind)
{
	if (NodeLodGroup->GetChildCount() < LodIndex)
	{
		AMSG(L"잘못된 인덱스");
		return nullptr;
	}
	fbxsdk::FbxNode* childNode = NodeLodGroup->GetChild(LodIndex);
	if (nullptr == childNode)
	{
		return nullptr;
	}
	return RecursiveGetFirstMeshNode(childNode, NodeToFind);
}

fbxsdk::FbxNode* Game_Fbx_Ex::RecursiveGetFirstMeshNode(fbxsdk::FbxNode* Node, fbxsdk::FbxNode* NodeToFind)
{
	if (Node == nullptr)
	{
		return nullptr;
	}
	if (Node->GetMesh() != nullptr)
		return Node;
	for (int ChildIndex = 0; ChildIndex < Node->GetChildCount(); ++ChildIndex)
	{
		fbxsdk::FbxNode* MeshNode = RecursiveGetFirstMeshNode(Node->GetChild(ChildIndex), NodeToFind);
		if (NodeToFind == nullptr)
		{
			if (MeshNode != nullptr)
			{
				return MeshNode;
			}
		}
		else if (MeshNode == NodeToFind)
		{
			return MeshNode;
		}
	}
	return nullptr;
}

void Game_Fbx_Ex::TraverseHierarchyNodeRecursively(HFbxExSceneInfo& SceneInfo, fbxsdk::FbxNode* ParentNode, HFbxExNodeInfo& ParentInfo)
{
	int NodeCount = ParentNode->GetChildCount();
	for (int NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
	{
		fbxsdk::FbxNode* ChildNode = ParentNode->GetChild(NodeIndex);
		HFbxExNodeInfo ChildInfo;
		ChildInfo.ObjectName = ChildNode->GetName();
		ChildInfo.UniqueId = ChildNode->GetUniqueID();
		ChildInfo.ParentName = ParentInfo.ObjectName;
		ChildInfo.ParentUniqueId = ParentInfo.UniqueId;
		ChildInfo.RotationPivot = ChildNode->RotationPivot.Get();
		ChildInfo.ScalePivot = ChildNode->ScalingPivot.Get();
		ChildInfo.Transform = ChildNode->EvaluateLocalTransform();
		if (ChildNode->GetNodeAttribute())
		{
			fbxsdk::FbxNodeAttribute* ChildAttribute = ChildNode->GetNodeAttribute();
			ChildInfo.AttributeUniqueId = ChildAttribute->GetUniqueID();
			if (ChildAttribute->GetName()[0] != '\0')
			{
				ChildInfo.AttributeName = ChildAttribute->GetName();
			}
			else
			{
				ChildInfo.AttributeName = ChildAttribute->GetNode()->GetName();
			}

			switch (ChildAttribute->GetAttributeType())
			{
			case fbxsdk::FbxNodeAttribute::eUnknown:
				ChildInfo.AttributeType = "eUnknown";
				break;
			case fbxsdk::FbxNodeAttribute::eNull:
				ChildInfo.AttributeType = "eNull";
				break;
			case fbxsdk::FbxNodeAttribute::eMarker:
				ChildInfo.AttributeType = "eMarker";
				break;
			case fbxsdk::FbxNodeAttribute::eSkeleton:
				ChildInfo.AttributeType = "eSkeleton";
				break;
			case fbxsdk::FbxNodeAttribute::eMesh:
				ChildInfo.AttributeType = "eMesh";
				break;
			case fbxsdk::FbxNodeAttribute::eNurbs:
				ChildInfo.AttributeType = "eNurbs";
				break;
			case fbxsdk::FbxNodeAttribute::ePatch:
				ChildInfo.AttributeType = "ePatch";
				break;
			case fbxsdk::FbxNodeAttribute::eCamera:
				ChildInfo.AttributeType = "eCamera";
				break;
			case fbxsdk::FbxNodeAttribute::eCameraStereo:
				ChildInfo.AttributeType = "eCameraStereo";
				break;
			case fbxsdk::FbxNodeAttribute::eCameraSwitcher:
				ChildInfo.AttributeType = "eCameraSwitcher";
				break;
			case fbxsdk::FbxNodeAttribute::eLight:
				ChildInfo.AttributeType = "eLight";
				break;
			case fbxsdk::FbxNodeAttribute::eOpticalReference:
				ChildInfo.AttributeType = "eOpticalReference";
				break;
			case fbxsdk::FbxNodeAttribute::eOpticalMarker:
				ChildInfo.AttributeType = "eOpticalMarker";
				break;
			case fbxsdk::FbxNodeAttribute::eNurbsCurve:
				ChildInfo.AttributeType = "eNurbsCurve";
				break;
			case fbxsdk::FbxNodeAttribute::eTrimNurbsSurface:
				ChildInfo.AttributeType = "eTrimNurbsSurface";
				break;
			case fbxsdk::FbxNodeAttribute::eBoundary:
				ChildInfo.AttributeType = "eBoundary";
				break;
			case fbxsdk::FbxNodeAttribute::eNurbsSurface:
				ChildInfo.AttributeType = "eNurbsSurface";
				break;
			case fbxsdk::FbxNodeAttribute::eShape:
				ChildInfo.AttributeType = "eShape";
				break;
			case fbxsdk::FbxNodeAttribute::eLODGroup:
				ChildInfo.AttributeType = "eLODGroup";
				break;
			case fbxsdk::FbxNodeAttribute::eSubDiv:
				ChildInfo.AttributeType = "eSubDiv";
				break;
			case fbxsdk::FbxNodeAttribute::eCachedEffect:
				ChildInfo.AttributeType = "eCachedEffect";
				break;
			case fbxsdk::FbxNodeAttribute::eLine:
				ChildInfo.AttributeType = "eLine";
				break;
			}
		}
		else
		{
			ChildInfo.AttributeUniqueId = 0xFFFFFFFFFFFFFFFF;
			ChildInfo.AttributeType = "eNull";
			ChildInfo.AttributeName = NULL;
		}

		SceneInfo.HierarchyInfo.push_back(ChildInfo);
		TraverseHierarchyNodeRecursively(SceneInfo, ChildNode, ChildInfo);
	}
}

fbxsdk::FbxAMatrix Game_Fbx_Ex::ComputeTotalMatrix(fbxsdk::FbxNode* Node)
{
	fbxsdk::FbxAMatrix Geometry;
	fbxsdk::FbxVector4 Translation, Rotation, Scaling;
	Translation = Node->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot);
	Rotation = Node->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot);
	Scaling = Node->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot);
	Geometry.SetT(Translation);
	Geometry.SetR(Rotation);
	Geometry.SetS(Scaling);

	fbxsdk::FbxAMatrix& GlobalTransform = m_pFbxScene->GetAnimationEvaluator()->GetNodeGlobalTransform(Node);
	return GlobalTransform * Geometry;
}

bool Game_Fbx_Ex::IsOddNegativeScale(const fbxsdk::FbxAMatrix& TotalMatrix)
{
	fbxsdk::FbxVector4 Scale = TotalMatrix.GetS();
	int NegativeNum = 0;

	if (Scale[0] < 0) NegativeNum++;
	if (Scale[1] < 0) NegativeNum++;
	if (Scale[2] < 0) NegativeNum++;

	return NegativeNum == 1 || NegativeNum == 3;
}

void Game_Fbx_Ex::DrawSetMaterialSetting(fbxsdk::FbxNode* _Node, DRAWSET* _DrawData)
{
	int MtrlCount = _Node->GetMaterialCount();

	if (MtrlCount > 0)
	{
		_DrawData->m_MatialData.push_back(std::vector<HFbxExMaterialData>());

		std::vector<HFbxExMaterialData>& MatrialSet = _DrawData->m_MatialData[_DrawData->m_MatialData.size() - 1];

		for (int i = 0; i < MtrlCount; i++)
		{
			fbxsdk::FbxSurfaceMaterial* pMtrl = _Node->GetMaterial(i);

			if (nullptr == pMtrl)
			{
				AMSG(L"if (nullptr == pMtrl) 머티리얼 정보 에러");
			}

			MatrialSet.push_back(HFbxExMaterialData());
			HFbxExMaterialData& MatData = MatrialSet[MatrialSet.size() - 1];
			MatData.Name = pMtrl->GetName();
			MatData.DifColor = MaterialColor(pMtrl, fbxsdk::FbxSurfaceMaterial::sDiffuse, fbxsdk::FbxSurfaceMaterial::sDiffuseFactor);
			MatData.AmbColor = MaterialColor(pMtrl, fbxsdk::FbxSurfaceMaterial::sAmbient, fbxsdk::FbxSurfaceMaterial::sAmbientFactor);
			MatData.SpcColor = MaterialColor(pMtrl, fbxsdk::FbxSurfaceMaterial::sSpecular, fbxsdk::FbxSurfaceMaterial::sSpecularFactor);
			MatData.EmvColor = MaterialColor(pMtrl, fbxsdk::FbxSurfaceMaterial::sEmissive, fbxsdk::FbxSurfaceMaterial::sEmissiveFactor);
			MatData.SpecularPower = MaterialFactor(pMtrl, fbxsdk::FbxSurfaceMaterial::sSpecularFactor);
			MatData.Shininess = MaterialFactor(pMtrl, fbxsdk::FbxSurfaceMaterial::sShininess);
			MatData.TransparencyFactor = MaterialFactor(pMtrl, fbxsdk::FbxSurfaceMaterial::sTransparencyFactor);
			MatData.DifTexture = MaterialTex(pMtrl, fbxsdk::FbxSurfaceMaterial::sDiffuse);
			MatData.BmpTexture = MaterialTex(pMtrl, fbxsdk::FbxSurfaceMaterial::sNormalMap);
			MatData.SpcTexture = MaterialTex(pMtrl, fbxsdk::FbxSurfaceMaterial::sSpecular);

			if (L"" != MatData.DifTexture)
			{
				if (true == HGAMEPATH::IsExits(MatData.DifTexture))
				{
					HTEXTURE::Load(MatData.DifTexture);
					MatData.DifTexture = HGAMEIO::FileName(MatData.DifTexture);
				}
			}

			if (L"" != MatData.BmpTexture)
			{
				if (true == HGAMEPATH::IsExits(MatData.BmpTexture))
				{
					HTEXTURE::Load(MatData.BmpTexture);
					MatData.BmpTexture = HGAMEIO::FileName(MatData.BmpTexture);
				}
			}

			if (L"" != MatData.SpcTexture)
			{
				if (true == HGAMEPATH::IsExits(MatData.SpcTexture))
				{
					HTEXTURE::Load(MatData.SpcTexture);
					MatData.SpcTexture = HGAMEIO::FileName(MatData.SpcTexture);
				}
			}

		}

	}
	else {
		AMSG(L"매쉬는 존재하지만 재질은 존재하지 않습니다.");
	}

}

float4 Game_Fbx_Ex::MaterialColor(fbxsdk::FbxSurfaceMaterial* pMtrl, const char* _ColorName, const char* _FactorName)
{
	FbxDouble3 vResult(0, 0, 0);
	double dFactor = 0;
	FbxProperty ColorPro = pMtrl->FindProperty(_ColorName);
	FbxProperty FactorPro = pMtrl->FindProperty(_FactorName);

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

	return float4((float)vResult[0], (float)vResult[1], (float)vResult[2]);
}

float Game_Fbx_Ex::MaterialFactor(fbxsdk::FbxSurfaceMaterial* pMtrl, const char* _FactorName) {
	double dFactor = 0;
	FbxProperty FactorPro = pMtrl->FindProperty(_FactorName);

	if (true == FactorPro.IsValid())
	{
		dFactor = FactorPro.Get<FbxDouble>();
	}

	return (float)dFactor;
}

Game_String Game_Fbx_Ex::MaterialTex(fbxsdk::FbxSurfaceMaterial* pMtrl, const char* _FactorName)
{
	FbxProperty TexturePro = pMtrl->FindProperty(_FactorName);
	Game_String Str;
	if (true == TexturePro.IsValid())
	{
		int iTexCount = TexturePro.GetSrcObjectCount<FbxFileTexture>();

		if (iTexCount > 0)
		{
			FbxFileTexture* pFileTex = TexturePro.GetSrcObject<FbxFileTexture>(0);

			if (nullptr != pFileTex)
			{
				Str = pFileTex->GetFileName();
			}
		}
		else
		{
			return L"";
		}
	}
	else
	{
		return L"";
	}

	return Str;
}

void Game_Fbx_Ex::LoadUv(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
{
	std::map<Game_String, int> UVSets;
	int layerCount = _Mesh->GetLayerCount();
	for (int uvLayerIndex = 0; uvLayerIndex < layerCount; ++uvLayerIndex)
	{
		fbxsdk::FbxLayer* pLayer = _Mesh->GetLayer(uvLayerIndex);
		int uvSetCount = pLayer->GetUVSetCount();
		if (0 != uvSetCount)
		{
			fbxsdk::FbxArray<const fbxsdk::FbxLayerElementUV*> EleUVs = pLayer->GetUVSets();
			for (int UVIndex = 0; UVIndex < uvSetCount; ++UVIndex)
			{
				const fbxsdk::FbxLayerElementUV* elementUv = EleUVs[UVIndex];
				if (nullptr != elementUv)
				{
					Game_String uvName = elementUv->GetName();
					if (true == uvName.IsEmpty())
					{
						uvName.Appendf("UVmap_%d", uvLayerIndex);
					}
					std::map<Game_String, int>::iterator findIter = UVSets.find(uvName);
					if (findIter == UVSets.end())
					{
						UVSets.insert(std::make_pair(uvName, 0));
					}
				}
			}
		}
	}

	if (UVSets.size() != 0)
	{
		for (size_t UVIndex = 0; UVIndex < UVSets.size(); ++UVIndex)
		{
			fbxsdk::FbxLayer* pLayer = _Mesh->GetLayer(static_cast<int>(UVIndex));
			int UVSetCount = pLayer->GetUVSetCount();
			if (UVSetCount != 0)
			{
				fbxsdk::FbxArray<const fbxsdk::FbxLayerElementUV*> EleUVs = pLayer->GetUVSets();
				for (int FbxUVIndex = 0; FbxUVIndex < UVSetCount; ++FbxUVIndex)
				{
					const fbxsdk::FbxLayerElementUV* elementUv = EleUVs[FbxUVIndex];
					if (nullptr != elementUv)
					{
						fbxsdk::FbxLayerElementUV* uvuv = const_cast<fbxsdk::FbxLayerElementUV*>(elementUv);
						auto rere = uvuv->GetReferenceMode();
						auto asad = uvuv->GetMappingMode();
					}
				}
			}
		}
	}

	int iCount = _Mesh->GetElementUVCount();

	if (0 == iCount)
	{
		AMSG(L"UV가 여러개 입니다.");
	}

	fbxsdk::FbxGeometryElementUV* pElement = _Mesh->GetElementUV();
	int iDataIndex = VtxId;

	if (pElement->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygonVertex)
	{
		if (fbxsdk::FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(VtxId);
		}
		else if (fbxsdk::FbxGeometryElement::eDirect == pElement->GetReferenceMode())
		{
			iDataIndex = _Index;
		}
		else
		{
			iDataIndex = VtxId;
		}
	}

	else if (pElement->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint)
	{
		if (fbxsdk::FbxGeometryElement::eDirect == pElement->GetReferenceMode())
		{
			iDataIndex = _Index;
		}
		else if (fbxsdk::FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(_Index);
		}
		else {
			AMSG(L"맵핑 방식중 처리할수 없는 방식입니다.");
		}
	}

	fbxsdk::FbxVector2 vUV = pElement->GetDirectArray().GetAt(iDataIndex);

	_ArrVtx[_Index].Uv.x = (float)(vUV.mData[0] - (int)vUV.mData[0]);
	_ArrVtx[_Index].Uv.y = (float)(1.0f - (vUV.mData[1] - (int)vUV.mData[1]));

}

void Game_Fbx_Ex::LoadBinormal(fbxsdk::FbxMesh* _Mesh, fbxsdk::FbxAMatrix _MeshMatrix, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
{
	int iCount = _Mesh->GetElementBinormalCount();

	if (0 == iCount)
	{
		return;

	}

	FbxGeometryElementBinormal* pElement = _Mesh->GetElementBinormal();
	int iDataIndex = VtxId;

	if (pElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(VtxId);
		}
		else
		{
			iDataIndex = VtxId;
		}
	}

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

	FbxVector4 BiNormal = pElement->GetDirectArray().GetAt(iDataIndex);

	fbxsdk::FbxAMatrix conversionMeshMatrix = _MeshMatrix.Transpose();
	BiNormal = conversionMeshMatrix.MultT(BiNormal);


	_ArrVtx[_Index].BiNormal.x = (float)BiNormal.mData[0];
	_ArrVtx[_Index].BiNormal.y = (float)BiNormal.mData[1];
	_ArrVtx[_Index].BiNormal.z = -(float)BiNormal.mData[2];
	_ArrVtx[_Index].BiNormal.w = (float)BiNormal.mData[3];
	_ArrVtx[_Index].BiNormal.NORMAL3D();
}

void Game_Fbx_Ex::LoadTangent(fbxsdk::FbxMesh* _Mesh, fbxsdk::FbxAMatrix _MeshMatrix, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
{
	int iCount = _Mesh->GetElementTangentCount();

	if (0 == iCount)
	{
		return;
	}
	FbxGeometryElementTangent* pElement = _Mesh->GetElementTangent();
	int iDataIndex = VtxId;

	if (pElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(VtxId);
		}
		else
		{
			iDataIndex = VtxId;
		}
	}
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

	FbxVector4 Tangent = pElement->GetDirectArray().GetAt(iDataIndex);


	fbxsdk::FbxAMatrix conversionMeshMatrix = _MeshMatrix.Transpose();
	Tangent = conversionMeshMatrix.MultT(Tangent);


	_ArrVtx[_Index].Tangent.x = (float)Tangent.mData[0];
	_ArrVtx[_Index].Tangent.y = (float)Tangent.mData[1];
	_ArrVtx[_Index].Tangent.z = -(float)Tangent.mData[2];
	_ArrVtx[_Index].Tangent.w = (float)Tangent.mData[3];
	_ArrVtx[_Index].Tangent.NORMAL3D();
}

void Game_Fbx_Ex::LoadNormal(fbxsdk::FbxMesh* _Mesh, fbxsdk::FbxAMatrix _MeshMatrix, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index)
{
	int iCount = _Mesh->GetElementNormalCount();

	if (0 == iCount)
	{
		AMSG(L"GetElementNormalCount가 여러개 입니다.");
	}


	FbxGeometryElementNormal* pElement = _Mesh->GetElementNormal();
	int iDataIndex = VtxId;

	if (pElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (FbxGeometryElement::eIndexToDirect == pElement->GetReferenceMode())
		{
			iDataIndex = pElement->GetIndexArray().GetAt(VtxId);
		}
		else
		{
			iDataIndex = VtxId;
		}
	}
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

	FbxVector4 Normal = pElement->GetDirectArray().GetAt(iDataIndex);

	fbxsdk::FbxAMatrix conversionMeshMatrix = _MeshMatrix.Transpose();
	Normal = conversionMeshMatrix.MultT(Normal);

	_ArrVtx[_Index].Normal.x = (float)Normal.mData[0];
	_ArrVtx[_Index].Normal.y = (float)Normal.mData[1];
	_ArrVtx[_Index].Normal.z = -(float)Normal.mData[2];
	_ArrVtx[_Index].Normal.w = (float)Normal.mData[3];
	// _ArrVtx[_Index].Normal.w = 0.0f;
	_ArrVtx[_Index].Normal.NORMAL3D();
}

void Game_Fbx_Ex::DrawSetMeshCreate(DRAWSET* _DrawData)
{
	if (_DrawData->m_FbxVtx.size() != _DrawData->m_FbxIdx.size())
	{
		AMSG(L"인덱스 버퍼 버텍스 버퍼 매칭 오류");
		return;
	}

	_DrawData->m_VB.resize(_DrawData->m_FbxVtx.size());
	_DrawData->m_IB.resize(_DrawData->m_FbxIdx.size());
	_DrawData->m_Mesh.resize(_DrawData->m_FbxVtx.size());

	for (size_t i = 0; i < _DrawData->m_FbxVtx.size(); i++)
	{
		HVBBUFFER* VB = new HVBBUFFER();
		VB->Create((*_DrawData->m_FbxVtx[i]).size(), sizeof(VTX3DMESHIN), &(*_DrawData->m_FbxVtx[i])[0], D3D11_USAGE::D3D11_USAGE_DEFAULT);
		_DrawData->m_VB[i] = VB;

		for (size_t j = 0; j < _DrawData->m_FbxIdx[i]->size(); j++)
		{
			if (0 == (*_DrawData->m_FbxIdx[i])[j].size())
			{
				continue;
			}
			//JKSharedPtr<JKIndexBuffer> IB = ResourcesManager()->Create<JKIndexBuffer>(this, (*_DrawData->m_FbxIdx[i])[j]);
			//_DrawData->m_IB[i].push_back(IB);
			Game_Ptr<HIBBUFFER> IB = new HIBBUFFER();
			IB->Create((*_DrawData->m_FbxIdx[i])[j].size(), sizeof(UINT), &(*_DrawData->m_FbxIdx[i])[j][0], DXGI_FORMAT::DXGI_FORMAT_R32_UINT, D3D11_USAGE::D3D11_USAGE_DEFAULT);
			_DrawData->m_IB[i].push_back(IB);

			Game_Ptr<HMESH> NewMesh = new HMESH();
			NewMesh->VB(VB);
			NewMesh->IB(IB);
			_DrawData->m_Mesh[i].push_back(NewMesh);
		}
	}

}

void Game_Fbx_Ex::LoadAnimationVertexData(DRAWSET* _DrawData, const std::vector<ClusterData>& vecClusterData)
{
	for (auto& clusterData : vecClusterData)
	{
		if (nullptr == clusterData.m_Cluster->GetLink())
		{
			continue;
		}

		Game_String StrBoneName = clusterData.LinkName;
		Bone* pBone = FindBone(StrBoneName);
		if (nullptr == pBone)
		{
			continue;
		}

		DrawSetWeightAndIndexSetting(_DrawData, clusterData.m_Mesh, clusterData.m_Cluster, pBone->Index);
	}
}

void Game_Fbx_Ex::CalAnimationVertexData(DRAWSET* _DrawData)
{
	for (auto& _WISet : _DrawData->m_MapWI)
	{
		std::vector<VTX3DMESHIN>* Ptr = _DrawData->m_FbxVtxMap[_WISet.first];

		if (nullptr == Ptr)
		{
			AMSG(L"버텍스 데이터와 수집한 가중치 데이터가 매칭되지 않습니다.");
		}

		std::vector<VTX3DMESHIN>& VertexData = *Ptr;


		for (auto& _WI : _WISet.second)
		{
			double dWeight = 0.0;
			for (int n = 0; n < _WI.second.size(); ++n)
			{
				dWeight += _WI.second[n].Weight;
			}

			for (int n = 0; n < _WI.second.size(); ++n)
			{
				_WI.second[n].Weight /= dWeight;
			}

			if (_WI.second.size() > 4)
			{
				std::sort(_WI.second.begin(), _WI.second.end(),
					[](const HFbxExIW& _Left, const HFbxExIW& _Right) {
						return _Left.Weight > _Right.Weight;
					}
				);

				double dInterPolate = 0.0;

				std::vector<HFbxExIW>::iterator IterErase = _WI.second.begin() + 4;
				for (; IterErase != _WI.second.end(); ++IterErase)
				{
					dInterPolate += IterErase->Weight;
				}
				IterErase = _WI.second.begin() + 4;

				_WI.second.erase(IterErase, _WI.second.end());
				_WI.second[0].Weight += dInterPolate;
			};

			float Weight[4] = { 0.0f };
			int Index[4] = { 0 };

			for (size_t i = 0; i < _WI.second.size(); i++)
			{
				Weight[i] = (float)_WI.second[i].Weight;
				Index[i] = _WI.second[i].Index;
			}

			memcpy_s(VertexData[_WI.first].Weight.Arr, sizeof(float4), Weight, sizeof(float4));
			memcpy_s(VertexData[_WI.first].Index, sizeof(float4), Index, sizeof(float4));
		}
	}
}

void Game_Fbx_Ex::DrawSetWeightAndIndexSetting(DRAWSET* _DrawSet, fbxsdk::FbxMesh* _Mesh, fbxsdk::FbxCluster* _Cluster, int _BoneIndex)
{
	int iIndexCount = _Cluster->GetControlPointIndicesCount();

	for (size_t i = 0; i < iIndexCount; i++)
	{
		HFbxExIW IW;
		IW.Index = _BoneIndex;

		IW.Weight = _Cluster->GetControlPointWeights()[i];
		int ControlPointIndices = _Cluster->GetControlPointIndices()[i];

		_DrawSet->m_MapWI[_Mesh][ControlPointIndices].push_back(IW);
	}
}

Bone* Game_Fbx_Ex::FindBone(const Game_String& _boneName)
{
	std::vector<Bone>::iterator iter = m_vecRefBones.begin();
	std::vector<Bone>::iterator iterEnd = m_vecRefBones.end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter).Name == _boneName)
		{
			return &(*iter);
		}
	}

	return nullptr;
}

Bone* Game_Fbx_Ex::FindBone(const int Index)
{
	if (m_vecRefBones.size() <= Index)
	{
		return nullptr;
	}

	return &m_vecRefBones[Index];
}
 

int Game_Fbx_Ex::GetFbxMeshCount()
{
	return (int)m_SceneInfo.MeshInfo.size();
}

Game_String Game_Fbx_Ex::GetMeshNameByIndex(const int Index)
{
	if (m_SceneInfo.MeshInfo.size() <= Index)
	{
		return L"None";
	}

	return m_SceneInfo.MeshInfo[Index].Name;
}

bool Game_Fbx_Ex::RetrievePoseFromBindPose(fbxsdk::FbxScene* pScene, const std::vector<fbxsdk::FbxNode*>& NodeArray, fbxsdk::FbxArray<fbxsdk::FbxPose*>& PoseArray)
{
	const int PoseCount = pScene->GetPoseCount();
	for (int PoseIndex = 0; PoseIndex < PoseCount; PoseIndex++)
	{
		fbxsdk::FbxPose* CurrentPose = pScene->GetPose(PoseIndex);

		if (CurrentPose && CurrentPose->IsBindPose())
		{
			Game_String PoseName = CurrentPose->GetName();
			fbxsdk::FbxStatus Status;

			for (auto Current : NodeArray)
			{
				Game_String CurrentName = Current->GetName();
				fbxsdk::NodeList pMissingAncestors, pMissingDeformers, pMissingDeformersAncestors, pWrongMatrices;

				if (CurrentPose->IsValidBindPoseVerbose(Current, pMissingAncestors, pMissingDeformers, pMissingDeformersAncestors, pWrongMatrices, 0.0001, &Status))
				{
					PoseArray.Add(CurrentPose);
					break;
				}
				else
				{
					for (int i = 0; i < pMissingAncestors.GetCount(); i++)
					{
						fbxsdk::FbxAMatrix mat = pMissingAncestors.GetAt(i)->EvaluateGlobalTransform(FBXSDK_TIME_ZERO);
						CurrentPose->Add(pMissingAncestors.GetAt(i), mat);
					}

					pMissingAncestors.Clear();
					pMissingDeformers.Clear();
					pMissingDeformersAncestors.Clear();
					pWrongMatrices.Clear();

					if (CurrentPose->IsValidBindPose(Current))
					{
						PoseArray.Add(CurrentPose);
						break;
					}
					else
					{
						fbxsdk::FbxNode* ParentNode = Current->GetParent();
						while (ParentNode)
						{
							fbxsdk::FbxNodeAttribute* Attr = ParentNode->GetNodeAttribute();
							if (Attr && Attr->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNull)
							{
								break;
							}

							ParentNode = ParentNode->GetParent();
						}

						if (ParentNode && CurrentPose->IsValidBindPose(ParentNode))
						{
							PoseArray.Add(CurrentPose);
							break;
						}
						else
						{
							Game_String ErrorString = Status.GetErrorString();
							Game_String CurrentName = Current->GetName();
							std::wcout << L"Pose ( " << PoseName.operator LPCWSTR() << L" ) " << L" Node ( " << CurrentName.operator LPCWSTR() << L" ) " << ErrorString.operator LPCWSTR() << std::endl;
						}
					}
				}
			}
		}
	}

	return (PoseArray.Size() > 0);
}

void Game_Fbx_Ex::BuildSkeletonSystem(fbxsdk::FbxScene* pScene, std::vector<fbxsdk::FbxCluster*>& ClusterArray, std::vector<fbxsdk::FbxNode*>& OutSortedLinks)
{
	fbxsdk::FbxNode* Link;
	std::vector<fbxsdk::FbxNode*> RootLinks;
	size_t ClusterIndex;
	for (ClusterIndex = 0; ClusterIndex < ClusterArray.size(); ClusterIndex++)
	{
		Link = ClusterArray[ClusterIndex]->GetLink();
		if (Link)
		{
			Link = GetRootSkeleton(pScene, Link);
			size_t LinkIndex;
			for (LinkIndex = 0; LinkIndex < RootLinks.size(); LinkIndex++)
			{
				if (Link == RootLinks[static_cast<int>(LinkIndex)])
				{
					break;
				}
			}

			if (LinkIndex == RootLinks.size())
			{
				RootLinks.push_back(Link);
			}
		}
	}

	for (size_t LinkIndex = 0; LinkIndex < RootLinks.size(); LinkIndex++)
	{
		RecursiveBuildSkeleton(RootLinks[LinkIndex], OutSortedLinks);
	}
}

fbxsdk::FbxNode* Game_Fbx_Ex::GetRootSkeleton(fbxsdk::FbxScene* pScene, fbxsdk::FbxNode* Link)
{
	fbxsdk::FbxNode* RootBone = Link;

	while (RootBone && RootBone->GetParent())
	{
		bool bIsBlenderArmatureBone = false;

		fbxsdk::FbxNodeAttribute* Attr = RootBone->GetParent()->GetNodeAttribute();
		if (Attr &&
			(Attr->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh ||
				(Attr->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNull && !bIsBlenderArmatureBone) ||
				Attr->GetAttributeType() == fbxsdk::FbxNodeAttribute::eSkeleton) &&
			RootBone->GetParent() != pScene->GetRootNode())
		{
			if (Attr->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh)
			{
				fbxsdk::FbxMesh* Mesh = (fbxsdk::FbxMesh*)Attr;
				if (Mesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin) > 0)
				{
					break;
				}
			}

			RootBone = RootBone->GetParent();
		}
		else
		{
			break;
		}
	}

	return RootBone;
}

void Game_Fbx_Ex::RecursiveBuildSkeleton(fbxsdk::FbxNode* Link, std::vector<fbxsdk::FbxNode*>& OutSortedLinks)
{
	if (IsBone(Link))
	{
		if (false == IsNull(Link))
		{
			OutSortedLinks.push_back(Link);
		}
		int ChildIndex;
		for (ChildIndex = 0; ChildIndex < Link->GetChildCount(); ChildIndex++)
		{
			RecursiveBuildSkeleton(Link->GetChild(ChildIndex), OutSortedLinks);
		}
	}
}

bool Game_Fbx_Ex::IsBone(fbxsdk::FbxNode* Link)
{
	fbxsdk::FbxNodeAttribute* Attr = Link->GetNodeAttribute();
	if (Attr)
	{
		fbxsdk::FbxNodeAttribute::EType AttrType = Attr->GetAttributeType();
		if (AttrType == fbxsdk::FbxNodeAttribute::eSkeleton ||
			AttrType == fbxsdk::FbxNodeAttribute::eMesh ||
			AttrType == fbxsdk::FbxNodeAttribute::eNull)
		{
			return true;
		}
	}

	return false;
}

bool Game_Fbx_Ex::IsNull(fbxsdk::FbxNode* Link)
{
	fbxsdk::FbxNodeAttribute* Attr = Link->GetNodeAttribute();
	if (Attr)
	{
		fbxsdk::FbxNodeAttribute::EType AttrType = Attr->GetAttributeType();
		if (AttrType == fbxsdk::FbxNodeAttribute::eNull)
		{
			return true;
		}
	}
	return false;
}

int Game_Fbx_Ex::GetDrawSetCount() 
{
	return (int)m_mapDrawsets.size();
}

DRAWSET* Game_Fbx_Ex::GetDrawSet(int LodLevel)
{
	if (0 == m_mapDrawsets.size())
	{
		return nullptr;
	}

	std::map<int, DRAWSET*>::iterator iter = m_mapDrawsets.find(LodLevel);
	if (iter == m_mapDrawsets.end())
	{
		return m_mapDrawsets.begin()->second;
	}
	return iter->second;
}

size_t Game_Fbx_Ex::GetBoneCount()
{
	return m_vecRefBones.size();
}





HMATRIX Game_Fbx_Ex::FbxMatToHMATRIX(const fbxsdk::FbxAMatrix& _BaseTrans)
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

fbxsdk::FbxAMatrix HMATRIXToFbxAMatrix(const HMATRIX& _HMATRIX)
{
	fbxsdk::FbxAMatrix mat;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			mat.mData[y].mData[x] = _HMATRIX.Arr2D[y][x];
		}
	}

	return mat;
}

float4 Game_Fbx_Ex::FbxVecTofloat4(const fbxsdk::FbxVector4& _BaseVector)
{
	float4 Vec;
	Vec.Arr[0] = (float)_BaseVector.mData[0];
	Vec.Arr[1] = (float)_BaseVector.mData[1];
	Vec.Arr[2] = (float)_BaseVector.mData[2];
	Vec.Arr[3] = (float)_BaseVector.mData[3];

	return Vec;

}

float4 Game_Fbx_Ex::FbxVecToTransform(const fbxsdk::FbxVector4& _BaseVector)
{
	float4 Vec;
	Vec.Arr[0] = (float)_BaseVector.mData[0];
	Vec.Arr[1] = (float)_BaseVector.mData[1];
	Vec.Arr[2] = -(float)_BaseVector.mData[2];
	Vec.Arr[3] = (float)_BaseVector.mData[3];
	return Vec;
}

float4 Game_Fbx_Ex::FbxQuaternionTofloat4(const fbxsdk::FbxQuaternion& _BaseQ)
{
	float4 Vec;
	Vec.Arr[0] = (float)_BaseQ.mData[0];
	Vec.Arr[1] = (float)_BaseQ.mData[1];
	Vec.Arr[2] = -(float)_BaseQ.mData[2];
	Vec.Arr[3] = -(float)_BaseQ.mData[3];
	return Vec;
}

bool Game_Fbx_Ex::IsMesh()
{
	return true;
}
bool Game_Fbx_Ex::IsSkeleton()
{
	return true;
}
bool Game_Fbx_Ex::IsAnimation()
{
	return 0 != m_UserAniData.size();
}

void Game_Fbx_Ex::CalHFbxExBoneFrameTransMatrix(Game_Ptr<Game_Fbx_Ex> _Fbx, const Game_String& _AniFbxName)
{
	if (0 == m_UserAniData.size())
	{
		return;
	}

	for (size_t i = 0; i < m_UserAniData.size(); i++)
	{
		m_UserAniData[i].m_AniFrameData.resize(_Fbx->m_vecRefBones.size());
	}

	ProcessAnimationLoad(_Fbx, _Fbx->m_pFbxRootNode, _AniFbxName);

	ProcessAnimationCheckState(_Fbx);
}

void Game_Fbx_Ex::ProcessAnimationLoad(Game_Ptr<Game_Fbx_Ex> _Fbx, fbxsdk::FbxNode* pNode, const Game_String& _AniFbxName)
{
	fbxsdk::FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
	if (nullptr != pNodeAttribute)
	{
		fbxsdk::FbxNodeAttribute::EType typetype = pNodeAttribute->GetAttributeType();
		switch (pNodeAttribute->GetAttributeType())
		{
		case fbxsdk::FbxNodeAttribute::eLODGroup:
		{
			if (0 != pNode->GetChildCount())
			{
				ProcessAnimationLoad(_Fbx, pNode->GetChild(0), _AniFbxName);
				return;
			}
		}
		break;
		case fbxsdk::FbxNodeAttribute::eMesh:
		{
			MeshAnimationLoad(_Fbx, pNode, _AniFbxName);
		}
		break;
		default:
			break;
		}
	}

	for (int n = 0; n < pNode->GetChildCount(); ++n)
	{
		ProcessAnimationLoad(_Fbx, pNode->GetChild(n), _AniFbxName);
	}
}

void Game_Fbx_Ex::ProcessAnimationCheckState(Game_Ptr<Game_Fbx_Ex> _Fbx)
{
	size_t userAniDataSize = m_UserAniData.size();
	for (size_t userAniDataIndex = 0; userAniDataIndex < userAniDataSize; ++userAniDataIndex)
	{
		HFbxExAniData& userAniData = m_UserAniData.at(userAniDataIndex);
		fbxsdk::FbxLongLong fbxTime = userAniData.EndTime.Get() - userAniData.StartTime.Get() + 1;
		size_t aniFrameDataSize = userAniData.m_AniFrameData.size();
		for (size_t aniFrameDataIndex = 0; aniFrameDataIndex < aniFrameDataSize; ++aniFrameDataIndex)
		{
			HFbxExBoneFrame& aniFrameData = userAniData.m_AniFrameData.at(aniFrameDataIndex);
			if (0 == aniFrameData.m_Data.size())
			{
				aniFrameData.m_Data.resize(fbxTime);
				Bone& curBone = _Fbx->m_vecRefBones[aniFrameDataIndex];
				aniFrameData.BoneIndex = curBone.Index;
				aniFrameData.BoneParentIndex = curBone.ParentIndex;
				if (-1 != curBone.ParentIndex)
				{
					HFbxExBoneFrame& parentAniFrameData = userAniData.m_AniFrameData.at(curBone.ParentIndex);
					for (fbxsdk::FbxLongLong start = 0; start < fbxTime; ++start)
					{
						aniFrameData.m_Data[start].Time = parentAniFrameData.m_Data[start].Time;
						aniFrameData.m_Data[start].LocalAnimation = HMATRIXToFbxAMatrix(curBone.BonePos.Local);
						aniFrameData.m_Data[start].GlobalAnimation = parentAniFrameData.m_Data[start].GlobalAnimation * aniFrameData.m_Data[start].LocalAnimation;
						aniFrameData.m_Data[start].FrameMat = FbxMatToHMATRIX(aniFrameData.m_Data[start].GlobalAnimation);
						aniFrameData.m_Data[start].S = FbxVecTofloat4(aniFrameData.m_Data[start].GlobalAnimation.GetS());
						aniFrameData.m_Data[start].Q = FbxQuaternionTofloat4(aniFrameData.m_Data[start].GlobalAnimation.GetQ());
						aniFrameData.m_Data[start].T = FbxVecToTransform(aniFrameData.m_Data[start].GlobalAnimation.GetT());
					}
				}
				else
				{
					for (fbxsdk::FbxLongLong start = 0; start < fbxTime; ++start)
					{
						aniFrameData.m_Data[start].Time = 0;
						aniFrameData.m_Data[start].LocalAnimation = HMATRIXToFbxAMatrix(curBone.BonePos.Local);
						aniFrameData.m_Data[start].GlobalAnimation = aniFrameData.m_Data[start].LocalAnimation;
						aniFrameData.m_Data[start].FrameMat = FbxMatToHMATRIX(aniFrameData.m_Data[start].GlobalAnimation);
						aniFrameData.m_Data[start].S = FbxVecTofloat4(aniFrameData.m_Data[start].GlobalAnimation.GetS());
						aniFrameData.m_Data[start].Q = FbxQuaternionTofloat4(aniFrameData.m_Data[start].GlobalAnimation.GetQ());
						aniFrameData.m_Data[start].T = FbxVecToTransform(aniFrameData.m_Data[start].GlobalAnimation.GetT());
					}
				}
			}
		}
	}
}

void Game_Fbx_Ex::MeshAnimationLoad(Game_Ptr<Game_Fbx_Ex> _Fbx, fbxsdk::FbxNode* pNode, const Game_String& _AniFbxName)
{
	if (LoadList.find(_AniFbxName) != LoadList.end()) //찾음
	{
		m_UserAniData = LoadList.find(_AniFbxName)->second;
		return;
	}

	std::vector<HFbxExBoneFrame> vecAniFrameData;
	vecAniFrameData.resize(_Fbx->GetBoneCount());

	fbxsdk::FbxMesh* pCurrMesh = pNode->GetMesh();
	int deformerCount = pCurrMesh->GetDeformerCount();
	size_t userAniDataSize = m_UserAniData.size();
	fbxsdk::FbxAMatrix geometryTransform = GetGeometryTransformation(pNode);

	for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
	{
		fbxsdk::FbxSkin* pCurrSkin = reinterpret_cast<fbxsdk::FbxSkin*>(pCurrMesh->GetDeformer(deformerIndex, fbxsdk::FbxDeformer::eSkin));
		if (nullptr == pCurrSkin)
		{
			continue;
		}

		for (size_t userAniDataIndex = 0; userAniDataIndex < userAniDataSize; ++userAniDataIndex)
		{
			unsigned int numOfClusters = pCurrSkin->GetClusterCount();
			for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
			{
				fbxsdk::FbxCluster* pCurrCluster = pCurrSkin->GetCluster(clusterIndex);
				Game_String currJointName = pCurrCluster->GetLink()->GetName();
				Bone* pBone = _Fbx->FindBone(currJointName);
				fbxsdk::FbxAMatrix transformMatrix;
				fbxsdk::FbxAMatrix transformLinkMatrix;
				fbxsdk::FbxAMatrix globalBindposeInverseMatrix;

				pCurrCluster->GetTransformMatrix(transformMatrix);
				pCurrCluster->GetTransformLinkMatrix(transformLinkMatrix);
				globalBindposeInverseMatrix = transformLinkMatrix.Inverse();

				fbxsdk::FbxAnimStack* currAnimStack = m_pFbxScene->GetSrcObject<fbxsdk::FbxAnimStack>(0);
				fbxsdk::FbxString animStackName = currAnimStack->GetName();
				fbxsdk::FbxTakeInfo* takeInfo = m_pFbxScene->GetTakeInfo(animStackName);
				fbxsdk::FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
				fbxsdk::FbxTime end = takeInfo->mLocalTimeSpan.GetStop();

				fbxsdk::FbxTime::EMode timeMode = m_pFbxScene->GetGlobalSettings().GetTimeMode();

				fbxsdk::FbxLongLong endTime = end.GetFrameCount(timeMode);
				fbxsdk::FbxLongLong startTime = start.GetFrameCount(timeMode);
				m_UserAniData[userAniDataIndex].EndTime = endTime;
				m_UserAniData[userAniDataIndex].StartTime = startTime;
				m_UserAniData[userAniDataIndex].TimeMode = timeMode;

				m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].m_Data.resize(endTime - startTime + 1);

				for (fbxsdk::FbxLongLong i = startTime; i <= endTime; ++i)
				{
					fbxsdk::FbxLongLong fixIndex = i - startTime;

					Game_String linkName = pCurrCluster->GetLink()->GetName();
					fbxsdk::FbxNode* pLinkNode = m_pFbxScene->FindNodeByName(linkName.operator std::string().c_str());
					fbxsdk::FbxTime currTime;
					currTime.SetFrame(fixIndex, timeMode);
					fbxsdk::FbxAMatrix currentTransformOffset = pNode->EvaluateGlobalTransform(currTime) * m_JointPostConversionMatrix * geometryTransform;
					fbxsdk::FbxAMatrix localTransform;
					fbxsdk::FbxAMatrix globalTransform = currentTransformOffset.Inverse() * pLinkNode->EvaluateGlobalTransform(currTime);


					localTransform.SetS(pLinkNode->EvaluateLocalScaling(currTime));
					localTransform.SetR(pLinkNode->EvaluateLocalRotation(currTime));
					localTransform.SetT(pLinkNode->EvaluateLocalTranslation(currTime));

					m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].BoneIndex = pBone->Index;
					m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].BoneParentIndex = pBone->ParentIndex;
					m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].m_Data[fixIndex].Time = currTime.GetSecondDouble();
					m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].m_Data[fixIndex].LocalAnimation = localTransform;
					m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].m_Data[fixIndex].GlobalAnimation = globalTransform;
					m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].m_Data[fixIndex].FrameMat = FbxMatToHMATRIX(m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].m_Data[fixIndex].GlobalAnimation);
					m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].m_Data[fixIndex].S = FbxVecTofloat4(m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].m_Data[fixIndex].GlobalAnimation.GetS());
					m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].m_Data[fixIndex].Q = FbxQuaternionTofloat4(m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].m_Data[fixIndex].GlobalAnimation.GetQ());
					m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].m_Data[fixIndex].T = FbxVecToTransform(m_UserAniData[userAniDataIndex].m_AniFrameData[pBone->Index].m_Data[fixIndex].GlobalAnimation.GetT());
				}
			}
		}
	}

	LoadList.insert({ _AniFbxName , m_UserAniData });
}

fbxsdk::FbxAMatrix Game_Fbx_Ex::GetGeometryTransformation(fbxsdk::FbxNode* pNode)
{
	fbxsdk::FbxVector4 translation = pNode->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot);
	fbxsdk::FbxVector4 rotation = pNode->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot);
	fbxsdk::FbxVector4 scale = pNode->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot);

	return fbxsdk::FbxAMatrix(translation, rotation, scale);
}
