#pragma once
#include "TypeDefine.h"



namespace JG
{
	namespace JGMath
	{
		template<class T>
		inline T AlignUp(T N, T alignMask)
		{
			return N + N - (N & (~(alignMask - 1)));
		}


	}
}
