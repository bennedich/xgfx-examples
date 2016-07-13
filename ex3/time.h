#pragma once

#include "xecs/type.h"

using xecs::u32;

struct Time
{
	u32 time;
	u32 t0;
	u32 t1;
	u32 dt;
};
