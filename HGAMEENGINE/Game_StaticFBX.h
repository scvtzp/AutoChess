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
	Game_Vector Q;/*  ���⼭ ȸ���� ���ʹϿ� �Դϴ�. */
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

/* ���׸��� �����Դϴ�. */
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
	Game_String DifTexture;	/* �� ������ �Ϲ������� ���� ǥ���ϴ� �ؽ����Դϴ�. */
	Game_String BmpTexture;
	Game_String SpcTexture;
public:
	LejFbxMaterialData() {}
	~LejFbxMaterialData() {}
};
/*	�� -> �ִϸ��̼� ���޵Ǵ� ������ �Դϴ�. ������ ��� �⺻�����Դϴ�.  */
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
			AMSG(L"ó���Ҽ� ���� ������ ī��Ʈ ����Դϴ�.");
			break;
		}
		AMSG(L"ó���Ҽ� ���� ������ ī��Ʈ ����Դϴ�.");
		return -1;
	}
public:
	LejFbxAniData() {}
	~LejFbxAniData() {}
};

/* ���� ���� ȭ����� ��� ��ġ, �����ϰ� �ϴ� ��� ����ü �Դϴ�. */
class LejFbxBoneData
{
	/* ���� ��Ʈ�� �ֽ��ϴ�. Ʈ������ó�� �Ǿ� �ֽ��ϴ�. �θ��ڽ������ ��������ϴ�. ROOT�̸� 0�̰�, int�� ������� �Ű����ϴ�.  */
	/* iIndex�� ������ ���� �Դϴ�. m_UserBoneData�� �ε����Դϴ�.  */
public:
	Game_String	Name;
	int				iDepth;
	int				iIndex;
	/* �θ����(��)�� ���� ���� INDEX==49 �̸� �ڽ������ 49��° INDEX�� �ִ� ���(��)�� ������ �մϴ�.  */
	int				iParentIndex;
	HMATRIX		Offset;
	HMATRIX		Bone;
public:
	LejFbxBoneData() {}
	~LejFbxBoneData() {}
};
/* ��Ű�׿� �ʿ��� �����Դϴ�. */
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
	/* FBX�� ��ǥ��� Y�� Z�� �ٲ����� �Դϴ�.	*/
	/* ���䵥��ũ�� ��ü ��Ʈ���� �����߽��ϴ�. �����ӿ�ũ���� XMMAT���� ��ȯ�˴ϴ�. */
	fbxsdk::FbxAMatrix ReMat;
	/* fbx �ý����� ���� �Դϴ�. */
	fbxsdk::FbxManager* m_Mgr;
	/* FBX������ IO-SYSTEM�� �����մϴ�. */
	fbxsdk::FbxIOSettings* m_Ios;
	/* 3d�ƽ� �������� ���� �����մϴ�. */
	fbxsdk::FbxScene* m_Scene;
	/* FBX�� �ε��ϴ� �������� ��ü�Դϴ�. */
	fbxsdk::FbxImporter* m_Imp;
	/* �⺻ �������͵��� ���� �Դϴ�. */
	fbxsdk::FbxAxisSystem eLocalSpaceType;

public:
	/* �����ӿ�ũ ��Ĵ�� �����մϴ�. */
	/* �޽���������, �ε屸���� ���� �޶����ϴ�.  */
	/* ������ VB �ϳ��� IB ������ �Դϴ�. */
	std::vector<std::vector<VTX3DMESHIN>> m_FbxVtx;
	std::vector<std::vector<std::vector<UINT>>> m_FbxIdx;
	std::vector<Game_Ptr<HVBBUFFER>> m_VB;
	std::vector<std::vector<Game_Ptr<HIBBUFFER>>> m_IB;

	/* �޽������Դϴ�. 	*/
	std::vector<Game_Ptr<HMESH>> m_Mesh; //�� �ٽ��ڷ� 

/* ������ ���׸��� ���� �Դϴ�. ���������� �Բ� � ���(��)�� �������� ȭ���� �ش���ġ�� �̵��մϴ�.  */
	std::vector<LejFbxMaterialData> m_UserMaterialData;

	/* �ٽ����� �����͵� �Դϴ�. */
	std::vector<LejFbxBoneData> m_UserBoneData;
	std::vector<LejFbxAniData>	m_UserAniData;

	/* �̸����� �����մϴ�. */
	std::map<Game_String, LejFbxBoneData*>	m_BoneDataMap;
	std::map<Game_String, LejFbxAniData*>		m_UserAniDataMap;

	/* �ִϸ��̼� ������ ����ġ �� �ε����� ���� �����մϴ�. ROOT�� �������ִ� �޽��� ����մϴ�. Ư�� �κп��� ���� ������� �ʽ��ϴ�. */
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

	/* ���׸��� ���� �Լ� �Դϴ�. */
	Game_Vector MaterialColor(FbxSurfaceMaterial* pMtrl, const char* _ColorName, const char* _FactorName);
	float MaterialFactor(FbxSurfaceMaterial* pMtrl, const char* _FactorName);
	Game_String MaterialTex(FbxSurfaceMaterial* pMtrl, const char* _FactorName);

	/* static �Դϴ�. */
	static HMATRIX FbxMatrixToLejMatrix(const fbxsdk::FbxAMatrix& _BaseTrans);
	static Game_Vector FbxVectorToLejVector(const fbxsdk::FbxVector4& _BaseVector);
	static Game_Vector FbxQuaterToLejVector(const fbxsdk::FbxQuaternion& _BaseQ);

private:
	int MeshCount;

public:
	void CreateMesh();
	void CreateStaticMesh(Game_Ptr<Game_Renderer> _NewRenderer, const Game_String& _Fbx);

};

