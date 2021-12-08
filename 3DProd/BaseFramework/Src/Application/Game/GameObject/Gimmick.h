#pragma once
#include "StageMap.h"
#include "Application/Game/GameSystem.h"

class Gimmick : public StageMap
{
public:
	CLASS_NAME(Gimmick)

	void Init()override
	{
		m_activeFlg = false;
		LoadModel("Data/Models/StageMap/Object/Door.gltf");
	}

	void Update()override;
	

	void ImGuiUpdate()override
	{
		StageMap::ImGuiUpdate();
		ImGui::Checkbox("Active", &m_activeFlg);
		ImGui::DragFloat3("Pos", &m_worldPos.x, 0.01f);
		m_mWorld = Math::Matrix::CreateTranslation(m_worldPos);
	}

	// 復元
	virtual void Deserialize(const json11::Json& json) override
	{
		Character::Deserialize(json);

		// 
		m_activeFlg = json["Active"].bool_value();
	}

	// 文字列化
	virtual void Serialize(json11::Json::object& json) override
	{
		Character::Serialize(json);

		// 自分の内容
		json["Active"] = m_activeFlg;
	}

	bool CheckCollisionBump(const SphereInfo& info, BumpResult& result)override
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

				m_activeFlg = true;
			}
		}

		if (!result.m_isHit)
		{
			m_activeFlg = false;
		}

		return result.m_isHit;
	}

	bool IsActive()
	{
		return m_activeFlg;
	}

	classID GetClassID() const override { return eGimmick; }

private:
	bool m_activeFlg;

	Math::Vector3	m_worldPos;
};