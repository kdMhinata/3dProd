#pragma once

#include"GameObject.h"
#include"Character.h"

class EnemyAIInput : public BaseInput
{
public:
	virtual void Update() override
	{
		if (1)
		{
			m_axisL.x = 1;
		}

		// 周囲を判定
		if (1)
		{
//			m_target = player;
		}

		// ステートベースAI
	}

private:
	std::weak_ptr<GameObject> m_target;
};

class Enemy : public Character
{
public: 
	CLASS_NAME(Enemy);

	Enemy(){}
	virtual ~Enemy() override { Release(); }

	void Init() override;
	void Update() override;
	void Draw2D()override;
	virtual bool IsAlive() override { return m_isAlive; }

	void SetPos(const Math::Vector3& pos) override { m_mWorld.Translation(pos); }
	void SetWPos(const Math::Vector3 pos) { m_worldPos.x += pos.x; m_worldPos.z += pos.z; m_worldPos.y = pos.y; }
	void SetMData(std::string filename) { m_modelWork.SetModel(GameResourceFactory.GetModelData(filename)); };
	void SetHP(int hp) { m_hp = hp; };
	void SetAttackRadius(float attackradius) { m_attackradius = attackradius; };
	void SetSuperArmor(bool sarmor) { m_sarmor = sarmor; };

	void SetTarget(std::shared_ptr<const GameObject> spTarget) { m_wpTarget = spTarget; }
	int GetHp()override { return m_hp; };

	virtual void NotifyDamage(DamageArg& arg) override;

	classID GetClassID() const override{ return eEnemy; }

private:

	void ScriptProc(const json11::Json& event)override;

	void Release();

	void UpdateMove();
	void UpdateRotate();
	void UpdateSearch();
	void UpdateCollition();		// 当たり判定の更新
	void DoAttack();

	Math::Vector3	m_worldPos;
	Math::Vector3	m_worldRot;
	Math::Vector3   m_prevPos;

	KdAnimator m_animator;

	KdAudioManager m_audioManager;

	std::weak_ptr<const GameObject> m_wpTarget;

	float m_stopDist = 1.01f;
	bool m_isAlive = true;
	bool m_canAttack = true;
	bool m_attackFlg = false;
	int m_canAttackCnt = 30;
	bool m_findTargetFlg = false;
	float m_attackradius;
	bool m_sarmor = false;




	bool CheckWait()
	{
		
	}

	bool CheckAttack()
	{
		if (m_attackFlg)
			return true;
		return false;
	}

	void ChangeWait()
	{
		m_spActionState->Exit(*this);
		m_spActionState = std::make_shared<ActionWait>();
		m_spActionState->Entry(*this);
	};
	void ChangeMove()
	{
		m_spActionState = std::make_shared<ActionMove>();
		m_spActionState->Entry(*this);
	};
	void ChangeAttack()
	{
		m_spActionState = std::make_shared<ActionAttack>();
		m_spActionState->Entry(*this);
	}
	void ChangeElimination()
	{
		m_spActionState = std::make_shared<ActionElimination>();
		m_spActionState->Entry(*this);
	}
	void ChangeGetHit()
	{
		m_spActionState = std::make_shared<ActionGetHit>();
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

};