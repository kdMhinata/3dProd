#pragma once

#include "EffectShader/KdEffectShader.h"
#include "StandardShader/KdStandardShader.h"
#include "SpriteShader/KdSpriteShader.h"

//==========================================================
//
// シェーダ関係をまとめたクラス
//
//==========================================================
class KdShaderManager
{
public:

	KdShaderManager() {}

	~KdShaderManager() { Release(); }

	//==============================================================
	//
	// 初期化・解放
	//
	//==============================================================

	// 初期化
	void Init();

	//==========================
	//
	// シェーダ
	//
	//==========================
	KdStandardShader		m_standardShader;		// 3Dモデル描画シェーダ
	KdEffectShader			m_effectShader;			// エフェクト描画シェーダ
	KdSpriteShader			m_spriteShader;			// 2Dテクスチャ描画シェーダ

	//==========================
	//
	// カメラ関係
	//
	//==========================

	// カメラ定数バッファ用構造体
	struct cbCamera
	{
		Math::Matrix		mView;		// ビュー行列
		Math::Matrix		mProj;		// 射影行列
		Math::Vector3		CamPos;		// カメラのワールド座標

		int					DistanceFogEnable = 0;			// 距離フォグ有効
		Math::Vector3		DistanceFogColor = { 1, 1, 1 };	// 距離フォグ色
		float				DistanceFogDensity = 0.001f;	// 距離フォグ減衰率
	};

	// カメラ定数バッファ
	KdConstantBuffer<cbCamera>	m_cb7_Camera;

	//==========================
	//
	// ライト関係
	//
	//==========================

	// ライト定数バッファ用構造体
	struct cbLight
	{
		// 環境光
		Math::Vector3		AmbientLight = { 0.3f, 0.3f, 0.3f };
		float				_blank1;

		// 平行光
		Math::Vector3		DL_Dir = { 1.0f,-1.0f,1.0f };    // 光の方向
		float				_blank2;
		Math::Vector3		DL_Color = { 1.0f,1.0f,1.0f };  // 光の色
		float				_blank3;
	};

	// ライト定数バッファ
	KdConstantBuffer<cbLight>		m_cb8_Light;

	//==========================
	//
	// パイプラインステート
	//
	//==========================

	//深度ステンシル
	ID3D11DepthStencilState* m_ds_ZEnable_ZWriteEnable = nullptr;		// 深度バッファの利用ON、 深度に書き込みON
	ID3D11DepthStencilState* m_ds_ZEnable_ZWriteDisable = nullptr;		// 深度バッファの利用ON、 深度に書き込みOFF
	ID3D11DepthStencilState* m_ds_ZDisable_ZWriteDisable = nullptr;		// 深度バッファの利用OFF、深度に書き込みOFF
	
	// ラスタライズステート
	ID3D11RasterizerState* m_rs_CullBack = nullptr;			// 描画カリングモード：裏面の描画省略
	ID3D11RasterizerState* m_rs_CullNone = nullptr;			// 描画カリングモード：描画省略なし

	// ブレンドステート
	ID3D11BlendState* m_bs_Alpha = nullptr;					// 透明度による色合成
	ID3D11BlendState* m_bs_Add = nullptr;					// 加算による色合成

	// サンプラーステート
	ID3D11SamplerState* m_ss_Anisotropic_Wrap = nullptr;	// 異方性補間：テクスチャ繰り返し
	ID3D11SamplerState* m_ss_Anisotropic_Clamp = nullptr;	// 異方性補間：テクスチャ範囲外は端のピクセルを延長
	ID3D11SamplerState* m_ss_Linear_Clamp = nullptr;		// 線形補間：　テクスチャ範囲外は端のピクセルを延長

private:

	// 解放
	void Release();
};
