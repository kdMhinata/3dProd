#include"GameSystem.h"
#include"GameObject/StageMap.h"
#include"GameObject/Player.h"
#include"GameObject/Enemy.h"

#include"Camera/TPSCamera.h"

#include"../main.h";

void GameSystem::Init()
{
//	m_sky.SetModel(m_resourceFactory.GetModelData("Data/Models/Sky/Sky.gltf"));

	// スカイスフィア拡大行列
	m_skyMat = m_skyMat.CreateScale(50.0f);

	
	std::shared_ptr<StageMap> spStage = std::make_shared<StageMap>(); // stageMapのインスタンス化
	spStage->Init();
	AddObject(spStage);

	std::shared_ptr<Player> spPlayer = std::make_shared<Player>();	// プレイヤーのインスタンス化
	spPlayer->Init();
	spPlayer->SetInput(std::make_shared<PlayerInput>());
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
	// カメラの情報をシェーダーに渡す
	if (m_spCamera)
	{
		m_spCamera->SetToShader();
	}

	// ①不透明物の描画から
   // 不透明物描画用シェーダーに切り替え
	SHADER->m_standardShader.SetToDevice();

	// ゲームオブジェクトの描画(範囲ベースfor文)
	for (std::shared_ptr<GameObject>& spObject : m_spObjects)
	{
		spObject->Draw();
	}
	// -------------------------------------------------------
	// ②次に透明物の描画
	SHADER->m_effectShader.SetToDevice();

	// 拡大行列を適用する
	SHADER->m_effectShader.DrawModel(m_sky, m_skyMat);
	{
		D3D.WorkDevContext()->OMSetDepthStencilState(SHADER->m_ds_ZEnable_ZWriteDisable, 0);

		// カリングなし(両面描画)
		D3D.WorkDevContext()->RSSetState(SHADER->m_rs_CullNone);
		
	
		// ゲームオブジェクト(透明物)の描画
		for (std::shared_ptr<GameObject>& spObject : m_spObjects)
		{
			spObject->DrawEffect();
		}

		D3D.WorkDevContext()->OMSetDepthStencilState(SHADER->m_ds_ZEnable_ZWriteEnable, 0);
		// 裏面カリング(表面のみ描画)
		D3D.WorkDevContext()->RSSetState(SHADER->m_rs_CullBack);
	}
}

const std::shared_ptr<KdCamera> GameSystem::GetCamera() const
{
	return m_spCamera;
}

void GameSystem::Release()
{
	m_spObjects.clear();
}
