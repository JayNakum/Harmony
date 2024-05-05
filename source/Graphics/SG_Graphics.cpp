#include "SG_Graphics.h"
#include "Helpers/HLog.h"

#include "sokol_log.h"

namespace Sokol
{
	Buffer::Buffer(const harmony::Buffer::Description& description)
		: mDescription(description) {}

	Buffer::~Buffer()
	{
		delete mDescription.data;
		mDescription = { 0 };
		sg_destroy_buffer(mBuffer);
	}
	
	void Buffer::initialize()
	{
		sg_buffer_desc desc = { 0 };
		const float vertices[] = {
			// positions            colors
			 0.0f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
		};

		desc.data = SG_RANGE(vertices);
		mBuffer = sg_make_buffer(desc);
	}

	void Buffer::deinitialize() {}

	const void* Buffer::getID() const
	{
		return &mBuffer;
	}

	const harmony::Buffer::Description& Buffer::getDescription() const
	{
		return mDescription;
	}
}

namespace Sokol
{
	Drawable::Drawable()
		: vertexBufferCount(0) 
	{
		mBind = { 0 };
	}

	const harmony::Drawable::Description& Drawable::getDescription() const
	{
		return mDescription;
	}

	void Drawable::setDescription(const harmony::Drawable::Description& description)
	{
		mDescription = description;
	}

	void Drawable::addBuffer(harmony::Buffer*& buffer)
	{
		switch (buffer->getDescription().type)
		{
		default:
			HLOG_WARNING("Tried to bind an invalid buffer type. Assuming Type::eVERTEX_BUFFER.");

		case harmony::Buffer::Type::eVERTEX_BUFFER:
			HASSERT(vertexBufferCount >= SG_MAX_VERTEX_BUFFERS, "Trying to add too many buffers. Max limit is 8.");
			mBind.vertex_buffers[vertexBufferCount++] = *((sg_buffer*)(buffer->getID()));
			break;

		case harmony::Buffer::Type::eINDEX_BUFFER:
			mBind.index_buffer = *((sg_buffer*)(buffer->getID()));
			break;
		}
	}

	void Drawable::initialize()
	{
		HASSERT(vertexBufferCount == 0, "Trying to initialize a drawable with no vertex buffers.");
	}

	void Drawable::bind() const
	{
		sg_apply_bindings(mBind);
	}

	void Drawable::draw()
	{
		sg_draw(mDescription.baseElement, mDescription.numElements, mDescription.numInstances);
	}

	void Drawable::unbind() const
	{
		sg_bindings unbind = { 0 };
		sg_apply_bindings(unbind);
	}
	
	void Drawable::deinitialize()
	{
		unbind();
	}
}

namespace Sokol
{
	Shader::Shader(const harmony::Shader::Description& description)
		: mDescription(description) {}

	Shader::~Shader()
	{
		sg_destroy_shader(mShaderProgram);
		mDescription = { 0 };
	}


	inline const void* Sokol::Shader::getID() const
	{
		return &mShaderProgram;
	}
	
	const harmony::Shader::Description& Shader::getDescription() const
	{
		return mDescription;
	}

	void Shader::initialize()
	{
		sg_shader_desc desc = { 0 };
		for (unsigned int i = 0; i < mDescription.attrCount; ++i)
		{
		}
		desc.attrs[0].sem_name = "POS";
		desc.attrs[1].sem_name = "COLOR";

		desc.vs.source = mDescription.vs.source;
		desc.fs.source = mDescription.fs.source;

		mShaderProgram = sg_make_shader(desc);
	}
	void Shader::deinitialize()
	{
	}
}

namespace Sokol
{
	Pipeline::Pipeline(const harmony::Pipeline::Description& description)
		: mDescription(description) {}

	Pipeline::~Pipeline()
	{
		sg_destroy_pipeline(mPipeline);
		// mDescription = { 0 };
	}

	const void* Pipeline::getID() const
	{
		return &mPipeline;
	}

	const harmony::Pipeline::Description& Pipeline::getDescription() const
	{
		return mDescription;
	}

	void Pipeline::initialize()
	{
		sg_pipeline_desc desc = { 0 };
		
			desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
			desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT4;

		desc.shader = *((sg_shader*)(mDescription.shader->getID()));

		mPipeline = sg_make_pipeline(desc);
	}

	void Pipeline::bind() const
	{
		sg_apply_pipeline(mPipeline);
	}

	void Pipeline::unbind() const
	{
		sg_pipeline_desc unbind = { 0 };
		sg_apply_pipeline(sg_make_pipeline(unbind));
	}

	void Pipeline::deinitialize()
	{
		unbind();
	}
}

namespace Sokol
{
	RenderPass::RenderPass(const harmony::RenderPass::Description& description)
		: mDescription(description) {}

	RenderPass::~RenderPass()
	{
		mDescription = { 0 };
	}

	const void* RenderPass::getID() const
	{
		return &mRenderPass;
	}

	const harmony::RenderPass::Description& RenderPass::getDescription() const
	{
		return mDescription;
	}

	void RenderPass::initialize()
	{
		mRenderPass = { 0 };

		sg_pass_action action;
		action.colors[0].clear_value = { 
			mDescription.action.colors[0].clearValue[0],
			mDescription.action.colors[0].clearValue[1],
			mDescription.action.colors[0].clearValue[2],
			mDescription.action.colors[0].clearValue[3]
		};
		action.colors[0].load_action = mLOAD_ACTION.at(mDescription.action.colors[0].loadAction);
		// action.colors[0].store_action = mSTORE_ACTION.at(mDescription.action.colors[0].storeAction);

		mRenderPass.action = action;
		mRenderPass.label = mDescription.label;
	}
	
	void RenderPass::begin()
	{
		mRenderPass.swapchain = d3d11_swapchain();
		sg_begin_pass(mRenderPass);
	}
	void RenderPass::end() const
	{
		sg_end_pass();
		sg_commit();
		d3d11_present();
	}
	void RenderPass::deinitialize()
	{
		
	}
}

namespace Sokol
{
	void Renderer::initialize()
	{
		d3d11_desc_t d3dDesc;
		d3dDesc.width = 640;
		d3dDesc.height = 480;
		d3dDesc.no_depth_buffer = true;
		d3dDesc.title = L"triangle-d3d11.c";
		d3dDesc.sample_count = 4;
		d3d11_init(&d3dDesc);

		sg_desc desc = { 0 };
		desc.environment = d3d11_environment();
		desc.logger.func = slog_func;
		sg_setup(desc);
	}
	void Renderer::render()
	{
		while (d3d11_process_events())
		{
			frame();
		}
	}
	void Renderer::deinitialize()
	{
		sg_shutdown();
		d3d11_shutdown();
	}
}