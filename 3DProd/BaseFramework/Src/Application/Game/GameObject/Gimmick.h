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

	bool CheckCollisionBump(const SphereInfo& info, BumpResult& result)override;

	classID GetClassID() const override { return eGimmick; }

private:
	int m_active=0;
	bool m_clearFlg = false;

	std::vector<std::weak_ptr<GameObject>> m_enemies;
};