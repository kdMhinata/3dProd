﻿#pragma once

// 前方宣言
class GameObject;

class BaseInput
{
public:

	BaseInput()
	{
		m_buttons.fill(0);
	}

	// キー情報を更新
	virtual void Update() {}

	// 
	const Math::Vector2& GetAxisL() { return m_axisL; }

	bool IsPressButton(int no, bool repeat = true)
	{
		if (repeat)return m_buttons[no] >= 1;
		else return m_buttons[no] == 1;
	}

protected:
	Math::Vector2 m_axisL;
	std::array<int, 20> m_buttons;
};

class PlayerInput : public BaseInput
{
public:

	virtual void Update() override
	{
		m_axisL = Math::Vector2::Zero;
//		m_buttons.fill(0);

		if (GetAsyncKeyState('W') & 0x8000) { m_axisL.y += 1.0f; }	// 前移動
		if (GetAsyncKeyState('S') & 0x8000) { m_axisL.y -= 1.0f; }	// 後ろ移動
		if (GetAsyncKeyState('A') & 0x8000) { m_axisL.x -= 1.0f; }	// 左移動
		if (GetAsyncKeyState('D') & 0x8000) { m_axisL.x += 1.0f; }	// 右移動

		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			m_buttons[0]++;
		}
		else
		{
			m_buttons[0] = 0;
		}
	}

};

class GameSystem
{
public:

	void Init();
	void Update();
	void Draw();

	void AddObject(std::shared_ptr<GameObject> spObject)
	{
		if (spObject){ m_spObjects.push_back(spObject);}
	}

	// 参照(&)は必須
	const std::list<std::shared_ptr<GameObject>>& GetObjects() { return m_spObjects; }

	void SetCamera(std::shared_ptr<KdCamera> spCamera) { m_spCamera = spCamera; }

	ResourceFactory& WorkResourceFactory() { return m_resourceFactory; }

	const std::shared_ptr<KdCamera> GetCamera() const;

private:
	void Release();		// 解放

	KdModelWork m_sky;	// スカイスフィア

	DirectX::SimpleMath::Matrix m_skyMat;	// キューブのワールド行列

	std::list<std::shared_ptr<GameObject>> m_spObjects;

	std::shared_ptr<KdCamera> m_spCamera = nullptr;

	ResourceFactory m_resourceFactory;

	// シングルトンパターン化
public:

	static GameSystem& GetInstance()
	{
		static GameSystem instance;

		return instance;
	}


private:
	GameSystem() {}	
	~GameSystem() { Release(); }
};

#define GameInstance GameSystem::GetInstance() 

#define GameResourceFactory GameSystem::GetInstance().WorkResourceFactory()