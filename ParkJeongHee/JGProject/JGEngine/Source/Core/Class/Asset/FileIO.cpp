#include "pch.h"
#include "FileIO.h"



namespace JG
{
	FileStreamWriter::FileStreamWriter()
	{

	}
	FileStreamWriter::~FileStreamWriter()
	{
		mFout.close();
	}


	bool FileStreamWriter::Open(const String& path, bool isAppend)
	{
		if (isAppend)
		{
			mFout.open(path, std::ios::out | std::ios::binary | std::ios::app);
		}
		else
		{
			mFout.open(path, std::ios::out | std::ios_base::binary);
		}
		
		

		return mFout.is_open();
	}
	void FileStreamWriter::Close()
	{
		if (mFout.is_open())
		{
			mFout.close();
		}
	}
	void FileStreamWriter::Write(const void* data, u64 size)
	{
		mFout.write(reinterpret_cast<const char*>(data), size);
	}
	bool FileStreamReader::Open(const String& path)
	{
		mFin.open(path, std::ios::in | std::ios::binary);



		


		return mFin.is_open();
	}
	void FileStreamReader::Close()
	{
	}
	void FileStreamReader::Read(void* data, u64 size)
	{
		mFin.read(reinterpret_cast<char*>(data), size);
	}
}