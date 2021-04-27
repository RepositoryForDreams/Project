#pragma once
#include "Class/Game/GameComponent.h"



namespace JG
{
	class Transform : public GameComponent
	{
		JGCLASS
	private:
		JVector3 mLocation;
		JVector3 mRotation;
		JVector3 mScale = { 1.0f,1.0f,1.0f };

		mutable bool mIsDirty = true;
	public:

		void SetLocation(const JVector3& location);
		void SetRotation(const JVector3& rotation);
		void SetScale(const JVector3& scale);


		const JVector3& GetLocalLocation() const;
		const JVector3& GetLocalRotation() const;
		JVector3 GetWorldLocation() const;
		JVector3 GetWorldRotation() const;
		const JVector3& GetScale() const;
	public:
		virtual void OnInspectorGUI() override;
	private:
		void SendDirty();
	};
}