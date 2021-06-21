#include "pch.h"
#include "Transform.h"
#include "Class/Game/GameNode.h"

namespace JG
{
	void Transform::SetLocalLocation(const JVector3& location)
	{
		bool isDirty = mLocation != location;
		mLocation = location;
		if (isDirty)
		{
			SendDirty();
		}
	}
	void Transform::SetLocalRotation(const JVector3& rotation)
	{
		bool isDirty = mRotation != rotation;
		mRotation = rotation;
		if (isDirty)
		{
			SendDirty();
		}
	}
	void Transform::SetScale(const JVector3& scale)
	{
		bool isDirty = mScale != scale;
		mScale = scale;
		if (isDirty)
		{
			SendDirty();
		}
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
		return mLocation;
	}
	JVector3 Transform::GetWorldRotation() const
	{
		return mRotation;
	}
	const JVector3& Transform::GetScale() const
	{
		return mScale;
	}
	const JMatrix& Transform::GetWorldMatrix() const
	{
		UpdateWorldMatrix();
		return mWorldMatrix;
	}
	void Transform::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		GameComponent::MakeJson(jsonData);
		jsonData->AddMember("Location", GetLocalLocation());
		jsonData->AddMember("Rotation", GetLocalRotation());
		jsonData->AddMember("Scale", GetScale());

	}
	void Transform::LoadJson(SharedPtr<JsonData> jsonData)
	{
		GameComponent::LoadJson(jsonData);

		auto val = jsonData->GetMember("Location");
		if (val)
		{
			SetLocalLocation(val->GetVector3());
		}
		val = jsonData->GetMember("Rotation");
		if (val)
		{
			SetLocalRotation(val->GetVector3());
		}
		val = jsonData->GetMember("Scale");
		if (val)
		{
			SetScale(val->GetVector3());
		}
	}
	void Transform::UpdateWorldMatrix() const
	{
		if (mIsDirty == false)
		{
			return;
		}
		mIsDirty = true;

		mLocalMatrix = JMatrix::Scaling(mScale) * JMatrix::Rotation(JQuaternion::ToQuaternion(
			JVector3(Math::ConvertToRadians(mRotation.x),
				Math::ConvertToRadians(mRotation.y),
				Math::ConvertToRadians(mRotation.z)))) * JMatrix::Translation(mLocation);

		mWorldMatrix = mLocalMatrix;
	}
	void Transform::OnInspectorGUI()
	{
		float padding = ImGui::CalcTextSize("Location").x - ImGui::CalcTextSize("Scale").x;
		
		auto location = GetLocalLocation();
		auto rotation = GetLocalRotation();
		auto scale = GetScale();



		ImGui::OnGui("Location", &location);
		ImGui::OnGui("Rotation", &rotation);
		ImGui::OnGui("Scale", &scale, padding);

		SetLocalLocation(location);
		SetLocalRotation(rotation);
		SetScale(scale);
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