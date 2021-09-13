#include"Attack.h"
#include"Enemy.h"


void Attack::Init()
{
	m_modelWork.SetModel(GameResourceFactory.GetModelData("Data/Models/Arrow/Arrow.gltf"));

	m_lifeSpan = 120;

	m_radius = 0.5f;
}

void Attack::Update()
{
	if (--m_lifeSpan < 0)
	{
		m_isAlive = false;
	}

	if (m_isStabbed) { return; }

	// 移動前の座標
	m_prevPos = m_mWorld.Translation();

	Math::Vector3 vMove = m_mWorld.Backward();

	vMove.Normalize();
	const float arrowSpd = 0.2f;
	vMove *= arrowSpd;

	// ワールド行列の作成
	Math::Matrix trans;
	trans = Math::Matrix::CreateTranslation(vMove);

	// 行列の合成
	m_mWorld *= trans;	// 前フレームの情報に重ねて合成

	UpdateRotate();

	// 当たり判定更新
	UpdateCollition();

	m_trail.AddPoint(m_mWorld);

	if (m_trail.GetNumPoints() > 30)
	{
		m_trail.DelPoint_Back();
	}
}

void Attack::Release()
{
	SetTarget(nullptr);
}

void Attack::UpdateRotate()
{
	RotateByGuide();
}

void Attack::RotateByGuide()
{
	// 見ている先が解放されているか
	if (m_wpTarget.expired()) { return; }

	std::shared_ptr<const GameObject> spTarget = m_wpTarget.lock();

	// ①自分の向いている方向
	Math::Vector3 nowDir = m_mWorld.Backward();

	//追加
	Math::Vector3 targetPos = spTarget->GetPos();
	targetPos.y += 1.0f;

	// ②ターゲットの方向
	Math::Vector3 targetDir = targetPos - m_mWorld.Translation();

	nowDir.Normalize();
	targetDir.Normalize();

	// 間の角度を求める
	// ーーーーーーーーーーーーーーーーーーーーーーーーー
	// ①と②の内積を求める
	float dot = nowDir.Dot(targetDir);

	// 内積の計算上の誤差修正
	dot = std::clamp(dot, -1.0f, 1.0f);

	// 内積から逆余弦の定理で間の角度を求める
	float betweenAng = acos(dot);
	betweenAng = DirectX::XMConvertToDegrees(betweenAng);

	float rotateAng = std::clamp(betweenAng, -TurnAng, TurnAng);

	// 3D上の回転軸を求める
	// ーーーーーーーーーーーーーーーーーーーーーーーーー
	Math::Vector3 rotAxis;

	// 二つのベクトルから垂直に伸びるベクトル
	nowDir.Cross(targetDir, rotAxis);

	// 二つのベクトルが同じ角度、また真反対の角度の時
	if (!rotAxis.LengthSquared()) { return; }

	// 行列作成
	// ーーーーーーーーーーーーーーーーーーーーーーーーー
	Math::Matrix rotation;

	rotation = Math::Matrix::CreateFromAxisAngle(rotAxis, DirectX::XMConvertToRadians(rotateAng));

	// 座標行列を取り出すため、現在の座標を一時保存
	Math::Vector3 pos = m_mWorld.Translation();

	// 座標情報を抜く
	m_mWorld.Translation({ 0.0f, 0.0f, 0.0f });

	//　回転行列の合成 累積行列 × 1フレームの行列
	m_mWorld *= rotation;

	m_mWorld.Translation(pos);
}

void Attack::UpdateCollition()
{
	/*for (const auto& spObj : GameSystem::GetInstance().GetObjects())
	{
		if (spObj->GetClassID() != GameObject::eEnemy) { continue; }

		bool result = spObj->CheckCollision(GetPos(), m_radius);

		if (result) { m_isAlive = false; }
	}*/

	for (const auto& spObj : GameSystem::GetInstance().GetObjects())
	{
		if (spObj->GetClassID() != GameObject::eStage) { continue; }

		Math::Vector3 rayPos = m_prevPos;

		Math::Vector3 rayDir = GetPos() - m_prevPos;//

		RayInfo info(rayPos, rayDir, rayDir.Length());

		BumpResult result;

		spObj->CheckCollisionBump(info, result);

		if (result.m_isHit)
		{
			m_isStabbed = true;

			// めり込んだ分を押し返す
			m_mWorld *= Math::Matrix::CreateTranslation(result.m_pushVec);
		}
	}
}
