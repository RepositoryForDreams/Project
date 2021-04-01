#pragma once
#include "Define.h"
#include "Abstract.h"


#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace JG
{
	class Log : public GlobalSingleton<Log>
	{
	private:
		SharedPtr<spdlog::logger> smCoreLogger;
		SharedPtr<spdlog::logger> smClientLogger;
	public:
		inline static SharedPtr<spdlog::logger>& GetCoreLogger()  { return GetInstance().smCoreLogger; }
		inline static SharedPtr<spdlog::logger>& GetClientLogger() { return GetInstance().smClientLogger; }
		Log()
		{
			spdlog::set_pattern("%^[%T] %n: %v%$");
			smCoreLogger = spdlog::stdout_color_mt("Core");
			smCoreLogger->set_level(spdlog::level::trace);
			smClientLogger = spdlog::stdout_color_mt("App");
			smClientLogger->set_level(spdlog::level::trace);
		}
		~Log()
		{
			smCoreLogger.reset();
			smClientLogger.reset();
			
			spdlog::shutdown();
		}

	};
}


#if   _DEBUG
#define JG_CORE_TRACE(...)   ::JG::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define JG_CORE_INFO(...)     ::JG::Log::GetCoreLogger()->info(__VA_ARGS__)
#define JG_CORE_WARN(...)     ::JG::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define JG_CORE_ERROR(...)    ::JG::Log::GetCoreLogger()->error(__VA_ARGS__)
#define JG_CORE_CRITICAL(...) ::JG::Log::GetCoreLogger()->critical(__VA_ARGS__)
#elif _RELEASE
#define JG_CORE_TRACE(...)   
#define JG_CORE_INFO(...)     
#define JG_CORE_WARN(...)     
#define JG_CORE_ERROR(...)    
#define JG_CORE_CRITICAL(...) 
#endif //  

#if   _DEBUG
#define JG_TRACE(...)   ::JG::Log::GetClientLogger()->trace(__VA_ARGS__)
#define JG_INFO(...)     ::JG::Log::GetClientLogger()->info(__VA_ARGS__)
#define JG_WARN(...)     ::JG::Log::GetClientLogger()->warn(__VA_ARGS__)
#define JG_ERROR(...)    ::JG::Log::GetClientLogger()->error(__VA_ARGS__)
#define JG_CRITICAL(...) ::JG::Log::GetClientLogger()->critical(__VA_ARGS__)
#elif _RELEASE
#define JG_TRACE(...)   
#define JG_INFO(...)     
#define JG_WARN(...)     
#define JG_ERROR(...)    
#define JG_CRITICAL(...) 
#endif //  