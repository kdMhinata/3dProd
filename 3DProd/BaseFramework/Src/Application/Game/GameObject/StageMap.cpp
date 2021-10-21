#include"StageMap.h"

void StageMap::Init()
{
	m_modelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/StageMap/DungeonStage.gltf"));

	//m_floorModel.SetModel(GameResourceFactory.GetModelData("Data/Models/StageMap/Floor/floor.gltf"));
}

bool StageMap::CheckCollisionBump(const SphereInfo& info, BumpResult& result)
{
	for (UINT i = 0; i < m_modelWork.GetDataNodes().size(); ++i)
	{
		const KdModelData::Node& dataNode = m_modelWork.GetDataNodes()[i];

		// メッシュがない場合飛ばす
		if (!dataNode.m_spMesh) { continue; }

		const KdModelWork::Node& workNode = m_modelWork.GetNodes()[i];

		// 押し返された座標
		Math::Vector3 localPushedPos;

		// メッシュと球との当たり判定
		if (KdSphereToMesh(info.m_pos, info.m_radius, *(dataNode.m_spMesh.get()),
			workNode.m_worldTransform * m_mWorld, localPushedPos))
		{
			result.m_isHit = true;

			result.m_pushVec += localPushedPos - info.m_pos;
		}
	}

	return result.m_isHit;
}


void StageMap::Release()
{
}
