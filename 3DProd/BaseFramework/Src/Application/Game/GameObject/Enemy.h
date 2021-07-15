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

	classID GetClassID() const override{ return eEnemy; }

private:

	void Release();

	void UpdateMove();
	void UpdateRotate();

	Math::Vector3	m_worldPos;
	Math::Vector3	m_worldRot;

	std::weak_ptr<const GameObject> m_wpTarget;

	float m_stopDist = 5.0f;

};