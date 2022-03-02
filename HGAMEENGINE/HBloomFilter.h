#pragma once
#include "HCAM.h"
#include "HRenderTarget.h"
#include <HPTR.h>

class Game_RenderTarget;
class HBloomFilter : public HFilter
{
public:
	Game_Ptr<HRENDERPLAYER> BloomCheckPlayer;
	Game_Ptr<Game_RenderTarget> SmallTarget;
	Game_Ptr<Game_RenderTarget> SmallTarget2;


	float4 SmallUvSize;
	Game_Ptr<HRENDERPLAYER> BloomPlayer;
	

public:
	void Init(int _Len);

private:
	void DefferdMergePrevEffect() override;
	void CamMergeNextEffect() override;
};

