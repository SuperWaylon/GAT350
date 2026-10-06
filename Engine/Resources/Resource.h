#pragma once
#include <memory>

namespace nu
{
	class Resource
	{
	public:
		Resource() = default;
		virtual ~Resource() = default;
	};

	template <typename T = Resource>
	using res_t = std::shared_ptr<T>;
}