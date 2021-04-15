#pragma once
#include "JGCore.h"
#include "Common/Type.h"
namespace JG		
{
#define GAMECLASS \
	virtual Type GetObjectType() const override { return Type(TypeID(this));} \

	template<class T>
	using GamePtr = WeakPtr<T>;



	class IGameObject
	{
		friend class GameObjectFactory;
	protected:
		virtual void Start()   = 0;
		virtual void Destory() = 0;
	public:
		virtual u64           GetID()   const = 0;
		virtual const String& GetName() const = 0;
		virtual Type GetObjectType() const = 0;
	public:
		virtual void SetName(const String& name) = 0;
	};


	class GameObject : public IGameObject, public ISubscriber
	{
		GAMECLASS
	private:
		mutable u64     mID = 0;
		mutable String mName;
		List<SharedPtr<ScheduleHandle>> mScheduleHandleList;
	public:
		GameObject() = default;
		virtual ~GameObject();
	protected:
		virtual void Start() override {}
		virtual void Destory() override {}
	protected: // ½ºÄÉÁì °ü·Ã
		SharedPtr<ScheduleHandle> Schedule(f32 delay, f32 tickCycle, i32 repeat, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleOnce(f32 delay, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleByFrame(i32 delayFrame, i32 frameCycle, i32 repeat, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleOnceByFrame(i32 delayFrame, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleAsync(const AsyncTaskFunction& task);

		template<class T>
		void SendEvent(T& e)
		{
			Application::GetInstance().SendEvent(e);
		}


		template<class T>
		void SendEventImmediate(T& e)
		{
			Application::GetInstance().SendEventImmediate(e);
		}

	public:
		virtual u64 GetID() const override;
		virtual const String& GetName() const override;
		virtual void SetName(const String& name) override;
	private:
		void ArrangeScheduleHandle();
		//void 
	};
}