#pragma once

struct KdSphereResult
{
	float			m_distance = FLT_MAX;	// お互いの球の間の距離が入る
	bool			m_hit = false;			// ヒットしたかどうか
};

struct KdRayResult
{
	float			m_distance = FLT_MAX;	// レイの座標からヒットした位置までの距離が入る
	Math::Vector3	m_HitPos;
	bool			m_hit = false;			// ヒットしたかどうか
};

// 球 vs 球判定 : 判定結果だけで良い場合はリザルトにnullptrを渡しておく
bool KdSphereToSphere(const DirectX::XMVECTOR& rPos1, const DirectX::XMVECTOR& rPos2, float radius1,
	float radius2, KdSphereResult* pResult = nullptr);

// レイ vs メッシュ判定 : 判定結果だけで良い場合はリザルトにnullptrを渡しておく
bool KdRayToMesh(const DirectX::XMVECTOR& rRayPos, const DirectX::XMVECTOR& rRayDir, float maxDistance,
	const KdMesh& rMesh, const DirectX::XMMATRIX& rMatrix, KdRayResult* pResult = nullptr);

// 球 vs メッシュ判定
bool KdSphereToMesh(const Math::Vector3& rSpherePos, float radius,
	const KdMesh& mesh, const DirectX::XMMATRIX& matrix, Math::Vector3& rPushedPos );

// 点 vs 三角形との最近接点を求める
void KdPointToTriangle(const DirectX::XMVECTOR& p, const DirectX::XMVECTOR& a,
	const DirectX::XMVECTOR& b, const DirectX::XMVECTOR& c, DirectX::XMVECTOR& outPt);