#pragma once
#include "Class/UI/UIViewModel.h"
#include "Class/Asset/Asset.h"



namespace JG
{
	// History ����
	// 
	class ContentsFileInfo
	{
	public:
		String Name;
		String Path;
		EAssetFormat FileFormat;

		ContentsFileInfo* OwnerDirectory = nullptr;
		List<ContentsFileInfo*> DirectoryList;
		List<ContentsFileInfo*> FileList;
	};

	struct ContentsDirectoryNode
	{
	public:
		String Path;
		u64  UserFlags  = 0;
		bool IsTreePop  = true;
		bool IsSelected = false;
	};

	class ContentsViewModel : public UIViewModel
	{
	private:
		struct ThreadLoadData
		{
			// �Ѱ��־���� ������
			Dictionary<String, ContentsDirectoryNode> ContentsDirectoryNodePool;

			// �ֹ߼� ������
			Dictionary<ContentsFileInfo*, UniquePtr<ContentsFileInfo>> ContentsFileInfoPool;
			Dictionary<String, ContentsFileInfo*> ContentsFileInfoByPath;
			List<ContentsFileInfo*> DirectoryList;
			List<String> AddedDirectoryList;
			List<String> RemovedDirectoryList;
	

			ContentsFileInfo* ContentsRoot = nullptr;
		};
	private:
		UniquePtr<ThreadLoadData> mThreadLoadData = nullptr;


		SharedPtr<ScheduleHandle> mControlUpdateHandle;
		SharedPtr<ScheduleHandle> mAsyncUpdateHandle;

		// 1�ʿ� �ѹ� �� �˻�
		Dictionary<ContentsFileInfo*, UniquePtr<ContentsFileInfo>> mContentsFileInfoPool;
		Dictionary<String, ContentsFileInfo*>                      mContentsFileInfoByPath;

		//
		Dictionary<String, ContentsDirectoryNode> mContentsDirectoryNodePool;
		ContentsDirectoryNode mContentsDirRootNode;


		

		Queue<String> mHistoryQueue;
		String mSelectedDir;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	public:
		void ForEeach(
			const std::function<bool(ContentsDirectoryNode*)>& pushAction,
			const std::function<void(ContentsDirectoryNode*)>& action,
			const std::function<void(ContentsDirectoryNode*)>& popAction);
		ContentsFileInfo* GetContentsFileInfo(const String& path) const;
		const String& GetSelectedContentsDirectory() const;
	private:

		void ForEeach(
			ContentsDirectoryNode* CurrNode,
			const std::function<bool(ContentsDirectoryNode*)>& pushAction,
			const std::function<void(ContentsDirectoryNode*)>& action,
			const std::function<void(ContentsDirectoryNode*)>& popAction);

		// �񵿱� �Լ�
	private:

		ContentsFileInfo* Async_CreateContentsFileInfo(
			const String& name, const String& path, EAssetFormat fileFormat, ContentsFileInfo* ownerDirectory, bool isDirectory);
		void Async_UpdateContentsDirectory();
		void Async_UpdateContentsDirectory(ContentsFileInfo* currentFileInfo, const fs::directory_iterator& currentDirIter);
	};
}