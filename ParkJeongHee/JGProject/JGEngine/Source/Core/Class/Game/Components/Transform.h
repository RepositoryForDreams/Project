#pragma once
#include "Class/Game/GameComponent.h"



namespace JG
{
	class Transform : public GameComponent
	{
		GAMECLASS
	private:
		JVector3 mLocation;
		JVector3 mRotation;
		JVector3 mScale;

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

	private:
		void SendDirty();
	};
}