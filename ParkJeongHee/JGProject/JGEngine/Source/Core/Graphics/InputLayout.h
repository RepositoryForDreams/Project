#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"



namespace JG
{
	class InputElement
	{
	public:
		const char* SementicName = nullptr;
		u32 SementicSlot = 0;
		ShaderDataType Type = ShaderDataType::unknown;
	public:
		InputElement(ShaderDataType type, const char* sementicName, u32 sementicSlot)
			: Type(type), SementicName(sementicName), SementicSlot(sementicSlot) {}
	};
	class InputLayout
	{
	private:
		std::vector<InputElement> mInputElements;
	public:
		void Add(ShaderDataType type, const char* sementicName, u32 sementicSlot)
		{
			mInputElements.push_back(InputElement(type, sementicName, sementicSlot));
		}
		void ForEach(const std::function<void(const InputElement&)>& action)
		{
			for (auto& element : mInputElements)
			{
				action(element);
			}
		}
		static SharedPtr<InputLayout> Create()
		{
			return CreateSharedPtr<InputLayout>();
		}

	};
}
