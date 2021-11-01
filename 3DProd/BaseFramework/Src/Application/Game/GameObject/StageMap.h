#pragma once

#include"GameObject.h"
#include "Character.h"

class StageMap : public Character
{
public:
	StageMap() {}
	~StageMap() { Release(); }

	void Init() override;

	virtual void ImGuiUpdate() override;

	bool CheckCollisionBump(const SphereInfo& info, BumpResult& result)override;

	classID GetClassID() const override { return eStage; }


private:
	void Release();		// ���

};

class DestuctibleBox : public StageMap
{
public :
	virtual void NotifyDamage(DamageArg& arg) override
	{
		// ���f���؂�ւ�
		// �A�j���[�V�����ύX
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

private:
	int m_hp = 100;
};