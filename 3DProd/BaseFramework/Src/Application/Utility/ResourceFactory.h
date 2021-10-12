#pragma once

class ResourceFactory
{
public:
	ResourceFactory() {}
	~ResourceFactory() { Release(); }

	const std::shared_ptr<KdModelData> GetModelData(const std::string& fileName);

	const std::shared_ptr<KdTexture> GetTexture(const std::string& fileName);

private:
	void Release();

	// 文字列とモデルデータを格納
	std::unordered_map<std::string, std::shared_ptr<KdModelData>> m_modelMap;

	//テクスチャデータ管理
	std::unordered_map < std::string, std::shared_ptr<KdTexture>> m_textureMap;
};
