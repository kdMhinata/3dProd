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
	void Release();		// 解放

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
		// モデル切り替え
		// アニメーション変更

		m_hp -= arg.damage;
		arg.ret_IsHit = true;
	}

	virtual void ImGuiUpdate()override
	{
		StageMap::ImGuiUpdate();

		ImGui::DragInt("Hp", &m_hp, 1.0f, 0, 200);
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

	classID GetClassID() const override { return eDestuctible; }

	bool CheckCollisionBump (const SphereInfo& info, BumpResult& result)override
	{
		Math::Vector3 selfPos = GetPos() + m_bumpSphereInfo.m_pos;

		Math::Vector3 distVec = info.m_pos - selfPos;

		// 2点間の距離
		float distanceSqr = distVec.LengthSquared();

		// 当たり判定の半径合計
		float hitRadius = m_bumpSphereInfo.m_radius + info.m_radius;

		// 判定の結果
		result.m_isHit = (distanceSqr <= (hitRadius * hitRadius));

		if (result.m_isHit)
		{
			// 押し戻し
			result.m_pushVec = distVec;
			result.m_pushVec.Normalize();

			float distance = std::sqrt(distanceSqr);

			// 押し戻すベクトル×押し戻す値
			result.m_pushVec *= hitRadius - distance;
		}
		return result.m_isHit;
	}

private:
	void ScriptProc(const json11::Json& event)override
	{

	}
};