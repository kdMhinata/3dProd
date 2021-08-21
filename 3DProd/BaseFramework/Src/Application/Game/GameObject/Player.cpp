#include"Player.h"
//#include"../Camera/FPSCamera.h"
#include"../Camera/TPSCamera.h"
#include"../GameSystem.h"

Player::Player()
{
}

Player::~Player()
{
	Release();
}

void Player::Init()
{
	m_modelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/Robot/Robot.gltf"));

	m_spCamera = std::make_shared<TPSCamera>();

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
}

// �X�V����
void Player::Update()
{
	Math::Vector3 vMove;

	// �ړ��̍X�V����
	UpdateMove(vMove);

	// ��]�̍X�V����
	UpdateRotate(vMove);

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

	if (GetAsyncKeyState('W') & 0x8000) { moveVec.z += 1.0f; }	// �O�ړ�
	if (GetAsyncKeyState('S') & 0x8000) { moveVec.z -= 1.0f; }	// ���ړ�
	if (GetAsyncKeyState('A') & 0x8000) { moveVec.x -= 1.0f; }	// ���ړ�
	if (GetAsyncKeyState('D') & 0x8000) { moveVec.x += 1.0f; }	// �E�ړ�

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

	m_mWorld = rotation * trans;
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
