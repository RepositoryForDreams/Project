#pragma once
#include "Define.h"
namespace JG
{


	template<class T, class ...Args>
	class GlobalSingleton
	{
	private:
		static T* smInstance;
	protected:
		static void Create(Args... args)
		{
			if (smInstance != nullptr) return;

			smInstance = new T(args...);
		}
		static void Destroy()
		{
			if (smInstance == nullptr) return;
			delete smInstance;
			smInstance = nullptr;
		}
	public:
		static bool IsValid()
		{
			return smInstance != nullptr;
		}
		static T& GetInstance() {
			return *smInstance;
		}
	};


	template<class T, class ...Args>
	T* GlobalSingleton<T, Args...>::smInstance = nullptr;


	//template<class T>
	//class CustomWeakPtr
	//{
	//protected:
	//	T*            mPtr = nullptr;
	//	WeakPtr<bool> mIsValid;
	//public:
	//	CustomWeakPtr(T* ptr) : mPtr(ptr)
	//	{
	//		ConstructorImpl();
	//	}
	//	CustomWeakPtr(const CustomWeakPtr& copy)
	//	{
	//		mPtr = copy.mPtr;
	//		mIsValid = copy.mIsValid;
	//	}
	//	CustomWeakPtr& operator=(const CustomWeakPtr& copy)
	//	{
	//		mPtr = copy.mPtr;
	//		mIsValid = copy.mIsValid;
	//		return *this;
	//	}
	//	T* operator->()
	//	{
	//		return mPtr;
	//	}
	//	~CustomWeakPtr()
	//	{
	//		DestructorImpl();
	//	}
	//	bool IsValid() const
	//	{
	//		auto _bool = mIsValid.lock();
	//		if (_bool)
	//		{
	//			return *(_bool.get());
	//		}
	//		return false;
	//	}
	//private:
	//	CustomWeakPtr(CustomWeakPtr&& rhs) = delete;
	//	CustomWeakPtr& operator=(CustomWeakPtr&& rhs) = delete;

	//protected:
	//	//bool 
	//	virtual void ConstructorImpl() = 0;
	//	virtual void DestructorImpl() = 0;
	//};




	template<class InterfaceClass, i32 BufferCount, class ...Args>
	class ObjectFactory : public GlobalSingleton<ObjectFactory<InterfaceClass, BufferCount, Args ...>>
	{
	protected:
		Dictionary<InterfaceClass*, SharedPtr<InterfaceClass>> mObjectPool;
		Queue<InterfaceClass*> mReseredDestroyObjectQueue[BufferCount];
		i32 mQueueBufferIndex = 0;
		std::shared_mutex mMutex;
	public:
		ObjectFactory()
		{
			Scheduler::GetInstance().ScheduleByFrame(0, 3, -1, SchedulePriority::DestroyObject, [&]() -> EScheduleResult
			{
				auto& currQueue = mReseredDestroyObjectQueue[mQueueBufferIndex];
				if (currQueue.empty())
				{
					return EScheduleResult::Continue;
				}


				while (currQueue.empty() == false)
				{
					auto destroyObject = currQueue.front(); currQueue.pop();
					DestroyObjectImmediate(destroyObject);
				}

				mQueueBufferIndex = (mQueueBufferIndex + 1) % BufferCount;
				return EScheduleResult::Continue;
			});
		}
	public:

		template<class T>
		T* CreateObject(Args ... args)
		{
			
			SharedPtr<T> object = CreateSharedPtr<T>(args...);
			{
				std::lock_guard<std::shared_mutex> lock(mMutex);
				mObjectPool[object.get()] = object;
			}
			CreateObjectImpl(object.get());
			return object.get();
		}
		void DestroyObjectImmediate(InterfaceClass* ifClass)
		{
			if (ifClass == nullptr)
			{
				return;
			}
			std::lock_guard<std::shared_mutex> lock(mMutex);
			auto iter = mObjectPool.find(ifClass);
			if (iter != mObjectPool.end())
			{
				DestroyObjectImpl(ifClass);
				mObjectPool.erase(iter);
			}
		}

		void DestroyObject(InterfaceClass* ifClass)
		{
			if (ifClass == nullptr)
			{
				return;
			}
			i32 nextIndex   = (mQueueBufferIndex + 1) % BufferCount;
			auto& nextQueue = mReseredDestroyObjectQueue[nextIndex];
			std::lock_guard<std::shared_mutex> lock(mMutex);
			auto iter = mObjectPool.find(ifClass);
			if (iter != mObjectPool.end())
			{
				ReserveDestroyObjectImpl(ifClass);
				nextQueue.push(ifClass);
			}
		}
	protected:
		virtual void CreateObjectImpl(InterfaceClass* ifClass)  = 0;
		virtual void DestroyObjectImpl(InterfaceClass* ifClass) = 0;
		virtual void ReserveDestroyObjectImpl(InterfaceClass* ifClass) = 0;
	};
}






