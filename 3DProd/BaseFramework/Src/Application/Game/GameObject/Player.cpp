#include"Player.h"
//#include"../Camera/FPSCamera.h"
#include"../Camera/TPSCamera.h"
#include"../GameSystem.h"
#include "Enemy.h"
#include "Effect2D.h"

Player::Player()
{
}

void Player::Init()
{
	m_modelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/Robot/chara.gltf"));

	m_spCamera = std::make_shared<TPSCamera>();

	GameSystem::GetInstance().SetCamera(m_spCamera);

	m_spCamera->Init();

	m_spCamera->SetProjectionMatrix(60.0f, 3000.0f);	// ����p�̐ݒ�i���E��60�x��120�x�j,�ő�`�拗��(�Z���قǔ��肪���m�ɂȂ�)

	// �J�����̒����_����5m�����
	m_spCamera->SetLocalPos(Math::Vector3(0.0f, 0.0f, -10.0f));

	// �L�����N�^�[���璍���_�ւ̃��[�J�����W�����3m�グ��
	m_spCamera->SetLocalGazePos(Math::Vector3(0.0f, 3.0f, 0.0f));

	// �J�����̐����p�x
	m_spCamera->SetClampAngle(-75.0f,90.0f);

	// �J�����̈ړ��X�s�[�h
	m_spCamera->SetRotationSpeed(0.25);

	m_radius = 0.5f;

	m_animator.SetAnimation(m_modelWork.GetData()->GetAnimation("Idle"));


	m_spActionState = std::make_shared<ActionWait>();

	//AudioEngin������
	DirectX::AUDIO_ENGINE_FLAGS eflags =
		DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;
	m_audioManager.Init();

	m_swordmodelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/Weapon/sword.gltf"));
}

// �X�V����
void Player::Update()
{
//	UpdateInput();

	m_input->Update();

	// �ʏ�
	if (m_hitStop <= 0)
	{
		// �s������
		if (m_spActionState)
		{
			m_spActionState->Update(*this);
		}

		// �����蔻��X�V
		UpdateCollition();

		// �A�j���[�V�����̐i�s
		m_animator.AdvanceTime(m_modelWork.WorkNodes(), 1.0f,
			std::bind(&Player::ScriptProc, this, std::placeholders::_1)
		);
	}
	// �q�b�g�X�g�b�v��
	else
	{
		m_hitStop--;
	}

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

	/*
	// �����_��
	auto onEvent = [this](const json11::Json& event)
	{
	};
	*/
	

	m_modelWork.CalcNodeMatrices();

	//�T�E���h�֘A�̍X�V
	{
		Math::Matrix listenerMat;

		if (m_spCamera)
		{
			listenerMat = m_spCamera->GetCameraMatrix();
		}
		m_audioManager.Update(listenerMat.Translation(), listenerMat.Backward());
	}
}

void Player::Draw()
{
	Character::Draw();
	/*
	auto node = m_modelWork.FindNode("Bip001 Head");
	if (node)
	{
		auto w = node->m_worldTransform * m_mWorld;
		SHADER->m_standardShader.DrawModel(m_swordmodelWork, w);
	}*/
}

void Player::NotifyDamage(DamageArg& arg)
{
	m_hp -= arg.damage;
	arg.ret_IsHit = true;
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
		DoAttack();
	}
	else if (eventName == "ConToAtk2")
	{
//		m_atkComboFlg = false;

		if (m_atkComboFlg)
		{
			m_atkCancelAnimName = event["AnimName"].string_value();
		}
	}
	else if (eventName == "invincible")
	{
		//���G�����@��ŏ���
	}
	else if (eventName == "End")
	{
		ChangeWait();
	}
	else if (eventName == "AttackEffect")
	{
		const std::string& EffectFile = event["EffectName"].string_value();
		//����
		std::shared_ptr<Effect2D> spEffect = std::make_shared<Effect2D>();
		Math::Vector3 effectPos = GetPos();
		effectPos += (m_mWorld.Up() * 1);

		Math::Matrix m = m_mWorld;
		m.Translation(m.Translation() + m.Up() * 1);

		m = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(-90)) * m;

		spEffect->Init();
		spEffect->SetAnimation(4, 3,1.0f);
//		spEffect->SetPos(effectPos);
		spEffect->SetMatrix(m);
		spEffect->SetLifeSpan(1000);
		spEffect->SetTexture(GameResourceFactory.GetTexture(EffectFile),4,4);

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

// �ړ��̍X�V����
void Player::UpdateMove(Math::Vector3& dstMove)
{
	// �L�����̈ړ�����
	float moveSpd = 0.1f;

	Math::Vector3 moveVec;

	moveVec.x = m_input->GetAxisL().x;
	moveVec.z = m_input->GetAxisL().y;

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
	rotateAng = std::clamp(rotateAng, -20.0f, 20.0f);
	
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
				if (chara->GetHp()<=0) { continue; }
				DamageArg arg;
				arg.damage = 10;
				chara->NotifyDamage(arg);

				if (arg.ret_IsHit)
				{
					m_hitStop = 3;

					//�q�b�g���s������
					//����
					std::shared_ptr<Effect2D> spEffect = std::make_shared<Effect2D>();

					Math::Vector3 effectPos = (attackPos+=(m_mWorld.Up()*0.5));

					spEffect->Init();
					spEffect->SetAnimation(4, 5,3.0f);
					spEffect->SetPos(effectPos);
					spEffect->SetLifeSpan(1000);
					spEffect->SetTexture(GameResourceFactory.GetTexture("Data/Textures/SlashH1.png"),5,5);
					GameInstance.AddObject(spEffect);
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
	if (!owner.CheckWait())
	{
		owner.ChangeMove();
	}

	if (owner.m_input->IsPressButton(0, false))
	{
//		owner.ChangeAttack();
		owner.ChangeAction<Player::ActionAttack>();
		owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Attack1"));
	}

	if (owner.m_input->IsPressButton(1, false))
	{
		owner.ChangeDodge();
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
		owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation("Attack1"));
	}

	if (owner.m_input->IsPressButton(1, false))
	{
		owner.ChangeDodge();
	}

	Math::Vector3 vMove;

	// �ړ��̍X�V����
	owner.UpdateMove(vMove);

	// ��]�̍X�V����
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
		owner.ChangeAttack();
		owner.m_animator.SetAnimation(owner.m_modelWork.GetData()->GetAnimation(owner.m_atkCancelAnimName));
		owner.m_atkCancelAnimName = "";
	}
	if (owner.m_input->IsPressButton(1, false))
	{
		owner.ChangeDodge();
	}
}

void Player::ActionDodge::Update(Player& owner)
{
	//���������Ă�����ɉ��������������������
	Math::Vector3 dodgeVec = owner.m_mWorld.Backward();

	dodgeVec.Normalize();
	dodgeVec *= 0.3f;

	owner.m_worldPos.x += dodgeVec.x;
	owner.m_worldPos.z += dodgeVec.z;
}

void Player::UpdateInput()
{
	/*
	m_axisL = Math::Vector2::Zero;

	if (GetAsyncKeyState('W')) { m_axisL.y += 1.0f; }	// �O�ړ�
	if (GetAsyncKeyState('S')) { m_axisL.y -= 1.0f; }	// ���ړ�
	if (GetAsyncKeyState('A')) { m_axisL.x -= 1.0f; }	// ���ړ�
	if (GetAsyncKeyState('D')) { m_axisL.x += 1.0f; }	// �E�ړ�
	*/
}


