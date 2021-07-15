#include"TPSCamera.h"

void TPSCamera::SetCameraMatrix(const Math::Matrix& mtrans)
{
	// キャラクターから注視点までのローカル座標
	Math::Matrix localGazePos;
	localGazePos = Math::Matrix::CreateTranslation(m_localGazePos);

	// 注視点からカメラまでのローカル座標
	Math::Matrix localPos;
	localPos = Math::Matrix::CreateTranslation(m_localPos);

	// カメラ回転
	Math::Matrix rotation;
	rotation = GetRotationMatrix();

	// ①注視点から5m離す　②注視点を中心に回転 ③3m上に上げる ④world座標
	KdCamera::SetCameraMatrix(localPos * rotation * localGazePos * mtrans);
}

