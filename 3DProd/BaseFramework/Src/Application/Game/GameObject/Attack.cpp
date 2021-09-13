#include"Attack.h"
#include"Enemy.h"


void Attack::Init()
{
	m_modelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/Arrow/Arrow.gltf"));

	m_lifeSpan = 120;

	m_radius = 0.5f;
}

void Attack::Update()
{
	if (--m_lifeSpan < 0)
	{
		m_isAlive = false;
	}

	if (m_isStabbed) { return; }

	// �ړ��O�̍��W
	m_prevPos = m_mWorld.Translation();

	Math::Vector3 vMove = m_mWorld.Backward();

	vMove.Normalize();
	const float arrowSpd = 0.2f;
	vMove *= arrowSpd;

	// ���[���h�s��̍쐬
	Math::Matrix trans;
	trans = Math::Matrix::CreateTranslation(vMove);

	// �s��̍���
	m_mWorld *= trans;	// �O�t���[���̏��ɏd�˂č���

	UpdateRotate();

	// �����蔻��X�V
	UpdateCollition();

	m_trail.AddPoint(m_mWorld);

	if (m_trail.GetNumPoints() > 30)
	{
		m_trail.DelPoint_Back();
	}
}

void Attack::Release()
{
	SetTarget(nullptr);
}

void Attack::UpdateRotate()
{
	RotateByGuide();
}

void Attack::RotateByGuide()
{
	// ���Ă���悪�������Ă��邩
	if (m_wpTarget.expired()) { return; }

	std::shared_ptr<const GameObject> spTarget = m_wpTarget.lock();

	// �@�����̌����Ă������
	Math::Vector3 nowDir = m_mWorld.Backward();

	//�ǉ�
	Math::Vector3 targetPos = spTarget->GetPos();
	targetPos.y += 1.0f;

	// �A�^�[�Q�b�g�̕���
	Math::Vector3 targetDir = targetPos - m_mWorld.Translation();

	nowDir.Normalize();
	targetDir.Normalize();

	// �Ԃ̊p�x�����߂�
	// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	// �@�ƇA�̓��ς����߂�
	float dot = nowDir.Dot(targetDir);

	// ���ς̌v�Z��̌덷�C��
	dot = std::clamp(dot, -1.0f, 1.0f);

	// ���ς���t�]���̒藝�ŊԂ̊p�x�����߂�
	float betweenAng = acos(dot);
	betweenAng = DirectX::XMConvertToDegrees(betweenAng);

	float rotateAng = std::clamp(betweenAng, -TurnAng, TurnAng);

	// 3D��̉�]�������߂�
	// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	Math::Vector3 rotAxis;

	// ��̃x�N�g�����琂���ɐL�т�x�N�g��
	nowDir.Cross(targetDir, rotAxis);

	// ��̃x�N�g���������p�x�A�܂��^���΂̊p�x�̎�
	if (!rotAxis.LengthSquared()) { return; }

	// �s��쐬
	// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	Math::Matrix rotation;

	rotation = Math::Matrix::CreateFromAxisAngle(rotAxis, DirectX::XMConvertToRadians(rotateAng));

	// ���W�s������o�����߁A���݂̍��W���ꎞ�ۑ�
	Math::Vector3 pos = m_mWorld.Translation();

	// ���W���𔲂�
	m_mWorld.Translation({ 0.0f, 0.0f, 0.0f });

	//�@��]�s��̍��� �ݐύs�� �~ 1�t���[���̍s��
	m_mWorld *= rotation;

	m_mWorld.Translation(pos);
}

void Attack::UpdateCollition()
{
	/*for (const auto& spObj : GameSystem::GetInstance().GetObjects())
	{
		if (spObj->GetClassID() != GameObject::eEnemy) { continue; }

		bool result = spObj->CheckCollision(GetPos(), m_radius);

		if (result) { m_isAlive = false; }
	}*/

	for (const auto& spObj : GameSystem::GetInstance().GetObjects())
	{
		if (spObj->GetClassID() != GameObject::eStage) { continue; }

		Math::Vector3 rayPos = m_prevPos;

		Math::Vector3 rayDir = GetPos() - m_prevPos;//

		RayInfo info(rayPos, rayDir, rayDir.Length());

		BumpResult result;

		spObj->CheckCollisionBump(info, result);

		if (result.m_isHit)
		{
			m_isStabbed = true;

			// �߂荞�񂾕��������Ԃ�
			m_mWorld *= Math::Matrix::CreateTranslation(result.m_pushVec);
		}
	}
}
