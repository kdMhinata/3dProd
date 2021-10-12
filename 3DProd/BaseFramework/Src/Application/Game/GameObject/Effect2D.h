#pragma once

#include "GameObject.h"

class Effect2D :public GameObject
{
public:
	Effect2D() {}
	~Effect2D() {}

	void Init() override;
	void Update() override;
	void Draw() override;
	void DrawEffect() override;

	void SetTexture(const std::shared_ptr<KdTexture> spTex
		, float w = 1.0f, float h = 1.0f, Math::Color col = kWhiteColor);

	void SetAnimation(int splitX, int splitY, float speed = 1.0f, bool isLoop = false);

	void SetLifeSpan(int lifeSpan) { m_lifeSpan = lifeSpan; }

private:

	KdSquarePolygon m_poly;

	float m_animSpd = 0.0f; //アニメーションの速度

	bool  m_isLoop = false;

	int m_lifeSpan = -1;
};