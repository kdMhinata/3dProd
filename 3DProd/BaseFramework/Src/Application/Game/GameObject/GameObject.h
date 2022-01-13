#pragma once

#define CLASS_NAME(name) virtual std::string ClassName() const override { return #name; }

// 球の情報
struct SphereInfo
{

	SphereInfo() {}
	// デフォルトコンストラクタ
	SphereInfo(const Math::Vector3& pos, float radius):
		m_pos(pos),
		m_radius(radius)
	{}

	Math::Vector3 m_pos;
	float m_radius = 1.0f;
};


// レイの情報
struct RayInfo
{
	RayInfo(const Math::Vector3& pos, const Math::Vector3& dir, float range):
		m_pos(pos),
		m_dir(dir),
		m_range(range){m_dir.Normalize();}

	Math::Vector3 m_pos;
	Math::Vector3 m_dir;
	float m_range = 0.0f;

};	

struct BumpResult
{
	bool m_isHit = false;
	Math::Vector3 m_pushVec;
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:

	enum classID
	{
		eBase,
		eStage,
		ePlayer,
		eEnemy,
		eDestuctible,
		eGimmick
	};

	GameObject(){}
	virtual ~GameObject() { Release(); }

	virtual void Init(){}
	virtual void Update(){}
	virtual void Draw();
	virtual void DrawEffect() {}
	virtual void Draw2D() {}

	virtual void ImGuiUpdate();

	virtual bool IsAlive() { return m_isAlive; }

	virtual void SetPos(const Math::Vector3& pos){m_mWorld.Translation(pos);}

	virtual const Math::Vector3 GetPos() const { return m_mWorld.Translation(); }

	void SetTag(std::string tag) { m_tag = tag; }

	virtual void SetMatrix(const Math::Matrix& m) { m_mWorld = m; }

	virtual classID GetClassID() const { return eBase; }

	virtual std::string ClassName() const { return "GameObject"; };

	// 押し戻しの衝突判定
	virtual bool CheckCollisionBump(const SphereInfo& info, BumpResult& result);
	// オブジェクト同士の衝突判定
	bool CheckCollisionBump(const RayInfo& info, BumpResult& result);

	const std::string& GetName() const { return  m_name; }
	const std::string& GetTag() const { return m_tag; }
	const Math::Matrix& GetMatrix()const { return m_mWorld; }

	// 復元
	virtual void Deserialize(const json11::Json& json)
	{
		m_name = json["Name"].string_value();
		m_modelFilename = json["ModelFilename"].string_value();
		m_tag = json["Tag"].string_value();
		LoadModel(m_modelFilename);
	
		Math::Vector3 pos;
		JsonToVec3(json["Pos"],pos);
		Math::Vector3 ang;
		JsonToVec3(json["Angle"], ang);

		// SR(x*y*z)T
		m_mWorld = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(ang.x)) *
					Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(ang.y))*
					Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(ang.z))*
					Math::Matrix::CreateTranslation(pos);
	}
	// 文字列化
	virtual void Serialize(json11::Json::object& json)
	{
		json["Name"] = m_name;
		json["ModelFilename"] = m_modelFilename;
		json["ClassName"] = ClassName();
		json["Tag"] = m_tag;

		json["Pos"] = Vec3ToJson(GetPos());
		json["Angle"] = Vec3ToJson(MatToAngle(GetMatrix()));
	}

	void LoadModel(const std::string& path)
	{
		m_modelFilename = path;
		auto model = GameResourceFactory.GetModelData(path);
		if (model == nullptr)return;
		m_modelWork.SetModel(model);
	}

	void Destroy()
	{
		m_isAlive = false;
	}

// 継承したもののみ触れる 
protected:

	template<class T>
	std::shared_ptr<T> GetSptr(T* pThis)
	{
		return std::static_pointer_cast<T>(shared_from_this());
	}


	KdModelWork		m_modelWork;
	Math::Matrix	m_mWorld;

	bool			m_isAlive = true;

	SphereInfo		m_bumpSphereInfo;

	float			m_alpha = 1.0f;

	std::string m_tag = "";
	std::string m_name = "GameObject";
	std::string m_modelFilename;

private:
	void Release() {}

};