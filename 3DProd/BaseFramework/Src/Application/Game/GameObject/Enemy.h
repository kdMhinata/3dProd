#pragma once

#include"GameObject.h"

class Enemy : public GameObject
{
public: 
	Enemy(){}
	virtual ~Enemy() override { Release(); }

	void Init() override;
	void Update() override;

	void SetTarget(std::shared_ptr<const GameObject> spTarget) { m_wpTarget = spTarget; }
	void SetHp(int hp) { m_hp = hp; };
	int GetHp() { return m_hp; };

	classID GetClassID() const override{ return eEnemy; }

private:

	void Release();

	void UpdateMove();
	void UpdateRotate();

	Math::Vector3	m_worldPos;
	Math::Vector3	m_worldRot;

	KdAnimator m_animator;

	std::weak_ptr<const GameObject> m_wpTarget;

	float m_stopDist = 1.01f;
	int m_hp=100;
	bool m_isAlive = true;

};