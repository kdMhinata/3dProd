#pragma once

#include "GameObject.h"

class Character;

class Effect2D :public GameObject
{
public:
	Effect2D() {}
	~Effect2D() {}

	void Init() override;
	void Update() override;
	void DrawEffect() override;

	void SetTexture(const std::shared_ptr<KdTexture> spTex
		, float w = 1.0f, float h = 1.0f, Math::Color col = kWhiteColor);

	void SetAnimation(int splitX, int splitY, float speed = 1.0f, bool isLoop = false);

	void SetBillboard(bool enablebillboard) { m_isBillboard = enablebillboard; }

	void SetLifeSpan(int lifeSpan) { m_lifeSpan = lifeSpan; }


	const KdSquarePolygon GetPolyData() const { return m_poly; }

	// 持ち主をセット
	void SetOwner(const std::shared_ptr<Character>& ownerChara)
	{
		m_ownerChara = ownerChara;
	}

	void SetLocalMode(bool enable) { m_localModeFlg = enable; }

private:

	KdSquarePolygon m_poly;

	float m_animSpd = 0.0f; //アニメーションの速度

	bool  m_isLoop = false;

	int m_lifeSpan = -1;

	std::weak_ptr<Character> m_ownerChara;

	bool m_isBillboard=false;

	bool m_localModeFlg = false;
};