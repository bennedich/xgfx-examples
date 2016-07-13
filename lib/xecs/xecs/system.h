#pragma once

namespace xecs
{
	class System
	{
	public:
		virtual ~System() {}
		virtual void tick() {}
	};
}
