#include"GameSystem.h"
#include"GameObject/StageMap.h"
#include"GameObject/Player.h"
#include"GameObject/Enemy.h"
#include"GameObject/Gimmick.h"

#include"TitleObject.h"
#include"Result.h"

#include"Camera/TPSCamera.h"

#include"../main.h";

void GameSystem::TitleInit()
{
	//背景のスプライト描画用GameObject
	//ゲームシーンに推移するためのボタンObject
	std::shared_ptr<GameObject> spTitle = std::make_shared<TitleObject>();
	spTitle->Init();
	AddObject(spTitle);

}

void GameSystem::GameInit()
{
	bool isLoaded = false;

	auto loadProc = [this, &isLoaded]()
	{
		Load("Data/Save/Dungeon1");

		// 共通エフェクト
		GameResourceFactory.GetTexture("Data/Textures/SlashH1.png");
		GameResourceFactory.GetTexture("Data/Textures/damagefont.png");

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

void GameSystem::ResultInit()
{
	//背景のスプライト描画用GameObject
	//タイトルに推移するためのボタンObject
	std::shared_ptr<GameObject> spResult = std::make_shared<ResultObject>();
	spResult->Init();
	AddObject(spResult);
}

void GameSystem::Init()
{
	CLASS_REGISTER(TitleObject);
	CLASS_REGISTER(GameObject);
	CLASS_REGISTER(ResultObject);
	CLASS_REGISTER(Player);
	CLASS_REGISTER(Enemy);
	CLASS_REGISTER(StageMap);
	CLASS_REGISTER(Gimmick);
	CLASS_REGISTER(DestuctibleBox);

	TitleInit();
}

void GameSystem::Update()
{
	// シーンの切り替え
	if (m_changeSceneFilename.empty() == false)
	{
		Load(m_changeSceneFilename);

		m_changeSceneFilename = "";
	}
	//ゲームモードの切り替え
	if (m_changeGameModeFlg)
	{
		m_spObjects.clear();

		switch (m_changeGameModeName)
		{
		case GameSystem::Title:
			TitleInit();
			break;
		case GameSystem::Game:
			GameInit();
			break;
		case GameSystem::Result:
			ResultInit();
			break;
		case GameSystem::GameOver:
			break;
		default:
			break;
		}

		m_changeGameModeFlg = false;
	}
 
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
			objectItr->reset(/*引数にポインタを入れて新しくこっちを見させる*/);

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

	// 
	if (_blackoutSpeed != 0)
	{
		_blackoutRate += _blackoutSpeed;
		_blackoutRate = std::clamp(_blackoutRate, 0.0f, 1.0f);
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

	// 黒幕描画
	{
	}
}

void GameSystem::ImGuiUpdate()
{
	if (ImGui::Begin("Object List"))
	{

		if (ImGui::Button("LoadScene"))
		{
			std::string path;
			if (KdWindow::OpenFileDialog(path))
			{
				json11::Json json = KdLoadJSONFile(path);

				m_spObjects.clear();

				json11::Json::array objArray = json.array_items();

				for (auto&& obj : objArray)
				{
					const auto& className = obj["ClassName"].string_value();
					std::shared_ptr<GameObject> newObj = CLASS_INST.Instantiate<GameObject>(className);

					newObj->Deserialize(obj);

					m_spObjects.push_back(newObj);
				}
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

		if (ImGui::Button("SelectObjDeleate"))
		{
			auto obj = m_editor.m_selectObject.lock();

			if (obj)
			{
				obj->Destroy();
			}
		}

		if (ImGui::IsKeyPressed(VK_DELETE, false))
		{
			auto obj = m_editor.m_selectObject.lock();

			if (obj)
			{
				obj->Destroy();
			}

		}

		if (ImGui::TreeNode("DestObjSet"))
		{
			if (ImGui::Button("Set"))
			{
				std::shared_ptr<DestuctibleBox> spDestBox = std::make_shared<DestuctibleBox>();
				spDestBox->Init();
				AddObject(spDestBox);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Gimmick"))
		{
			if (ImGui::Button("Set"))
			{
				std::shared_ptr<Gimmick> spGimmick = std::make_shared<Gimmick>();
				spGimmick->Init();
				AddObject(spGimmick);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("EnptyObject"))
		{
			if (ImGui::Button("Set"))
			{
				std::shared_ptr<GameObject> spEnpty = std::make_shared<GameObject>();
				spEnpty->Init();
				AddObject(spEnpty);
			}
			ImGui::TreePop();
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

			if (ImGui::Button("Set"))
			{
				auto obj = m_editor.m_selectObject.lock();

				if (!obj)return;

				std::shared_ptr<Enemy> spEnemy = std::make_shared<Enemy>();
				spEnemy->Init();
				AddObject(spEnemy);
				
				if (obj)
				{
					spEnemy->SetWPos(obj->GetPos());
					spEnemy->LoadModel(m_editor.selectEnemyModelName);
					spEnemy->SetTag("Enemy");
				}
			}
			ImGui::TreePop();
		}

		for (auto&& obj : m_spObjects)
		{
			ImGui::PushID(obj.get());

			bool isSelect = m_editor.m_selectObject.lock() == obj;

			bool isClicked = ImGui::Selectable(obj->GetName().c_str(), isSelect);
			if (isClicked)
			{
				// Clickされた
				m_editor.m_selectObject = obj;
			}

			if (ImGui::BeginPopupContextItem("TestTest", 1))
			{
				if (ImGui::Selectable("Delete"))
				{
					auto obj = m_editor.m_selectObject.lock();

					if (obj)
					{
						obj->Destroy();
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();
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
	}
	ImGui::End();

}

const std::shared_ptr<KdCamera> GameSystem::GetCamera() const
{
	return m_spCamera;
}

void GameSystem::EnemyInstance(std::shared_ptr<GameObject> target, Math::Vector3& pos, std::string& modelname)
{
	std::shared_ptr<Enemy> spEnemy = std::make_shared<Enemy>();
	spEnemy->Init();
	AddObject(spEnemy);
	spEnemy->SetWPos(pos);
	spEnemy->LoadModel(modelname);
	spEnemy->SetTarget(target);
}

void GameSystem::Load(const std::string& filename)
{		//・Objectのロード
	json11::Json json = KdLoadJSONFile(filename);

	m_spObjects.clear();

	json11::Json::array objArray = json.array_items();

	for (auto&& obj : objArray)
	{
		const auto& className = obj["ClassName"].string_value();
		std::shared_ptr<GameObject> newObj = CLASS_INST.Instantiate<GameObject>(className);

		newObj->Deserialize(obj);

		m_spObjects.push_back(newObj);
	}

	//・設定のロード
	//・
}

void GameSystem::EnterStage()
{
	//ブラックアウトからゲームに切り替え
		//BlackOut() 逆

	//空のPlayerSpawnPointと一時的に保存しておいたプレイヤーを置き換える
	AddObject(LoadWaitingRoom());
	//FindObjectWithTag("SpawnPoint")->GetPos();
}

/*
void GameSystem::ExitStage(Math::Matrix mat)
{
	std::shared_ptr<GameObject> obj = FindObjectWithTag("Player");

	auto player = std::dynamic_pointer_cast<Player>(obj);

	if (player == nullptr) { return; }

	//行列をプレイヤー用の角度に変換
	Math::Vector3 vec = MatToAngle(mat);
	
	player->Exit(vec);

	//プレイヤーを一時的に保存しておく
	SaveWaitingRoom(FindObjectWithTag("Player"));

	//ブラックアウトする
	BlackOut();

	//ステージを変更する
	//ReserveChangeScene("Data/Save/Dungeon2test");
}
*/

std::shared_ptr<GameObject> GameSystem::FindObjectWithTag(const std::string& tag)
{
	//タグ検索
	for (const std::shared_ptr<GameObject>& spObj : GameSystem::GetInstance().GetObjects())
	{
		if (tag == spObj->GetTag()) { return spObj; }

	}
	return nullptr;
}

std::vector<std::shared_ptr<GameObject>> GameSystem::FindObjectsWithTag(const std::string& tag)
{
	std::vector<std::shared_ptr<GameObject>> vector;
	//タグ検索
	for (const std::shared_ptr<GameObject>& spObj : GameSystem::GetInstance().GetObjects())
	{
		if (tag == spObj->GetTag()) { vector.push_back(spObj); }
	}
	return vector;
}

void GameSystem::BlackOut(float speed)
{
	_blackoutSpeed = speed;
}

void GameSystem::Release()
{
	m_spObjects.clear();
}
