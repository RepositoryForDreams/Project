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

	// ������ ����
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

		// ���̾� �̸�
		String Name;
		//
		i64 Priority = 0;
		// ���ĵǾ��ִ� ������ ����..
		Dictionary<i64, GameRenderItem> SortedRenderItems;
		// ���ε��Ǿ��ִ� ī�޶�
		List<SharedPtr<Camera>>			BindedCameraList;
	};
/// <summary>
/// 1. ���̾� �ϳ��� ī�޶� ������ �Ҵ�
/// 2. ���������� ���ϴ� ���̾ Depth, ���̾� �̸��� �����ϸ� ���̴��� �Ҵ�� ī�޶󿡼� �׸���.
/// 3. ���������δ� ���� ī�޶� ��� ���̾ �׸���.( 2D ������ )
/// </summary>
/// 
	class SortingLayerSystem : public GlobalGameSystem
	{
		JGCLASS
	private:
		List<UniquePtr<SortingLayer>>     mSortingLayerList;
		Dictionary<String, SortingLayer*> mSortingLayerByName;
		// ���̾� �̸� ����Ʈ
		// ���̾ ���� 
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