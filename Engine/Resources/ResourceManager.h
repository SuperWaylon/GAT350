#pragma once
#include "Resource.h"
#include "Core/Singleton.h"
#include <map>
#include <string>
#include <iostream>

namespace nu
{
	class ResourceManager : public Singleton<ResourceManager>
	{
	public:
		template<typename T, typename ... Args>
			requires std::derived_from<T, Resource>
		res_t<T> Get(const std::string& name, Args&& ... args);

		template<typename T, typename ... Args>
			requires std::derived_from<T, Resource>
		res_t<T> GetWithID(const std::string& id, const std::string& name, Args&& ... args);

	private:
		friend class Singleton<ResourceManager>;
		ResourceManager() = default;

	private:
		std::map<std::string, res_t<Resource>> m_resources;
	};

	template<typename T, typename ...Args>
		requires std::derived_from<T, Resource>
	inline res_t<T> ResourceManager::Get(const std::string& name, Args && ...args)
	{
		auto iter = m_resources.find(name);
		// check if resource exists
		if (iter != m_resources.end())
		{
			auto base = iter->second;
			auto resource = std::dynamic_pointer_cast<T>(base);

			if (resource == nullptr)
			{
				std::cerr << "Resource type mismatch: " << name << std::endl;
				return res_t<T>();
			}

			return resource;
		}

		std::cout << "load: " << name << std::endl;

		// resource doesn't exist, create and load
		res_t<T> resource = std::make_shared<T>();
		if (!resource->Load(name, std::forward<Args>(args)...))
		{
			std::cerr << "Could not load resource: " << name << std::endl;
			return res_t<T>();
		}

		// store resource in map
		m_resources[name] = resource;

		return resource;
	}

	template<typename T, typename ...Args>
		requires std::derived_from<T, Resource>
	inline res_t<T> ResourceManager::GetWithID(const std::string& id, const std::string& name, Args && ...args)
	{
		auto iter = m_resources.find(id);
		// check if resource exists
		if (iter != m_resources.end())
		{
			auto base = iter->second;
			auto resource = std::dynamic_pointer_cast<T>(base);

			if (resource == nullptr)
			{
				std::cerr << "Resource type mismatch: " << name << std::endl;
				return res_t<T>();
			}

			return resource;
		}

		// resource doesn't exist, create and load
		res_t<T> resource = std::make_shared<T>();
		if (!resource->Load(name, std::forward<Args>(args)...))
		{
			std::cerr << "Could not load resource: " << name << std::endl;
			return res_t<T>();
		}

		// store resource in map
		m_resources[id] = resource;

		return resource;
	}

	inline ResourceManager& Resources() { return ResourceManager::Instance(); }
}


