#pragma once


#include "Event/Events.h"
#include "Command.h"


namespace JG
{
	class ILayer : public ISubscriber
	{
	public:
		virtual ~ILayer() {}
	public:
		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;


		virtual void Update()     = 0;
		virtual void LateUpdate() = 0;

		virtual void Begin()   = 0;
		virtual void Destroy() = 0;

		virtual void OnEvent(IEvent& e) = 0;

		virtual String GetLayerName() = 0;
	};

	
}