#pragma once

#include"GameObject.h"
#include "Character.h"
// �O���錾
class Enemy;

class TPSCamera;

class Player : public Character
{
public:
	Player();
	virtual ~Player() override { Release(); };

	void Init() override;
	void Update() override;

	int GetHp() { return m_hp; };

	virtual void NotifyDamage(DamageArg& arg) override;

	const std::shared_ptr<const TPSCamera> GetCamera() const { return m_spCamera; }	// �Q�Ɨp
	std::shared_ptr<TPSCamera> WorkCamera() { return m_spCamera; }				// �ύX�p

	const Math::Vector3 GetPos() const override{ return m_worldPos; }

	classID GetClassID() const override { return ePlayer; }

	// 
	void SetInput(const std::shared_ptr<BaseInput>& input)
	{
		m_input = input;
	}

private:
	void ScriptProc(const json11::Json& event);

	void Release();		// ���

	void UpdateMove(Math::Vector3& dstMove);
	void UpdateRotate(const Math::Vector3& srcMove);
	void UpdateMatrix();
	void DoAttack();

	void UpdateCollition();		// �����蔻��̍X�V

	void UpdateInput();
//	Math::Vector2 m_axisL;

	Math::Vector3	m_worldPos;		// ���[���h�s�����邽�߂̍��W
	Math::Vector3	m_worldRot;		// ���[���h�s�����邽�߂̉�]

	std::shared_ptr<TPSCamera>		m_spCamera;
	std::shared_ptr<Enemy>			m_enemy;
	
	std::shared_ptr<BaseInput>		m_input;

	int m_hp = 50;
	bool m_canAttack = true;
	bool m_atkComboFlg = false;

	std::string m_atkCancelAnimName = "";

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
	}
	void ChangeDodge()
	{
		m_spActionState = std::make_shared<ActionDodge>();
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
		void Update(Player& owner) override;
	};

	std::shared_ptr<BaseAction> m_spActionState = nullptr;


	KdAnimator m_animator;

	//�I�[�f�B�I�Ǘ��N���X
	KdAudioManager m_audioManager;

};

