#pragma once

#include"FPSCamera.h"

class TPSCamera : public FPSCamera
{
public:
	TPSCamera() {}
	~TPSCamera(){}
	
	void SetCameraMatrix(const Math::Matrix& mtrans) override;

	void SetLocalGazePos(const Math::Vector3& localPos) { m_localGazePos = localPos;}

private:

	// キャラ原点から注視点の相対座標
	Math::Vector3	m_localGazePos;

};
