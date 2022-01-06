#include "Bullet.h"

void Bullet::Init()
{
	LoadModel("Data/Models/enemy/slime.gltf");

	m_lifeSpan = 120;

	m_bumpSphereInfo.m_pos.y = 0.45f;
	m_bumpSphereInfo.m_radius = 0.3f;
	
	m_name = "Bullet";
}

void Bullet::Update()
{
	m_lifeSpan--;

	if (m_lifeSpan < 0)
	{
		m_isAlive = false;
	}

	m_prevPos = m_mWorld.Translation();

	Math::Vector3 vMove = m_mWorld.Backward();

	vMove.Normalize();
	const float bulletSpd = 0.1f;
	vMove *= bulletSpd;

	//ワールド行列の作成
	Math::Matrix trans;
	trans = Math::Matrix::CreateTranslation(vMove);

	//行列の作成
	m_mWorld *= trans; //前フレームの情報に重ねて合成

	//当たり判定の更新
	UpdateCollition();
}

void Bullet::Release()
{
}

void Bullet::UpdateCollition()
{	
	for (const std::shared_ptr<GameObject>& spStageObj : GameInstance.GetObjects())
	{
		if (spStageObj->GetClassID() != GameObject::eStage) { continue; }

		BumpResult result;

		//壁判定
		SphereInfo sphereInfo(GetPos() + m_bumpSphereInfo.m_pos, m_bumpSphereInfo.m_radius - 0.2);

		if (spStageObj->CheckCollisionBump(sphereInfo, result))
		{
			m_isAlive = false;
		}
	}

	for (const std::shared_ptr<GameObject>& spObj : GameInstance.GetObjects())
	{
		if (spObj->GetClassID()!=GameObject::ePlayer) { continue; }

		SphereInfo sphereInfo(GetPos(), m_bumpSphereInfo.m_radius);

		BumpResult result;

		//相手の判定関数を利用する
		if (spObj->CheckCollisionBump(sphereInfo, result))
		{
			auto chara = std::dynamic_pointer_cast<Character>(spObj);

			if (chara == nullptr) { continue; }
			DamageArg arg;
			arg.damage = 10;
			arg.attackPos = GetPos();
			chara->NotifyDamage(arg);
			
			//ヒット時行う処理
			if (arg.ret_IsHit)
			{
				m_isAlive = false;
			}
		}
	}

}
