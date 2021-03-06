#include"Player.h"
#include"../Camera/TPSCamera.h"
#include"../GameSystem.h"
#include "Enemy.h"
#include "StageMap.h"
#include "Effect2D.h"
#include "System/Utility/KdUtility.h"

Player::Player()
{
	m_name = "Player";
}

const float Player::s_limitOfStepHeight = 0.1f;

void Player::Deserialize(const json11::Json& json)
{
	Character::Deserialize(json);

	m_worldPos = m_mWorld.Translation();

	m_spCamera = std::make_shared<TPSCamera>();

	GameSystem::GetInstance().SetCamera(m_spCamera);

	m_spCamera->Init();

	m_spCamera->SetProjectionMatrix(60.0f, 3000.0f);	// 視野角の設定（左右に60度＝120度）,最大描画距離(短いほど判定が正確になる)

	// カメラの注視点から5m離れる
	cameraMat = Math::Vector3(0.0f, 0.0f, -10.0f);
	m_spCamera->SetLocalPos(cameraMat);

	// キャラクターから注視点へのローカル座標を上に3m上げる
	cameraGazeMat = Math::Vector3(0.0f, 3.0f, 0.0f);
	m_spCamera->SetLocalGazePos(cameraGazeMat);

	// カメラの制限角度
	m_spCamera->SetClampAngle(-75.0f, 90.0f);

	// カメラの移動スピード
	m_spCamera->SetRotationSpeed(0.25);

	m_bumpSphereInfo.m_pos.y = 0.65f;
	m_bumpSphereInfo.m_radius = 0.4f;

	m_hp = 400;
	SetMaxHp(400);

	//角度をデシリアライズ
	Math::Vector3 rotate;
	JsonToVec3(json["Angle"], rotate);
	SetRotate(rotate);

	m_animator.SetAnimation(m_modelWork.GetData()->GetAnimation("Idle"));

	m_spActionState = std::make_shared<ActionWait>();

	//AudioEngin初期化
	DirectX::AUDIO_ENGINE_FLAGS eflags =
		DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;
	m_audioManager.Init();

	m_hpBarTex = GameResourceFactory.GetTexture("Data/Textures/bar.png");
	m_hpFrameTex = GameResourceFactory.GetTexture("Data/Textures/frame.png");

	m_damageFont = GameResourceFactory.GetTexture("Data/Textures/damagefont.png");

	m_spShadow = std::make_shared<Effect2D>();
	m_spShadow->Init();
	m_spShadow->SetPos(GetPos());
	m_spShadow->SetTexture(GameResourceFactory.GetTexture("Data/Textures/shadow.png"));

	m_swordmodelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/Weapon/sword.gltf"));

	m_input = std::make_shared<PlayerInput>();
}

void Player::Init()
{
}

// 更新処理
void Player::Update()
{
	m_force.y -= 0.02f;
	m_prevPos = GetPos();

	m_input->Update();

	//消す
	if (GetAsyncKeyState(VK_UP))
	{
		GameInstance.ReserveChangeScene("Data/Save/Dungeon2");
	}

	// 通常
	if (m_hitStop <= 0)
	{
		// 行動処理
		if (m_spActionState)
		{
			m_spActionState->Update(*this);
		}

		// 当たり判定更新
		UpdateCollition();

		// アニメーションの進行
		m_animator.AdvanceTime(m_modelWork.WorkNodes(), 1.0f,
			std::bind(&Player::ScriptProc, this, std::placeholders::_1)
		);
	}
	// ヒットストップ中
	else
	{
		m_hitStop--;
	}

	// 行列の更新(最終的な座標を確定してから)
	UpdateMatrix();

	// カメラを構成する行列の合成結果をセット
		if (m_spCamera&&m_useCamera)
		{
			m_spCamera->Update();

			Math::Matrix trans = Math::Matrix::CreateTranslation(m_worldPos);

			// プレイヤーの絶対行列のセット
			m_spCamera->SetCameraMatrix(trans);
		}

	m_worldPos += m_force;

	// 摩擦
	m_force *= 0.87f;

	m_modelWork.CalcNodeMatrices();

	//サウンド関連の更新
	{
		Math::Matrix listenerMat;

		if (m_spCamera)
		{
			listenerMat = m_spCamera->GetCameraMatrix();
		}
		m_audioManager.Update(listenerMat.Translation(), listenerMat.Backward());
	}

	if (m_hp <= 0)
	{
		m_isAlive = false;
		GameInstance.ReserveChangeMode(GameSystem::Result);
	}
}

void Player::Draw()
{
	Character::Draw();
	
	auto node = m_modelWork.FindNode("W_R");
	if (node)
	{
		auto w = node->m_worldTransform * m_mWorld;
		SHADER->m_standardShader.DrawModel(m_swordmodelWork, w);
	}
}

void Player::Draw2D()
{
	if (!m_hpBarTex||!m_hpFrameTex) { return; }
	Math::Vector3 _pos = Math::Vector3::Zero;
	// hpの描画処理
	GameInstance.GetCamera()->ConvertWorldToScreenDetail(GetPos(), _pos);
		Math::Rectangle barrec = { 0,0,500,20 };
		Math::Rectangle framerec = { 0,0,500,20 };
		Math::Color col= kWhiteColor;
		float maxhp = (float)GetMaxHp();
		float hpcalc = (m_hp / maxhp);
		SHADER->m_spriteShader.SetMatrix(Math::Matrix::Identity);
		SHADER->m_spriteShader.DrawTex(m_hpFrameTex.get(), -302, -290, 500, 20, &framerec, &col, { 0.5,0.5 });
		SHADER->m_spriteShader.DrawTex(m_hpBarTex.get(), -552, -290 , 500*hpcalc, 20,&barrec, &col, { 0.0,0.5 });
}

void Player::DrawEffect()
{
	if (!m_spShadow) { return; }

	Math::Matrix mDraw;

	Math::Vector3 scale = Math::Vector3::One;
	mDraw = Math::Matrix::CreateScale(scale);

	mDraw *= Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(90));

	mDraw.Translation(GetPos());

	SHADER->m_effectShader.DrawSquarePolygon(m_spShadow->GetPolyData(), mDraw);
}

void Player::NotifyDamage(DamageArg& arg)
{
	//無敵状態ならヒットしていない事にする
	if (invincibleFlg){arg.ret_IsHit = false; return;}

		m_hp -= arg.damage;
		DamageDisplay(arg.damage);
		arg.ret_IsHit = true;
}

void Player::DamageDisplay(int damage)
{
	if (!m_damageFont) { return; }
	{
		Math::Vector3 _pos = Math::Vector3::Zero;
		// hpの描画処理
		GameInstance.GetCamera()->ConvertWorldToScreenDetail(GetPos(), _pos);
		Math::Rectangle font = { 110,0,110,150 };
		SHADER->m_spriteShader.DrawTex(m_damageFont.get(), _pos.x, _pos.y, 22, 30, &font, &kWhiteColor, { 0.5,0.5 });
	}
}

void Player::ScriptProc(const json11::Json& event)
{
	std::string eventName = event["EventName"].string_value();

	if (eventName == "PlaySound")
	{
		const std::string& soundFile = event["SoundName"].string_value();
		m_audioManager.Play(soundFile);
	}
	else if (eventName == "DoAttack")
	{
		int damage = event["Damage"].int_value();
		DoAttack(damage);
	}
	else if (eventName == "ConToAtk")
	{
		if (m_atkComboFlg)
		{
			m_atkCancelAnimName = event["AnimName"].string_value();
		}
	}
	else if (eventName == "End")
	{
		ChangeAction < Player::ActionWait>();
	}
	else if (eventName == "AttackEffect")
	{
		const std::string& EffectFile = event["EffectName"].string_value();
		float	Size = (float)event["Size"].number_value();
		if (event["Size2"].is_number())
		{
			float	Size2 = (float)event["Size2"].number_value();
			Size = Size + (Size2 - Size) * m_effectValue;
		}
		float Speed = event["Speed"].int_value();
		int SpX = event["SpX"].int_value();
		int SpY = event["SpY"].int_value();

		Math::Vector3 Angle;
		JsonToVec3(event["Angle"], Angle);

		Math::Vector3 Pos;
		JsonToVec3(event["Pos"], Pos);

		bool localMode = false;
		JsonToBool(event["LocalMode"], localMode);
		
		std::shared_ptr<Effect2D> spEffect = std::make_shared<Effect2D>();
	
		spEffect->Init();
		spEffect->SetTexture(GameResourceFactory.GetTexture(EffectFile), Size, Size);
		
		if (localMode)
		{
			Math::Matrix m = m_mWorld;

			m = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(Angle.x))*
				Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(Angle.y))*
				Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(Angle.z))*
				Math::Matrix::CreateTranslation(Pos);

			spEffect->SetMatrix(m);
		}
		spEffect->SetLifeSpan(1000);
		spEffect->SetAnimation(SpX, SpY, 1.0f);
		spEffect->SetLocalMode(localMode);

		auto p = shared_from_this();

		spEffect->SetOwner(GetSptr(this));

		GameInstance.AddObject(spEffect);
	}
}

void Player::Release()
{
	m_audioManager.StopAllSound();
	m_spCamera.reset();
}

// 移動の更新処理
void Player::UpdateMove(Math::Vector3& dstMove)
{
	// キャラの移動処理
	float moveSpd = 0.1f;

	Math::Vector3 moveVec;

	moveVec.x = m_input->GetAxisL().x;
	moveVec.z = m_input->GetAxisL().y;

	// 斜め移動による加速を補正
	moveVec.Normalize();
	moveVec *= moveSpd;

	if (m_spCamera)
	{
		// 進行方向をカメラ回転を加味して修正する
	//	moveVec = moveVec.TransformNormal(moveVec, m_spCamera->GetRotationYMatrix()); //カメラ回転無くすために消し
	}

	// 座標確定
	m_worldPos.x += moveVec.x;
	m_worldPos.z += moveVec.z;

	dstMove = moveVec;
}

// 回転の更新処理
void Player::UpdateRotate(const Math::Vector3& srcMove)
{
	if (!m_spCamera) { return;}

	// ベクトルの長さがゼロだったら
	if (srcMove.LengthSquared() == 0.0f) { return; }

	// キャラの正面方向ベクトル：出発地
	Math::Vector3 nowDir = m_mWorld.Backward();
	
	// 移動方向のベクトル：目標地
	Math::Vector3 targetDir = srcMove;

	nowDir.Normalize();
	targetDir.Normalize();

	// それぞれのDegree角度を求める
	float nowAng = atan2(nowDir.x, nowDir.z);
	nowAng = DirectX::XMConvertToDegrees(nowAng);

	float targetAng = atan2(targetDir.x, targetDir.z);
	targetAng = DirectX::XMConvertToDegrees(targetAng);

	// ２つの間の角度を求める
	float rotateAng = targetAng - nowAng;

	// 回転の補正
	if (rotateAng > 180) 
	{
		rotateAng -= 360; 
	}
	else if (rotateAng < -180)
	{
		rotateAng += 360;
	}

	// 回転量代入
	rotateAng = std::clamp(rotateAng, -20.0f, 20.0f);
	
	m_worldRot.y += rotateAng;
}

// 行列更新
void Player::UpdateMatrix()
{
	// ワールド行列を作成
	Math::Matrix trans;
	trans = Math::Matrix::CreateTranslation(m_worldPos);
	Math::Matrix rotation;
	rotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));

	m_mWorld =rotation * trans;
}

void Player::DoAttack(int damage)
{	
		for (const std::shared_ptr<GameObject>& spObj : GameSystem::GetInstance().GetObjects())
		{
 			if (spObj->GetClassID() != GameObject::eEnemy&& spObj->GetClassID()!=GameObject::eDestuctible) { continue; }

    			Math::Vector3 attackPos = GetPos();
			attackPos += (m_mWorld.Backward() * 0.5);

			SphereInfo info(attackPos,m_bumpSphereInfo.m_radius+2.0f);

 			BumpResult result;

			// 相手の判定関数を利用する
			if (spObj->CheckCollisionBump(info, result))
			{
				auto chara = std::dynamic_pointer_cast<Character>(spObj);

				if (chara==nullptr) { continue; }
				if (chara->GetHp()<=0) { continue; }


				DamageArg arg;
				arg.damage = damage;
				arg.attackPos = attackPos;
   				chara->NotifyDamage(arg);

				if (arg.ret_IsHit)
				{
					m_hitStop = 3;

					//ヒット時行う処理
					//爆発
					std::shared_ptr<Effect2D> spEffect = std::make_shared<Effect2D>();

					Math::Vector3 effectPos = (attackPos += (m_mWorld.Up() * 0.5)+=(m_mWorld.Backward()*0.5));

					spEffect->Init();
					spEffect->SetAnimation(4, 5, 3.0f);
					spEffect->SetPos(effectPos);
					spEffect->SetLifeSpan(1000);
					spEffect->SetTexture(GameResourceFactory.GetTexture("Data/Textures/SlashH1.png"),15,15);
					GameInstance.AddObject(spEffect);
				}
			}
		}
}

// 当たり判定の更新
void Player::UpdateCollition()
{
	for (const std::shared_ptr<GameObject>& spObj : GameSystem::GetInstance().GetObjects())
	{
		if (spObj->GetClassID() != GameObject::eEnemy) { continue; }

		SphereInfo info(GetPos()+m_bumpSphereInfo.m_pos, m_bumpSphereInfo.m_radius);

		BumpResult result;

		// 相手の判定関数を利用する
		if (spObj->CheckCollisionBump(info, result))
		{
			m_worldPos += result.m_pushVec;
		}
	}

	for (const std::shared_ptr<GameObject>& spStageObj : GameSystem::GetInstance().GetObjects())
	{
		if (spStageObj->GetClassID() != GameObject::eStage&& spStageObj->GetClassID() != GameObject::eDestuctible && spStageObj->GetClassID() != GameObject::eGimmick) { continue; }

		BumpResult result;

		//壁判定
		SphereInfo sphereInfo(GetPos() + m_bumpSphereInfo.m_pos, m_bumpSphereInfo.m_radius);

		if (spStageObj->CheckCollisionBump(sphereInfo,result))
		{
			m_worldPos += result.m_pushVec;
			
		}

		//地面判定
		Math::Vector3 rayPos = m_prevPos;
		//　歩いて移動できる地面の限界の段差
		rayPos.y += s_limitOfStepHeight;

		RayInfo rayInfo(rayPos, Math::Vector3(0.0f, -1.0f, 0.0f),s_limitOfStepHeight);

		spStageObj->CheckCollisionBump(rayInfo, result);

		if (result.m_isHit)
		{
			m_worldPos += result.m_pushVec;
			m_force.y = 0.0f;
		}
	}

}

void Player::ActionWait::Update(Player& owner)
{
	if (!owner.CheckWait())
	{
		owner.ChangeAction < Player::ActionMove>();
	}

	if (owner.m_input->IsPressButton(0, false))
	{
		owner.ChangeAction<Player::ActionAttack>();
		owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Attack1"), false);
	}

	if (owner.m_input->IsPressButton(1, false))
	{
		owner.ChangeAction<Player::ActionDodge>();
	}

	if (owner.m_input->IsPressButton(2, false))
	{
		owner.ChangeAction<Player::ActionSkill>();
	}
	if (owner.m_input->IsPressButton(3, false))
	{
		owner.ChangeAction<Player::ActionSkill>();
	}
}

void Player::ActionMove::Update(Player& owner)
{
	if (owner.CheckWait())
	{
		owner.ChangeAction<Player::ActionWait>();
	}
	
	if (owner.m_input->IsPressButton(0, false))
	{
		owner.ChangeAction<Player::ActionAttack>();
		owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Attack1"), false);
	}

	if (owner.m_input->IsPressButton(1, false))
	{
		owner.ChangeAction < Player::ActionDodge>();
	}

	if (owner.m_input->IsPressButton(2, false))
	{
		owner.ChangeAction<Player::ActionSkill>();
	}
	if (owner.m_input->IsPressButton(3, false))
	{
		owner.ChangeAction<Player::ActionSkill>();
	}

	Math::Vector3 vMove;

	// 移動の更新処理
	owner.UpdateMove(vMove);

	// 回転の更新処理
	owner.UpdateRotate(vMove);


}

void Player::ActionAttack::Update(Player& owner)
{

	if (owner.m_input->IsPressButton(0, false))
	{
		owner.m_atkComboFlg = true;
	}

	if (owner.m_atkCancelAnimName.size() > 0)
	{
		owner.ChangeAction < Player::ActionAttack>();
		owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation(owner.m_atkCancelAnimName), false);
		owner.m_atkCancelAnimName = "";
	}
	if (owner.m_input->IsPressButton(1, false))
	{
		owner.ChangeAction < Player::ActionDodge>();
	}

	if (owner.m_input->IsPressButton(2, false))
	{
		owner.ChangeAction<Player::ActionSkill>();
	}
	if (owner.m_input->IsPressButton(3, false))
	{
		owner.ChangeAction<Player::ActionSkill>();
	}

	if (owner.m_input->GetAxisL().y != 0 || owner.m_input->GetAxisL().x != 0)
	{
		Math::Vector3 attackVec = owner.m_mWorld.Backward();

		attackVec.Normalize();
		attackVec *= 0.05f;

		owner.m_worldPos.x += attackVec.x;
		owner.m_worldPos.z += attackVec.z;
	}
	else
	{
		Math::Vector3 attackVec = owner.m_mWorld.Backward();

		attackVec.Normalize();
		attackVec *= 0.025f;

		owner.m_worldPos.x += attackVec.x;
		owner.m_worldPos.z += attackVec.z;
	}
}

void Player::ActionDodge::Update(Player& owner)
{
}

void Player::UpdateInput()
{

}

void Player::ActionSkill::Update(Player& owner)
{
	
}

void Player::ActionExit::Update(Player& owner)
{
	Math::Vector3 vec = owner.m_exitVec;
	vec.Normalize();
	vec*= 0.1f;

	owner.m_worldPos.x += vec.x;
	owner.m_worldPos.z += vec.z;
}
