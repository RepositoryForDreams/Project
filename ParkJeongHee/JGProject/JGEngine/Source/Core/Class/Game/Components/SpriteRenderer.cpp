#include "pch.h"
#include "SpriteRenderer.h"
#include "Transform.h"

#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"

#include "Class/Game/GameWorld.h"
#include "Class/Asset/Asset.h"
namespace JG
{
	void SpriteRenderer::Awake()
	{
		mSpriteRI = CreateUniquePtr<StandardSpriteRenderItem>();
		mSpriteRI->TargetLayer = GetOwner()->GetLayer();
		
		auto shader = ShaderLibrary::Get(ShaderScript::Standard2DShader);
		if (shader != nullptr)
		{
			mSpriteRI->Material = IMaterial::Create(shader->GetName() + TT("_Material"), shader);
			mSpriteRI->Material->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
			mSpriteRI->Material->SetBlendState(0, EBlendStateTemplate::Transparent_Default);
		}

		auto inputLayout = InputLayout::Create();
		inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
		inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);

		auto vBuffer = IVertexBuffer::Create(TT("Sprite_VBuffer"), EBufferLoadMethod::CPULoad);
		auto iBuffer = IIndexBuffer::Create(TT("Sprite_IBuffer"), EBufferLoadMethod::CPULoad);


		JGQuadVertex quadVertex[4];
		u32 quadIndex[6];
		quadVertex[0].Position = JVector3(-0.5f, -0.5f, 0.0f);
		quadVertex[1].Position = JVector3(-0.5f, +0.5f, 0.0f);
		quadVertex[2].Position = JVector3(+0.5f, +0.5f, 0.0f);
		quadVertex[3].Position = JVector3(+0.5f, -0.5f, 0.0f);

		quadVertex[0].Texcoord = JVector2(0.0f, 1.0f);
		quadVertex[1].Texcoord = JVector2(0.0f, 0.0f);
		quadVertex[2].Texcoord = JVector2(1.0f, 0.0f);
		quadVertex[3].Texcoord = JVector2(1.0f, 1.0f);
		
		quadIndex[0] = 0;
		quadIndex[1] = 1;
		quadIndex[2] = 2;
		quadIndex[3] = 0;
		quadIndex[4] = 2;
		quadIndex[5] = 3;
		vBuffer->SetData(quadVertex, sizeof(JGQuadVertex), 4);
		iBuffer->SetData(quadIndex, 6);
		mSpriteRI->Mesh = IMesh::Create(TT("QuadMesh"));
		mSpriteRI->Mesh->AddVertexBuffer(vBuffer);
		mSpriteRI->Mesh->SetIndexBuffer(iBuffer);
		mSpriteRI->Mesh->SetInputLayout(inputLayout);
		mSpriteRI->WorldMatrix = JMatrix::Identity();
		mSpriteRI->Color = Color::White();
	}
	void SpriteRenderer::Start()
	{
		BaseRenderer::Start();

	}
	void SpriteRenderer::Destory()
	{
		BaseRenderer::Destory();
	}

	void SpriteRenderer::Update()
	{
	}

	void SpriteRenderer::LateUpdate()
	{

	}

	SharedPtr<IRenderItem> SpriteRenderer::PushRenderItem()
	{
		return mSpriteRI;
	}

	void SpriteRenderer::OnChange(const ChangeData& data)
	{
		BaseRenderer::OnChange(data);

		if (data.Type == JGTYPE(Transform))
		{
			auto transform = static_cast<Transform*>(data.Object);
			mSpriteRI->WorldMatrix = transform->GetWorldMatrix();


		}

	}
	void SpriteRenderer::OnInspectorGUI()
	{
		BaseRenderer::OnInspectorGUI();
		
		ImGui::ColorEdit4("Color", (float*)(&mSpriteRI->Color));

	}
}
