#include"StageMap.h"

void StageMap::Init()
{
	m_wallModel.SetModel(GameResourceFactory.GetModelData("Data/Models/StageMap/uploads_files_2941209_ModularDungeon.gltf"));

	//m_floorModel.SetModel(GameResourceFactory.GetModelData("Data/Models/StageMap/Floor/floor.gltf"));
}

void StageMap::Draw()
{
	SHADER->m_standardShader.DrawModel(m_floorModel, m_mWorld);

	const KdModelWork::Node* pNode = m_floorModel.FindNode("front");

	/*if (pNode)
	{
		const KdModelData::Node& dataNode = m_wallModel.GetDataNodes()[0];
		if (dataNode.m_spMesh != nullptr) {

			DirectX::SimpleMath::Matrix trans = m_mWorld * pNode->m_worldTransform;
			dataNode.m_worldTransform._41 = trans._41;
		}
	}*/

	SHADER->m_standardShader.DrawModel(m_wallModel, m_mWorld);
}

void StageMap::Release()
{
}
