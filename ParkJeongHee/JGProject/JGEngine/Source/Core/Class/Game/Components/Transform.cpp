#include "pch.h"
#include "Transform.h"
#include "Class/Game/GameNode.h"

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
		return mLocation;
	}
	const JVector3& Transform::GetLocalRotation() const
	{
		return mRotation;
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
		return mScale;
	}
	void Transform::OnInspectorGUI()
	{
		float padding = ImGui::CalcTextSize("Location").x - ImGui::CalcTextSize("Scale").x;
		
		ImGui::OnGui("Location", &mLocation);
		ImGui::OnGui("Rotation", &mRotation);
		ImGui::OnGui("Scale", &mScale, padding);
	}
	void Transform::SendDirty()
	{
		mIsDirty = true;
		ChangeData data;
		data.Type = GetType();
		data.Object = this;
		GetOwner()->SendChangeData(data);
	}
}