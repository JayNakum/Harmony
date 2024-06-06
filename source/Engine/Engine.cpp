#include <iostream>

#include "Harmony.h"

#include <sokol_app.h>

#define GLM_FORCE_SSE2
#define GLM_FORCE_INLINE
#define GLM_FORCE_ALIGNED
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Engine
{
public:
	void init()
	{
		// Window init
		{
			harmony::Window::Description windowDescription;
			windowDescription.width = 1024;
			windowDescription.height = 576;
			windowDescription.isFullScreen = false;
			harmony::createWindow(windowDescription);
			window = harmony::getWindow();
			window->initialize();
		}
		
		// Renderer init
		{
			renderer = harmony::getRenderer();
			renderer->initialize();
		}

		// Scene creation
		{
			harmony::Buffer::Description vBufferDescription;
			{
				const float vertices[] = {
					// positions        colors
					-1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
					 1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
					 1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
					-1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,

					-1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
					 1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
					 1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
					-1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,

					-1.0, -1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
					-1.0,  1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
					-1.0,  1.0,  1.0,   0.0, 0.0, 1.0, 1.0,
					-1.0, -1.0,  1.0,   0.0, 0.0, 1.0, 1.0,

					1.0, -1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
					1.0,  1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
					1.0,  1.0,  1.0,   1.0, 0.5, 0.0, 1.0,
					1.0, -1.0,  1.0,   1.0, 0.5, 0.0, 1.0,

					-1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,
					-1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
					 1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
					 1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,

					-1.0,  1.0, -1.0,   1.0, 0.0, 0.5, 1.0,
					-1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
					 1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
					 1.0, 1.0, -1.0, 1.0, 0.0, 0.5, 1.0
				};
				vBufferDescription.label = "Test Vertex Buffer";
				vBufferDescription.type = harmony::Buffer::Type::eVERTEX_BUFFER;
				vBufferDescription.data = vertices;
				vBufferDescription.size = sizeof(vertices);
			}
			auto vertexBuffer = harmony::createBuffer(vBufferDescription);
			vertexBuffer->initialize();

			harmony::Buffer::Description iBufferDescription;
			{
				const uint16_t indices[] = {
					0, 1, 2,  0, 2, 3,
					6, 5, 4,  7, 6, 4,
					8, 9, 10,  8, 10, 11,
					14, 13, 12,  15, 14, 12,
					16, 17, 18,  16, 18, 19,
					22, 21, 20, 23, 22, 20
				};
				iBufferDescription.label = "Test Index Buffer";
				iBufferDescription.type = harmony::Buffer::Type::eINDEX_BUFFER;

				iBufferDescription.data = indices;
				iBufferDescription.size = sizeof(indices);
			}
			auto indexBuffer = harmony::createBuffer(iBufferDescription);
			indexBuffer->initialize();

			harmony::Drawable::Description drawableDescription;
			{
				drawableDescription.label = "Test Drawable";
				drawableDescription.vertexBuffer = vertexBuffer;
				drawableDescription.indexBuffer = indexBuffer;
			}
			drawable = harmony::createDrawable(drawableDescription);

			harmony::Shader::Description shaderDescription;
			{
				shaderDescription.label = "Test Shader Program";
				shaderDescription.attributesCount = 2;
				shaderDescription.attrs[0].name = "POSITION";
				shaderDescription.attrs[1].name = "COLOR";
				shaderDescription.attrs[1].index = 1;

				shaderDescription.vs.constantBlocksCount = 1;
				shaderDescription.vs.constantBlocks[0].size = sizeof(glm::mat4);
				shaderDescription.vs.source =
					"cbuffer params: register(b0) {\n"
					"  float4x4 mvp;\n"
					"};\n"
					"struct vs_in {\n"
					"  float4 pos: POSITION;\n"
					"  float4 color: COLOR1;\n"
					"};\n"
					"struct vs_out {\n"
					"  float4 color: COLOR0;\n"
					"  float4 pos: SV_Position;\n"
					"};\n"
					"vs_out main(vs_in inp) {\n"
					"  vs_out outp;\n"
					"  outp.pos = mul(mvp, inp.pos);\n"
					"  outp.color = inp.color;\n"
					"  return outp;\n"
					"};\n";

				shaderDescription.fs.source =
					"float4 main(float4 color: COLOR0): SV_Target0 {\n"
					"  return color;\n"
					"}\n";
			}

			auto shaderProgram1 = harmony::createShader(shaderDescription);
			shaderProgram1->initialize();

			harmony::Pipeline::Description pipelineDescription;
			{
				pipelineDescription.label = "Test Pipeline";
				pipelineDescription.layout.attributesCount = 2;
				pipelineDescription.layout.attributes[0].format = harmony::Pipeline::VertexFormat::eFLOAT3;
				pipelineDescription.layout.attributes[1].format = harmony::Pipeline::VertexFormat::eFLOAT4;
				pipelineDescription.shaderProgram = shaderProgram1;
				pipelineDescription.indexType = harmony::Pipeline::IndexType::eUINT16;
				pipelineDescription.cullMode = harmony::Pipeline::CullMode::eBACK;
				//pipelineDescription.depthBufferEnabled = true;
			}
			pipeline = harmony::createPipeline(pipelineDescription);

			harmony::RenderPass::Description passDescription;
			{
				passDescription.label = "Test Render Pass";
				passDescription.action.colors.value[0] = 1.0f;
				passDescription.action.colors.value[1] = 1.0f;
				passDescription.action.colors.value[2] = 1.0f;
				passDescription.action.colors.value[3] = 1.0f;
			}
			pass = harmony::createRenderPass(passDescription);
		}

		// Init sequence
		{
			drawable->initialize();
			pipeline->initialize();
			pass->initialize();
		}
	}

	void frame()
	{
		auto& windowDescription = window->getDescription();
		glm::mat4 proj = glm::perspective(90.0f, (float)windowDescription.width / (float)windowDescription.height, 0.01f, 10.0f);
		glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 model(1);
		
		rx -= 1.0f;
		model *= glm::scale(model, glm::vec3(1.3f));
		model *= glm::rotate(model, glm::radians(rx), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 mvp = proj * view * model;

		pass->begin();
		pipeline->bind();
		drawable->bind();
		pipeline->applyConstants(0, &mvp, sizeof(mvp));
		drawable->draw(0, 36, 1);
		pass->end();
	}

	void cleanup()
	{
		drawable->deinitialize();
		pipeline->deinitialize();
		pass->deinitialize();

		delete drawable;
		delete pipeline;
		delete pass;

		renderer->deinitialize();
		window->deinitialize();
		delete renderer;
		delete window;
	}

private:
	float rx = 0.0f;
	
	harmony::Window* window			= nullptr;
	harmony::Renderer* renderer		= nullptr;
	harmony::Drawable* drawable		= nullptr;
	harmony::Pipeline* pipeline		= nullptr;
	harmony::RenderPass* pass		= nullptr;
}engine;

// this is some extra code required because of sokol GFX
namespace sokol
{
	static void init()
	{
		engine.init();
	}

	static void frame()
	{
		harmony::getWindow()->update();
		engine.frame();
	}

	static void cleanup()
	{
		engine.cleanup();
	}

	static void event(const sapp_event* e)
	{
		if (e->key_code == sapp_keycode::SAPP_KEYCODE_ESCAPE)
		{
			sapp_request_quit();
		}
		harmony::getWindow()->resize(e->window_width, e->window_height);
	}
}

sapp_desc sokol_main(int argc, char* argv[])
{
	sapp_desc appDesc = { 0 };
	{
		const auto& windowDesc = harmony::getWindow()->getDescription();
		appDesc.window_title = windowDesc.title;
		appDesc.width = windowDesc.width;
		appDesc.height = windowDesc.height;
		appDesc.fullscreen = windowDesc.isFullScreen;
		appDesc.high_dpi = true;
		appDesc.init_cb = sokol::init;
		appDesc.frame_cb = sokol::frame;
		appDesc.cleanup_cb = sokol::cleanup;
		appDesc.event_cb = sokol::event;
	}
	return appDesc;
}
