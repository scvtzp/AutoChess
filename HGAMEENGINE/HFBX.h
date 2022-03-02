#pragma once
#include "HGAMERES.h"
#include "Game_Device.h"
#include <DirectXTex.h>
#include "HSHADER.h"
#include <fbxsdk.h>
#include "HVTXSHADER.h"
#include "DEFVTXHEADER.h"
// #include "HRENDERTARGET" // 3d�� �ٽ�.

#pragma comment(lib, "zlib-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "libfbxsdk-md.lib")

class BoneFrameData
{
public:
	Game_Vector S; // ũ
	Game_Vector Q; // ��
	Game_Vector T; // ��
	double Time;
	HMATRIX FrameMat;
};

class BoneFrame
{
public:
	int BoneIndex;

	// ���� 100 ° �ִϸ��̼��� �Ѵٸ�
	// 1434                      
	// �����Ӻ� �� ������ ����ִ�.
	// 1434 ���߿��� 100�� 101��° �������� ���� ������ ���;� �Ѵ�.
	std::vector<BoneFrameData> m_Data;
};

class HFBXANIDATA
{
public:
	Game_String AniName;
	fbxsdk::FbxTime StartTime; // 0
	fbxsdk::FbxTime EndTime;   // 1434 / 30
	fbxsdk::FbxLongLong TimeStartCount; // 0
	fbxsdk::FbxLongLong TimeEndCount; // 1434
	fbxsdk::FbxLongLong FrameCount;
	fbxsdk::FbxTime::EMode TimeMode;
	int FbxModeCount;

	// 134 ���� ����
	// ������ �����ϴ� �ִϸ��̼� ������
	// m_AniFrameData[0].size() == 1434
	// m_AniFrameData[1].size() == 1434
	// m_AniFrameData[2].size() == 1434
	// m_AniFrameData[3].size() == 1434
	// m_AniFrameData[4].size() == 1434
	// m_AniFrameData[5].size() == 1434
	// m_AniFrameData[6].size() == 1434
	// m_AniFrameData[7].size() == 1434
	// m_AniFrameData[8].size() == 1434
	// m_AniFrameData[9].size() == 1434
	// ....
	// m_AniFrameData[134].size() == 1434

	// HMAT ArrAniMat[134]
	// 30 0.0333333333333 1
	// 30 1.0000000000000 0
	// 30 0.0234881
	// 1434
	// 0      1                     2                3
	// 0      0.03333333333333333        0.06666666666666666666
	// 0      1                          2

	// ArrAniMat[0] = m_AniFrameData[0][0] ������ �߰����� m_AniFrameData[0][1]
	// ArrAniMat[1] = m_AniFrameData[1][0]
	// ArrAniMat[2] = m_AniFrameData[2][0]
	// ArrAniMat[3] = m_AniFrameData[3][0]
	// ArrAniMat[4] = m_AniFrameData[4][0]
	// 134���ε�
	std::vector<BoneFrame> m_AniFrameData;

public:
	// 30������ �ִϸ��̼��̰�
	// 1434 �϶�
	// 30 == 1��

	// 600 �̸� ��ŸŸ������ ���ϱ�?
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
	HFBXANIDATA() {}
	~HFBXANIDATA() {}
};

class HFBXBONEDATA
{
public:
	Game_String Name;
	int			iDepth; // ��Ʈ�� 0 -> �¿�� 
	int			iIndex; // ���� �������� �������.
	int			iParentIndex; // ���θ� 49��° �ִ�.
	HMATRIX		Offset;
	HMATRIX		Bone;

public:
	HFBXBONEDATA() {}
	~HFBXBONEDATA() {}
};

class HFBXMATDATA
{
public:
	// �ȼ����̴����� �׳� �������� �����ָ� �Ǵ� ����.
	Game_Vector DifColor;
	Game_Vector AmbColor; 
	Game_Vector SpcColor; 
	Game_Vector EmvColor; 
	float SpecularPower;		
	float TransparencyFactor;	
	float Shininess;			
	Game_String DifTexture;	// ��ǻ�� 
	// DifTexture �� �༮�� �����е��� �Ϲ������� �˰� �ִ� �����̶�� ���� ǥ�����ִ� ��ǻ�� �ؽ�ó�̴�.
	Game_String BmpTexture; // ���� ���� ����
	Game_String SpcTexture; // ����ŧ�� ���� ����

public:
	HFBXMATDATA() {}
	~HFBXMATDATA() {}
};

class HFBXIW
{
public:
	int Index;
	double Weight;
};

// 134 
// 40 40 40 14 

class HMESH;
class HVBBUFFER;
class HIBBUFFER;
class HRENDERPIPELINE;
class HFBX : public HFILEMGR<HFBX>
{
public:
	static Game_Ptr<HFBX> Load(const HGAMEFILE& _File)
	{
		Game_Ptr<HFBX> Res = CreateToInsertToFile(_File.FullPath());
		Res->Load();
		return Res;
	}


	// 
	static void LoadToUserFormat(const HGAMEFILE& _File, const Game_String& _UserPath)
	{
		Game_Ptr<HFBX> Res = CreateToInsertToFile(_File.FullPath());
		Res->LoadUserFormat(_UserPath);
		return;
	}

private:
	// 1000
	// 0010
	// 0100
	// 0001

	fbxsdk::FbxAMatrix ReMat;
	fbxsdk::FbxManager* m_Mgr; // ���� �⺻�� �Ǵ� fbx �ý����� ����.
	fbxsdk::FbxIOSettings* m_Ios; // fbx ������ io�ý����� �����ϴ� �׸� fbx�� ��Ƽ �÷����̱� ������ �Ƹ� �̷��� �����ҰŶ� �����Ѵ�.
	fbxsdk::FbxScene* m_Scene; // 3d�ƽ� �������� ���� �⺻���� ���� �����ϴ� �༮.
	fbxsdk::FbxImporter* m_Imp; // �̳༮�� fbx�� �ε��ϴ� �������� ��ü
	fbxsdk::FbxAxisSystem eLocalSpaceType; // �⺻ ���������� �⺻���� ������ �˾Ƴ���.
	// 1 0 0 0
	// 0 1 0 0
	// 0 0 -1 0
	// 0 0 0 1

	// ���Ĵٵ� ���̺귯����?
	// std::string

public:

	// STATIC MESH
	// ���� �׷����� ����. �׷��� �ִٸ�.
	// �����׷��� ����    
	// �̳༮�� �������϶��� �ְ�
	std::vector<       std::vector<VTX3DMESHIN>          > m_FbxVtx; // ���ؽ�

	// �̰ɷ� ��������.
	// std::vector<std::vector<UINT>>
	// �׸��� �ε��� ���۰� 4����� ���� ������ 4���̴�.
	std::vector<       std::vector<std::vector<UINT>>    > m_FbxIdx; // �ε���

	// �� �����͵��� �츮 ������ ��ȯ���Ѿ� �ϱ�� ������.

	// �츮������ ��ȯ��Ų �༮��
	std::vector<Game_Ptr<HVBBUFFER>> m_VB;
	std::vector<std::vector<Game_Ptr<HIBBUFFER>>> m_IB;

	// ���׸���
	std::vector<HFBXMATDATA> m_UserMatData;
	// STATIC MESH END

	// ���� ������������ �ȴ�. �������������ο� ���� �ؽ�ó ���÷� �÷�
	// ��� ������ ��������.

	std::map<Game_String, HFBXANIDATA*> m_UserAniDataMap; // �ִϸ��̼� 

	std::map<Game_String, HFBXBONEDATA*> m_MapBoneData;



	std::vector<Game_Ptr<HMESH>> m_Mesh;
	std::vector<HFBXANIDATA> m_UserAniData; // �ִϸ��̼� 
	std::vector<HFBXBONEDATA> m_UserBoneData; // ��



	// �ִϸ��̼� ����ġ �� �ε��� ������ �ڷᱸ��.
	// ��񾲰� �Ⱦ���.
	// �̳༮�� �Ž��� ������ �������� ����� ���ε�. Root�� ������ �������� ����� ���ε�.
	// 
	std::vector<std::map<int, std::vector<HFBXIW>>> m_MapWI;

public:
	size_t UserAniDataSize()
	{
		return m_UserAniData.size();
	}

	Game_String UserAniDataName(size_t _Index)
	{
		return m_UserAniData[_Index].AniName;
	}


	// ��͵��� ����ؼ� ���� ����� ���̴�.
	// �ٽ� �����Ϸ���?
	// Ŭ������ �׳������Ҽ��� ����.

	// std::vector<std::vector<VTX3DMESHIN>> m_FbxVtx; // ���ؽ�
	// std::vector<std::vector<std::vector<UINT>>> m_FbxIdx; // �ε���
	// std::vector<HFBXBONEDATA> m_UserBoneData; // ��
	// std::vector<HFBXANIDATA> m_UserAniData; // �ִϸ��̼� 
	// std::vector<HFBXMATDATA> m_UserMatData;

public:
	HFBX();
	~HFBX();

public:
	void Load();
	void LoadUserFormat(const Game_String& _Path);
	void SaveUserFormat(const Game_String& _Path);

public:
	HFBXANIDATA* FindAniData(const Game_String& _Name)
	{
		if (m_UserAniDataMap.end() == m_UserAniDataMap.find(_Name))
		{
			return nullptr;
		}

		return m_UserAniDataMap[_Name];
	}

private:

	void AniCheck();
	void BoneCheck();
	void Trangulate(fbxsdk::FbxNode* _Node, int& _MatCount);
	// ���������������� ���鶧 �ʿ��� �����̴�.
	// ���� ���׸����̶�� �Ѵ�.
	// ���⼭ ������� ���׸����� ������ ����
	// ���������� ���������ִ� ����°����� �����ϴ�.


	// �� �����Լ�
	void LoadBoneCount(fbxsdk::FbxNode* _Node, int& _Count);
	void LoadBone(fbxsdk::FbxNode* _Node, int& _iIndex, int _iDepth = 0, int _iParent = -1);

	// ���� ���� �Լ�.
	void MeshVtxData(fbxsdk::FbxNode* _Node);
	void VtxData(fbxsdk::FbxMesh* _Mesh);

	int MeshCount;
	void AnimationClipCheck(fbxsdk::FbxNode* _Node);
	void CalAnimationClip(fbxsdk::FbxMesh* _Mesh);
	void CalWeightAndIndex();


	void LoadNormal(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index);
	void LoadUv(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index);
	void LoadTangent(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index);
	void LoadBinormal(fbxsdk::FbxMesh* _Mesh, std::vector<VTX3DMESHIN>& _ArrVtx, int VtxId, int _Index);


	void LoadWeightAndIndex(fbxsdk::FbxCluster* _Cluster, int _BoneIndex, std::vector<VTX3DMESHIN>& _ArrVtx);
	void LoadOffsetMatrix(fbxsdk::FbxCluster* _Cluster, const fbxsdk::FbxAMatrix& _BaseTrans, HFBXBONEDATA* _Bone);
	void LoadTimeTransForm(fbxsdk::FbxNode* _Node, fbxsdk::FbxCluster* _Cluster, const fbxsdk::FbxAMatrix& _BaseTrans, HFBXBONEDATA* _Bone);


	// std::map<Game_String, HFBXBONEDATA*> m_MapBoneData;

	HFBXBONEDATA* FindBone(const Game_String& _Name);

	// ���׸��� ���� �Լ�.
	void MaterialCheck(fbxsdk::FbxNode* _Node, int& _Count);
	Game_Vector MaterialColor(FbxSurfaceMaterial* pMtrl, const char* _ColorName, const char* _FactorName);
	float MaterialFactor(FbxSurfaceMaterial* pMtrl, const char* _FactorName);
	Game_String MaterialTex(FbxSurfaceMaterial* pMtrl, const char* _FactorName);


	static HMATRIX FbxMatToHMAT(const fbxsdk::FbxAMatrix& _BaseTrans);
	static Game_Vector FbxVecToHVEC(const fbxsdk::FbxVector4& _BaseVector);
	static Game_Vector FbxQToHVEC(const fbxsdk::FbxQuaternion& _BaseQ);


public:
	void CreateMesh();
};

