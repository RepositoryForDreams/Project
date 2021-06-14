#pragma once
#include <fstream>
#include "Common/Define.h"


namespace JG
{
	class ISerializable
	{
		friend class FileStreamWriter;
		friend class FileStreamReader;
	protected:
		virtual void Serialize(FileStreamWriter* writer)   const = 0;
		virtual void DeSerialize(FileStreamReader* reader) = 0;
	public:
		virtual ~ISerializable() = default;
	};
	class FileStream
	{
		
	public:
		class Header
		{
		public:
			Dictionary<String, u64> DataMap;
			u64 Offset = 0;
		public:
			void Write(std::ofstream& fout);
			void Read(std::ifstream& fin);
		public:
			u64 GetOffset() const;
			u64 CalcSize() const;
		};

		class Data
		{
		public:
			List<jbyte> ByteData;
		};
	protected:
		Header     mHeader;
		List<Data> mDatas;
	public:
		virtual ~FileStream() = default;
	};

	class FileStreamWriter : public FileStream
	{
	private:
		std::ofstream mFout;
		u64 mOffset = 0;
	public:
		FileStreamWriter();
		virtual ~FileStreamWriter();
	public:
		bool Open(const String& path, bool isAppend = false);
		void Close();
		bool IsOpen() const {
			return mFout.is_open();
		}

		template<class T>
		void Write(const String& key, const T& data)
		{
			mHeader.DataMap[key] = mOffset;
			Write(data);
		}

	private:
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

		template<class T1, class T2>
		void Write(const Dictionary<T1, T2>& dic)
		{
			if (dic.empty() == true)
			{
				return;
			}
			if (IsOpen() == true)
			{
				Write(dic.size());
				for (auto& _pair : dic)
				{
					Write(_pair.first);
					Write(_pair.second);
				}
			}
		}

		template<class T1, class T2>
		void Write(const SortedDictionary<T1, T2>& dic)
		{
			if (dic.empty() == true)
			{
				return;
			}
			if (IsOpen() == true)
			{
				Write(dic.size());
				for (auto& _pair : dic)
				{
					Write(_pair.first);
					Write(_pair.second);
				}
			}
		}
	private:
		void Write(const void* data, u64 size);
	};

	class FileStreamReader : public FileStream
	{
		std::ifstream mFin;
	public:
		virtual ~FileStreamReader() = default;
	public:
		bool Open(const String& path);
		void Close();
		bool IsOpen() const {
			return mFin.is_open();
		}

		template<class T>
		void Read(const String& key, T* data)
		{
			auto& iter = mHeader.DataMap.find(key);
			if (iter == mHeader.DataMap.end())
			{
				data = nullptr;
				return;
			}
			u64 pos = iter->second + mHeader.GetOffset();
			mFin.seekg(pos, std::ios_base::beg);
			Read(data);
		}
	private:
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

		template<class T1, class T2>
		void Read(Dictionary<T1, T2>* dic)
		{
			if (dic == nullptr)
			{
				return;
			}
			if (IsOpen() == true)
			{
				u64 size = 0;
				Read(&size);
				for (u64 i = 0; i < size; ++i)
				{
					T1 key;
					T2 value;
					Read(&key);
					Read(&value);
					dic->emplace(key, value);
				}
			}
		}

		template<class T1, class T2>
		void Read(SortedDictionary<T1, T2>* dic)
		{
			if (dic == nullptr)
			{
				return;
			}
			if (IsOpen() == true)
			{
				u64 size = 0;
				Read(&size);

				for (u64 i = 0; i < size; ++i)
				{
					T1 key;
					T2 value;
					Read(&key);
					Read(&value);
					dic->emplace(key, value);
				}
			}
		}
	private:
		void Read(void* data, u64 size);





	};
}