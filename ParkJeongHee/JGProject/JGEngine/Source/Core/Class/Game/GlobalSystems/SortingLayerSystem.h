#pragma once


#include "Class/Game/GameSystem.h"
namespace JG
{

	class IMesh;
	class IMaterial;
	class Camera;
	struct SortingLayerInfo
	{
		String LayerName = TT("Default");
		i64    SortDepth = 0;
	};

	// 렌더링 정보
	struct GameRenderItem
	{
		//
		SortingLayerInfo LayerInfo;
		SharedPtr<IMesh> Mesh;
		SharedPtr<IMaterial> Material;
	};

	class SortingLayer
	{
		friend class SortingLayerSystem;

		// 레이어 이름
		String Name;
		//
		i64 Priority = 0;
		// 정렬되어있는 렌더링 재료들..
		Dictionary<i64, GameRenderItem> SortedRenderItems;
		// 바인딩되어있는 카메라
		List<SharedPtr<Camera>>			BindedCameraList;
	};
/// <summary>
/// 1. 레이어 하나에 카메라 여러개 할당
/// 2. 렌더러에서 원하는 레이어에 Depth, 레이어 이름을 전달하면 레이더에 할당된 카메라에서 그린다.
/// 3. 최종적으로는 메인 카메라에 모든 레이어를 그린다.( 2D 렌더러 )
/// </summary>
/// 
	class SortingLayerSystem : public GlobalGameSystem
	{
		JGCLASS
	private:
		List<UniquePtr<SortingLayer>>     mSortingLayerList;
		Dictionary<String, SortingLayer*> mSortingLayerByName;
		// 레이어 이름 리스트
		// 레이어별 담을 
	public:
		void AddSortingLayer(const String& name, i64 priority);
		void RemoveSortingLayer(const String& name);
		//
		void BindCamera(const String& layerName, SharedPtr<Camera> camera);
		void UnBindCamera(const String& layerName, SharedPtr<Camera> camera);
		//
		// 
		//
		SortingLayer* GetSortingLayer(const String& name) const;

	};
}