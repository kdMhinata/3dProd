#pragma once

class KdTexture;

//===========================================
//
// 便利機能
//
//===========================================

// 算術系短縮名
namespace Math = DirectX::SimpleMath;

// 角度変換
constexpr float KdToRadians = (3.141592654f / 180.0f);
constexpr float KdToDegrees = (180.0f / 3.141592654f);

// 安全にReleaseするための関数
template<class T>
void KdSafeRelease(T*& p)
{
	if (p)
	{
		p->Release();
		p = nullptr;
	}
}

// 安全にDeleteするための関数
template<class T>
void KdSafeDelete(T*& p)
{
	if (p)
	{
		delete p;
		p = nullptr;
	}
}

template<class T>
void DebugOutputNumber(T num)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << num;
	std::string str = stream.str();

	OutputDebugStringA(str.c_str());
}

//===========================================
//
// 色定数
//
//===========================================
static const Math::Color	kWhiteColor		= Math::Color(1, 1, 1, 1);
static const Math::Color	kBlackColor		= Math::Color(0, 0, 0, 1);
static const Math::Color	kRedColor		= Math::Color(1, 0, 0, 1);
static const Math::Color	kGreenColor		= Math::Color(0, 1, 0, 1);
static const Math::Color	kBlueColor		= Math::Color(0, 0, 1, 1);


//===========================================
//
// ファイル
//
//===========================================

// ファイルパスから、親ディレクトリまでのパスを取得
inline std::string KdGetDirFromPath(const std::string &path)
{
	const std::string::size_type pos = std::max<signed>(path.find_last_of('/'), path.find_last_of('\\'));
	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}

// JSON読み込み
inline json11::Json KdLoadJSONFile(const std::string& filename)
{
	// JSONファイルを開く
	std::ifstream ifs(filename);
	if (!ifs)return nullptr;

	// 文字列として全読みおk
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	// 文字列のJSONを解析(パース)する
	std::string err;
	json11::Json jsonObj = json11::Json::parse(strJson, err);
	if (err.size() > 0)return nullptr;

	return jsonObj;
}

inline json11::Json::array Vec3ToJson(const Math::Vector3& v)
{
	return json11::Json::array({ v.x, v.y, v.z });
}

inline void JsonToVec3(json11::Json json, Math::Vector3& vec3)
{
	if (!json.is_array())return;
	if (json.array_items().size() != 3)return;
	vec3.x = (float)json.array_items()[0].number_value();
	vec3.y = (float)json.array_items()[1].number_value();
	vec3.z = (float)json.array_items()[2].number_value();
}

inline void JsonToBool(json11::Json json, bool& ret)
{
	if (!json.is_bool())return;
	ret = json.bool_value();
}

inline Math::Vector3 MatToAngle(const Math::Matrix& mat)
{
	Math::Matrix m = mat;
	Math::Vector3 v = m.Right(); //vxにmatのX軸を取得
	v.Normalize(); //vxを正規化
	m.Right(v); //正規化したvxをmatのX軸に設定

	v = m.Up();
	v.Normalize();
	m.Up(v);

	v = m.Backward();
	v.Normalize();
	m.Backward(v);

	Math::Vector3 angles;
	angles.x = DirectX::XMConvertToDegrees(atan2f(m.m[1][2], m.m[2][2]));
	angles.y = DirectX::XMConvertToDegrees(atan2f(-m.m[0][2], sqrtf(m.m[1][2] * m.m[1][2] + m.m[2][2] * m.m[2][2])));
	angles.z = DirectX::XMConvertToDegrees(atan2f(m.m[0][1], m.m[0][0]));
	return angles;
}

//===========================================
//
// 文字列関係
//
//===========================================

// std::string版 sprintf
template <typename ... Args>
std::string KdFormat(const std::string& fmt, Args ... args)
{
	size_t len = std::snprintf(nullptr, 0, fmt.c_str(), args ...);
	std::vector<char> buf(len + 1);
	std::snprintf(&buf[0], len + 1, fmt.c_str(), args ...);
	return std::string(&buf[0], &buf[0] + len);
}

void KdGetTextuxxreInfo(ID3D11View* view, D3D11_TEXTURE2D_DESC& outDesc);