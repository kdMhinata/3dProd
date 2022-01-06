#pragma once

#include"GameObject.h"
#include"Character.h"

class Bullet : public Character
{
public:
	Bullet() {}
	virtual ~Bullet() override { Release(); }

	void Init() override;
	void Update() override;

	void SetWorldMatrix(const Math::Matrix& world) { m_mWorld = world; }

private:

	void Release();

	void UpdateCollition();	// 当たり判定の更新

	int		m_lifeSpan = 0;		// 生存時間

	bool	m_isStabbed = false;

	Math::Vector3		m_prevPos;	// 1フレーム前の座標格納用

	KdTrailPolygon m_trail;
};
