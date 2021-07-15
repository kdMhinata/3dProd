#include"StageMap.h"

void StageMap::Init()
{
	m_modelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/StageMap/StageMap.gltf"));
}

void StageMap::Release()
{
}
