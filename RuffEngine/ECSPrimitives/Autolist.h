#pragma once

#include <vector>
#include <algorithm>

namespace ecs
{
	template<typename T>
	class Autolist
	{
	public:
		Autolist()
		{
			s_references.push_back(static_cast<T*>(this));
		}
		virtual ~Autolist()
		{
			auto iter = std::find(s_references.begin(), s_references.end(), this);
			if (iter != s_references.end())
			{
				s_references.erase(iter);
			}
		}
		static size_t Size()
		{
			return s_references.size();
		}
		static T* Get(size_t index)
		{
			return s_references[index];
		}
	private:
		static std::vector<T*> s_references;
	};

	template<typename T>
	std::vector<T*> Autolist<T>::s_references;
}
