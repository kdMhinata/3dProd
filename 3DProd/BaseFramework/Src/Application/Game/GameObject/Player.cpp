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

	m_spCamera->SetProjectionMatrix(60.0f, 3000.0f);	// ����p�̐ݒ�i���E��60�x��120�x�j,�ő�`�拗��(�Z���قǔ��肪���m�ɂȂ�)

	// �J�����̒����_����5m�����
	m_spCamera->SetLocalPos(Math::Vector3(0.0f, 0.0f, -5.0f));

	// �L�����N�^�[���璍���_�ւ̃��[�J�����W�����3m�グ��
	m_spCamera->SetLocalGazePos(Math::Vector3(0.0f, 3.0f, 0.0f));

	// �J�����̐����p�x
	m_spCamera->SetClampAngle(-75.0f,90.0f);

	// �J�����̈ړ��X�s�[�h
	m_spCamera->SetRotationSpeed(0.25);

	m_radius = 0.5f;

	m_animator.SetAnimation(m_modelWork.GetData()->GetAnimation("Idle"));


	m_spActionState = std::make_shared<ActionWait>();
}

// �X�V����
void Player::Update()
{
	UpdateInput();

	if (m_spActionState)
	{
		m_spActionState->Update(*this);
	}

	// �����蔻��X�V
	UpdateCollition();

	// �s��̍X�V(�ŏI�I�ȍ��W���m�肵�Ă���)
	UpdateMatrix();

	// �J�������\������s��̍������ʂ��Z�b�g
	if (m_spCamera)
	{
		m_spCamera->Update();

		Math::Matrix trans = Math::Matrix::CreateTranslation(m_worldPos);

		// �v���C���[�̐�΍s��̃Z�b�g
		m_spCamera->SetCameraMatrix(trans);
	}

	// �����_��
	auto onEvent = [this](const json11::Json& event)
	{
		std::string eventName = event["EventName"].string_value();
		if (eventName == "PlaySound")
		{

		}
		else if (eventName == "Cancel")
		{

		}
	};

	m_animator.AdvanceTime(m_modelWork.WorkNodes(), 1.0f, onEvent);


	m_modelWork.CalcNodeMatrices();
}

void Player::Release()
{
	m_spCamera.reset();
}

// �ړ��̍X�V����
void Player::UpdateMove(Math::Vector3& dstMove)
{
	// �L�����̈ړ�����
	float moveSpd = 0.05f;

	Math::Vector3 moveVec;

	moveVec.x = m_axisL.x;
	moveVec.z = m_axisL.y;

	// �΂߈ړ��ɂ�������␳
	moveVec.Normalize();
	moveVec *= moveSpd;

	if (m_spCamera)
	{
		// �i�s�������J������]���������ďC������
	//	moveVec = moveVec.TransformNormal(moveVec, m_spCamera->GetRotationYMatrix()); //�J������]���������߂ɏ���
	}

	// ���W�m��
	m_worldPos.x += moveVec.x;
	m_worldPos.z += moveVec.z;

	dstMove = moveVec;
}

// ��]�̍X�V����
void Player::UpdateRotate(const Math::Vector3& srcMove)
{
	if (!m_spCamera) { return;}

	// �x�N�g���̒������[����������
	if (srcMove.LengthSquared() == 0.0f) { return; }

	// �L�����̐��ʕ����x�N�g���F�o���n
	Math::Vector3 nowDir = m_mWorld.Backward();
	
	// �ړ������̃x�N�g���F�ڕW�n
	Math::Vector3 targetDir = srcMove;

	nowDir.Normalize();
	targetDir.Normalize();

	// ���ꂼ���Degree�p�x�����߂�
	float nowAng = atan2(nowDir.x, nowDir.z);
	nowAng = DirectX::XMConvertToDegrees(nowAng);

	float targetAng = atan2(targetDir.x, targetDir.z);
	targetAng = DirectX::XMConvertToDegrees(targetAng);

	// �Q�̊Ԃ̊p�x�����߂�
	float rotateAng = targetAng - nowAng;

	// ��]�̕␳
	if (rotateAng > 180) 
	{
		rotateAng -= 360; 
	}
	else if (rotateAng < -180)
	{
		rotateAng += 360;
	}

	// ��]�ʑ��
	rotateAng = std::clamp(rotateAng, -8.0f, 8.0f);
	
	m_worldRot.y += rotateAng;
}

// �s��X�V
void Player::UpdateMatrix()
{
	// ���[���h�s����쐬
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

			// ����̔���֐��𗘗p����
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

// �����蔻��̍X�V
void Player::UpdateCollition()
{
	for (const std::shared_ptr<GameObject>& spObj : GameSystem::GetInstance().GetObjects())
	{
		if (spObj->GetClassID() != GameObject::eEnemy) { continue; }

		SphereInfo info(GetPos(),m_radius);

		BumpResult result;

		// ����̔���֐��𗘗p����
		if (spObj->CheckCollisionBump(info, result))
		{
			m_worldPos += result.m_pushVec;
		}
	}
}

void Player::ActionWait::Update(Player& owner)
{
	//�ړ��L�[�������ꂽ�H
	

	//�W�����v�L�[�����ꂽ�H


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

	// �ړ��̍X�V����
	owner.UpdateMove(vMove);

	// ��]�̍X�V����
	owner.UpdateRotate(vMove);


}



void Player::ActionAttack::Update(Player& owner)
{
	//�U���\�Ŗ�����������Idle��Ԃɖ߂�
	if (!owner.m_canAttack)
		owner.ChangeWait();

	if()
	

	
	//�U���̏���
	owner.DoAttack();
}

void Player::UpdateInput()
{
	m_axisL = Math::Vector2::Zero;

	if (GetAsyncKeyState('W')) { m_axisL.y += 1.0f; }	// �O�ړ�
	if (GetAsyncKeyState('S')) { m_axisL.y -= 1.0f; }	// ���ړ�
	if (GetAsyncKeyState('A')) { m_axisL.x -= 1.0f; }	// ���ړ�
	if (GetAsyncKeyState('D')) { m_axisL.x += 1.0f; }	// �E�ړ�
	
}


