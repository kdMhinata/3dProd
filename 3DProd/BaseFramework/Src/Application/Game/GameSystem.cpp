#include"GameSystem.h"
#include"GameObject/StageMap.h"
#include"GameObject/Player.h"
#include"GameObject/Enemy.h"

#include"Camera/TPSCamera.h"

#include"../main.h";

void GameSystem::Init()
{
	bool isLoaded = false;

	// 
	auto loadProc = [this, &isLoaded]()
	{
		// スカイスフィア拡大行列
		m_skyMat = m_skyMat.CreateScale(50.0f);


		std::shared_ptr<StageMap> spStage = std::make_shared<StageMap>(); // stageMapのインスタンス化
		spStage->Init();
		AddObject(spStage);

		std::shared_ptr<Player> spPlayer = std::make_shared<Player>();	// プレイヤーのインスタンス化
		spPlayer->Init();
		spPlayer->SetInput(std::make_shared<PlayerInput>());
		AddObject(spPlayer);

		Math::Vector3 pos = { 0.0,0.0,5.0 };
		std::string modelname = "Data/Models/enemy/slime.gltf";
		EnemyInstance(spPlayer, pos, modelname, 50);
		pos = { 8.0,0.0,25.0 };
		modelname = "Data/Models/enemy/slime.gltf";
		EnemyInstance(spPlayer, pos, modelname, 50);
		pos = { 0.0,0.0,25.0 };
		modelname = "Data/Models/enemy/slime.gltf";
		EnemyInstance(spPlayer, pos, modelname, 50);
		pos = { -8.0,0.0,25.0 };
		modelname = "Data/Models/enemy/slime.gltf";
		EnemyInstance(spPlayer, pos, modelname, 50);
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
		EnemyInstance(spPlayer, pos, modelname, 200, 5.0f, false);

		//予め呼んでおきたい重いデータ等絶対使うデータ等
		GameResourceFactory.GetTexture("Data/Textures/Slash1.png");
		GameResourceFactory.GetTexture("Data/Textures/Slash2.png");
		GameResourceFactory.GetTexture("Data/Textures/SlashH1.png");
		GameResourceFactory.GetModelData("Data/Models/robot/chara.gltf");
		GameResourceFactory.GetModelData("Data/Models/enemy/skeleton.gltf");
		GameResourceFactory.GetModelData("Data/Models/enemy/golem.gltf");
		GameResourceFactory.GetModelData("Data/Models/StageMap/Dungeon/Dungeon1/DungeonStage.gltf");

		isLoaded = true;
	};

	std::thread loadThread(loadProc);

	while (isLoaded == false)
	{
		APP.m_window.ProcessMessage();

		DirectX::SimpleMath::Color col(1.0f, 0.0f, 0.0f, 1.0f);

		D3D.WorkDevContext()->ClearRenderTargetView(D3D.WorkBackBuffer()->WorkRTView(), col); //書き込めるテクスチャをクリア

		D3D.WorkDevContext()->ClearDepthStencilView(D3D.WorkZBuffer()->WorkDSView(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


		D3D.WorkSwapChain()->Present(0, 0);

		Sleep(100);
	}


	// 
	loadThread.join();	// スレッドの終了を待つ
}

void GameSystem::Update()
{
	ImGuiUpdate();

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

	if (m_editor.editFlg)
	{
		//m_editor.camera.
	}
}

void GameSystem::Draw()
{
	if (!m_editor.editFlg)
	{
		// カメラの情報をシェーダーに渡す
		if (m_spCamera)
		{
			m_spCamera->SetToShader();
		}
	}
	else
	{
			m_editor.camera.SetToShader();
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

void GameSystem::ImGuiUpdate()
{
	// ImGui Objectrisuto windou 
	if (ImGui::Begin("Object List"))
	{
		if (ImGui::Button("Load"))
		{
			std::string path;
			if (KdWindow::OpenFileDialog(path))
			{

			}
		}

		if (ImGui::Button("SaveScene"))
		{
			json11::Json::array objArray;
			for (auto&& obj : m_spObjects)
			{
				json11::Json::object serial;
				obj->Serialize(serial);

				objArray.push_back(serial);
			}

			std::string path;
			if (KdWindow::SaveFileDialog(path))
			{
				// 文字列化
				json11::Json json(objArray);
				std::string strJson = json.dump(true);

				std::ofstream ofs(path);
				if (ofs)
				{
					ofs.write(strJson.c_str(), strJson.size());
				}
			}
		}

		if (ImGui::Button("Reset"))
		{
			Release();
		}
		if (ImGui::Button("Start"))
		{
			Init();
		}

		if (ImGui::TreeNode("EnemySet"))
		{

			if (ImGui::TreeNode("Model"))
			{
				static KdTexture tex("Data/Textures/enemy/golem.png");

				if (ImGui::ImageButton(tex.WorkSRView(), { 80.f,45.f }))
				{
					m_editor.selectEnemyModelName = "Data/Models/enemy/golem.gltf";
				}

				static KdTexture tex2("Data/Textures/enemy/skeleton.png");
				if (ImGui::ImageButton(tex2.WorkSRView(), { 80.f,45.f }))
				{
					m_editor.selectEnemyModelName = "Data/Models/enemy/skeleton.gltf";
				}

				static KdTexture tex3("Data/Textures/enemy/slime.png");
				if (ImGui::ImageButton(tex3.WorkSRView(), { 80.f,45.f }))
				{
					m_editor.selectEnemyModelName = "Data/Models/enemy/slime.gltf";
				}
				ImGui::TreePop();
			}
		
			if(ImGui::Button("Set"))
			{
				std::shared_ptr<Enemy> spEnemy = std::make_shared<Enemy>();
				spEnemy->Init();
				AddObject(spEnemy);
				auto obj = m_editor.m_selectObject.lock();
				if (obj)
				{
					spEnemy->SetTarget(obj);
					spEnemy->SetWPos(obj->GetPos());
					spEnemy->SetMData(m_editor.selectEnemyModelName);
				}
			}
			ImGui::TreePop();
		}

		for (auto&& obj : m_spObjects)
		{
			ImGui::PushID(obj.get());

			bool isSelect = m_editor.m_selectObject.lock() == obj;

				if (ImGui::Selectable(obj->GetName().c_str(), isSelect))
				{
					// Clickされた
					m_editor.m_selectObject = obj;
				}

			ImGui::PopID();
		}
	}
	ImGui::End();

	// Inspector
	if (ImGui::Begin("Inspector"))
	{
		auto obj = m_editor.m_selectObject.lock();
		if (obj)
		{
			obj->ImGuiUpdate();
		}
	}
	ImGui::End();
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
