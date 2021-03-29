#pragma once

namespace JG
{


	template<class T, class ...Args>
	class GlobalSingleton
	{
	private:
		static T* smInstance;
	public:
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

	




	template<class T>
	class CustomWeakPtr
	{
	protected:
		T*            mPtr = nullptr;
		WeakPtr<bool> mIsValid;
	public:
		CustomWeakPtr(T* ptr) : mPtr(ptr)
		{
			ConstructorImpl();
		}
		CustomWeakPtr(const CustomWeakPtr& copy)
		{
			mPtr = copy.mPtr;
			mIsValid = copy.mIsValid;
		}
		CustomWeakPtr& operator=(const CustomWeakPtr& copy)
		{
			mPtr = copy.mPtr;
			mIsValid = copy.mIsValid;
			return *this;
		}
		T* operator->()
		{
			return mPtr;
		}
		~CustomWeakPtr()
		{
			DestructorImpl();
		}
		bool IsValid() const
		{
			auto _bool = mIsValid.lock();
			if (_bool)
			{
				return *(_bool.get());
			}
			return false;
		}
	private:
		CustomWeakPtr(CustomWeakPtr&& rhs) = delete;
		CustomWeakPtr& operator=(CustomWeakPtr&& rhs) = delete;

	protected:
		//bool 
		virtual void ConstructorImpl() = 0;
		virtual void DestructorImpl() = 0;
	};



}






