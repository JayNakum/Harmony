#include "Renderer.h"

#define GLM_FORCE_SSE2
#define GLM_FORCE_INLINE
#define GLM_FORCE_ALIGNED
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace harmony
{
	void Renderer::initialize()
	{
		auto& windowDesc = HARMONY->getWindow()->getDescription();

		harmony::Swapchain::Description createDesc;
		createDesc.label = "dx12 swapchain";
		createDesc.width = windowDesc.width;
		createDesc.height = windowDesc.height;
		mSwapchain = HARMONY->createSwapchain(createDesc);

		mSwapchain->initialize();

		harmony::Shader::Description vShaderDescription;
		{
			vShaderDescription.label = "Test Shader Program";
			vShaderDescription.type = harmony::Shader::Type::eVERTEX;
			vShaderDescription.constantBlocksCount = 1;
			vShaderDescription.constantBlocks[0].size = sizeof(glm::mat4);
			vShaderDescription.source = R"(
struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer MVP : register(b0)
{
    float4x4 mvp;
};

PSInput main(VSInput input)
{
    PSInput result;

    result.position = mul(input.position, mvp);
    result.color = input.color;

    return result;
}
			)";
		}
		auto vShader = HARMONY->createShader(vShaderDescription);
		vShader->initialize();

		harmony::Shader::Description fShaderDescription;
		{
			fShaderDescription.texturesCount = 0;
			fShaderDescription.textures[0].imageSlot = 0;
			fShaderDescription.textures[0].samplerSlot = 0;
			fShaderDescription.textures[0].used = false;

			fShaderDescription.type = harmony::Shader::Type::eFRAGMENT;

			fShaderDescription.source = R"(
struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
    return input.color;
}
			)";
		}
		auto fShader = HARMONY->createShader(fShaderDescription);
		fShader->initialize();

		harmony::Pipeline::Description pipelineDescription;
		{
			pipelineDescription.label = "Test Pipeline";
			pipelineDescription.layout.attributesCount = 2;

			pipelineDescription.layout.attributes[0].name = "POSITION";
			pipelineDescription.layout.attributes[0].index = 0;
			pipelineDescription.layout.attributes[0].format = harmony::Pipeline::VertexFormat::eFLOAT3;

			pipelineDescription.layout.attributes[1].name = "COLOR";
			pipelineDescription.layout.attributes[1].index = 0;
			pipelineDescription.layout.attributes[1].format = harmony::Pipeline::VertexFormat::eFLOAT4;

			pipelineDescription.vertexShader = vShader;
			pipelineDescription.fragmentShader = fShader;

			pipelineDescription.indexType = harmony::Pipeline::IndexType::eUINT16;
			pipelineDescription.cullMode = harmony::Pipeline::CullMode::eNONE;

			pipelineDescription.depthBufferEnabled = true;
		}
		pipeline = HARMONY->createPipeline(pipelineDescription);
		pipeline->initialize();
	}


	void Renderer::render(const harmony::Scene* gameScene)
	{
		HARMONY->getRenderAPI()->frameBegin();
		{
			mSwapchain->prepare();

			harmony::RenderTarget::Action action;
			action.colors.value[0] = 0.1f;
			action.colors.value[1] = 0.1f;
			action.colors.value[2] = 0.1f;
			action.colors.value[3] = 1.0f;
			mSwapchain->getCurrentRenderTarget()->bind(action);


			auto& windowDescription = HARMONY->getWindow()->getDescription();
			glm::mat4 proj = glm::perspective(90.0f, (float)windowDescription.width / (float)windowDescription.height, 0.01f, 10.0f);
			glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			glm::mat4 vp = proj * view;

			for (const auto& entity : gameScene->getVisualEntities())
			{
				glm::mat4 model(1.0f);
				/*auto transform = entity->getDescription().transform;
				if (transform.has_value())
				{
					model *= glm::scale(model, glm::vec3(transform->scale[0], transform->scale[1], transform->scale[2]));
					model *= glm::rotate(model, glm::radians(transform->rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
					model *= glm::rotate(model, glm::radians(transform->rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
					model *= glm::rotate(model, glm::radians(transform->rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
					model *= glm::translate(model, glm::vec3(transform->position[0], transform->position[1], transform->position[2]));
				}*/

				glm::mat4 mvp = vp * model;
				auto drawable = entity->getDrawable();

				pipeline->bind();
				pipeline->applyConstants(0, &mvp, 16 * sizeof(float));
				pipeline->unbind();

				drawable->bind();
				drawable->draw();
				drawable->unbind();
			}


			mSwapchain->getCurrentRenderTarget()->unbind();
		}
		HARMONY->getRenderAPI()->frameEnd();

		mSwapchain->present();

		HARMONY->getRenderAPI()->present();
	}

	void Renderer::deinitialize()
	{
		pipeline->deinitialize();
		mSwapchain->deinitialize();
	}

	Renderer::~Renderer()
	{
		//delete pipeline;
		delete mSwapchain;
	}
}
