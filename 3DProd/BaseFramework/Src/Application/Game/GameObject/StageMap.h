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
	void Release();		// 解放

};

class DestuctibleBox : public StageMap
{
public :
	virtual void NotifyDamage(DamageArg& arg) override
	{
		// モデル切り替え
		// アニメーション変更
	}

	// 復元
	virtual void Deserialize(const json11::Json& json) override
	{
		Character::Deserialize(json);

		// 
		m_hp = json["Hp"].int_value();
	}

	// 文字列化
	virtual void Serialize(json11::Json::object& json) override
	{
		Character::Serialize(json);

		// 自分の内容
		json["Hp"] = m_hp;
	}

private:
	int m_hp = 100;
};