#include "ResourceFactory.h"

const std::shared_ptr<KdModelData> ResourceFactory::GetModelData(const std::string& fileName)
{
	// リスト内のキーで検索
 	//std::unordered_map<std::string,std::shared_ptr<KdModelData>>::iterator
		//foundItr = m_modelMap.find(fileName);
	auto foundItr = m_modelMap.find(fileName);
	// もしリストの中にある
	if (foundItr != m_modelMap.end())
	{
		// 見つけたデータを返す
		return (*foundItr).second;	// second…2つ目の要素(モデルデータ)を返す
	}

	// もしリスト内にない場合
	std::shared_ptr<KdModelData> newModelData = std::make_shared<KdModelData>();

	// モデルを新規で読み込む
	if (newModelData->Load(fileName))
	{
		// リスト追加
		// pair型で2つの要素を入れる
		/*m_modelMap.insert(std::pair<std::string, 
		  std::shared_ptr<KdModelData>>(fileName, newModelData));*/
		m_modelMap[fileName] = newModelData;
		// 読み込んだデータを返す
		return newModelData;
	}
		
	// 読み込みが失敗
	std::string errorMsg = "ResourceFactory::GetModelData() ModelLoad Error" + fileName;
	
	assert(0 && errorMsg.c_str());

	return nullptr;
}

const std::shared_ptr<KdTexture> ResourceFactory::GetTexture(const std::string& key)
{
	// リスト内のキーで検索
	//std::unordered_map<std::string,std::shared_ptr<KdModelData>>::iterator
		//foundItr = m_modelMap.find(fileName);
	auto foundItr = m_textureMap.find(key);
	// リストの中にない
	if (foundItr == m_textureMap.end())
	{
		return nullptr;
		// 見つけたデータを返す
//		return (*foundItr).second.m_data;	// second…2つ目の要素(モデルデータ)を返す
	}

	if ((*foundItr).second.m_data != nullptr)
	{
		return (*foundItr).second.m_data;
	}

	// もしリスト内にない場合
	std::shared_ptr<KdTexture> newTexture = std::make_shared<KdTexture>();

	// モデルを新規で読み込む
	if (newTexture->Load((*foundItr).second.m_path))
	{
		(*foundItr).second.m_data = newTexture;
//		m_textureMap[key].m_data = newTexture;
		return newTexture;
	}

	// 読み込みが失敗
	std::string errorMsg = "ResourceFactory::GetTexture()  Failed" + (*foundItr).second.m_path;

	assert(0 && errorMsg.c_str());

	return nullptr;
}

void ResourceFactory::Initialize(const std::string& databasePath)
{
	m_textureMap["EffectTex_Slash1"] = { "Data/Textures/Slash1.png" , nullptr};
	// ・・・・・
}

void ResourceFactory::Release()
{
	m_modelMap.clear();
}
