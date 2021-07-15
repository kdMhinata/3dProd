
//===========================================
//
// 定数バッファ
//  ゲームプログラム側から持ってくるデータ
//
//===========================================

// 定数バッファ(オブジェクト単位更新)
cbuffer cbPerObject : register(b0)
{
	// オブジェクト情報
	row_major float4x4 g_mWorld; // ワールド変換行列

	// UV関係
	float2 g_UVOffset;
	float2 g_UVTiling;
};

// マテリアル構造体
struct Material
{
	float4 BaseColor; // ベース色
};

cbuffer cbMaterial : register(b1)
{
	Material g_Material;
};

//===========================================
// 頂点シェーダから出力するデータ
//===========================================
struct VSOutput
{
	float4 Pos : SV_Position;   // 射影座標
	float2 UV : TEXCOORD0;      // UV座標
	float4 Color : TEXCOORD1;   // 色
};
