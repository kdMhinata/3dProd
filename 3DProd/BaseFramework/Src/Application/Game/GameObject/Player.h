#pragma once

#include"GameObject.h"
#include "Character.h"
// 前方宣言
class Enemy;

class TPSCamera;

class Effect2D;

class Player : public Character
{
public:
	CLASS_NAME(Player)
	
	Player();
	virtual ~Player() override { Release(); };

	void Init() override;
	void Update() override;
	void Draw() override;
	void Draw2D()override;
	void DrawEffect()override;

	virtual void ImGuiUpdate() override;

	int GetHp()override { return m_hp; };

	virtual void NotifyDamage(DamageArg& arg) override;

	const std::shared_ptr<const TPSCamera> GetCamera() const { return m_spCamera; }	// 参照用
	std::shared_ptr<TPSCamera> WorkCamera() { return m_spCamera; }					// 変更用

	const Math::Vector3 GetPos() const override{ return m_worldPos; }

	classID GetClassID() const override { return ePlayer; }

	void SetInput(const std::shared_ptr<BaseInput>& input)
	{
		m_input = input;
	}

	void SetRotate(const Math::Vector3& r)
	{
		m_worldRot.x = r.x;
		m_worldRot.y = r.y;
		m_worldRot.z = r.z;
	}

	virtual void Deserialize(const json11::Json& json);

private:
	void ScriptProc(const json11::Json& event)override;

	void Release();		// 解放

	void UpdateMove(Math::Vector3& dstMove);
	void UpdateRotate(const Math::Vector3& srcMove);
	void UpdateMatrix();
	void DoAttack();

	void UpdateCollition();		// 当たり判定の更新

	void UpdateInput();

	Math::Vector3	m_worldPos;		// ワールド行列を作るための座標
	Math::Vector3	m_worldRot;		// ワールド行列を作るための回転
	Math::Vector3   m_prevPos;
	Math::Vector3 cameraMat;		//カメラImGuiでいじる為に
	Math::Vector3 cameraGazeMat;

	static const float s_limitOfStepHeight;

	std::shared_ptr<TPSCamera>		m_spCamera;
	std::shared_ptr<Effect2D> m_spShadow = nullptr;
	std::shared_ptr<Enemy>			m_enemy;

	KdModelWork        m_swordmodelWork;
	
	std::shared_ptr<BaseInput>		m_input;

	bool m_canAttack = true;
	bool m_atkComboFlg = false;
	Math::Vector3 m_force;

	std::string m_atkCancelAnimName = "";

	float m_effectValue = 0;

	bool CheckWait()
	{
		if (m_input->GetAxisL().LengthSquared() <= 0)
			return true; 
		return false;
	}

	bool CheckAttack()
	{
		if(m_canAttack)
		return true;
		return false;
	}

	template<class Type>
	void ChangeAction()
	{
		m_spActionState->Exit(*this);

		m_spActionState = std::make_shared<Type>();

		m_spActionState->Entry(*this);
	}

	class BaseAction
	{
	public:
		virtual void Entry(Player& owner) {};
		virtual void Update(Player& owner) = 0;
		virtual void Exit(Player& owner) {};
	};

	class ActionWait : public BaseAction
	{
	public:
		void Entry(Player& owner) { owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Idle")); };
		void Update(Player& owner) override;
	};

	class ActionMove : public BaseAction
	{
	public:
		void Entry(Player& owner){owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Run"));}
		void Update(Player& owner) override;
	};

	class ActionAttack : public BaseAction
	{
	public:
		void Entry(Player& owner)
		{
			owner.m_atkComboFlg = false;
		}
		void Update(Player& owner) override;
	};

	class ActionDodge : public BaseAction
	{
	public:
		void Entry(Player& owner) 
		{
			owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Dodge"), false);
			owner.invincibleFlg = true;

			Math::Vector3 dodgeVec = owner.m_mWorld.Backward();

			dodgeVec.Normalize();
			dodgeVec *= 0.3f;

			owner.m_force.x += dodgeVec.x;
			owner.m_force.z += dodgeVec.z;
		}
		void Update(Player& owner) override;
		void Exit(Player& owner) { owner.invincibleFlg = false; }
	};

	class ActionSkill : public BaseAction
	{
	public:
		void Entry(Player& owner) 
		{
			owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Thrust"), false);
	/*	Math::Vector3 skillVec = owner.m_mWorld.Backward();

		skillVec.Normalize();
		skillVec *= 0.5f;

		owner.m_force.x += skillVec.x;
		owner.m_force.z += skillVec.z;*/
		}
		void Update(Player& owner)override;
	};

	std::shared_ptr<BaseAction> m_spActionState = nullptr;

	KdAnimator m_animator;
	//オーディオ管理クラス
	KdAudioManager m_audioManager;

};

