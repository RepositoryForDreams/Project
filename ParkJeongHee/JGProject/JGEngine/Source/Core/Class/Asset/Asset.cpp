#include "pch.h"
#include "Asset.h"
#include "AssetManager.h"
#include "Application.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"
namespace JG
{
	void TextureAssetStock::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		jsonData->AddMember("Name", Name);
		jsonData->AddMember("Width", Width);
		jsonData->AddMember("Height", Height);
		jsonData->AddMember("Channels", Channels);
		jsonData->AddMember("PixelPerUnit", PixelPerUnit);
		jsonData->AddMember("Pixels", Pixels);

	}
	void TextureAssetStock::LoadJson(SharedPtr<JsonData> jsonData)
	{
		auto Val = jsonData->GetMember("Name");
		if (Val && Val->IsString())
		{
			Name = Val->GetString();
		}
		Val = jsonData->GetMember("Width");
		if (Val && Val->IsInt32())
		{
			Width = Val->GetInt32();
		}
		Val = jsonData->GetMember("Height");
		if (Val && Val->IsInt32())
		{
			Height = Val->GetInt32();
		}
		Val = jsonData->GetMember("Channels");
		if (Val && Val->IsInt32())
		{
			Channels = Val->GetInt32();
		}
		Val = jsonData->GetMember("PixelPerUnit");
		if (Val && Val->GetUint32())
		{
			PixelPerUnit = Val->GetUint32();
		}

		Val = jsonData->GetMember("Pixels");
		if (Val && Val->IsByteList())
		{
			Pixels = Val->GetByteList();
		}
	}
	void StaticMeshAssetStock::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		jsonData->AddMember("Name", Name);
		jsonData->AddMember("IsSkinned", IsSkinned);
		jsonData->AddMember("BoundingBox", BoundingBox);

		auto meshJson = jsonData->CreateJsonData();
		auto cnt = SubMeshNames.size();
		for (auto i = 0; i < cnt; ++i)
		{
			auto viJson = meshJson->CreateJsonData();
			viJson->AddMember("Name", SubMeshNames[i]);



			auto vArrayJson = viJson->CreateJsonData();
			for (auto& v : Vertices[i])
			{
				auto vertexJson = viJson->CreateJsonData();
				v.MakeJson(vertexJson);
				vArrayJson->AddMember(vertexJson);
			}

			viJson->AddMember("Vertices", vArrayJson);
			viJson->AddMember("Indices", Indices[i]);


			meshJson->AddMember(viJson);
		}
		jsonData->AddMember("SubMeshs", meshJson);
	}
	void StaticMeshAssetStock::LoadJson(SharedPtr<JsonData> jsonData)
	{
		SubMeshNames.clear();
		Vertices.clear();
		Indices.clear();
		auto val = jsonData->GetMember("Name");
		if (val)
		{
			Name = val->GetString();
		}
		val = jsonData->GetMember("IsSkinned");
		if (val)
		{
			IsSkinned = val->GetBool();
		}
		val = jsonData->GetMember("BoundingBox");
		if (val)
		{
			BoundingBox = val->GetBoundingBox();
		}
		val = jsonData->GetMember("SubMeshs");
		if (val)
		{
			auto cnt = val->GetSize();
			Vertices.resize(cnt);
			Indices.resize(cnt);
			SubMeshNames.resize(cnt);
			for (auto i = 0; i < cnt; ++i)
			{
				auto meshJson = val->GetJsonDataFromIndex(i);

				auto verticesJson = meshJson->GetMember("Vertices");
				auto indicesJson  = meshJson->GetMember("Indices");
				auto meshName     = meshJson->GetMember("Name");

				SubMeshNames[i] = meshName->GetString();
				Vertices[i].resize(verticesJson->GetSize());
				Indices[i].resize(indicesJson->GetSize());
				for (auto j = 0; j < verticesJson->GetSize(); ++j)
				{
					auto vJson = verticesJson->GetJsonDataFromIndex(j);
					JGVertex v;
					v.LoadJson(vJson);
					Vertices[i][j] = v;
				}
				for (auto j = 0; j < indicesJson->GetSize(); ++j)
				{
					auto iJson = indicesJson->GetJsonDataFromIndex(j);
					u32 index;
					index = iJson->GetUint32();
					Indices[i][j] = index;
				}

			}
		}
	}
	void MaterialAssetStock::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		jsonData->AddMember("Name", Name);
		jsonData->AddMember("ShaderName", ShaderName);

		auto scriptList = jsonData->CreateJsonData();
		for (auto& script : ShaderScript)
		{
			auto scriptJson = jsonData->CreateJsonData();
			scriptJson->SetString(script);
			scriptList->AddMember(scriptJson);
		}

		jsonData->AddMember("ShaderScriptList", scriptList);
	}
	void MaterialAssetStock::LoadJson(SharedPtr<JsonData> jsonData)
	{
		auto val = jsonData->GetMember("Name");
		if (val && val->IsString())
		{
			Name = val->GetString();
		}

		val = jsonData->GetMember("ShaderName");
		if (val && val->IsString())
		{
			ShaderName = val->GetString();
		}

		val = jsonData->GetMember("ShaderScriptList");
		if (val)
		{
			ShaderScript.resize(val->GetSize());
			for(i32 i = 0; i<val->GetSize(); ++i)
			{
				auto scriptVal = val->GetJsonDataFromIndex(i);
				if (scriptVal == nullptr) continue;

				ShaderScript[i] = scriptVal->GetString();
			}
		}

	}

	AssetDataBase::AssetDataBase()
	{

	}
	AssetDataBase::~AssetDataBase()
	{
		mAssetManagerPool.clear();

		if (mAssetLoadScheduleHandle)
		{
			mAssetLoadScheduleHandle->Reset();
			mAssetLoadScheduleHandle = nullptr;
		}
		if (mAssetUnLoadScheduleHandle)
		{
			mAssetUnLoadScheduleHandle->Reset();
			mAssetUnLoadScheduleHandle = nullptr;
		}
	


		while(mLoadAssetDataQueue.empty() == false)
		{
			mLoadAssetDataQueue.pop();
		}
		while (mUnLoadAssetDataQueue.empty() == false)
		{
			mUnLoadAssetDataQueue.pop();
		}
		mOriginAssetDataPool.clear();
		mAssetDataPool.clear();
	}
	SharedPtr<AssetManager> AssetDataBase::RequestAssetManager()
	{
		SharedPtr<AssetManager> result = nullptr;
		result = CreateSharedPtr<AssetManager>();
		mAssetManagerPool[result.get()] = result;
		return result;
	}
	void AssetDataBase::ReturnAssetManager(SharedPtr<AssetManager> assetManager)
	{
		mAssetManagerPool.erase(assetManager.get());
	}

	EAssetFormat AssetDataBase::GetAssetFormat(const String& path)
	{
		String absolutePath;
		String resourcePath;
		if (GetResourcePath(path, absolutePath, resourcePath) == false)
		{
			return EAssetFormat::None;
		}
		{
			std::shared_lock<std::shared_mutex> lock(mAssetFormatMutex);
			auto iter = mOriginAssetFormatPool.find(resourcePath);
			if (iter != mOriginAssetFormatPool.end())
			{
				return iter->second;
			}
		}


		LoadOriginAsset(resourcePath);
		return Json::GetAssetFormat(absolutePath);
	}

	AssetID AssetDataBase::LoadOriginAsset(const String& path)
	{
		String resourcePath;
		String absolutePath;
		if (GetResourcePath(path, absolutePath, resourcePath) == false)
		{
			return AssetID();
		}

		if (mAssetLoadScheduleHandle == nullptr)
		{
			mAssetLoadScheduleHandle = 
				Scheduler::GetInstance().ScheduleByFrame(0, 1, -1, SchedulePriority::EndSystem, SCHEDULE_BIND_FN(&AssetDataBase::LoadAsset_Update));
		}
		// 에셋 검사
		auto iter = mOriginAssetDataPool.find(resourcePath);
		if (iter != mOriginAssetDataPool.end())
		{
			iter->second->RefCount++;
			return iter->second->ID;
		}
	
		auto assetID   = RequestOriginAssetID(resourcePath);
		auto assetData = CreateUniquePtr<AssetData>();
		AssetLoadData assetLoadData;

		assetData->ID       = assetID;
		assetData->State    = EAssetDataState::Loading;
		assetData->Path		= absolutePath;
		assetLoadData.ID	= assetID;
		strcpy(assetLoadData.Path, path.c_str());

		// 에셋 추가
		mOriginAssetDataPool.emplace(resourcePath, assetData.get());
		mLoadAssetDataQueue.push(assetLoadData);
		mAssetDataPool.emplace(assetID, std::move(assetData));
	
		return assetID;
	}
	AssetID AssetDataBase::LoadReadWriteAsset(AssetID originID)
	{
		return AssetID();
	}
	void AssetDataBase::UnLoadAsset(AssetID id)
	{
		if (mAssetUnLoadScheduleHandle == nullptr)
		{
			mAssetUnLoadScheduleHandle =
				Scheduler::GetInstance().ScheduleByFrame(0, 1, -1, SchedulePriority::BeginSystem, SCHEDULE_BIND_FN(&AssetDataBase::UnLoadAsset_Update));
		}
		auto iter = mAssetDataPool.find(id);
		if (iter == mAssetDataPool.end())
		{
			return;
		}
		iter->second->RefCount--;
		if (iter->second->RefCount <= 0)
		{
			AssetUnLoadData unLoadData;
			unLoadData.Asset = iter->second->Asset;
			unLoadData.ID = iter->second->ID;
			mUnLoadAssetDataQueue.push(unLoadData);
			if (id.IsOrigin())
			{
				mOriginAssetDataPool.erase(id.ResourcePath);
			}
			mAssetDataPool.erase(id);
		}
	}

	AssetID AssetDataBase::RequestOriginAssetID(const String& resourcePath)
	{
		AssetID id;
		if (mAssetIDQueue.empty() == false)
		{
			id.Origin	 = mAssetIDQueue.front(); mAssetIDQueue.pop();
			id.ID		 = id.Origin;
		}
		else
		{
			id.Origin    = mAssetIDOffset++;
			id.ID		 = id.Origin;
		}
		strcpy(id.ResourcePath, resourcePath.c_str());
		return id;
	}

	AssetID AssetDataBase::RequestRWAssetID(AssetID originID)
	{
		if (originID.IsValid() == false || originID.IsOrigin() == false)
		{
			return AssetID();
		}
		if (mAssetDataPool.find(originID) == mAssetDataPool.end())
		{
			return AssetID();
		}


		AssetID id = RequestOriginAssetID(originID.ResourcePath);
		id.Origin = originID.GetID();
		return id;
	}
	void AssetDataBase::LoadAssetInternal(AssetLoadData* LoadData)
	{
		fs::path assetPath = LoadData->Path;
		if (fs::exists(assetPath) == false)
		{
			return;
		}
		EAssetFormat assetFormat = EAssetFormat::None;
		auto json = CreateSharedPtr<Json>();
		if (Json::Read(assetPath.string(), json) == false)
		{
			return;
		}
		auto assetFormatVal = json->GetMember(JG_ASSET_FORMAT_KEY);
		if (assetFormatVal)
		{
			assetFormat = (EAssetFormat)assetFormatVal->GetUint64();
		}
		auto assetVal = json->GetMember(JG_ASSET_KEY);
		if (assetVal == nullptr)
		{
			return;
		}

		// 에셋 로드
		switch (assetFormat)
		{
		case EAssetFormat::Texture:
		{
			LoadData->Stock = CreateSharedPtr<TextureAssetStock>();
			LoadData->Asset = CreateSharedPtr<Asset<ITexture>>(assetPath.string());
			LoadData->OnComplete = std::bind(&AssetDataBase::TextureAsset_OnCompelete, this, std::placeholders::_1);
			LoadData->Stock->LoadJson(assetVal);
			break;
		}
		case EAssetFormat::Mesh:
		{
			StaticMeshAssetStock stock;
			stock.LoadJson(assetVal);


			auto meshAsset = CreateSharedPtr<Asset<IMesh>>(assetPath.string());
			meshAsset->mData = IMesh::Create(stock);


			LoadData->Asset = meshAsset;
			break;
		}
		case EAssetFormat::Material:
		{
			List<String> scriptNameList;
			MaterialAssetStock stock;
			stock.LoadJson(assetVal);


			i32 cnt = 0;
			for (auto& script : stock.ShaderScript)
			{
				auto name = stock.Name + "_Script_" + std::to_string(cnt);
				ShaderLibrary::GetInstance().RegisterScirpt(IShaderScript::CreateMaterialScript(name, script));
				scriptNameList.push_back(name);

				cnt++;
			}

			auto materialAsset = CreateSharedPtr<Asset<IMaterial>>(assetPath.string());
			auto shader = ShaderLibrary::GetInstance().GetShader(stock.ShaderName, scriptNameList);
			if (shader == nullptr)
			{
				return;
			}

			materialAsset->mData = IMaterial::Create(stock.Name, shader);
			LoadData->Asset = materialAsset;
			break;
		}
		default:
			JG_CORE_ERROR("{0} AssetFormat is not supported in LoadAsset", (int)assetFormat);
			break;
		}

		{
			std::lock_guard<std::shared_mutex> lock(mAssetFormatMutex);
			mOriginAssetFormatPool[LoadData->ID.ResourcePath] = assetFormat;
		}

	}


	EScheduleResult AssetDataBase::LoadAsset_Update()
	{
	
		{
			std::lock_guard<std::mutex> lock(mCompeleteMutex);
			while (mLoadCompeleteAssetDataQueue.empty() == false)
			{
				auto compeleteData = mLoadCompeleteAssetDataQueue.front(); mLoadCompeleteAssetDataQueue.pop();

				if (compeleteData.Asset == nullptr)
				{
					auto iter = mAssetDataPool.find(compeleteData.ID);
					if (iter != mAssetDataPool.end())
					{
						JG_CORE_ERROR("Asset Load Fail  : {0}", iter->second->Path);
						mOriginAssetDataPool.erase(iter->second->Path);
						mAssetDataPool.erase(compeleteData.ID);
					}
				}
				else
				{
					if (compeleteData.OnComplete)
					{
						compeleteData.OnComplete(&compeleteData);
					}
					auto iter = mAssetDataPool.find(compeleteData.ID);
					if (iter != mAssetDataPool.end())
					{
						iter->second->Asset = compeleteData.Asset;
						iter->second->State = EAssetDataState::None;
						JG_CORE_INFO("Asset Load Success : {0}", iter->second->Path);
					}
				}

			}
		}
		u32 loopCnt = 0;
		while (mLoadAssetDataQueue.empty() == false)
		{
			auto loadData = mLoadAssetDataQueue.front(); mLoadAssetDataQueue.pop();
			JG_CORE_INFO("Asset Loading... : {0}", loadData.Path);
			Scheduler::GetInstance().ScheduleAsync(
				[&](void* userData)
			{
				LoadAssetInternal((AssetLoadData*)userData);
				AssetLoadData* _LoadData = (AssetLoadData*)userData;
				AssetLoadCompeleteData data;
				data.Asset		= _LoadData->Asset;
				data.ID			= _LoadData->ID;
				data.Stock		= _LoadData->Stock;
				data.OnComplete = _LoadData->OnComplete;
				{
					std::lock_guard<std::mutex> lock(mCompeleteMutex);
					mLoadCompeleteAssetDataQueue.push(data);
				}
			}, &loadData, sizeof(AssetLoadData));
			++loopCnt;

			if (loopCnt <= mMaxLoadAssetDataCount)
			{
				break;
			}
		}
		return EScheduleResult::Continue;
	}
	EScheduleResult AssetDataBase::UnLoadAsset_Update()
	{
		u32 bufferCnt = Application::GetInstance().GetGraphicsAPI()->GetBufferCount();
		List<AssetUnLoadData> aliveList;
		u32 loopCnt = 0;
		while (mUnLoadAssetDataQueue.empty() == false)
		{
			auto unLoadData = mUnLoadAssetDataQueue.front(); mUnLoadAssetDataQueue.pop();


			if (unLoadData.FrameCount < bufferCnt)
			{
				aliveList.push_back(unLoadData);
			}
			else
			{
				unLoadData.Asset.reset();
				unLoadData.Asset = nullptr;
				mAssetIDQueue.push(unLoadData.ID.GetID());
			}
			++unLoadData.FrameCount;
			++loopCnt;
			if (loopCnt <= mMaxUnLoadAssetDataCount)
			{
				break;
			}
		}
		for (auto& data : aliveList)
		{
			mUnLoadAssetDataQueue.push(data);
		}

		return EScheduleResult::Continue;
	}
	void AssetDataBase::TextureAsset_OnCompelete(AssetLoadCompeleteData* data)
	{
		if (data == nullptr || data->Stock == nullptr || data->Asset == nullptr)
		{
			return;
		}
		auto textureAsset = static_cast<Asset<ITexture>*>(data->Asset.get());
		textureAsset->mData = ITexture::Create(*(static_cast<TextureAssetStock*>(data->Stock.get())));
	}

	bool AssetDataBase::GetResourcePath(const String& path, String& out_absolutePath, String& out_resourcePath) const
	{
		auto originPath = path;
		auto absolutePath      = fs::absolute(path).string();
		auto absoluteAssetPath = fs::absolute(Application::GetAssetPath()).string();
		auto homePath = fs::current_path().string();

		originPath        = ReplaceAll(originPath, "\\", "/");
		absolutePath      = ReplaceAll(absolutePath, "\\", "/");
		absoluteAssetPath = ReplaceAll(absoluteAssetPath, "\\", "/");
		homePath		  = ReplaceAll(homePath, "\\", "/");


		String resourcePath;

		// 
		if (absolutePath.find(absoluteAssetPath) == String::npos )
		{
			if (path.find_first_of("Asset/") != String::npos) {

				resourcePath = ReplaceAll(absolutePath, homePath + "/", "");
				absolutePath = ReplaceAll(absolutePath, homePath + "/", "");
				absolutePath = ReplaceAll(absolutePath, "Asset/", "");
				absolutePath = CombinePath(absoluteAssetPath, absolutePath);
				out_absolutePath = absolutePath;
				out_resourcePath = resourcePath;
				return true;
			}
		}
		// in AssetPath
		else
		{
			resourcePath = ReplaceAll(absolutePath, absoluteAssetPath, "Asset");
			out_absolutePath = absolutePath;
			out_resourcePath = resourcePath;
			return true;
		}


		JG_CORE_ERROR("{0} is not exist in AssetFolder", path);
		return false;
	}

}