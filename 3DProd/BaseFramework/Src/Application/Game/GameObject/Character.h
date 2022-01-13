#pragma once

#include "GameObject.h"

struct DamageArg
{
	int damage = 0;
	bool ret_IsHit = true;
	Math::Vector3 attackPos = { 0,0,0 };
};

class Character : public GameObject
{
public:
	virtual void NotifyDamage(DamageArg& arg) {}
	virtual int GetHp() { return m_hp; }
	virtual int GetMaxHp() { return m_maxHp; }
	virtual void SetMaxHp(int maxhp) { m_maxHp = maxhp; }

	int m_hitStop = 0;
	int m_hp;
	int m_maxHp;
	bool invincibleFlg = false;

	std::shared_ptr<KdTexture> m_hpBarTex;
	std::shared_ptr<KdTexture> m_hpFrameTex;

	std::shared_ptr<KdTexture> m_damageFont;
private:
	virtual void ScriptProc(const json11::Json& event) {}
};