#pragma once
#include "Define.h"
#include "Type.h"
#include "Class/FileIO.h"
namespace JG
{

#define JGCLASS \
public: \
	virtual Type GetType() const override { return Type(TypeID(this));} \
private: \


	class IInspectorGUI
	{
	public:
		virtual void OnInspectorGUI() = 0;
	};

	class ISubscriber
	{
	public:
		virtual ~ISubscriber() = default;
	};



	

	class IJGObject : public IInspectorGUI, public ISubscriber, public ISerializable
	{
	public:
		virtual Type GetType() const  = 0;
		virtual const String& GetName() const 
		{
			return GetType().GetName();
		}
	protected:
		virtual void Serialize(FileStreamWriter* writer)   const override {}
		virtual void DeSerialize(FileStreamReader* reader) override {}
	public:
		virtual void OnInspectorGUI() override {}
		virtual ~IJGObject() = default;
	};
}