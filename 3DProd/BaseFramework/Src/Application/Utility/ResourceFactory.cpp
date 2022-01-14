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
	//csvのデータをデータベースに登録する
	
	//for
	//if csvの１列目があったら
	//1列目をキーとして登録
	//2列目をパスとして登録
	//次の行を読み込む

	//FILE* fp; //ファイルの情報を格納する構造体

	////ファイル読み込み
	//fp = fopen("Data/DataBase/Textures.csv", "r");
	//fclose(fp);

	const char* DELIMS = " ,"; //スペース,カンマ
	const int MAX_LINE_LENGTH = 1014;

	std::fstream file(databasePath);

	//行を読み込むためのバッファを用意する
	char buffer[MAX_LINE_LENGTH] = {};

	while (file.getline(buffer, MAX_LINE_LENGTH))
	{
		const char* row = strtok(buffer, DELIMS);
		const char* col = strtok(NULL, DELIMS);

		m_textureMap[std::string(row)] = { std::string(col),nullptr };
	}

	file.close();
}

void ResourceFactory::Release()
{
	m_modelMap.clear();
}
