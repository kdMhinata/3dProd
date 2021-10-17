#include"Enemy.h"
#include"Player.h"
#include "Effect2D.h"

void Enemy::Init()
{
	m_modelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/enemy/slime.gltf"));
	
	m_radius = 0.5f;

	m_animator.SetAnimation(m_modelWork.GetData()->GetAnimation("Run"));

	m_spActionState = std::make_shared<ActionMove>();

	m_worldPos = { 0.0,0.0,5.0 };

	//AudioEngin初期化
	DirectX::AUDIO_ENGINE_FLAGS eflags =
		DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;
	m_audioManager.Init();
}

void Enemy::Update()
{
	if (m_spActionState)
	{
		m_spActionState->Update(*this);
	}

	// ワールド行列生成
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_worldPos);

	Math::Matrix rotation = Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_worldRot.y),
		DirectX::XMConvertToRadians(m_worldRot.x),
		DirectX::XMConvertToRadians(m_worldRot.z)
	);

	m_mWorld = rotation * trans;

	m_animator.AdvanceTime(m_modelWork.WorkNodes());
	m_modelWork.CalcNodeMatrices();

	m_animator.AdvanceTime(m_modelWork.WorkNodes(), 1.0f,
		std::bind(&Enemy::ScriptProc, this, std::placeholders::_1)
	);

	m_modelWork.CalcNodeMatrices();

	//サウンド関連の更新
	{
		Math::Matrix listenerMat;
		m_audioManager.Update(listenerMat.Translation(), listenerMat.Backward());
	}
}

void Enemy::NotifyDamage(DamageArg& arg)
{
	m_hp -= arg.damage;
	arg.ret_IsHit = true;

	if (arg.ret_IsHit)
	{
		ChangeGetHit();
	}
}

void Enemy::ScriptProc(const json11::Json& event)
{
	std::string eventName = event["EventName"].string_value();

	if (eventName == "PlaySound")
	{
		const std::string& soundFile = event["SoundName"].string_value();
		m_audioManager.Play(soundFile);
	}
	else if (eventName == "DoAttack")
	{
		DoAttack();
	}
	else if (eventName == "Elim")
	{
		m_isAlive = false;
	}
	else if (eventName == "End")
	{
		ChangeWait();
	}
}

void Enemy::Release()
{
	// 取得したm_pTargetの中身をを解放
	SetTarget(nullptr);

	m_audioManager.StopAllSound();
}

void Enemy::UpdateMove()
{
	// 見ている先が解放されているか
	if (m_wpTarget.expired()) { return; }

	std::shared_ptr<const GameObject> spTarget = m_wpTarget.lock();

	// プレイヤーに向かう方向ベクトル
	Math::Vector3 targetDir = spTarget->GetPos() - m_worldPos;

	// ターゲットとの距離
	float targetDistSqr = targetDir.LengthSquared();

	//攻撃処理(仮)
	float attackRange= 2.00f;
	m_canAttackCnt--;
	if (m_canAttackCnt <= 0) { m_canAttack = true; }

	if (targetDistSqr < attackRange * attackRange && m_canAttack) { m_attackFlg = true; m_canAttack = false; }
	if (targetDistSqr < m_stopDist * m_stopDist) { return; }
	
	// エネミーの向いてる方向ベクトル
	Math::Vector3 moveVec = m_mWorld.Backward();
	moveVec.Normalize();
	
	const float moveSpd = 0.03f;
	moveVec *= moveSpd;

	m_worldPos += moveVec;

}

void Enemy::UpdateRotate()
{
	// 見ている先が解放されているか
	if (m_wpTarget.expired()) { return; }

	std::shared_ptr<const GameObject> spTarget = m_wpTarget.lock();

	// 向いてる方向ベクトル
	Math::Vector3 nowDir = m_mWorld.Backward();
	
	// 移動方向のベクトル(プレイヤーの座標)
	Math::Vector3 targetDir = spTarget->GetPos() - m_worldPos;
	
	// 長さを1に
	nowDir.Normalize();
	targetDir.Normalize();

	// それぞれの現在の角度計算
	float nowAngle = atan2(nowDir.x, nowDir.z);
	nowAngle = DirectX::XMConvertToDegrees(nowAngle);
	float targetAngle = atan2(targetDir.x, targetDir.z);
	targetAngle = DirectX::XMConvertToDegrees(targetAngle);

	// 二点の間の角度を求める(目的地 - 出発地)
	float betweenAng = targetAngle - nowAngle;

	// 回転の補正
	if (betweenAng > 180)
	{
		betweenAng -= 360;
	}
	else if (betweenAng < -180)
	{
		betweenAng += 360;
	}

	// 一フレーム当たりの回転量の制御
	float rotSpd = 10.0f;	// ここの角度によって振り向き速度が変わる
	float rotateAng = std::clamp(betweenAng, -rotSpd, rotSpd);

	// 角度更新
	m_worldRot.y += rotateAng;
}

void Enemy::DoAttack()
{
	for (const std::shared_ptr<GameObject>& spObj : GameSystem::GetInstance().GetObjects())
	{
		if (spObj->GetClassID() != GameObject::ePlayer) { continue; }

		Math::Vector3 attackPos = GetPos();
		attackPos += (m_mWorld.Backward() * 1);

		SphereInfo info(attackPos, m_radius + 0.05f);

		BumpResult result;

		// 相手の判定関数を利用する
		if (spObj->CheckCollisionBump(info, result))
		{
			auto chara = std::dynamic_pointer_cast<Character>(spObj);

			if (chara == nullptr) { continue; }
			DamageArg arg;
			arg.damage = 10;
			arg.attackPos = attackPos;
			chara->NotifyDamage(arg);

			if (arg.ret_IsHit)
			{
				//ヒット時行う処理
				//爆発
				std::shared_ptr<Effect2D> spEffect = std::make_shared<Effect2D>();

				Math::Vector3 effectPos = (attackPos += (m_mWorld.Up() * 0.5));

				spEffect->Init();
				spEffect->SetAnimation(5, 5);
				spEffect->SetPos(effectPos);
				spEffect->SetTexture(GameResourceFactory.GetTexture("Data/Textures/Explosion.png"));

				GameInstance.AddObject(spEffect);
			}
		}
	}
}

void Enemy::ActionWait::Update(Enemy& owner)
{
	owner.ChangeMove();
}

void Enemy::ActionMove::Update(Enemy& owner)
{
	if (owner.m_attackFlg)
	{
		owner.ChangeAttack();
	}

	if (owner.m_hp <= 0)
	{
		owner.ChangeElimination();
	}
		// 回転の更新処理
		owner.UpdateRotate();
		// 移動の更新処理
		owner.UpdateMove();
	
}

void Enemy::ActionAttack::Update(Enemy& owner)
{

}

void Enemy::ActionElimination ::Update(Enemy& owner)
{
}

void Enemy::ActionGetHit::Update(Enemy& owner)
{
	std::shared_ptr<const GameObject> spTarget = owner.m_wpTarget.lock();

	// キャラの正面方向ベクトル：出発地
	Math::Vector3 nowDir = owner.m_mWorld.Backward();

	// ノックバック時向く方向のベクトル：攻撃座標
	Math::Vector3 targetDir = spTarget->GetPos() - owner.m_worldPos;

	nowDir.Normalize();
	targetDir.Normalize();

	// それぞれのDegree角度を求める
	float nowAng = atan2(nowDir.x, nowDir.z);
	nowAng = DirectX::XMConvertToDegrees(nowAng);

	float targetAng = atan2(targetDir.x, targetDir.z);
	targetAng = DirectX::XMConvertToDegrees(targetAng);

	// ２つの間の角度を求める
	float rotateAng = targetAng - nowAng;

	// 回転量代入
	rotateAng = std::clamp(rotateAng, -20.0f, 20.0f);
	owner.m_worldRot.y += rotateAng;

	Math::Vector3 knockBackVec = owner.m_mWorld.Forward();

	knockBackVec.Normalize();
	knockBackVec *= 0.1f;

	owner.m_worldPos.x += knockBackVec.x;
	owner.m_worldPos.z += knockBackVec.z;
}
