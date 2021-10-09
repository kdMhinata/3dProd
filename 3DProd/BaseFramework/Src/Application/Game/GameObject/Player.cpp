#include"Player.h"
//#include"../Camera/FPSCamera.h"
#include"../Camera/TPSCamera.h"
#include"../GameSystem.h"
#include "Enemy.h"
Player::Player()
{
}

Player::~Player()
{
	Release();
}

void Player::Init()
{
	m_modelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/Robot/chara.gltf"));

	m_spCamera = std::make_shared<TPSCamera>();

	m_enemy = std::make_shared<Enemy>();

	GameSystem::GetInstance().SetCamera(m_spCamera);

	m_spCamera->Init();

	m_spCamera->SetProjectionMatrix(60.0f, 3000.0f);	// 視野角の設定（左右に60度＝120度）,最大描画距離(短いほど判定が正確になる)

	// カメラの注視点から5m離れる
	m_spCamera->SetLocalPos(Math::Vector3(0.0f, 0.0f, -5.0f));

	// キャラクターから注視点へのローカル座標を上に3m上げる
	m_spCamera->SetLocalGazePos(Math::Vector3(0.0f, 3.0f, 0.0f));

	// カメラの制限角度
	m_spCamera->SetClampAngle(-75.0f,90.0f);

	// カメラの移動スピード
	m_spCamera->SetRotationSpeed(0.25);

	m_radius = 0.5f;

	m_animator.SetAnimation(m_modelWork.GetData()->GetAnimation("Idle"));


	m_spActionState = std::make_shared<ActionWait>();
}

// 更新処理
void Player::Update()
{
	UpdateInput();

	if (m_spActionState)
	{
		m_spActionState->Update(*this);
	}

	// 当たり判定更新
	UpdateCollition();

	// 行列の更新(最終的な座標を確定してから)
	UpdateMatrix();

	// カメラを構成する行列の合成結果をセット
	if (m_spCamera)
	{
		m_spCamera->Update();

		Math::Matrix trans = Math::Matrix::CreateTranslation(m_worldPos);

		// プレイヤーの絶対行列のセット
		m_spCamera->SetCameraMatrix(trans);
	}

	/*
	// ラムダ式
	auto onEvent = [this](const json11::Json& event)
	{
	};
	*/
	
	m_animator.AdvanceTime(m_modelWork.WorkNodes(), 1.0f, 
		std::bind(&Player::ScriptProc, this, std::placeholders::_1)
	);

	m_modelWork.CalcNodeMatrices();
}

void Player::ScriptProc(const json11::Json& event)
{
	std::string eventName = event["EventName"].string_value();
	if (eventName == "PlaySound")
	{
		event["SoundName"];
	}
	else if (eventName == "Cancel")
	{

	}
}

void Player::Release()
{
	m_spCamera.reset();
}

// 移動の更新処理
void Player::UpdateMove(Math::Vector3& dstMove)
{
	// キャラの移動処理
	float moveSpd = 0.05f;

	Math::Vector3 moveVec;

	moveVec.x = m_axisL.x;
	moveVec.z = m_axisL.y;

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
	rotateAng = std::clamp(rotateAng, -8.0f, 8.0f);
	
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

void Player::DoAttack()
{	
		for (const std::shared_ptr<GameObject>& spObj : GameSystem::GetInstance().GetObjects())
		{
			if (spObj->GetClassID() != GameObject::eEnemy) { continue; }

			Math::Vector3 attackPos = GetPos();
			attackPos += (m_mWorld.Backward() * 0.5);

			SphereInfo info(attackPos, m_radius+0.05f);

			BumpResult result;

			// 相手の判定関数を利用する
			if (spObj->CheckCollisionBump(info, result))
			{
				auto chara = std::dynamic_pointer_cast<Character>(spObj);

				if (chara==nullptr) { continue; }
				DamageArg arg;
				arg.damage = 10;
				chara->NotifyDamage(arg);

				if (arg.ret_IsHit)
				{
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

		SphereInfo info(GetPos(),m_radius);

		BumpResult result;

		// 相手の判定関数を利用する
		if (spObj->CheckCollisionBump(info, result))
		{
			m_worldPos += result.m_pushVec;
		}
	}
}

void Player::ActionWait::Update(Player& owner)
{
	//移動キーが押された？
	

	//ジャンプキー押された？


	if (!owner.CheckWait())
	{
		owner.ChangeMove();
		owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Run"));
	}

	if (GetAsyncKeyState(VK_SPACE))
	{
		owner.ChangeAttack();
		owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Attack1"));
	}
}


void Player::ActionMove::Update(Player& owner)
{
	if (owner.CheckWait())
	{
		owner.ChangeWait();

		owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Idle"));

		return;
	}
	
	if (GetAsyncKeyState(VK_SPACE))
	{
		owner.ChangeAttack();
		owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Attack1"));
	}

	Math::Vector3 vMove;

	// 移動の更新処理
	owner.UpdateMove(vMove);

	// 回転の更新処理
	owner.UpdateRotate(vMove);


}



void Player::ActionAttack::Update(Player& owner)
{
	//攻撃可能で無かった時はIdle状態に戻す
	if (!owner.m_canAttack)
		owner.ChangeWait();
	
	//攻撃の処理
	owner.DoAttack();
}

void Player::UpdateInput()
{
	m_axisL = Math::Vector2::Zero;

	if (GetAsyncKeyState('W')) { m_axisL.y += 1.0f; }	// 前移動
	if (GetAsyncKeyState('S')) { m_axisL.y -= 1.0f; }	// 後ろ移動
	if (GetAsyncKeyState('A')) { m_axisL.x -= 1.0f; }	// 左移動
	if (GetAsyncKeyState('D')) { m_axisL.x += 1.0f; }	// 右移動
	
}


