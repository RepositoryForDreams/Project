#pragma once


#include "JGCore.h"



namespace JG
{
	class GameSettings
	{
	private:
		static JVector2 Resolution;
	public:
		static const JVector2& GetResolution() {
			return Resolution;
		}
	};
}