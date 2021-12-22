#include"Enemy.h"
#include"Player.h"
#include "Effect2D.h"
#include"../GameSystem.h"

Enemy::Enemy()
{
	m_name = "Enemy";
}

void Enemy::Deserialize(const json11::Json& json)
{
	Character::Deserialize(json);

	m_worldPos = m_mWorld.Translation();

//		LoadModel("Data/Models/enemy/slime.gltf");

	//デシリアライズ時にtagがPlayerのGameObjectをターゲットする

	m_bumpSphereInfo.m_pos.y = 0.45f;
	m_bumpSphereInfo.m_radius = 0.3f;

	m_animator.SetAnimation(m_modelWork.GetData()->GetAnimation("Idle"));

	m_spActionState = std::make_shared<ActionWait>();

	m_worldRot.y = 180;

	m_hp = 100;
	SetMaxHp(100);

	//角度をデシリアライズ
	Math::Vector3 rotate;
	JsonToVec3(json["Angle"], rotate);
	SetRotate(rotate);

	m_attackradius = 1.0f;

	//AudioEngin初期化
	DirectX::AUDIO_ENGINE_FLAGS eflags =
		DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;
	m_audioManager.Init();

	m_hpBarTex = GameResourceFactory.GetTexture("Data/Textures/ebar.png");
	m_hpFrameTex = GameResourceFactory.GetTexture("Data/Textures/frame.png");

	/*
	uuid m_uuid = json["TargetUUID"].string_value();
	// ※Searchダメ

	uuid m_uuid2 = json["TargetUUID"].string_value();
	*/
}

void Enemy::Serialize(json11::Json::object& json)
{
	Character::Serialize(json);

}

void Enemy::Init()
{
	LoadModel("Data/Models/enemy/slime.gltf");
	
	m_bumpSphereInfo.m_pos.y = 0.45f;
	m_bumpSphereInfo.m_radius = 0.3f;

	m_animator.SetAnimation(m_modelWork.GetData()->GetAnimation("Idle"));

	m_spActionState = std::make_shared<ActionWait>();

	m_worldPos = { 0.0,0.0,5.0 };

	m_worldRot.y =180;

	m_hp = 100;
	SetMaxHp(100);

	m_hpBarTex = GameResourceFactory.GetTexture("Data/Textures/ebar.png");
	m_hpFrameTex = GameResourceFactory.GetTexture("Data/Textures/frame.png");

	//AudioEngin初期化
	DirectX::AUDIO_ENGINE_FLAGS eflags =
		DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;
	m_audioManager.Init();
}

void Enemy::Draw2D()
{
	if (!m_hpBarTex || !m_hpFrameTex) { return; }
	if (m_hp == GetMaxHp() || m_hp <= 0) { return; }

	Math::Vector3 _pos = Math::Vector3::Zero;
	GameInstance.GetCamera()->ConvertWorldToScreenDetail(GetPos(), _pos);
	Math::Rectangle barrec = { 0,0,60,5 };
	Math::Rectangle framerec = { 0,0,60,5 };
	Math::Color col = kWhiteColor;
	float maxhp = (float)GetMaxHp();
	float hpcalc = (m_hp / maxhp);
	SHADER->m_spriteShader.SetMatrix(Math::Matrix::Identity);
	SHADER->m_spriteShader.DrawTex(m_hpFrameTex.get(), _pos.x, _pos.y-20, 60, 5, &framerec, &col, { 0.5,0.5 });
	SHADER->m_spriteShader.DrawTex(m_hpBarTex.get(), _pos.x- (60 / 2), _pos.y-20, 60 * hpcalc, 5, &barrec, &col, { 0.0,0.5 });
}

void Enemy::ImGuiUpdate()
{
	Character::ImGuiUpdate();

	ImGui::DragFloat3("Pos", &m_worldPos.x, 0.01f);

	ImGui::DragFloat("Angle", &m_worldRot.y, 0.1f);

	ImGui::DragInt("Hp", &m_hp, 1.0f, 0, 200);

	if (ImGui::ListBoxHeader("Action"))
	{
		if (ImGui::Button("Wait"))
		{
			ChangeAction < Enemy::ActionWait>();
		}
		if (ImGui::Button("Attack"))
		{
			ChangeAction < Enemy::ActionAttack>();
		}
		if (ImGui::Button("Move"))
		{
			ChangeAction < Enemy::ActionMove>();
		}

	}
	ImGui::ListBoxFooter();

	if (m_spActionState)
	{
		ImGui::LabelText("State", typeid(*m_spActionState).name());
	}
}

void Enemy::Update()
{
	if (m_spActionState)
	{
		m_spActionState->Update(*this);
	}

	UpdateSearch();

	UpdateCollition();

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

	if (arg.ret_IsHit&&!m_sarmor)
	{
		ChangeAction < Enemy::ActionGetHit>();
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
		ChangeAction < Enemy::ActionWait>();
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
	float attackRange= 1.5f;
	attackRange += m_attackradius;
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

void Enemy::UpdateSearch()
{
	m_wpTarget=GameInstance.FindObjectWithTag("Player");

	// 見ている先が解放されているか
	if (m_wpTarget.expired()) { return; }

	std::shared_ptr<const GameObject> spTarget = m_wpTarget.lock();

	// プレイヤーに向かう方向ベクトル
	Math::Vector3 targetDir = spTarget->GetPos() - m_worldPos;

	// ターゲットとの距離
	float targetDistSqr = targetDir.LengthSquared();

	//攻撃処理(仮)
	float findRange =11.00f;
	if (targetDistSqr < findRange * findRange) { m_findTargetFlg = true; }
	else { m_findTargetFlg = false; }
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

		SphereInfo info(attackPos, m_bumpSphereInfo.m_radius+ m_attackradius);

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

void Enemy::UpdateCollition()
{
	for (const std::shared_ptr<GameObject>& spStageObj : GameSystem::GetInstance().GetObjects())
	{
		if (spStageObj->GetClassID() != GameObject::eStage) { continue; }

		BumpResult result;

		//壁判定
		SphereInfo sphereInfo(GetPos() + m_bumpSphereInfo.m_pos, m_bumpSphereInfo.m_radius);

		if (spStageObj->CheckCollisionBump(sphereInfo, result))
		{
			m_worldPos += result.m_pushVec;

		}
	}
}

void Enemy::ActionWait::Update(Enemy& owner)
{
	if (owner.m_findTargetFlg)
	{
		owner.ChangeAction<ActionMove>();
	}

	if (owner.m_hp <= 0)
	{
		owner.ChangeAction < Enemy::ActionElimination>();
	}
}

void Enemy::ActionMove::Update(Enemy& owner)
{
	if (owner.m_attackFlg)
	{
		owner.ChangeAction < Enemy::ActionAttack>();
	}

	if (owner.m_hp <= 0)
	{
		owner.ChangeAction < Enemy::ActionElimination>();
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
	owner.m_alpha -= 0.01f;
	if (owner.m_alpha <= 0.0f)
	{
		owner.m_alpha = 0;
	}
}

void Enemy::ActionGetHit::Update(Enemy& owner)
{
	Math::Vector3 knockBackVec = owner.m_mWorld.Forward();

	knockBackVec.Normalize();
	knockBackVec *= 0.02f;

	owner.m_worldPos.x += knockBackVec.x;
	owner.m_worldPos.z += knockBackVec.z;

	if (owner.m_hp <= 0)
	{
		owner.ChangeAction < Enemy::ActionElimination>();
	}
}
