#include <iostream>

#include "Harmony.h"

#include <sokol_app.h>

class Engine
{
public:
	void init()
	{
		auto renderer = harmony::getRenderer();
		renderer->initialize();

		harmony::Buffer::Description vBufferDescription;
		{
			const float vertices[] = {
				-0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
				 0.5f,  0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
				 0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f,
				-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 0.0f, 1.0f
			};
			vBufferDescription.label = "Quad Vertex Buffer";
			vBufferDescription.type = harmony::Buffer::Type::eVERTEX_BUFFER;
			vBufferDescription.data = vertices;
			vBufferDescription.size = sizeof(vertices);
		}
		auto vertexBuffer = harmony::createBuffer(vBufferDescription);
		vertexBuffer->initialize(); // this gets passed to drawable which means that drawable has it's ownership and drawable will ensure that this gets deinitialized and deleted

		harmony::Buffer::Description iBufferDescription;
		{
			const uint16_t indices[] = {
				0, 1, 2,
				0, 2, 3
			};
			iBufferDescription.label = "Quad Index Buffer";
			iBufferDescription.type = harmony::Buffer::Type::eINDEX_BUFFER;
			
			iBufferDescription.data = indices;
			iBufferDescription.size = sizeof(indices);
		}
		auto indexBuffer = harmony::createBuffer(iBufferDescription);
		indexBuffer->initialize(); // this gets passed to drawable which means that drawable has it's ownership and drawable will ensure that this gets deinitialized and deleted

		harmony::Drawable::Description drawableDescription;
		{
			drawableDescription.label = "Quad Drawable";
			drawableDescription.vertexBuffer = vertexBuffer;
			drawableDescription.indexBuffer = indexBuffer;
		}
		triangle = harmony::createDrawable(drawableDescription);

		harmony::Shader::Description shaderDescription;
		{
			shaderDescription.label = "Quad Shader Program";
			shaderDescription.attributesCount = 2;
			shaderDescription.attrs[0].name = "POS";
			shaderDescription.attrs[1].name = "COLOR";
			shaderDescription.vs.source =
				"struct vs_in {\n"
				"  float4 pos: POS;\n"
				"  float4 color: COLOR;\n"
				"};\n"
				"struct vs_out {\n"
				"  float4 color: COLOR0;\n"
				"  float4 pos: SV_Position;\n"
				"};\n"
				"vs_out main(vs_in inp) {\n"
				"  vs_out outp;\n"
				"  outp.pos = inp.pos;\n"
				"  outp.color = inp.color;\n"
				"  return outp;\n"
				"}\n";
			shaderDescription.fs.source =
				"float4 main(float4 color: COLOR0): SV_Target0 {\n"
				"  return color;\n"
				"}\n";
		}
		auto shaderProgram = harmony::createShader(shaderDescription);
		shaderProgram->initialize(); // this gets passed to pipeline which means that pipeline has it's ownership and pipeline will ensure that this gets deinitialized and deleted

		harmony::Pipeline::Description pipelineDescription;
		{
			pipelineDescription.label = "Quad Pipeline";
			pipelineDescription.layout.attributesCount = 2;
			pipelineDescription.layout.attributes[0].format = harmony::Pipeline::VertexFormat::eFLOAT3;
			pipelineDescription.layout.attributes[1].format = harmony::Pipeline::VertexFormat::eFLOAT4;
			pipelineDescription.shaderProgram = shaderProgram;
			pipelineDescription.indexType = harmony::Pipeline::IndexType::eUINT16;
		}
		pipeline = harmony::createPipeline(pipelineDescription);

		harmony::RenderPass::Description passDescription;
		{
			passDescription.label = "Quad Render Pass";
			passDescription.action.colors.value[0] = 1.0f;
			passDescription.action.colors.value[1] = 1.0f;
			passDescription.action.colors.value[2] = 1.0f;
			passDescription.action.colors.value[3] = 1.0f;
		}
		pass = harmony::createRenderPass(passDescription);

		{// Init sequence
			triangle->initialize();
			pipeline->initialize();
			pass->initialize();
		}
	}

	void frame()
	{
		pass->begin();
		pipeline->bind();
		triangle->bind();
		triangle->draw(0, 6, 1);
		pass->end();
	}

	void cleanup()
	{
		triangle->deinitialize();
		pipeline->deinitialize();
		pass->deinitialize();

		delete triangle;
		delete pipeline;
		delete pass;

		delete harmony::getRenderer();
		delete harmony::getWindow();
	}

private:
	harmony::Drawable* triangle = nullptr;
	harmony::Pipeline* pipeline = nullptr;
	harmony::RenderPass* pass = nullptr;
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
		engine.frame();
	}

	static void cleanup()
	{
		engine.cleanup();
	}
}

sapp_desc sokol_main(int argc, char* argv[])
{
	{// Window init
		harmony::Window::Description windowDescription;
		windowDescription.width = 1024;
		windowDescription.height = 576;
		windowDescription.isFullScreen = false;
		harmony::createWindow(windowDescription);
		auto window = harmony::getWindow();
		window->initialize();
	}

	sapp_desc appDesc = { 0 };
	{
		const auto& windowDesc = harmony::getWindow()->getDescription();
		appDesc.window_title = windowDesc.title;
		appDesc.width = windowDesc.width;
		appDesc.height = windowDesc.height;
		appDesc.fullscreen = windowDesc.isFullScreen;

		appDesc.init_cb = sokol::init;
		appDesc.frame_cb = sokol::frame;
		appDesc.cleanup_cb = sokol::cleanup;
	}
	return appDesc;
}
