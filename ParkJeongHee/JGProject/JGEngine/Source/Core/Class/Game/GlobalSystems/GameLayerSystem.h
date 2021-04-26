#pragma once


#include "Class/Game/GameSystem.h"
namespace JG
{

	class IMesh;
	class IMaterial;
	class Camera;
	struct GameLayerInfo
	{
		String LayerName = TT("Default");
		i64    SortDepth = 0;
	};

	// ������ ����
	struct GameRenderItem
	{
		//
		GameLayerInfo LayerInfo;
		SharedPtr<IMesh> Mesh;
		SharedPtr<IMaterial> Material;
	};

	class GameLayer
	{
		friend class GameLayerSystem;

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
	class GameLayerSystem : public GlobalGameSystem
	{
		GAMECLASS
	private:
		List<UniquePtr<GameLayer>> mGameLayerList;
		Dictionary<String, GameLayer*> mGameLayerByName;
		// ���̾� �̸� ����Ʈ
		// ���̾ ���� 
	public:
		void AddGameLayer(const String& name, i64 priority);
		void RemoveGameLayer(const String& name);
		GameLayer* GetGameLayer(const String& name) const;

	};
}