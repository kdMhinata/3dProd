#include"Player.h"
//#include"../Camera/FPSCamera.h"
#include"../Camera/TPSCamera.h"
#include"../GameSystem.h"
#include "Enemy.h"
#include "StageMap.h"
#include "Effect2D.h"

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

	m_spCamera->SetProjectionMatrix(60.0f, 3000.0f);	// ����p�̐ݒ�i���E��60�x��120�x�j,�ő�`�拗��(�Z���قǔ��肪���m�ɂȂ�)

	// �J�����̒����_����5m�����
	cameraMat = Math::Vector3(0.0f, 0.0f, -10.0f);
	m_spCamera->SetLocalPos(cameraMat);

	// �L�����N�^�[���璍���_�ւ̃��[�J�����W�����3m�グ��
	cameraGazeMat = Math::Vector3(0.0f, 3.0f, 0.0f);
	m_spCamera->SetLocalGazePos(cameraGazeMat);

	// �J�����̐����p�x
	m_spCamera->SetClampAngle(-75.0f, 90.0f);

	// �J�����̈ړ��X�s�[�h
	m_spCamera->SetRotationSpeed(0.25);

	m_bumpSphereInfo.m_pos.y = 0.65f;
	m_bumpSphereInfo.m_radius = 0.4f;

	m_hp = 200;
	SetMaxHp(200);

	m_animator.SetAnimation(m_modelWork.GetData()->GetAnimation("Idle"));

	m_spActionState = std::make_shared<ActionWait>();

	//AudioEngin������
	DirectX::AUDIO_ENGINE_FLAGS eflags =
		DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;
	m_audioManager.Init();

	m_hpBarTex = GameResourceFactory.GetTexture("Data/Textures/bar.png");
	m_hpFrameTex = GameResourceFactory.GetTexture("Data/Textures/frame.png");

	m_spShadow = std::make_shared<Effect2D>();
	m_spShadow->Init();
	m_spShadow->SetPos(GetPos());
	m_spShadow->SetTexture(GameResourceFactory.GetTexture("Data/Textures/shadow.png"));

	m_swordmodelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/Weapon/sword.gltf"));

	m_input = std::make_shared<PlayerInput>();
}

void Player::Init()
{
	/*
	m_modelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/robot/chara.gltf"));

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

	m_bumpSphereInfo.m_pos.y =0.65f;
	m_bumpSphereInfo.m_radius = 0.4f;

	m_hp = 200;

	m_animator.SetAnimation(m_modelWork.GetData()->GetAnimation("Idle"));


	m_spActionState = std::make_shared<ActionWait>();

	//AudioEngin������
	DirectX::AUDIO_ENGINE_FLAGS eflags =
		DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;
	m_audioManager.Init();

	m_hpBarTex = GameResourceFactory.GetTexture("Data/Textures/bar.png");
	m_hpFrameTex = GameResourceFactory.GetTexture("Data/Textures/frame.png");

	m_spShadow = std::make_shared<Effect2D>();
	m_spShadow->Init();
	m_spShadow->SetPos(GetPos());
	m_spShadow->SetTexture(GameResourceFactory.GetTexture("Data/Textures/shadow.png"));

	m_swordmodelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/Weapon/sword.gltf"));
	*/
}

// �X�V����
void Player::Update()
{
	m_force.y -= 0.02f;
	m_prevPos = GetPos();

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
//	m_worldPos.y += m_gravity;
	m_worldPos += m_force;

	// ���C
/*	if (��)
	{
		m_force *= 0.99f;
	}
	else
	{
		m_force *= 0.9f;
	}*/
	m_force *= 0.87f;

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
	// 
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

void Player::ImGuiUpdate()
{
	Character::ImGuiUpdate();

	ImGui::DragFloat3("Pos", &m_worldPos.x, 0.01f);

	ImGui::DragFloat("Angle", &m_worldRot.y, 0.1f);

	ImGui::DragInt("Hp", &m_hp,1.0f,0,200);

	ImGui::DragFloat3("CameraPos", &cameraMat.x, 0.01f);
	m_spCamera->SetLocalPos(cameraMat);

	ImGui::DragFloat3("CameraGazePos", &cameraGazeMat.x, 0.01f);
	m_spCamera->SetLocalGazePos(cameraGazeMat);

	if (ImGui::Button("Debug"))
	{
		m_worldPos.y = 37;
		cameraMat.z = -10;
	}

	ImGui::Checkbox("noDamage", &invincibleFlg);

	if (ImGui::ListBoxHeader("Action"))
	{
		if (ImGui::Button("Wait"))
		{
			ChangeAction < Player::ActionWait>();
		}
		if (ImGui::Button("Attack"))
		{
			ChangeAction < Player::ActionAttack>();
		}
		if (ImGui::Button("Move"))
		{
			ChangeAction < Player::ActionMove>();
		}
		if (ImGui::Button("Dodge"))
		{
			ChangeAction < Player::ActionDodge>();
		}
		if (ImGui::Button("Skill"))
		{
			ChangeAction < Player::ActionSkill>();
		}

	}
	ImGui::ListBoxFooter();

	if (m_spActionState)
	{
		ImGui::LabelText("State", typeid(*m_spActionState).name());
	}
}

void Player::NotifyDamage(DamageArg& arg)
{
	if (!invincibleFlg)
	{
		m_hp -= arg.damage;
		arg.ret_IsHit = true;
	}
	else
	{
		arg.ret_IsHit = false;
	}
}


// 


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
	else if (eventName == "ConToAtk")
	{
//		m_atkComboFlg = false;

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

		bool localMode = false;
		JsonToBool(event["LocalMode"], localMode);
		
		std::shared_ptr<Effect2D> spEffect = std::make_shared<Effect2D>();
		/*Math::Vector3 effectPos = GetPos();
		effectPos += (m_mWorld.Up() * 1);*/

		spEffect->Init();
		spEffect->SetTexture(GameResourceFactory.GetTexture(EffectFile), Size, Size);
//		spEffect->SetPos(effectPos);

		//�o�O����̂ŏ������]
		if (localMode)
		{
			Math::Matrix m = m_mWorld;
			m.Translation(m.Translation() + m.Up() * 1);

			m = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(-90)) * m;

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
 			if (spObj->GetClassID() != GameObject::eEnemy&& spObj->GetClassID()!=GameObject::eDestuctible) { continue; }

    			Math::Vector3 attackPos = GetPos();
			attackPos += (m_mWorld.Backward() * 0.5);

			SphereInfo info(attackPos,m_bumpSphereInfo.m_radius+1.0f);

 			BumpResult result;

			// ����̔���֐��𗘗p����
			if (spObj->CheckCollisionBump(info, result))
			{
				auto chara = std::dynamic_pointer_cast<Character>(spObj);

				if (chara==nullptr) { continue; }
				if (chara->GetHp()<=0) { continue; }


				DamageArg arg;
				arg.damage = 10;
				arg.attackPos = attackPos;
   				chara->NotifyDamage(arg);

				if (arg.ret_IsHit)
				{
					m_hitStop = 3;

					//�q�b�g���s������
					//����
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

// �����蔻��̍X�V
void Player::UpdateCollition()
{
	for (const std::shared_ptr<GameObject>& spObj : GameSystem::GetInstance().GetObjects())
	{
		if (spObj->GetClassID() != GameObject::eEnemy) { continue; }

		SphereInfo info(GetPos()+m_bumpSphereInfo.m_pos, m_bumpSphereInfo.m_radius);

		BumpResult result;

		// ����̔���֐��𗘗p����
		if (spObj->CheckCollisionBump(info, result))
		{
			m_worldPos += result.m_pushVec;
		}
	}

	for (const std::shared_ptr<GameObject>& spStageObj : GameSystem::GetInstance().GetObjects())
	{
		if (spStageObj->GetClassID() != GameObject::eStage&& spStageObj->GetClassID() != GameObject::eDestuctible && spStageObj->GetClassID() != GameObject::eGimmick) { continue; }

		BumpResult result;

		//�ǔ���
		SphereInfo sphereInfo(GetPos() + m_bumpSphereInfo.m_pos, m_bumpSphereInfo.m_radius);

		if (spStageObj->CheckCollisionBump(sphereInfo,result))
		{
			m_worldPos += result.m_pushVec;
			
		}

		//�n�ʔ���
		Math::Vector3 rayPos = m_prevPos;
		//�@�����Ĉړ��ł���n�ʂ̌��E�̒i��
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
//		owner.ChangeAttack();
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

	Math::Vector3 attackVec = owner.m_mWorld.Backward();

	attackVec.Normalize();
	attackVec *= 0.05f;

	owner.m_worldPos.x += attackVec.x;
	owner.m_worldPos.z += attackVec.z;
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
