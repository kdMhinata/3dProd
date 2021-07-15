#pragma once

// 四角形ポリゴンクラス
class KdSquarePolygon
{
public:

	// ポリゴン設定
	// w(幅), h(高さ), color(色)
	void Init(float w, float h, const Math::Vector4& _color);

	// 描画
	void Draw(int setTextureNo);

	// 画像をセット
	inline void SetTexture(const std::shared_ptr<KdTexture>& tex)
	{
		m_texture = tex;
	}

	// アニメーションの分割数を設定
	inline void SetAnimationInfo(int splitX, int splitY) {
		m_animSplitX = splitX;
		m_animSplitY = splitY;
	}

	// 分割設定をもとに、アニメーション位置設定
	// no	… 画像番号
	void SetAnimationPos(float no);

	// アニメーションを進行させる
	// ・speed		… 進行速度 1.0で１コマずつ
	// ・loop		… ループ再生する？
	void Animation(float speed, bool loop);

	// アニメーションの再生が終わった？
	bool IsAnimationEnd();

private:

	// アニメーション関係
	int		m_animSplitX = 1;	// 横の分割数
	int		m_animSplitY = 1;	// 縦の分割数

	float	m_animPos = 0;		// 現在のコマ位置

	// １頂点の形式
	struct Vertex
	{
		Math::Vector3 pos;
		Math::Vector2 UV;
		Math::Vector4 color;
	};

	// 頂点配列
	Vertex m_vertex[4];

	std::shared_ptr<KdTexture>	m_texture;
};