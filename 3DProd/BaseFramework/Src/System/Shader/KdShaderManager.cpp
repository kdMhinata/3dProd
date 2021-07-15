#include "System/KdSystem.h"

#include "KdShaderManager.h"

void KdShaderManager::Init()
{
	//============================================
	// シェーダ
	//============================================

	// 基本シェーダ
	m_standardShader.Init();
	// エフェクトシェーダ
	m_effectShader.Init();
	// 2Dスプライトシェーダ
	m_spriteShader.Init();

	//============================================
	// 定数バッファ
	//============================================

	// カメラ
	m_cb7_Camera.Create();
	D3D.WorkDevContext()->VSSetConstantBuffers(7, 1, m_cb7_Camera.GetAddress());
	D3D.WorkDevContext()->PSSetConstantBuffers(7, 1, m_cb7_Camera.GetAddress());

	// ライト
	m_cb8_Light.Create();

	m_cb8_Light.Work().DL_Dir.Normalize();

	m_cb8_Light.Write();
	D3D.WorkDevContext()->VSSetConstantBuffers(8, 1, m_cb8_Light.GetAddress());
	D3D.WorkDevContext()->PSSetConstantBuffers(8, 1, m_cb8_Light.GetAddress());

	//============================================
	// パイプラインステート関係
	//============================================

	//深度ステンシルステート作成
	m_ds_ZEnable_ZWriteEnable = D3D.CreateDepthStencilState(true, true);
	m_ds_ZEnable_ZWriteDisable = D3D.CreateDepthStencilState(true, false);
	m_ds_ZDisable_ZWriteDisable = D3D.CreateDepthStencilState(false, false);

	// ラスタライザステート作成
	m_rs_CullBack = D3D.CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, true, false);
	m_rs_CullNone = D3D.CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID, true, false);

	// ブレンドステート作成
	m_bs_Alpha = D3D.CreateBlendState(KdBlendMode::Alpha);
	m_bs_Add = D3D.CreateBlendState(KdBlendMode::Add);

	// サンプラーステート作成
	m_ss_Anisotropic_Wrap = D3D.CreateSamplerState(KdSamplerFilterMode::Anisotropic, 4, KdSamplerAddressingMode::Wrap, false);
	m_ss_Anisotropic_Clamp = D3D.CreateSamplerState(KdSamplerFilterMode::Anisotropic, 4, KdSamplerAddressingMode::Clamp, false);
	m_ss_Linear_Clamp = D3D.CreateSamplerState(KdSamplerFilterMode::Linear, 4, KdSamplerAddressingMode::Clamp, false);
	D3D.WorkDevContext()->PSSetSamplers(0, 1, &m_ss_Anisotropic_Wrap);
}

void KdShaderManager::Release()
{
	m_standardShader.Release();
	m_effectShader.Release();
	m_spriteShader.Release();

	m_cb7_Camera.Release();
	m_cb8_Light.Release();

	//深度ステンシルステート開放
	KdSafeRelease(m_ds_ZEnable_ZWriteEnable);
	KdSafeRelease(m_ds_ZEnable_ZWriteDisable);
	KdSafeRelease(m_ds_ZDisable_ZWriteDisable);

	// ラスタライザステート解放
	KdSafeRelease(m_rs_CullBack);
	KdSafeRelease(m_rs_CullNone);

	// ブレンドステート解放
	KdSafeRelease(m_bs_Alpha);
	KdSafeRelease(m_bs_Add);

	// サンプラーステート解放
	KdSafeRelease(m_ss_Anisotropic_Wrap);
	KdSafeRelease(m_ss_Anisotropic_Clamp);
	KdSafeRelease(m_ss_Linear_Clamp);
}
