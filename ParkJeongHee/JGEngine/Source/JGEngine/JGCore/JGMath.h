#pragma once
#include "TypeDefine.h"



namespace JG
{
	namespace JGMath
	{
		template<class T>
		inline T AlignUp(T N, u32 alignMask)
		{
			return N + N - (N & (~(alignMask - 1)));
		}


	}
}
