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
	void Release();		// 解放

};

class DestuctibleBox : public StageMap
{
public :
	CLASS_NAME(DestuctibleBox)
	void Init()override
	{
		m_hp = 10;
		LoadModel("Data/Models/StageMap/Object/WoodBox.gltf");

		//AudioEngin初期化
		DirectX::AUDIO_ENGINE_FLAGS eflags =
			DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;
		m_audioManager.Init();
	}

	void Update()override
	{
		if (m_hp<=0)
		{
			m_alpha -= 0.01f;
			if (m_alpha <= 0.0f)
			{
				m_alpha = 0;
				m_isAlive = false;
			}
		}

		//サウンド関連の更新
			Math::Matrix listenerMat;
			m_audioManager.Update(listenerMat.Translation(), listenerMat.Backward());

		m_animator.AdvanceTime(m_modelWork.WorkNodes(), 1.0f);

		m_modelWork.CalcNodeMatrices();

	}

	virtual void NotifyDamage(DamageArg& arg) override
	{
		if (m_hp <= 0) { return; }

		m_audioManager.Play("Data/Audio/SE/hit1.wav");

		// モデル切り替え
		// アニメーション変更
		LoadModel("Data/Models/StageMap/Object/WoodBox_dest.gltf");
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

	Math::Vector3	m_worldPos;

	KdAnimator m_animator;

	KdAudioManager m_audioManager;
};