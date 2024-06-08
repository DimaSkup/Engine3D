#pragma once

#include <vector>

namespace Utils
{
	template<class T>
	void AppendArray(std::vector<T>& head, const std::vector<T>& tail)
	{
		head.insert(head.end(), tail.begin(), tail.end());
	}
}

