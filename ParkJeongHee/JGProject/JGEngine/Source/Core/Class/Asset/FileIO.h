#pragma once
#include "JGCore.h"
#include <fstream>



namespace JG
{
	class FileStreamWriter;
	class FileStreamReader;
	class ISerializable
	{
		friend FileStreamWriter;
		friend FileStreamReader;
	protected:
		virtual void Serialize(FileStreamWriter* writer)   const = 0;
		virtual void DeSerialize(FileStreamReader* reader)  = 0;
	};


	class FileStreamWriter
	{

		std::ofstream mFout;
	public:
		FileStreamWriter();
		~FileStreamWriter();


	public:
		bool Open(const String& path, bool isAppend = false);
		void Close();
		bool IsOpen() const {
			return mFout.is_open();
		}
		template<class T>
		void Write(const T& data)
		{
			if (IsOpen() == true)
			{
				if constexpr (std::is_base_of<ISerializable, T>::value == true)
				{
					static_cast<const ISerializable*>(&data)->Serialize(this);
				}
				else
				{
					Write(&data, sizeof(T));
				}
			}
		}

		template<>
		void Write(const String& data)
		{
			if (data.empty() == true)
			{
				return;
			}
			if (IsOpen() == true)
			{
				Write(data.length());
				Write((void*)data.c_str(), sizeof(wchar_t) * data.length());
			}

		}

		template<class T>
		void Write(const List<T>& dataList)
		{
			if (dataList.empty() == true)
			{
				return;
			}
			if (IsOpen() == true)
			{
				Write(dataList.size());
				for (auto& data : dataList)
				{
					Write(data);
				}
			}

		}
	private:
		void Write(const void* data, u64 size);
	};

	class FileStreamReader
	{
		std::ifstream mFin;
	public:
		bool Open(const String& path);
		void Close();
		bool IsOpen() const {
			return mFin.is_open();
		}

		template<class T>
		void Read(T* data)
		{
			if (data == nullptr)
			{
				return;
			}
			if (IsOpen() == true)
			{
				if constexpr (std::is_base_of<ISerializable, T>::value == true)
				{
					static_cast<ISerializable*>(data)->DeSerialize(this);
				}
				else
				{
					Read((void*)data, sizeof(T));
				}
			}
		}
		template<>
		void Read(String* data)
		{
			if (data == nullptr)
			{
				return;
			}
			if (IsOpen() == true)
			{
				u64 length = 0;
				Read(&length);
				data->resize(length);
				Read(&(*data)[0], length * sizeof((*data)[0]));
			}


		}

		template<class T>
		void Read(List<T>* dataList)
		{
			if (dataList == nullptr)
			{
				return;
			}
			if (IsOpen() == true)
			{
				u64 size = 0;
				Read(&size);
				dataList->resize(size);
				for (u64 i = 0; i < size; ++i)
				{
					Read(&(*dataList)[i]);
				}
			}
		
		}
	private:
		void Read(void* data, u64 size);





	};
}