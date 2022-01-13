#pragma once

#include"GameObject.h"
#include"Character.h"

class Enemy : public Character
{
public: 
	CLASS_NAME(Enemy);

	Enemy();
	virtual ~Enemy() override { Release(); }

	void Init() override;
	void Update() override;
	void Draw2D()override;
	virtual bool IsAlive() override { return m_isAlive; }

	virtual void ImGuiUpdate() override;

	void SetPos(const Math::Vector3& pos) override { m_mWorld.Translation(pos); }
	void SetWPos(const Math::Vector3 pos) { m_worldPos.x += pos.x; m_worldPos.z += pos.z; m_worldPos.y = pos.y; }

	void SetTarget(std::shared_ptr<const GameObject> spTarget) { m_wpTarget = spTarget; }

	virtual void NotifyDamage(DamageArg& arg) override;

	classID GetClassID() const override{ return eEnemy; }

	void SetRotate(const Math::Vector3& r)
	{
		m_worldRot.x = r.x;
		m_worldRot.y = r.y;
		m_worldRot.z = r.z;
	}

	virtual void Deserialize(const json11::Json& json);
//	virtual void AfterDeserialize();
	virtual void Serialize(json11::Json::object& json);

private:

	void ScriptProc(const json11::Json& event)override;

	void Release();

	void UpdateMove();
	void UpdateRotate();
	void UpdateSearch();
	void UpdateCollition();		// 当たり判定の更新
	void DoAttack();
	void DoShot();

	Math::Vector3	m_worldPos;
	Math::Vector3	m_worldRot;
	Math::Vector3   m_prevPos;

	KdAnimator m_animator;

	KdAudioManager m_audioManager;

	std::weak_ptr<const GameObject> m_wpTarget;
	std::shared_ptr<BaseInput>		m_input;

	float m_stopDist = 1.01f;
	bool m_canAttack = true;
	bool m_attackFlg = false;
	int m_canAttackCnt = 30;
	bool m_findTargetFlg = false;
	float m_attackradius;
	bool m_sArmor = false;

	bool CheckWait()
	{
		
	}

	bool CheckAttack()
	{
		if (m_attackFlg)
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
		virtual void Entry(Enemy& owner) {};
		virtual void Update(Enemy& owner) = 0;
		virtual void Exit(Enemy& owner) {};
	};

	class ActionWait : public BaseAction
	{
	public:
		void Entry(Enemy& owner) { owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Idle")); };
		void Update(Enemy& owner) override;
	};

	class ActionMove : public BaseAction
	{
	public:
		void Entry(Enemy& owner) { owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Run")); }
		void Update(Enemy& owner) override;
	};

	class ActionAttack : public BaseAction
	{
	public:
		void Entry(Enemy& owner) {owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Attack"),false);}
		void Update(Enemy& owner) override;
		void Exit(Enemy& owner) { owner.m_canAttackCnt = 100; owner.m_attackFlg = false; }
	};

	class ActionElimination :public BaseAction
	{
	public:
		void Entry(Enemy& owner) { owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Die"), false); }
		void Update(Enemy& owner) override;
	};

	class ActionGetHit : public BaseAction
	{
	public:
		void Entry(Enemy& owner) { owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("GetHit"), false);}
		void Update(Enemy& owner) override;
		void Exit(Enemy& owner) { owner.m_canAttackCnt = 50; owner.m_attackFlg = false; }
	};

	std::shared_ptr<BaseAction> m_spActionState = nullptr;


	//class EnemyAIInput : public BaseInput
	//{
	//public:
	//	virtual void Update() override
	//	{
	//		if (1)
	//		{
	//			m_axisL.x = 1;
	//		}

	//		// 周囲を判定
	//		if (1)
	//		{
	//			//m_target = player;
	//		}

	//		// ステートベースAI
	//	}

	//	void PressButton(int index)
	//	{
	//		m_buttons[index] = 1;
	//	}

	//	void SetTarget(std::shared_ptr<const GameObject> spTarget) { m_wpTarget = spTarget; }
	//private:
	//	std::weak_ptr<const GameObject> m_wpTarget;

	//	SphereInfo viewSphere;

	//	Enemy* m_owner = nullptr;

	//	class AIState_Base
	//	{
	//	public:
	//		virtual void Update(EnemyAIInput& input) = 0;
	//	};
	//	class AIState_Idle : public AIState_Base
	//	{
	//	public:
	//		virtual void Update(EnemyAIInput& input)
	//		{
	//			if ( )
	//			{
	//				input.PressButton(0);
	//			}
	//		}
	//	};
	//};

};
