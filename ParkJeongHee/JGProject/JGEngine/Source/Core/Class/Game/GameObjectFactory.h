#pragma once
#include "JGCore.h"
#include "GameObject.h"

namespace JG
{
	class GameObjectFactory : public ObjectFactory<GameObjectFactory, IGameObject, 2>
	{
		friend class GameLayer;
		List<IGameObject*>  mReserveStartObjects;
		List<IGameObject*>  mReservedDestroyObjects;

	protected:
		virtual void CreateObjectImpl(IGameObject* gameObject)  override;
		virtual void DestroyObjectImpl(IGameObject* gameObject) override;
		virtual void ReserveDestroyObjectImpl(IGameObject* gameObject) override;
	private:
		EScheduleResult UpdateStartObject();
		EScheduleResult UpdateDestroyObject();

	};
}