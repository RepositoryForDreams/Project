#include "pch.h"
#include "Material.h"


namespace JG
{
	SharedPtr<IMaterial> IMaterial::Create(SharedPtr<IShader> shader)
	{
		return SharedPtr<IMaterial>();
	}


	SharedPtr<IMaterialInstance> IMaterialInstance::Create(SharedPtr<IMaterial> material)
	{
		return SharedPtr<IMaterialInstance>();
	}
}