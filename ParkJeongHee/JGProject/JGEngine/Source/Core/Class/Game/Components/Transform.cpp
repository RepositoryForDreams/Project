#include "pch.h"
#include "Transform.h"


namespace JG
{
	void Transform::SetLocation(const JVector3& location)
	{
		mLocation = location;
		SendDirty();
	}
	void Transform::SetRotation(const JVector3& rotation)
	{
		mRotation = rotation;
		SendDirty();
	}
	void Transform::SetScale(const JVector3& scale)
	{
		mScale = scale;
		SendDirty();
	}
	const JVector3& Transform::GetLocalLocation() const
	{

		// TODO: ���⿡ return ���� �����մϴ�.
	}
	const JVector3& Transform::GetLocalRotation() const
	{
		// TODO: ���⿡ return ���� �����մϴ�.
	}
	JVector3 Transform::GetWorldLocation() const
	{
		return JVector3();
	}
	JVector3 Transform::GetWorldRotation() const
	{
		return JVector3();
	}
	const JVector3& Transform::GetScale() const
	{
		// TODO: ���⿡ return ���� �����մϴ�.
	}
	void Transform::SendDirty()
	{
		mIsDirty = true;
	}
}