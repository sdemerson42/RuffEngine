#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>

namespace ecs
{
	template<typename T>
	class Autolist
	{
	public:
		Autolist(const std::string& sceneLayer)
		{
			s_references[sceneLayer].push_back(static_cast<T*>(this));
			s_allRefs.push_back(static_cast<T*>(this));
		}
		virtual ~Autolist()
		{
			auto iter = std::find(s_allRefs.begin(), s_allRefs.end(), this);
			if (iter != s_allRefs.end())
			{
				s_allRefs.erase(iter);
			}

			for (auto& refs : s_references)
			{
				auto iter = std::find(refs.second.begin(), refs.second.end(), this);
				if (iter != refs.second.end())
				{
					refs.second.erase(iter);
					return;
				}
			}
		}
		static size_t Size(const std::string& sceneLayer)
		{
			return s_references[sceneLayer].size();
		}
		static T* Get(const std::string& sceneLayer, size_t index)
		{
			return s_references[sceneLayer][index];
		}

		static size_t SizeAll()
		{
			return s_allRefs.size();
		}
		static T* GetAll(int index)
		{
			return s_allRefs[index];
		}
	private:
		static std::unordered_map<std::string, std::vector<T*>> s_references;
		static std::vector<T*> s_allRefs;
	};

	template<typename T>
	std::unordered_map<std::string, std::vector<T*>> Autolist<T>::s_references;

	template<typename T>
	std::vector<T*> Autolist<T>::s_allRefs;

	
}
