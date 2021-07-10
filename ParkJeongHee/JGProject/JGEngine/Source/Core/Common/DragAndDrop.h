#pragma once
#include "Type.h"
#include "Object.h"




namespace JG
{
	class IDragAndDropData : public IJGObject
	{
		JGCLASS
	};

	class DDDContentsFile : public IDragAndDropData
	{
		JGCLASS
	public:
		void* pFileInfoPtr = nullptr;
	};

}
