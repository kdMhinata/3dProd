#include "KdCamera.h"

// 射影行列の設定
void KdCamera::SetProjectionMatrix(float fov, float maxRange, float minRange, float aspectRatio)
{
	m_mProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspectRatio, minRange, maxRange);
	
	m_dirtyProj = true;
}

// カメラ行列・ビュー行列セット
void KdCamera::SetCameraMatrix(const DirectX::SimpleMath::Matrix& mCam)
{
	// カメラ行列セット
	m_mCam = mCam;

	// カメラ変更フラグON
	m_dirtyCam = true;
}

void KdCamera::ConvertWorldToScreenDetail(const Math::Vector3& pos, Math::Vector3& result)
{
	//ビューポートを取得する
	Math::Viewport vp;
	D3D.GetViewport(vp);

	//ワールド変換行列*ビュー行列*射影行列
	Math::Matrix world = Math::Matrix::CreateTranslation(pos);
	Math::Matrix wvp = world * GetViewMatrix() * GetProjMatrix();

	//奥行情報(W=._44で割る必要がある)
	wvp._41 /= wvp._44;
	wvp._42 /= wvp._44;
	wvp._43 /= wvp._44;

	//射影行列系での2D(みたいな)座標
	//-1~1の範囲の座標
	Math::Vector3 localPos = wvp.Translation();

	//ここで幅や高さを考慮して計算する(これで正確なスクリーン座標になる)
	result.x = localPos.x * (vp.width * 0.5f);
	result.y = localPos.y * (vp.height * 0.5f);
	result.z = wvp._44;

}

void KdCamera::ConvertScreenToWorldDetail(const Math::Vector2& pos, Math::Vector3& result)
{
	//ビューポートを取得する
	Math::Viewport vp;
	D3D.GetViewport(vp);

	//射影行列系に変換
	Math::Matrix trans = Math::Matrix::CreateTranslation(pos.x / (vp.width * 0.5f), pos.y / (vp.height * 0.5f), 0);

	//2D->3D変換用行列
	Math::Matrix convertMat = trans * GetProjMatrix().Invert() * GetViewMatrix().Invert();
	//奥行情報を元に戻す
	convertMat._41 /= convertMat._44;
	convertMat._42 /= convertMat._44;
	convertMat._43 /= convertMat._44;

	result = convertMat.Translation();

}

// カメラ情報(ビュー・射影行列など)をシェーダへセット
void KdCamera::SetToShader()
{
	// カメラが変更されていれば更新
	if (m_dirtyCam)
	{
		// カメラ行列からビュー行列を算出
		m_mView = m_mCam.Invert();

		// カメラ座標をセット
		SHADER->m_cb7_Camera.Work().CamPos = m_mCam.Translation();
		// ビュー行列をセット
		SHADER->m_cb7_Camera.Work().mView = m_mView;
	}

	// 射影行列が変更されていれば更新
	if (m_dirtyProj)
	{
		// 射影行列をセット
		SHADER->m_cb7_Camera.Work().mProj = m_mProj;
	}

	// 変更があれば書き込み
	if (m_dirtyCam || m_dirtyProj)
	{
		// カメラ情報(ビュー行列、射影行列)を、シェーダの定数バッファへ書き込む
		SHADER->m_cb7_Camera.Write();

		m_dirtyCam  = false;
		m_dirtyProj = false;
	}
}

DirectX::SimpleMath::Matrix& KdCamera::WorkCamera()
{
	// カメラ変更フラグON
	m_dirtyCam = true;

	// 更新可能なカメラ行列を返す
	return m_mCam;
}