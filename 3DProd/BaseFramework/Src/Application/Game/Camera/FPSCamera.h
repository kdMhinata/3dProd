#pragma once


class FPSCamera : public KdCamera
{
public:
	FPSCamera() {};
	~FPSCamera() {};

	void Init();
	void Update();

	const Math::Matrix GetRotationMatrix();
	const Math::Matrix GetRotationYMatrix()
	{
		return Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng.y));
	}

	const Math::Vector3& GetRotationAngles() const { return m_degAng; }

	virtual void SetCameraMatrix(const DirectX::SimpleMath::Matrix& mWorld) override;

	void SetLocalPos(const Math::Vector3& pos) { m_localPos = pos; }

	void SetClampAngle(float minDeg, float maxDeg) { m_miniAngX = minDeg; m_maxAngX = maxDeg; }

	void SetRotationSpeed(float sp){m_rotSpd=sp;}

	static const POINT s_fixMousePos;

protected:
	Math::Vector3 m_localPos;	// ローカル座標

private:
	Math::Vector3 m_degAng;	// カメラの回転

	// 回転角度の限界値
	float m_miniAngX = -FLT_MAX;
	float m_maxAngX = FLT_MAX;

	float m_rotSpd = 0.25;
};