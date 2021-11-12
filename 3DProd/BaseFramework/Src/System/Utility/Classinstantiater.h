#pragma once

class ClassInstantiater
{
public:

	template<class T>
	void Register(std::string className)
	{
		m_map[className] = []() {return std::make_shared<T>(); };
	}

	template<class T>
	std::shared_ptr<T> Instantiate(std::string className)
	{
		auto it = m_map.find(className);
		if (it == m_map.end())return nullptr;

		return std::static_pointer_cast<T>( (*it).second() );
	}

private:
	std::unordered_map<std::string, std::function<std::shared_ptr<void>()>> m_map;

	ClassInstantiater() {}

public:
	static ClassInstantiater& GetInstance()
	{
		static ClassInstantiater instance;
		return instance;
	}
};

#define CLASS_INST ClassInstantiater::GetInstance()
#define CLASS_REGISTER(className) ClassInstantiater::GetInstance().Register<className>(#className)

// inst.Register<Player>("Player");
// ClassInstantiater a;
// CLASS_INST.Register<Player>("Player");
// auto p = CLASS_INST.Instantiate<GameObject>("Player");
