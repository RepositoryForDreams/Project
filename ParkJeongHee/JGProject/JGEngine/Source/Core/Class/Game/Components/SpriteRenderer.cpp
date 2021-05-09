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

		struct SpriteVertex
		{
			JVector3 Position;
			JVector2 Texcoord;
		};

		SpriteVertex spriteVertex[4];
		u32 spriteIndex[6];
		//spriteVertex[0].Position = JVector3(-0.5f, -0.5f, 0.0f);
		//spriteVertex[1].Position = JVector3(-0.5f, +0.5f, 0.0f);
		//spriteVertex[2].Position = JVector3(+0.5f, +0.5f, 0.0f);
		//spriteVertex[3].Position = JVector3(+0.5f, -0.5f, 0.0f);

		spriteVertex[0].Position = JVector3(-50.0f, -50.0f, 0.0f);
		spriteVertex[1].Position = JVector3(-50.0f, +50.0f, 0.0f);
		spriteVertex[2].Position = JVector3(+50.0f, +50.0f, 0.0f);
		spriteVertex[3].Position = JVector3(+50.0f, -50.0f, 0.0f);

		spriteVertex[0].Texcoord = JVector2(0.0f, 1.0f);
		spriteVertex[1].Texcoord = JVector2(0.0f, 0.0f);
		spriteVertex[2].Texcoord = JVector2(1.0f, 0.0f);
		spriteVertex[3].Texcoord = JVector2(1.0f, 1.0f);
		
		spriteIndex[0] = 0;
		spriteIndex[1] = 1;
		spriteIndex[2] = 2;
		spriteIndex[3] = 0;
		spriteIndex[4] = 2;
		spriteIndex[5] = 3;
		vBuffer->SetData(spriteVertex, sizeof(SpriteVertex), 4);
		iBuffer->SetData(spriteIndex, 6);
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
		}

	}
	void SpriteRenderer::OnInspectorGUI()
	{
		BaseRenderer::OnInspectorGUI();
		
		ImGui::ColorEdit4("Color", (float*)(&mSpriteRI->Color));

	}
}
