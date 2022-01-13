#include "Gimmick.h"

class FadeIn : public GameObject
{
public:

	void Update()
	{
		GameInstance.BlackOut(-0.01f);
		Destroy();
	}
};

void Gimmick::Update()
{	
	if (!m_clearFlg)
	{
		if (GameInstance.FindObjectWithTag("Enemy") == nullptr)
		{
			m_clearFlg = true;
		}
	}

	if (m_active==1)
	{

		if (GameInstance.GetBlackoutRate() >= 1.0f)
		{
			GameInstance.ReserveChangeScene("Data/Save/Dungeon2");
			std::shared_ptr<FadeIn> spFadeIn = std::make_shared<FadeIn>();
			spFadeIn->Update();
			//GameInstance.EnterStage();
		}
	}
}

bool Gimmick::CheckCollisionBump(const SphereInfo& info, BumpResult& result)
{
	if (m_active == 0)
	{
		for (UINT i = 0; i < m_modelWork.GetDataNodes().size(); ++i)
		{
			const KdModelData::Node& dataNode = m_modelWork.GetDataNodes()[i];

			// メッシュがない場合飛ばす
			if (!dataNode.m_spMesh) { continue; }

			const KdModelWork::Node& workNode = m_modelWork.GetNodes()[i];

			// 押し返された座標
			Math::Vector3 localPushedPos;

			// メッシュと球との当たり判定
			if (KdSphereToMesh(info.m_pos, info.m_radius, *(dataNode.m_spMesh.get()),
				workNode.m_worldTransform * m_mWorld, localPushedPos))
			{
				result.m_isHit = true;

				if (m_clearFlg)
				{
					// 
					std::shared_ptr<GameObject>& obj = GameInstance.FindObjectWithTag("Player");

					auto player = std::dynamic_pointer_cast<Player>(obj);

					if (player == nullptr) { return result.m_isHit; }

					//行列をプレイヤー用の角度に変換
					Math::Vector3 vec = MatToAngle(GetMatrix());

					player->Exit(vec);

					//プレイヤーを一時的に保存しておく
					GameInstance.SaveWaitingRoom(player);

					GameInstance.BlackOut(0.01f);


					m_active = 1;
				}
				else
				{
					result.m_pushVec += localPushedPos - info.m_pos;
				}
			}
		}
		if (!result.m_isHit)
		{
			m_active = 0;
		}
	}

	return result.m_isHit;
}
