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

	// アニメーション進行
	if (m_poly.GetAnimationSize() >= 1)
	{
		m_poly.Animation(m_animSpd, m_isLoop);
	}

	// 寿命管理
	if (--m_lifeSpan <= 0)
	{
		m_isAlive = false;
	}
	if (m_poly.IsAnimationEnd())
	{
		m_isAlive = false;
	}

	/*
	//アニメーションが無いならアニメーションの更新・終了判定を行わない
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
		//描画用の行列
		Math::Matrix mDraw;

		//行列の拡大=S
		//----------------------------------------------------------
		Math::Vector3 scale;
		scale.x = m_mWorld.Right().Length();
		scale.y = m_mWorld.Up().Length();
		scale.z = m_mWorld.Forward().Length();
		mDraw = Math::Matrix::CreateScale(scale);

		//行列の回転=R
		//----------------------------------------------------------
		std::shared_ptr<KdCamera> gameCam = GameInstance.GetCamera();

		if (gameCam)
		{
			//カメラの逆行列の合成=ワールド行列->カメラから見た行列にする
			Math::Matrix mCamInv = gameCam->GetCameraMatrix();
			mCamInv.Invert();

			//カメラの目の前に持ってくる
			mDraw *= mCamInv;
		}

		//行列の移動=T
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
	// 0コマ目にする
	m_poly.SetAnimationPos(0);

	m_animSpd = speed;
	m_isLoop = isLoop;
}
