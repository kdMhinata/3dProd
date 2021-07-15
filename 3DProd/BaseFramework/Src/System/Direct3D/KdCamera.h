#pragma once

//=========================================
// カメラクラス
//=========================================
class KdCamera
{
public:
	// コンストラクター　オーナーの設定と射影行列の作成
	KdCamera() {}

	~KdCamera() {}

	void SetProjectionMatrix(float fov, float maxRange = 2000, float minRange = 0.01f, float aspectRatio = D3D.WorkBackBuffer()->GetAspectRatio());
	void SetProjectionMatrix(const DirectX::SimpleMath::Matrix& rProj) { m_mProj = rProj; m_dirtyProj = true; }

	// カメラ行列取得
	inline const DirectX::SimpleMath::Matrix& GetCameraMatrix() const { return m_mCam; }

	// ビュー行列取得
	inline const DirectX::SimpleMath::Matrix& GetViewMatrix() const { return m_mView; }
	
	// 射影行列取得
	inline const DirectX::SimpleMath::Matrix& GetProjMatrix() const { return m_mProj; }

	// カメラ行列・ビュー行列
	virtual void SetCameraMatrix(const DirectX::SimpleMath::Matrix& mCam);

	// カメラ情報(ビュー・射影行列など)をシェーダへセット
	void SetToShader();

	DirectX::SimpleMath::Matrix& WorkCamera();

protected:

	// カメラ行列
	DirectX::SimpleMath::Matrix	m_mCam;
	// ビュー行列
	DirectX::SimpleMath::Matrix	m_mView;
	// 射影行列
	DirectX::SimpleMath::Matrix	m_mProj;

private:
	bool m_dirtyCam = false;
	bool m_dirtyProj = false;
};
