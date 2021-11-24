#pragma once

#include"GameObject.h"
#include "Character.h"

class StageMap : public Character
{
public:
	CLASS_NAME(StageMap)

	StageMap() {}
	~StageMap() { Release(); }

	void Init() override;

	virtual void ImGuiUpdate() override;

	virtual bool CheckCollisionBump(const SphereInfo& info, BumpResult& result)override;

	classID GetClassID() const override { return eStage; }


private:
	void Release();		// ���

};

class Gimmick : public StageMap
{
public:
	CLASS_NAME(Gimmick)
	void Init()override
	{
		m_activeFlg = false;
		LoadModel("Data/Models/StageMap/Object/Door.gltf");
	}

	void ImGuiUpdate()override
	{
		StageMap::ImGuiUpdate();
		ImGui::Checkbox("Active", &m_activeFlg);
		ImGui::DragFloat3("Pos",&m_pos.x,0.01f);
		m_mWorld = Math::Matrix::CreateTranslation(m_pos);
	}

	// ����
	virtual void Deserialize(const json11::Json& json) override
	{
		Character::Deserialize(json);

		// 
		m_activeFlg = json["Active"].bool_value();
	}

	// ������
	virtual void Serialize(json11::Json::object& json) override
	{
		Character::Serialize(json);

		// �����̓��e
		json["Active"] = m_activeFlg;
	}

	bool CheckCollisionBump(const SphereInfo& info, BumpResult& result)override
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

	classID GetClassID() const override { return eStage; }

private:
	bool m_activeFlg;

	Math::Vector3 m_pos;
};

class DestuctibleBox : public StageMap
{
public :
	CLASS_NAME(DestuctibleBox)
	void Init()override
	{
		m_hp = 10;
		LoadModel("Data/Models/StageMap/Object/WoodBox.gltf");
	}

	void Update()override
	{
		if (m_hp <= 0)
		{
			m_isAlive = false;
		}
	}

	virtual void NotifyDamage(DamageArg& arg) override
	{
		// ���f���؂�ւ�
		// �A�j���[�V�����ύX

		m_hp -= arg.damage;
		arg.ret_IsHit = true;
	}

	virtual void ImGuiUpdate()override
	{
		StageMap::ImGuiUpdate();

		ImGui::DragInt("Hp", &m_hp, 1.0f, 0, 200);
	}

	// ����
	virtual void Deserialize(const json11::Json& json) override
	{
		Character::Deserialize(json);

		// 
		m_hp = json["Hp"].int_value();
	}

	// ������
	virtual void Serialize(json11::Json::object& json) override
	{
		Character::Serialize(json);

		// �����̓��e
		json["Hp"] = m_hp;
	}

	classID GetClassID() const override { return eDestuctible; }

	bool CheckCollisionBump (const SphereInfo& info, BumpResult& result)override
	{
		Math::Vector3 selfPos = GetPos() + m_bumpSphereInfo.m_pos;

		Math::Vector3 distVec = info.m_pos - selfPos;

		// 2�_�Ԃ̋���
		float distanceSqr = distVec.LengthSquared();

		// �����蔻��̔��a���v
		float hitRadius = m_bumpSphereInfo.m_radius + info.m_radius;

		// ����̌���
		result.m_isHit = (distanceSqr <= (hitRadius * hitRadius));

		if (result.m_isHit)
		{
			// �����߂�
			result.m_pushVec = distVec;
			result.m_pushVec.Normalize();

			float distance = std::sqrt(distanceSqr);

			// �����߂��x�N�g���~�����߂��l
			result.m_pushVec *= hitRadius - distance;
		}
		return result.m_isHit;
	}

private:
	void ScriptProc(const json11::Json& event)override
	{

	}
};