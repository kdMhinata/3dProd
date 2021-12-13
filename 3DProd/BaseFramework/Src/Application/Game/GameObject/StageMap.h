#pragma once

#include"GameObject.h"
#include "Character.h"
#include "Application/Game/GameSystem.h"
#include "Player.h"

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

class DestuctibleBox : public StageMap
{
public :
	CLASS_NAME(DestuctibleBox)
	void Init()override
	{
		m_hp = 100;
		LoadModel("Data/Models/StageMap/Object/WoodBox_dest.gltf");
	}

	void Update()override
	{
		if (m_hp <= 0)
		{
			m_isAlive = false;
		}

		m_animator.AdvanceTime(m_modelWork.WorkNodes(), 1.0f);

		m_modelWork.CalcNodeMatrices();
	}

	virtual void NotifyDamage(DamageArg& arg) override
	{
		// ���f���؂�ւ�
		// �A�j���[�V�����ύX

		m_animator.SetAnimation(m_modelWork.GetData()->GetAnimation("Dest"),false);

		m_hp -= arg.damage;
		arg.ret_IsHit = true;
	}

	virtual void ImGuiUpdate()override
	{
		StageMap::ImGuiUpdate();

		ImGui::DragInt("Hp", &m_hp, 1.0f, 0, 200);

		ImGui::DragFloat3("Pos", &m_worldPos.x, 0.01f);
		m_mWorld = Math::Matrix::CreateTranslation(m_worldPos);

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

	Math::Vector3	m_worldPos;

	KdAnimator m_animator;
};