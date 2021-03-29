#pragma once
#include "Class/UI/UIViewModel.h"




namespace JG
{
	class StatisticsViewModel : public UIViewModel
	{
	public:
		struct Renderer2DStatistics
		{
			u64 DrawCalls = 0;
			u64 QuadCount = 0;
		public:
			Renderer2DStatistics(u64 drawCalls, u64 quadCount) : DrawCalls(drawCalls), QuadCount(quadCount) {}
		};


	protected:
		virtual bool Initialize() override;
		virtual void Destroy() override;

	public:
		Renderer2DStatistics GetRenderer2DStats() const;
	};
}