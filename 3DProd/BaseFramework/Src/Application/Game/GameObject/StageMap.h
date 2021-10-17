#pragma once

#include"GameObject.h"

class StageMap : public GameObject
{
public:
	StageMap() {}
	~StageMap() { Release(); }

	void Init() override;
	bool CheckCollisionBump(const SphereInfo& info, BumpResult& result)override;

	classID GetClassID() const override { return eStage; }

private:
	void Release();		// ‰ð•ú
};
