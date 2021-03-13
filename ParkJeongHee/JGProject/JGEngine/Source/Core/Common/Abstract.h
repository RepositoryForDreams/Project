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
		static T& GetInstance() {
			return *smInstance;
		}
	};


	template<class T, class ...Args>
	T* GlobalSingleton<T, Args...>::smInstance = nullptr;

	




}






