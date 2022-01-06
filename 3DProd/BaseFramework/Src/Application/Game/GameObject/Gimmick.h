#pragma once
#include "StageMap.h"
#include "Application/Game/GameSystem.h"

class Gimmick : public StageMap
{
public:
	CLASS_NAME(Gimmick)

	void Init()override
	{
		LoadModel("Data/Models/StageMap/Object/Door.gltf");
	}

	void Update()override;
	
	void Draw() override
	{
		SHADER->m_standardShader.SetAlpha(m_alpha);

		SHADER->m_standardShader.DrawModel(m_modelWork, m_mWorld);

		SHADER->m_standardShader.SetAlpha(1.0f);
	}

	// 復元
	virtual void Deserialize(const json11::Json& json) override
	{
		Character::Deserialize(json);
		m_alpha = 0.5;
	}

	// 文字列化
	virtual void Serialize(json11::Json::object& json) override
	{
		Character::Serialize(json);
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

				if (m_clearFlg)
				{
					m_active++;
				}
				else
				{
					result.m_pushVec += localPushedPos - info.m_pos;
				}
			}
		}

		if (!result.m_isHit)
		{
			m_active =0;
		}

		return result.m_isHit;
	}

	classID GetClassID() const override { return eGimmick; }

private:
	int m_active=0;
	bool m_clearFlg = false;
};