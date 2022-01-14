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

#include <fstream>
#include <string>
#include <sstream>
#include <vector>

class CSVLoader
{
public:

	const std::vector<std::vector<std::string>>& GetResult() const { return m_data; }

	bool Load(const std::string& filename)
	{
		std::ifstream ifs(filename);
		if (!ifs)return false;

		char bom[3];
		ifs.read(bom, 3);

		// UTF-8 BOM 0xEF 0xBB 0xBF
		if (!(bom[0] == (char)0xEF && bom[1] == (char)0xBB && bom[2] == (char)0xBF))
		{
			ifs.seekg(0, std::ios_base::beg);
		}

		std::string lineBuf;

		m_data.clear();
		bool  isTitleLine = true;
		// getline関数で1行ずつ読み込む(読み込んだ内容はstr_bufに格納)
		while (std::getline(ifs, lineBuf))
		{
			if (isTitleLine)
			{
				std::istringstream i_stream(lineBuf);
				std::string colBuf;
				while (std::getline(i_stream, colBuf, ','))
				{
					m_title.push_back(colBuf);
				}
				isTitleLine = false;
				continue;
			}

			// 「,」区切りごとにデータを読み込むためにistringstream型にする
			std::istringstream i_stream(lineBuf);

			m_data.push_back(std::vector<std::string>());
			m_data.back().resize(m_title.size());

			// 「,」区切りごとにデータを読み込む
			std::string colBuf;
			int rowIndex = 0;
			while (std::getline(i_stream, colBuf, ','))
			{
				if (rowIndex >= m_title.size())break;
				m_data.back()[rowIndex] = colBuf;
				rowIndex++;
			}
		}

		return true;
	}
private:
	std::vector<std::string> m_title;
	std::vector<std::vector<std::string>> m_data;
};

void ResourceFactory::Initialize(const std::string& databasePath)
{
	CSVLoader csvLoader;
	if (csvLoader.Load(databasePath))
	{
		auto& result = csvLoader.GetResult();
		for (size_t iRow = 0; iRow < result.size(); iRow++)
		{
			auto& row = result[iRow];

			std::vector<std::string> groups;
			std::istringstream i_stream(row[2]);
			std::string colBuf;
			while (std::getline(i_stream, colBuf, '/'))
			{
				groups.push_back(colBuf);
			}
			m_textureMap[row[0]] = { row[1], nullptr, groups };
		}
	}	
	
	/*
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
	*/
}

void ResourceFactory::Release()
{
	m_modelMap.clear();
}
