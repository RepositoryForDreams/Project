#pragma once
#include "JGCore.h"



namespace JG
{
	class Scheduler : public GlobalSingleton<Scheduler>
	{
		//- Sequence �켱���� -> �⺻ 0
		// Start, Update, LateUpdate
		// tick ���� ��� ȣ��  ( ���������� ȣ��), �񵿱� ����, Delay���
		// Frame ���� ��� ȣ�� ( ���������� ȣ�� ), �񵿱� ����
	
		// TimerManager���� Timer ����
		// StopWatch, Timer, 
	};
}