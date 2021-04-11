#pragma once
#include "JGCore.h"
#include "GameObject.h"

namespace JG
{
	class GameObjectFactory : public ObjectFactory<IGameObject, 2>
	{
		List<IGameObject*>  mReserveStartObjects;
		List<IGameObject*>  mReservedDestroyObjects;
	protected:
		virtual void CreateObjectImpl(IGameObject* gameObject)  override;
		virtual void DestroyObjectImpl(IGameObject* gameObject) override;
		virtual void ReserveDestroyObjectImpl(IGameObject* gameObject) override;
	private:
		EScheduleResult StartObject();
		EScheduleResult DestroyObject();

	};
}