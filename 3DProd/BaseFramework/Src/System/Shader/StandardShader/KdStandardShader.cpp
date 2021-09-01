#include "System/KdSystem.h"

#include "KdStandardShader.h"


bool KdStandardShader::Init()
{
	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdStandardShader_VS.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.WorkDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点入力レイアウト作成
		if (FAILED(D3D.WorkDev()->CreateInputLayout(
			&layout[0],				// 入力エレメント先頭アドレス
			layout.size(),			// 入力エレメント数
			&compiledBuffer[0],		// 頂点バッファのバイナリデータ
			sizeof(compiledBuffer),	// 上記のバッファサイズ
			&m_inputLayout))
		) {
			assert(0 && "CreateInputLayout失敗");
			Release();
			return false;
		}
	}
	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdStandardShader_VS_Skin.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.WorkDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS_Skin))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SKININDEX",	0, DXGI_FORMAT_R16G16B16A16_SINT,	0, 36,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SKINWEIGHT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 44,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点入力レイアウト作成
		if (FAILED(D3D.WorkDev()->CreateInputLayout(
			&layout[0],				// 入力エレメント先頭アドレス
			layout.size(),			// 入力エレメント数
			&compiledBuffer[0],		// 頂点バッファのバイナリデータ
			sizeof(compiledBuffer),	// 上記のバッファサイズ
			&m_inputLayout_Skin))
			) {
			assert(0 && "CreateInputLayout失敗");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// ピクセルシェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdStandardShader_PS.inc"

		if (FAILED(D3D.WorkDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// 定数バッファ作成
	//-------------------------------------
	m_cb0.Create();
	m_cb1_Material.Create();
	m_cb2_Bones.Create();

	return true;
}

void KdStandardShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_VS_Skin);
	KdSafeRelease(m_inputLayout);
	KdSafeRelease(m_inputLayout_Skin);
	KdSafeRelease(m_PS);

	m_cb0.Release();
	m_cb1_Material.Release();
	m_cb2_Bones.Release();
}

void KdStandardShader::SetToDevice()
{
	D3D.WorkDevContext()->PSSetShader(m_PS, 0, 0);

	// 定数バッファをセット
	D3D.WorkDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.WorkDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());

	D3D.WorkDevContext()->VSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
	D3D.WorkDevContext()->PSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());

	D3D.WorkDevContext()->VSSetConstantBuffers(2, 1, m_cb2_Bones.GetAddress());
	D3D.WorkDevContext()->PSSetConstantBuffers(2, 1, m_cb2_Bones.GetAddress());
}

void KdStandardShader::DrawMesh(const KdMesh* mesh, const std::vector<KdMaterial>& materials)
{
	if (mesh == nullptr)return;

	// StaticMeshとSkinMeshによって頂点シェーダーと入力レイアウトを切り替える
	if (mesh->IsSkinMesh())
	{
		// 頂点シェーダをセット
		D3D.WorkDevContext()->VSSetShader(m_VS_Skin, 0, 0);
		// 頂点レイアウトをセット
		D3D.WorkDevContext()->IASetInputLayout(m_inputLayout_Skin);

		m_cb2_Bones.Write();
	}
	else
	{
		// 頂点シェーダをセット
		D3D.WorkDevContext()->VSSetShader(m_VS, 0, 0);
		// 頂点レイアウトをセット
		D3D.WorkDevContext()->IASetInputLayout(m_inputLayout);
	}

	// 定数バッファ書き込み
	m_cb0.Write();

	// メッシュ情報をセット
	mesh->SetToDevice();

	// 全サブセット
	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		// 面が１枚も無い場合はスキップ
		if (mesh->GetSubsets()[subi].FaceCount == 0)continue;

		// マテリアルセット
		const KdMaterial& material = materials[mesh->GetSubsets()[subi].MaterialNo];

		//-----------------------
		// マテリアル情報を定数バッファへ書き込む
		//-----------------------
		m_cb1_Material.Work().BaseColor = material.BaseColor;
		m_cb1_Material.Work().Emissive = material.Emissive;
		m_cb1_Material.Work().Metallic = material.Metallic;
		m_cb1_Material.Work().Roughness = material.Roughness;
		m_cb1_Material.Write();

		//-----------------------
		// テクスチャセット
		//-----------------------
		ID3D11ShaderResourceView* srvs[3] = {};

		// BaseColor
		srvs[0] = material.BaseColorTex->WorkSRView();
		// Emissive
		srvs[1] = material.EmissiveTex->WorkSRView();
		// Metallic Roughness
		srvs[2] = material.MetallicRoughnessTex->WorkSRView();

		// セット
		D3D.WorkDevContext()->PSSetShaderResources(0, _countof(srvs), srvs);

		//-----------------------
		// サブセット描画
		//-----------------------
		mesh->DrawSubset(subi);
	}
}

void KdStandardShader::DrawModel(const KdModelWork& rModel, const Math::Matrix& mWorld)
{
	// 有効じゃないときはスキップ
	if (!rModel.IsEnable()) { return; }

	const std::shared_ptr<KdModelData>& data = rModel.GetData();

	auto& workNodes = rModel.GetNodes();

	// モデルがないときはスキップ
	if (data == nullptr) { return; }

	// スキンメッシュモデルの場合：ボーン情報を書き込み
	if (data->IsSkinMesh())
	{
		// ノード内からボーン情報を取得
		auto& dataNodes = data->GetOriginalNodes();

		for (auto&& nodeIdx : data->GetBoneNodeIndices())
		{
			if (nodeIdx >= KdStandardShader::maxBoneBufferSize) { assert(0 && "転送できるボーンの上限数を超えました"); return; }

			auto& dataNode = dataNodes[nodeIdx];
			auto& workNode = workNodes[nodeIdx];

			// ボーン情報からGPUに渡す行列の計算
			SHADER->m_standardShader.BoneCB().Work().mBones[dataNode.m_boneIndex] = dataNode.m_boneInverseWorldMatrix * workNode.m_worldTransform;
		}
	}

	// 全メッシュノードを描画
	for (auto& nodeIdx : data->GetMeshNodeIndices())
	{
		auto& rWorkNode = workNodes[nodeIdx];

		const std::shared_ptr<KdMesh>& spMesh = rModel.GetMesh(nodeIdx);

		// 行列セット
		SetWorldMatrix(rWorkNode.m_worldTransform * mWorld);

		// 描画
		DrawMesh(spMesh.get(), data->GetMaterials());
	}
}