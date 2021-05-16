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


		mutable JMatrix mWorldMatrix;
		mutable JMatrix mLocalMatrix;
		mutable bool mIsDirty = true;
	public:

		void SetLocalLocation(const JVector3& location);
		void SetLocalRotation(const JVector3& rotation);
		void SetScale(const JVector3& scale);


		const JVector3& GetLocalLocation() const;
		const JVector3& GetLocalRotation() const;
		JVector3 GetWorldLocation() const;
		JVector3 GetWorldRotation() const;
		const JVector3& GetScale() const;

		const JMatrix& GetWorldMatrix() const;
	private:
		void UpdateWorldMatrix() const;
	public:
		virtual void OnInspectorGUI() override;
	private:
		void SendDirty();
	};
}