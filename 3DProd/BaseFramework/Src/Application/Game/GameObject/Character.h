#pragma once

#include "GameObject.h"

struct DamageArg
{
	int damage = 0;
	bool ret_IsHit = true;
};

class Character : public GameObject
{
public:
	virtual void NotifyDamage(DamageArg& arg) {}
	virtual int GetHp() { return m_hp; }

	int m_hitStop = 0;
	int m_hp;

};