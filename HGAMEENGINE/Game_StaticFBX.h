#pragma once
#include <HMGR.h>
#include <fbxsdk.h>
#include <vector>
#include <HGAMESTRING.h>
#include "HRENDERER.h"
#include "HMATERIAL.h"
#include "DEFVTXHEADER.h"

class LejFbxBoneFrameData
{
public:
	Game_Vector S;
	Game_Vector Q;/*  여기서 회전은 쿼터니온 입니다. */
	Game_Vector T;
	double Time;
	HMATRIX FrameMat;
};


class LejFbxBoneFrame
{
public:
	int BoneIndex;
	std::vector<LejFbxBoneFrameData> m_Data;
};

/* 머테리얼 정보입니다. */
class LejFbxMaterialData
{
public:
	Game_Vector DifColor;
	Game_Vector AmbColor;
	Game_Vector SpcColor;
	Game_Vector EmvColor;
	float SpecularPower;
	float TransparencyFactor;
	float Shininess;
	Game_String DifTexture;	/* 이 변수는 일반적으로 색을 표현하는 텍스쳐입니다. */
	Game_String BmpTexture;
	Game_String SpcTexture;
public:
	LejFbxMaterialData() {}
	~LejFbxMaterialData() {}
};
/*	본 -> 애니메이션 전달되는 데이터 입니다. 프레임 계산 기본정보입니다.  */
class LejFbxAniData
{
public:
	Game_String AniName;
	fbxsdk::FbxTime StartTime; // 0
	fbxsdk::FbxTime EndTime; // 1434 // 30
	fbxsdk::FbxLongLong TimeStartCount; // 0
	fbxsdk::FbxLongLong TimeEndCount; // 1434
	fbxsdk::FbxLongLong FrameCount;
	fbxsdk::FbxTime::EMode TimeMode;
	int FbxModeCount;

	std::vector<LejFbxBoneFrame> m_AniFrameData;

public:
	float FrameTime(int _Frame)
	{
		fbxsdk::FbxTime tTime = {};
		tTime.SetFrame(_Frame, TimeMode);
		return (float)tTime.GetSecondDouble();
	}

public:
	int FrameModeCount()
	{
		switch (TimeMode)
		{
		case fbxsdk::FbxTime::eFrames120:
			return 120;
		case fbxsdk::FbxTime::eFrames100:
			return 100;
		case fbxsdk::FbxTime::eFrames60:
			return 60;
		case fbxsdk::FbxTime::eFrames50:
			return 50;
		case fbxsdk::FbxTime::eFrames48:
			return 48;
		case fbxsdk::FbxTime::eFrames30:
			return 30;
		case fbxsdk::FbxTime::eFrames24:
			return 24;
		default:
			AMSG(L"처리할수 없는 프레임 카운트 모드입니다.");
			break;
		}
		AMSG(L"처리할수 없는 프레임 카운트 모드입니다.");
		return -1;
	}
public:
	LejFbxAniData() {}
	~LejFbxAniData() {}
};

/* 본은 단지 화면상의 어디에 위치, 정렬하게 하는 행렬 그자체 입니다. */
class LejFbxBoneData
{
	/* 본은 루트가 있습니다. 트리구조처럼 되어 있습니다. 부모자식행렬이 만들어집니다. ROOT이면 0이고, int가 순서대로 매겨집니다.  */
	/* iIndex는 임의의 변수 입니다. m_UserBoneData의 인덱스입니다.  */
public:
	Game_String	Name;
	int				iDepth;
	int				iIndex;
	/* 부모행렬(본)이 가진 것이 INDEX==49 이면 자식행렬은 49번째 INDEX에 있는 행렬(본)과 연산을 합니다.  */
	int				iParentIndex;
	HMATRIX		Offset;
	HMATRIX		Bone;
public:
	LejFbxBoneData() {}
	~LejFbxBoneData() {}
};
/* 스키닝에 필요한 정보입니다. */
class LejFbxIndexWeight
{
public:
	int Index;
	double Weight;
};


class Game_StaticFBX : public HFILEMGR<Game_StaticFBX>
{
public:
	static Game_Ptr<Game_StaticFBX> Load(const HGAMEFILE& _File)
	{
		Game_Ptr<Game_StaticFBX> Res = CreateToInsertToFile(_File.FullPath());
		Res->Load();
		return Res;
	}

private:
	/* FBX의 좌표계는 Y와 Z가 바뀐형태 입니다.	*/
	/* 오토데스크는 자체 매트릭스 개발했습니다. 프레임워크에서 XMMAT으로 변환됩니다. */
	fbxsdk::FbxAMatrix ReMat;
	/* fbx 시스템의 권한 입니다. */
	fbxsdk::FbxManager* m_Mgr;
	/* FBX파일의 IO-SYSTEM을 정의합니다. */
	fbxsdk::FbxIOSettings* m_Ios;
	/* 3d맥스 편집에서 씬을 정의합니다. */
	fbxsdk::FbxScene* m_Scene;
	/* FBX를 로드하는 실질적인 개체입니다. */
	fbxsdk::FbxImporter* m_Imp;
	/* 기본 기저벡터들의 방향 입니다. */
	fbxsdk::FbxAxisSystem eLocalSpaceType;

public:
	/* 프레임워크 방식대로 저장합니다. */
	/* 메쉬폴더구조, 로드구조에 따라 달라집니다.  */
	/* 지금은 VB 하나에 IB 여러개 입니다. */
	std::vector<std::vector<VTX3DMESHIN>> m_FbxVtx;
	std::vector<std::vector<std::vector<UINT>>> m_FbxIdx;
	std::vector<Game_Ptr<HVBBUFFER>> m_VB;
	std::vector<std::vector<Game_Ptr<HIBBUFFER>>> m_IB;

	/* 메쉬정보입니다. 	*/
	std::vector<Game_Ptr<HMESH>> m_Mesh; //☆ 핵심자료 

/* 유저의 머테리얼 정보 입니다. 정점정보와 함께 어떤 행렬(본)에 곱해지면 화면의 해당위치로 이동합니다.  */
	std::vector<LejFbxMaterialData> m_UserMaterialData;

	/* 핵심적인 데이터들 입니다. */
	std::vector<LejFbxBoneData> m_UserBoneData;
	std::vector<LejFbxAniData>	m_UserAniData;

	/* 이름으로 관리합니다. */
	std::map<Game_String, LejFbxBoneData*>	m_BoneDataMap;
	std::map<Game_String, LejFbxAniData*>		m_UserAniDataMap;

	/* 애니메이션 정보의 가중치 및 인덱스를 따로 관리합니다. ROOT가 여러개있는 메쉬를 대비합니다. 특정 부분에서 쓰고 사용하지 않습니다. */
	std::vector<std::map<int, std::vector<LejFbxIndexWeight>>> m_MapWI;
	

public:
	size_t UserAniDataSize()
	{
		return m_UserAniData.size();
	}

	Game_String UserAniDataName(size_t _Index)
	{
		return m_UserAniData[_Index].AniName;
	}

public:
	Game_StaticFBX();
	~Game_StaticFBX();

public:
	void Load();
	void LoadUserFormat(const Game_String& _Path);
	void SaveUserFormat(const Game_String& _Path);

private:
	void AniCheck();
	void BoneCheck();
	void MaterialCheck(fbxsdk::FbxNode* _Node, int& _Count);
	void AnimationClipCheck(fbxsdk::FbxNode* _Node);

	void Trangulate(fbxsdk::FbxNode* _Node, int& _MatCount);
	void LoadBone(fbxsdk::FbxNode* _Node, int& _iIndex, int _iDepth = 0, int _iParent = -1);
	void LoadBoneCount(fbxsdk::FbxNode* _Node, int& _Count);

	void VtxData(fbxsdk::FbxMesh* _Mesh);
	void MeshVtxData(fbxsdk::FbxNode* _Node);

	void CalAnimationClip_Static(fbxsdk::FbxMesh* _Mesh);
	void CalWeightAndIndex();

	void LoadNormal(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index);
	void LoadUv(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index);
	void LoadTangent(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index);
	void LoadBinormal(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index);
	void LoadWeightAndIndex(fbxsdk::FbxCluster* _Cluster, int _BoneIndex, std::vector<VTX3DMESHIN>& _ArrVtx);
	void LoadOffsetMatrix(fbxsdk::FbxCluster* _Cluster, const fbxsdk::FbxAMatrix& _BaseTrans, LejFbxBoneData* _Bone);
	void LoadTimeTransForm(fbxsdk::FbxNode* _Node, fbxsdk::FbxCluster* _Cluster, const fbxsdk::FbxAMatrix& _BaseTrans, LejFbxBoneData* _Bone);

private:
	LejFbxBoneData* FindBone(const Game_String& _Name);

	/* 머테리얼 관련 함수 입니다. */
	Game_Vector MaterialColor(FbxSurfaceMaterial* pMtrl, const char* _ColorName, const char* _FactorName);
	float MaterialFactor(FbxSurfaceMaterial* pMtrl, const char* _FactorName);
	Game_String MaterialTex(FbxSurfaceMaterial* pMtrl, const char* _FactorName);

	/* static 입니다. */
	static HMATRIX FbxMatrixToLejMatrix(const fbxsdk::FbxAMatrix& _BaseTrans);
	static Game_Vector FbxVectorToLejVector(const fbxsdk::FbxVector4& _BaseVector);
	static Game_Vector FbxQuaterToLejVector(const fbxsdk::FbxQuaternion& _BaseQ);

private:
	int MeshCount;

public:
	void CreateMesh();
	void CreateStaticMesh(Game_Ptr<Game_Renderer> _NewRenderer, const Game_String& _Fbx);

};

