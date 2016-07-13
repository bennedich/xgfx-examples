#pragma once

#include "xecs/type.h"

using xecs::u32;

struct TimeDelta
{
	u32 t0;
	u32 t1;
	u32 dt;
};
