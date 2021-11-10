#include"StageMap.h"

void StageMap::Init()
{
//	m_modelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/StageMap/Dungeon/Dungeon1/DungeonStage.gltf"));
	LoadModel("Data/Models/StageMap/Dungeon/Dungeon1/DungeonStage.gltf");
}

void StageMap::ImGuiUpdate()
{
	GameObject::ImGuiUpdate();

	if (ImGui::Button("Load Model"))
	{
		std::string path;
		if (KdWindow::OpenFileDialog(path))
		{
			LoadModel(path);
		}
	}
}

bool StageMap::CheckCollisionBump(const SphereInfo& info, BumpResult& result)
{
	for (UINT i = 0; i < m_modelWork.GetDataNodes().size(); ++i)
	{
		const KdModelData::Node& dataNode = m_modelWork.GetDataNodes()[i];

		// ���b�V�����Ȃ��ꍇ��΂�
		if (!dataNode.m_spMesh) { continue; }

		const KdModelWork::Node& workNode = m_modelWork.GetNodes()[i];

		// �����Ԃ��ꂽ���W
		Math::Vector3 localPushedPos;

		// ���b�V���Ƌ��Ƃ̓����蔻��
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