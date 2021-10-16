#include "KdSquarePolygon.h" 

void KdSquarePolygon::Init(float w, float h, const Math::Vector4& _color)
{
	// 頂点座標
	m_vertex[0].pos = { -w / 2, -h / 2, 0 };
	m_vertex[1].pos = { -w / 2,  h / 2, 0 };
	m_vertex[2].pos = {  w / 2, -h / 2, 0 };
	m_vertex[3].pos = {  w / 2,  h / 2, 0 };
	// 頂点色
	m_vertex[0].color = _color;
	m_vertex[1].color = _color;
	m_vertex[2].color = _color;
	m_vertex[3].color = _color;
	// UV座標
	m_vertex[0].UV = { 0, 1 };
	m_vertex[1].UV = { 0, 0 };
	m_vertex[2].UV = { 1, 1 };
	m_vertex[3].UV = { 1, 0 };
}

void KdSquarePolygon::Draw(int setTextureNo) const
{
	// テクスチャセット
	if (m_texture)
	{
		D3D.WorkDevContext()->PSSetShaderResources(setTextureNo, 1, m_texture->WorkSRViewAddress());
	}
	else
	{
		D3D.WorkDevContext()->PSSetShaderResources(setTextureNo, 1, D3D.GetWhiteTex()->WorkSRViewAddress());
	}

	// 指定した頂点配列を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &m_vertex, sizeof(Vertex));
}

void KdSquarePolygon::SetAnimationPos(float no)
{
	// マス座標
	int x = (int)no % m_animSplitX;
	int y = (int)no / m_animSplitX;
	// １マスの幅と高さ
	float w = 1.0f / m_animSplitX;
	float h = 1.0f / m_animSplitY;

	// UV座標
	m_vertex[0].UV = { x * w,		(y + 1) * h };
	m_vertex[1].UV = { x * w,		y * h };
	m_vertex[2].UV = { (x + 1) * w,	(y + 1) * h };
	m_vertex[3].UV = { (x + 1) * w, 	y * h };

	// アニメーションの位置を更新
	m_animPos = no;
}

void KdSquarePolygon::Animation(float speed, bool loop)
{
	// アニメーション位置を進める
	m_animPos += speed;

	// 終了判定
	if (m_animPos >= (m_animSplitX * m_animSplitY))
	{

		// ループするとき
		if (loop)
		{
			m_animPos = 0;	// 巻き戻す
		}
		// ループしないとき
		else
		{
			// 最後のコマにする
			m_animPos = (float)(m_animSplitX * m_animSplitY) - 1;
		}
	}
	// UV更新
	SetAnimationPos(m_animPos);
}

bool KdSquarePolygon::IsAnimationEnd()
{
	// 終了判定
	if (m_animPos >= (m_animSplitX * m_animSplitY) - 1) { return true; }

	return false;
}
