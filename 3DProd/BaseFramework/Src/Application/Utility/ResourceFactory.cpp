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

void ResourceFactory::Release()
{
	m_modelMap.clear();
}
