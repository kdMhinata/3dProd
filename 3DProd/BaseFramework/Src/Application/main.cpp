#include "main.h"

#include"Game/GameSystem.h"

//===================================================================
// メイン
//===================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{
	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL, "japanese");

	//===================================================================
	// 実行
	//===================================================================
	APP.Execute();

	// COM解放
	CoUninitialize();

	return 0;
}


// アプリケーション初期設定
bool Application::Init(int w, int h)
{
	//===================================================================
	// ウィンドウ作成
	//===================================================================
	if (m_window.Create(w, h, "3D GameProgramming", "Window") == false) {
		MessageBoxA(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}

	//===================================================================
	// フルスクリーン確認
	//===================================================================
	bool bFullScreen = false;
	if (MessageBoxA(m_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
		bFullScreen = true;
	}

	//===================================================================
	// Direct3D初期化
	//===================================================================

	// デバイスのデバッグモードを有効にする
	bool deviceDebugMode = false;
	#ifdef _DEBUG
	deviceDebugMode = true;
	#endif

	// Direct3D初期化
	std::string errorMsg;
	if (D3D.Init(m_window.GetWndHandle(), w, h, deviceDebugMode, errorMsg) == false) {
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	// フルスクリーン設定
	if (bFullScreen) {
		D3D.WorkSwapChain()->SetFullscreenState(TRUE, 0);
	}

	//===================================================================
	// オーディオ初期化
	//===================================================================

	return true;
}

// アプリケーション終了
void Application::Release()
{
	// Direct3D解放
	D3D.Release();

	// オーディオ解放
	KD_AUDIO.Release();

	// ウィンドウ削除
	m_window.Release();
}

// アプリケーション実行
void Application::Execute()
{
	//===================================================================
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	//===================================================================
	if (APP.Init(1280, 720) == false) {
		return;
	}

	GameSystem::GetInstance().Init();
	
	//===================================================================
	// ゲームループ
	//===================================================================

	// 時間
	DWORD baseTime = timeGetTime();
	DWORD prevTime = baseTime;
	int count = 0;

	// ループ
	while (1)
	{
		// 処理開始時間Get
		DWORD startTime = timeGetTime();

		// ゲーム終了指定があるときはループ終了
		if (m_endFlag)
		{ 
			break;
		}

		//=========================================
		//
		// ウィンドウ関係の処理
		//
		//=========================================

		// ウィンドウのメッセージを処理する
		m_window.ProcessMessage();

		// ウィンドウが破棄されてるならループ終了
		if (m_window.IsCreated() == false)
		{
			break;
		}

		//=========================================
		//
		// オーディオ処理
		//
		//=========================================


		//=========================================
		//
		// ゲーム処理
		//
		//=========================================

		GameSystem::GetInstance().Update();

		DirectX::SimpleMath::Color col(0.0f, 0.0f, 1.0f, 1.0f);

		D3D.WorkDevContext()->ClearRenderTargetView(D3D.WorkBackBuffer()->WorkRTView(),col); //書き込めるテクスチャをクリア

		D3D.WorkDevContext()->ClearDepthStencilView(D3D.WorkZBuffer()->WorkDSView(), 
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		GameSystem::GetInstance().Draw();	// クリア後スワップチェイン前

		D3D.WorkSwapChain()->Present(0, 0);


		//=========================================
		//
		// フレームレート制御
		//
		//=========================================

		constexpr int kSecond = 1000;				// １秒のミリ秒
		constexpr float kFpsRefreshFrame = 500;		// FPS計測のタイミング

		// 処理終了時間Get
		DWORD endTime = timeGetTime();
		// Fps制御
		DWORD ms = kSecond / m_maxFps;
		if (endTime - startTime < ms)
		{
			Sleep(ms - (endTime - startTime));	// 速すぎたら待つ
		}

		// FPS計測
		count++;
		if (startTime - prevTime >= kFpsRefreshFrame)
		{
			m_fps = (count * kSecond) / (startTime - prevTime);
			prevTime = startTime;
			count = 0;
		}
	}

	//===================================================================
	// アプリケーション解放
	//===================================================================
	Release();
}