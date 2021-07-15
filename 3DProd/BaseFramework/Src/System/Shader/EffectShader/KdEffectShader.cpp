#include "System/KdSystem.h"

#include "KdEffectShader.h"


void KdEffectShader::DrawLine(const Math::Vector3 & p1, const Math::Vector3 & p2, const Math::Color & color)
{
	// 定数バッファ書き込み
	m_cb0.Write();

	// 頂点レイアウトをセット
	D3D.WorkDevContext()->IASetInputLayout(m_inputLayout);

	// 
	Vertex vertex[2] = {
		{p1, {0,0}, color},
		{p2, {1,0}, color},
	};

	// 頂点を描画
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, 2, &vertex[0], sizeof(Vertex));
}

void KdEffectShader::DrawVertices(const std::vector<Vertex>& vertices, D3D_PRIMITIVE_TOPOLOGY topology)
{
	// 定数バッファ書き込み
	m_cb0.Write();

	// 頂点レイアウトをセット
	D3D.WorkDevContext()->IASetInputLayout(m_inputLayout);

	// 頂点を描画
	D3D.DrawVertices(topology, vertices.size(), &vertices[0], sizeof(Vertex));
}

void KdEffectShader::DrawMesh(const KdMesh* mesh, const std::vector<KdMaterial>& materials)
{
	if (mesh == nullptr)return;

	// 定数バッファ書き込み
	m_cb0.Write();

	// メッシュ情報をセット
	mesh->SetToDevice();

	// 頂点レイアウトをセット
	D3D.WorkDevContext()->IASetInputLayout(m_inputLayout_Model);

	// 全サブセット
	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		// 面が１枚も無い場合はスキップ
		if (mesh->GetSubsets()[subi].FaceCount == 0)continue;

		// マテリアルセット
		const KdMaterial& material = materials[ mesh->GetSubsets()[subi].MaterialNo ];

		//-----------------------
		// マテリアル情報を定数バッファへ書き込む
		//-----------------------
		m_cb1_Material.Work().BaseColor = material.BaseColor;
		m_cb1_Material.Write();

		//-----------------------
		// テクスチャセット
		//-----------------------

		ID3D11ShaderResourceView* srvs[1] = {};
		// BaseColor
		srvs[0] = material.BaseColorTex->WorkSRView();
		// セット
		D3D.WorkDevContext()->PSSetShaderResources(0, _countof(srvs), srvs);

		//-----------------------
		// サブセット描画
		//-----------------------
		mesh->DrawSubset(subi);
	}
}

void KdEffectShader::DrawModel(const KdModelWork& rModel, const Math::Matrix& mWorld)
{
	// 有効じゃないときはスキップ
	if (!rModel.IsEnable()) { return; }

	const std::shared_ptr<KdModelData>& data = rModel.GetData();

	// モデルがないときはスキップ
	if (data == nullptr) { return; }

	// 全メッシュノードを描画
	for (auto& nodeIdx : data->GetMeshNodeIndices())
	{
		auto& rWorkNode = rModel.GetNodes()[nodeIdx];

		const std::shared_ptr<KdMesh>& spMesh = rModel.GetMesh(nodeIdx);

		// 行列セット
		SetWorldMatrix(rWorkNode.m_worldTransform * mWorld);

		// 描画
		DrawMesh(spMesh.get(), data->GetMaterials());
	}
}

bool KdEffectShader::Init()
{
	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdEffectShader_VS.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.WorkDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		{
			// １頂点の詳細な情報
			std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			// 頂点入力レイアウト作成
			if (FAILED(D3D.WorkDev()->CreateInputLayout(
				&layout[0],			// 入力エレメント先頭アドレス
				layout.size(),		// 入力エレメント数
				&compiledBuffer[0],				// 頂点バッファのバイナリデータ
				sizeof(compiledBuffer),			// 上記のバッファサイズ
				&m_inputLayout))					// 
				) {
				assert(0 && "CreateInputLayout失敗");
				Release();
				return false;
			}
		}

		// DrawModel用
		{
			// １頂点の詳細な情報
			std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			// 頂点入力レイアウト作成
			if (FAILED(D3D.WorkDev()->CreateInputLayout(
				&layout[0],			// 入力エレメント先頭アドレス
				layout.size(),		// 入力エレメント数
				&compiledBuffer[0],				// 頂点バッファのバイナリデータ
				sizeof(compiledBuffer),			// 上記のバッファサイズ
				&m_inputLayout_Model))					// 
				) {
				assert(0 && "CreateInputLayout失敗");
				Release();
				return false;
			}		
		}
	}


	//-------------------------------------
	// ピクセルシェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdEffectShader_PS.inc"

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

	return true;
}

void KdEffectShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_PS);
	KdSafeRelease(m_inputLayout);
	KdSafeRelease(m_inputLayout_Model);
	m_cb0.Release();
	m_cb1_Material.Release();
}

void KdEffectShader::SetToDevice()
{
	// 頂点シェーダをセット
	D3D.WorkDevContext()->VSSetShader(m_VS, 0, 0);
	// 頂点レイアウトをセット
	D3D.WorkDevContext()->IASetInputLayout(m_inputLayout);

	// ピクセルシェーダをセット
	D3D.WorkDevContext()->PSSetShader(m_PS, 0, 0);

	// 定数バッファをセット
	D3D.WorkDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.WorkDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());

	D3D.WorkDevContext()->VSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
	D3D.WorkDevContext()->PSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
}

// テクスチャセット

void KdEffectShader::SetTexture(ID3D11ShaderResourceView * srv)
{
	if (srv)
	{
		D3D.WorkDevContext()->PSSetShaderResources(0, 1, &srv);
	}
	else
	{
		// テクスチャが無い場合は、白テクスチャをセットする
		D3D.WorkDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->WorkSRViewAddress());
	}
}
