#include "Effect2D.h"

#include "Character.h"

void Effect2D::Init()
{
	m_poly.Init(1.0f, 1.0f, kWhiteColor);
}

void Effect2D::Update()
{
	auto owner = m_ownerChara.lock();
	if (owner)
	{
		if (owner->m_hitStop >= 1)return;
	}

	// �A�j���[�V�����i�s
	if (m_poly.GetAnimationSize() >= 1)
	{
		m_poly.Animation(m_animSpd, m_isLoop);
	}

	// �����Ǘ�
	if (--m_lifeSpan <= 0)
	{
		m_isAlive = false;
	}
	if (m_poly.IsAnimationEnd())
	{
		m_isAlive = false;
	}

	/*
	//�A�j���[�V�����������Ȃ�A�j���[�V�����̍X�V�E�I��������s��Ȃ�
	if (m_poly.GetAnimationSize() <= 1) { return; }

	m_poly.Animation(m_animSpd, m_isLoop);

	if (m_poly.IsAnimationEnd())
	{
		m_isAlive = false;
	}
	*/
}


void Effect2D::DrawEffect()
{
	if (m_isBillboard)
	{
		//�`��p�̍s��
		Math::Matrix mDraw;

		//�s��̊g��=S
		//----------------------------------------------------------
		Math::Vector3 scale;
		scale.x = m_mWorld.Right().Length();
		scale.y = m_mWorld.Up().Length();
		scale.z = m_mWorld.Forward().Length();
		mDraw = Math::Matrix::CreateScale(scale);

		//�s��̉�]=R
		//----------------------------------------------------------
		std::shared_ptr<KdCamera> gameCam = GameInstance.GetCamera();

		if (gameCam)
		{
			//�J�����̋t�s��̍���=���[���h�s��->�J�������猩���s��ɂ���
			Math::Matrix mCamInv = gameCam->GetCameraMatrix();
			mCamInv.Invert();

			//�J�����̖ڂ̑O�Ɏ����Ă���
			mDraw *= mCamInv;
		}

		//�s��̈ړ�=T
		//-------------------------------------------------------------
		mDraw.Translation(m_mWorld.Translation());

		if (m_localModeFlg)
		{
			SHADER->m_effectShader.DrawSquarePolygon(m_poly, m_ownerChara.lock()->GetMatrix());
		}
		else
		{
			SHADER->m_effectShader.DrawSquarePolygon(m_poly, mDraw);
		}
		
	}
	else
	{
		if (m_localModeFlg)
		{
			SHADER->m_effectShader.DrawSquarePolygon(m_poly, m_ownerChara.lock()->GetMatrix());
		}
		else
		{
			SHADER->m_effectShader.DrawSquarePolygon(m_poly, m_mWorld);
		}
	}
}
void Effect2D::SetTexture(const std::shared_ptr<KdTexture> spTex, float w, float h, Math::Color col)
{
	if (!spTex) { return; }
	m_poly.SetTexture(spTex);

	m_poly.Init(w, h, col);
}

void Effect2D::SetAnimation(int splitX, int splitY, float speed, bool isLoop)
{
	m_poly.SetAnimationInfo(splitX, splitY);
	// 0�R�}�ڂɂ���
	m_poly.SetAnimationPos(0);

	m_animSpd = speed;
	m_isLoop = isLoop;
}
