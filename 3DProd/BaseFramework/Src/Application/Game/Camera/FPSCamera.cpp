#include"FPSCamera.h"

const POINT FPSCamera::s_fixMousePos = { 640,360 };

void FPSCamera::Init()
{
	SetCursorPos(s_fixMousePos.x, s_fixMousePos.y);

}

void FPSCamera::Update()
{
	// カメラの回転処理
	POINT nowPos;	// 現在のマウス座標
	GetCursorPos(&nowPos);

	POINT mouseMove;
	mouseMove.x = nowPos.x - s_fixMousePos.x;		// 目的地ー出発地
	mouseMove.y = nowPos.y - s_fixMousePos.y;		// 目的地ー出発地

	SetCursorPos(s_fixMousePos.x, s_fixMousePos.y);

	// カメラ回転処理
	m_degAng.x += mouseMove.y * m_rotSpd;
	m_degAng.y += mouseMove.x * m_rotSpd;

	// X軸の回転制限
	m_degAng.x = std::clamp(m_degAng.x, m_miniAngX, m_maxAngX);
}

const Math::Matrix FPSCamera::GetRotationMatrix()
{
	return Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_degAng.y),
		DirectX::XMConvertToRadians(m_degAng.x),
		DirectX::XMConvertToRadians(m_degAng.z)
	);
}

void FPSCamera::SetCameraMatrix(const DirectX::SimpleMath::Matrix& mWorld)
{
	Math::Matrix localPos;
	localPos = Math::Matrix::CreateTranslation(m_localPos);

	Math::Matrix rotation;
	rotation = GetRotationMatrix();

	KdCamera::SetCameraMatrix(rotation * localPos * mWorld);
}


