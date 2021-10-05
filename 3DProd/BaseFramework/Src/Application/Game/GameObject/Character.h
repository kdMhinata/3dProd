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
};