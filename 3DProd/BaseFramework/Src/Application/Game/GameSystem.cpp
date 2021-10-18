﻿#include"GameSystem.h"
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

	Math::Vector3 pos = {0.0,0.0,5.0};
	std::string modelname = "Data/Models/enemy/slime.gltf";
	EnemyInstance(spPlayer,pos, modelname,50);
	pos = { 8.0,0.0,25.0 };
	modelname = "Data/Models/enemy/slime.gltf";
	EnemyInstance(spPlayer, pos,modelname, 50);
	pos = { 0.0,0.0,25.0 };
	modelname = "Data/Models/enemy/slime.gltf";
	EnemyInstance(spPlayer, pos,modelname, 50);
	pos = { -8.0,0.0,25.0 };
	modelname = "Data/Models/enemy/slime.gltf";
	EnemyInstance(spPlayer, pos,modelname, 50);
	pos = { 5.0,0.0,20.0 };
	modelname = "Data/Models/enemy/slime.gltf";
	EnemyInstance(spPlayer, pos, modelname, 50);
	pos = { -5.0,0.0,20.0 };
	modelname = "Data/Models/enemy/slime.gltf";
	EnemyInstance(spPlayer, pos, modelname, 50);

	pos = { 5.0,0.0,40.0 };
	modelname = "Data/Models/enemy/skeleton.gltf";
	EnemyInstance(spPlayer, pos, modelname, 100);
	pos = { -5.0,0.0,40.0 };
	modelname = "Data/Models/enemy/skeleton.gltf";
	EnemyInstance(spPlayer, pos, modelname, 100);
	pos = { 5.0,0.0,45.0 };
	modelname = "Data/Models/enemy/skeleton.gltf";
	EnemyInstance(spPlayer, pos, modelname, 100);
	pos = { -5.0,0.0,45.0 };
	modelname = "Data/Models/enemy/skeleton.gltf";
	EnemyInstance(spPlayer, pos, modelname, 100);

	pos = { 7.0,0.0,65.0 };
	modelname = "Data/Models/enemy/skeleton.gltf";
	EnemyInstance(spPlayer, pos, modelname, 100);
	pos = { -7.0,0.0,65.0 };
	modelname = "Data/Models/enemy/skeleton.gltf";
	EnemyInstance(spPlayer, pos, modelname, 100);
	pos = { 5.0,0.0,70.0 };
	modelname = "Data/Models/enemy/skeleton.gltf";
	EnemyInstance(spPlayer, pos, modelname, 100);
	pos = { -5.0,0.0,70.0 };
	modelname = "Data/Models/enemy/skeleton.gltf";
	EnemyInstance(spPlayer, pos, modelname, 100);
	pos = { 0.0,0.0,65.0 };
	modelname = "Data/Models/enemy/golem.gltf";
	EnemyInstance(spPlayer, pos, modelname, 200,5.0f,false);

	//予め呼んでおきたい重いデータ等絶対使うデータ等
	GameResourceFactory.GetTexture("Data/Textures/Slash1.png");
	GameResourceFactory.GetTexture("Data/Textures/Slash2.png");
	GameResourceFactory.GetTexture("Data/Textures/SlashH1.png");
	GameResourceFactory.GetModelData("Data/Models/robot/chara.gltf");
	GameResourceFactory.GetModelData("Data/Models/enemy/skeleton.gltf");
	GameResourceFactory.GetModelData("Data/Models/enemy/golem.gltf");
	GameResourceFactory.GetModelData("Data/Models/StageMap/DungeonStage.gltf");

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


		SHADER->m_spriteShader.Begin();

		//2D系描画はこの範囲内で行う

		for (std::shared_ptr<GameObject>& spObject : m_spObjects)
		{
			spObject->Draw2D();
		}

		SHADER->m_spriteShader.End();

		D3D.WorkDevContext()->OMSetDepthStencilState(SHADER->m_ds_ZEnable_ZWriteEnable, 0);
		// 裏面カリング(表面のみ描画)
		D3D.WorkDevContext()->RSSetState(SHADER->m_rs_CullBack);
	}
}

const std::shared_ptr<KdCamera> GameSystem::GetCamera() const
{
	return m_spCamera;
}

void GameSystem::EnemyInstance(std::shared_ptr<GameObject> target,Math::Vector3& pos, std::string& modelname,int hp,float attackradius,bool sarmor)
{
	std::shared_ptr<Enemy> spEnemy = std::make_shared<Enemy>();
	spEnemy->Init();
	AddObject(spEnemy);
	spEnemy->SetWPos(pos);
	spEnemy->SetMData(modelname);
	spEnemy->SetHP(hp);
	spEnemy->SetAttackRadius(attackradius);
	spEnemy->SetSuperArmor(sarmor);
	spEnemy->SetTarget(target);
}

void GameSystem::Release()
{
	m_spObjects.clear();
}
