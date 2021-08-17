#include"GameSystem.h"
#include"GameObject/StageMap.h"
#include"GameObject/Player.h"
#include"GameObject/Enemy.h"

#include"Camera/TPSCamera.h"

#include"../main.h";

void GameSystem::Init()
{
	m_sky.SetModel(m_resourceFactory.GetModelData("Data/Models/Sky/Sky.gltf"));

	// スカイスフィア拡大行列
	m_skyMat = m_skyMat.CreateScale(50.0f);

	
	std::shared_ptr<StageMap> spStage = std::make_shared<StageMap>(); // stageMapのインスタンス化
	spStage->Init();
	AddObject(spStage);

	std::shared_ptr<Player> spPlayer = std::make_shared<Player>();	// プレイヤーのインスタンス化
	spPlayer->Init();
	AddObject(spPlayer);

	std::shared_ptr<Enemy> spEnemy = std::make_shared<Enemy>();
	spEnemy->Init();
	AddObject(spEnemy);
	spEnemy->SetTarget(spPlayer);
}

void GameSystem::Update()
{
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		if (MessageBoxA(APP.m_window.GetWndHandle(), "本当にゲームを終了しますか？",
			"確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) 
		{
			APP.End();
		}

		// キャンセル時元の位置に戻す
		SetCursorPos(FPSCamera::s_fixMousePos.x, FPSCamera::s_fixMousePos.y);
	}

	// スカイスフィア回転
	DirectX::SimpleMath::Matrix rotSky;
	rotSky = rotSky.CreateRotationY(DirectX::XMConvertToRadians(1.0f * 0.05f));

	// スカイスフィア行列合成
	m_skyMat = rotSky * m_skyMat;

	// objectの更新(範囲for文は参照必須)
	for (std::shared_ptr<GameObject>& spObject : m_spObjects)
	{
		spObject->Update();
	}

	// GameObjectの寿命が尽きたらリストから除去(メモリ解放)
	auto objectItr = m_spObjects.begin();
	//std::list<std::shared_ptr<GameObject>>::iterator objectItr = m_spObjects.begin();
	// 配列の最後の次の配列外まで
	while (objectItr != m_spObjects.end())
	{
		if (!(*objectItr)->IsAlive())
		{
			objectItr->reset(/*引数にポインタを入れて新しくこっちを見さす*/);
			
			// 消したイテレータを受け取る
			objectItr = m_spObjects.erase(objectItr);

			continue;
		}
		
		++objectItr;
	}
	
}

void GameSystem::Draw()
{
	if (m_spCamera)
	{
		m_spCamera->SetToShader();
	}

	SHADER->m_effectShader.SetToDevice();	// 描画前に必要

	SHADER->m_effectShader.DrawModel(m_sky,m_skyMat);

	// 陰影をつける
	SHADER->m_standardShader.SetToDevice();

	for (std::shared_ptr<GameObject>& spObject : m_spObjects)
	{
		spObject->Draw();
	}
}

void GameSystem::Release()
{
	m_spObjects.clear();
}
