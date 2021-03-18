#pragma once
#include "JGCore.h"



namespace JG
{
	class Scheduler : public GlobalSingleton<Scheduler>
	{
		//- Sequence 우선순위 -> 기본 0
		// Start, Update, LateUpdate
		// tick 마다 몇번 호출  ( 끝날때까지 호출), 비동기 여부, Delay등등
		// Frame 마다 몇번 호출 ( 끝날때까지 호출 ), 비동기 여부
	
		// TimerManager에서 Timer 생성
		// StopWatch, Timer, 
	};
}