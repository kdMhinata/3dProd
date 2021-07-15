#pragma once

// 前方宣言
class GameObject;

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

private:
	void Release();		// 解放

	KdModelWork m_sky;	// スカイスフィア

	KdModelWork m_cube;	// キューブ

	DirectX::SimpleMath::Matrix m_cubeMat;	// キューブのワールド行列
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

#define GameResourceFactory GameSystem::GetInstance().WorkResourceFactory()