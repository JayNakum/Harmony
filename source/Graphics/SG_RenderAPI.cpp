#include "SG_RenderAPI.h"

#include "Helpers/HLog.h"

#include <sokol_glue.h>

namespace sokol
{
	RenderAPI::RenderAPI(const harmony::RenderAPI::Description& description)
		: mDescription(description) {}

	RenderAPI::~RenderAPI()
	{
		sg_shutdown();
	}

	const harmony::RenderAPI::Description& RenderAPI::getDescription() const
	{
		return mDescription;
	}

	void RenderAPI::initialize()
	{
		sg_desc createDesc = { 0 };
		createDesc.environment = sglue_environment();
		// createDesc.logger.func = slog_func;
		sg_setup(createDesc);
	}

	void RenderAPI::frameBegin()
	{
	}


	void RenderAPI::frameEnd()
	{
	}

	void RenderAPI::deinitialize()
	{
	}
}

/*
* In sokol, there is no support for separate render targets and multi-buffer rendering
* Hence here, creating a render target just means creating a pass action that defined the clear values
* And creating a swapchain means creating a sg_pass.
* In sokol, there is no good reason to handle swapchains manually so this seems like the place to keep this.
* 
* This is here because it seems like a logical hack 
* without having to implement a proper system where render targets means an sg_image and writing to it instead and blit it to a quad.
* And there is no good reason to implement that for now.
* 
* So for now we will create a common swapchain (that is sg_renderpass) and a singular render target (which is the action that will happen on that render pass).
* This is done to have a sort of unified interface with other low level APIs.
*/

namespace sokol
{
	void RenderTarget::initialize()
	{}

	const void* RenderTarget::getID() const
	{
		return &mAction;
	}

	void RenderTarget::bind(const Action& action)
	{
		mAction.colors->load_action = SG_LOADACTION_CLEAR;
		mAction.colors[0].clear_value = {
			action.colors.value[0],
			action.colors.value[1],
			action.colors.value[2],
			action.colors.value[3]
		};

	}

	void RenderTarget::deinitialize()
	{}
}

namespace sokol
{
	Swapchain::Swapchain(const harmony::Swapchain::Description& description)
		: mDescription(description), mPass{0}, mRenderTarget(new RenderTarget())
	{}

	Swapchain::~Swapchain()
	{}

	const void* Swapchain::getID() const
	{
		return nullptr;
	}

	const harmony::Swapchain::Description& Swapchain::getDescription() const
	{
		return mDescription;
	}

	harmony::RenderTarget* Swapchain::getCurrentRenderTarget() const
	{
		return mRenderTarget;
	}
	
	void Swapchain::initialize()
	{
	}

	void Swapchain::prepare()
	{
		mPass.swapchain = sglue_swapchain();
		sg_begin_pass(mPass);
	}

	void Swapchain::resize(unsigned int width, unsigned int height)
	{}

	void Swapchain::present()
	{
		sg_end_pass();
		sg_commit();
	}

	void Swapchain::deinitialize()
	{}
}

namespace sokol
{
	Buffer::Buffer(const harmony::Buffer::Description& description)
		: mDescription(description), mBuffer{ 0 } {}

	Buffer::~Buffer()
	{
		mDescription = { 0 };
		sg_destroy_buffer(mBuffer);
	}
	
	const void* Buffer::getID() const
	{
		return &mBuffer;
	}
	
	const harmony::Buffer::Description& Buffer::getDescription() const
	{
		return mDescription;
	}
	
	void Buffer::initialize()
	{
		sg_buffer_desc createDesc = { 0 };
		createDesc.label = mDescription.label;
		createDesc.data.ptr = mDescription.data;
		createDesc.size = mDescription.size;
		createDesc.type = Buffer::sTYPE.at(mDescription.type);
		createDesc.usage = Buffer::sUSAGE.at(mDescription.usage);

		mBuffer = sg_make_buffer(createDesc);

	}

	void Buffer::deinitialize()
	{
		sg_dealloc_buffer(mBuffer);
	}
}

namespace sokol
{
	Image::Image(const harmony::Image::Description& description)
		: mDescription(description), mImage{ 0 }
	{}

	const void* Image::getID() const
	{
		return &mImage;
	}

	const harmony::Image::Description& Image::getDescription() const
	{
		return mDescription;
	}

	void Image::initialize()
	{
		sg_image_desc createDesc = { 0 };
		createDesc.label = mDescription.imagePath;
		createDesc.width = mDescription.width;
		createDesc.height = mDescription.height;
		createDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
		createDesc.data.subimage[0][0].ptr = mDescription.memory;
		createDesc.data.subimage[0][0].size = mDescription.size;
		mImage = sg_make_image(createDesc);
	}

	void Image::deinitialize()
	{
		sg_dealloc_image(mImage);
	}

	Image::~Image() 
	{
		sg_destroy_image(mImage);
		mDescription = { 0 };
	}

	Sampler::Sampler(const harmony::Sampler::Description& description)
		: mDescription(description), mSampler{ 0 } {}

	const void* Sampler::getID() const
	{
		return &mSampler;
	}

	const harmony::Sampler::Description& Sampler::getDescription() const
	{
		return mDescription;
	}

	void Sampler::initialize()
	{
		sg_sampler_desc createDesc = { 0 };
		createDesc.label = mDescription.label;
		createDesc.min_filter = sFILTER.at(mDescription.minFilter);
		createDesc.mag_filter = sFILTER.at(mDescription.magFilter);

		mSampler = sg_make_sampler(createDesc);
	}

	void Sampler::deinitialize()
	{
		sg_dealloc_sampler(mSampler);
	}

	Sampler::~Sampler()
	{
		sg_destroy_sampler(mSampler);
		mDescription = { 0 };
	}
}

namespace sokol
{
	sokol::Drawable::Drawable(const harmony::Drawable::Description& description)
		: mDescription(description), mBind{ 0 } {}

	const harmony::Drawable::Description& Drawable::getDescription() const
	{
		return mDescription;
	}

	void Drawable::initialize()
	{
		mDescription.vertexBuffer->initialize();
		mBind.vertex_buffers[0] = *((sg_buffer*)mDescription.vertexBuffer->getID());
		
		if (mDescription.indexBuffer)
		{
			mDescription.indexBuffer->initialize();
			mBind.index_buffer = *((sg_buffer*)mDescription.indexBuffer->getID());
		}

		{// Vertex Shader Bindings
			if (mDescription.vs.imgCount)
			{
				for (unsigned int i = 0; i < mDescription.vs.imgCount; ++i)
				{
					mDescription.vs.images[i]->initialize();
					mBind.vs.images[i] = *((sg_image*)mDescription.vs.images[i]->getID());
				}
			}

			if (mDescription.vs.smpCount)
			{
				for (unsigned int i = 0; i < mDescription.vs.smpCount; ++i)
				{
					mDescription.vs.samplers[i]->initialize();
					mBind.vs.samplers[i] = *((sg_sampler*)mDescription.vs.samplers[i]->getID());
				}
			}
		}

		{// Fragment Shader Bindings
			if (mDescription.fs.imgCount)
			{
				for (unsigned int i = 0; i < mDescription.fs.imgCount; ++i)
				{
					mDescription.fs.images[i]->initialize();
					mBind.fs.images[i] = *((sg_image*)mDescription.fs.images[i]->getID());
				}
			}

			if (mDescription.fs.smpCount)
			{
				for (unsigned int i = 0; i < mDescription.fs.smpCount; ++i)
				{
					mDescription.fs.samplers[i]->initialize();
					mBind.fs.samplers[i] = *((sg_sampler*)mDescription.fs.samplers[i]->getID());
				}
			}
		}
	}
	
	void Drawable::bind(unsigned int slot) const
	{
		sg_apply_bindings(&mBind);
	}
	
	void Drawable::draw() const
	{
		sg_draw(mDescription.drawDescription.baseElement, mDescription.drawDescription.numElements, mDescription.drawDescription.numInstances);
	}

	void Drawable::unbind() const
	{
		/*sg_bindings unbinds = { 0 };
		sg_apply_bindings(&unbinds);*/
	}

	void Drawable::deinitialize()
	{
		unbind();

		for (unsigned int i = 0; i < mDescription.vs.imgCount; ++i)
		{
			mDescription.vs.images[i]->deinitialize();
		}

		for (unsigned int i = 0; i < mDescription.vs.smpCount; ++i)
		{
			mDescription.vs.samplers[i]->deinitialize();
		}

		for (unsigned int i = 0; i < mDescription.fs.imgCount; ++i)
		{
			mDescription.fs.images[i]->deinitialize();
		}

		for (unsigned int i = 0; i < mDescription.fs.smpCount; ++i)
		{
			mDescription.fs.samplers[i]->deinitialize();
		}

		mDescription.vertexBuffer->deinitialize();
		mDescription.indexBuffer->deinitialize();
	}

	Drawable::~Drawable()
	{
		mBind = { 0 };

		delete mDescription.vertexBuffer;
		delete mDescription.indexBuffer;

		for (unsigned int i = 0; i < mDescription.vs.imgCount; ++i)
		{
			delete mDescription.vs.images[i];
		}

		for (unsigned int i = 0; i < mDescription.vs.smpCount; ++i)
		{
			delete mDescription.vs.samplers[i];
		}

		for (unsigned int i = 0; i < mDescription.fs.imgCount; ++i)
		{
			delete mDescription.fs.images[i];
		}

		for (unsigned int i = 0; i < mDescription.fs.smpCount; ++i)
		{
			delete mDescription.fs.samplers[i];
		}
	}
}

namespace sokol
{
	sokol::Shader::Shader(const harmony::Shader::Description& description)
		: mDescription(description) {}

	const void* sokol::Shader::getID() const
	{
		return nullptr;
	}

	const harmony::Shader::Description& Shader::getDescription() const
	{
		return mDescription;
	}

	void Shader::initialize() {}

	void Shader::deinitialize() {}

	Shader::~Shader() {}
}

namespace sokol
{
	Pipeline::Pipeline(const harmony::Pipeline::Description& description)
		: mDescription(description), mPipeline{ 0 }, mShaderProgram{ 0 } {}

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
		sg_pipeline_desc createDesc = { 0 };
		createDesc.label = mDescription.label;
		
		for (unsigned int i = 0; i < mDescription.layout.attributesCount; ++i)
		{
			createDesc.layout.attrs[i].format = sVERTEX_FORMAT.at(mDescription.layout.attributes[i].format);	
		}

		createDesc.index_type = sINDEX_TYPE.at(mDescription.indexType);

		// sokol shader program init
		{
			sg_shader_desc createDesc = { 0 };

			createDesc.label = mDescription.label;

			for (unsigned int i = 0; i < mDescription.layout.attributesCount; ++i)
			{
				createDesc.attrs[i].sem_name = mDescription.layout.attributes[i].name;
				if (mDescription.layout.attributes[i].index >= 0)
				{
					createDesc.attrs[i].sem_index = mDescription.layout.attributes[i].index;
				}
			}

			{
				const auto& vs = mDescription.vertexShader->getDescription();
				createDesc.vs.source = vs.source;
				for (unsigned int i = 0; i < vs.constantBlocksCount; ++i)
				{
					createDesc.vs.uniform_blocks[i].size = vs.constantBlocks[i].size;
					for (unsigned int j = 0; j < vs.constantBlocks[i].constantCount; ++j)
					{
						createDesc.vs.uniform_blocks[i].uniforms[j].name = vs.constantBlocks[i].constants[j].name;
						createDesc.vs.uniform_blocks[i].uniforms[j].type = sCONSTANT_TYPE.at(vs.constantBlocks[i].constants[j].type);
					}
				}
				for (unsigned int i = 0; i < vs.texturesCount; ++i)
				{
					createDesc.vs.images[i].used = vs.textures[i].used;
					createDesc.vs.samplers[i].used = vs.textures[i].used;
					createDesc.vs.image_sampler_pairs[i].used = vs.textures[i].used;
					createDesc.vs.image_sampler_pairs[i].image_slot = vs.textures[i].imageSlot;
					createDesc.vs.image_sampler_pairs[i].sampler_slot = vs.textures[i].samplerSlot;
				}
			}

			{
				const auto& fs = mDescription.fragmentShader->getDescription();
				createDesc.fs.source = fs.source;
				for (unsigned int i = 0; i < fs.constantBlocksCount; ++i)
				{
					createDesc.fs.uniform_blocks[i].size = fs.constantBlocks[i].size;
					for (unsigned int j = 0; j < fs.constantBlocks[i].constantCount; ++j)
					{
						createDesc.fs.uniform_blocks[i].uniforms[j].name = fs.constantBlocks[i].constants[j].name;
						createDesc.fs.uniform_blocks[i].uniforms[j].type = sCONSTANT_TYPE.at(fs.constantBlocks[i].constants[j].type);
					}
				}
				for (unsigned int i = 0; i < fs.texturesCount; ++i)
				{
					createDesc.fs.images[i].used = fs.textures[i].used;
					createDesc.fs.samplers[i].used = fs.textures[i].used;
					createDesc.fs.image_sampler_pairs[i].used = fs.textures[i].used;
					createDesc.fs.image_sampler_pairs[i].image_slot = fs.textures[i].imageSlot;
					createDesc.fs.image_sampler_pairs[i].sampler_slot = fs.textures[i].samplerSlot;
				}
			}

			mShaderProgram = sg_make_shader(createDesc);
		}

		createDesc.shader = mShaderProgram;

		createDesc.primitive_type = sPRIMITIVE_TYPE.at(mDescription.primitiveType);
		createDesc.cull_mode = sCULL_MODE.at(mDescription.cullMode);
		createDesc.face_winding = sFACE_WINDING.at(mDescription.faceWinding);

		if (mDescription.depthBufferEnabled)
		{
			createDesc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
			createDesc.depth.write_enabled = true;
		}

		mPipeline = sg_make_pipeline(createDesc);
	}

	void Pipeline::bind() const
	{
		sg_apply_pipeline(mPipeline);
	}

	void Pipeline::applyConstants(unsigned int constantBlockIndex, const void* data, size_t size, const harmony::Pipeline::ShaderStageType type) const
	{
		sg_apply_uniforms(sSTAGE_TYPE.at(type), (int)constantBlockIndex, { data, size });
	}

	void Pipeline::unbind() const
	{
		sg_pipeline unbindDesc = { 0 };
		sg_apply_pipeline(unbindDesc);
	}

	void Pipeline::deinitialize()
	{
		sg_dealloc_pipeline(mPipeline);
		sg_dealloc_shader(mShaderProgram);
	}

	Pipeline::~Pipeline()
	{
		sg_destroy_pipeline(mPipeline);
		sg_destroy_shader(mShaderProgram);
		mDescription = { 0 };
	}
}

namespace sokol
{
	const std::unordered_map<harmony::Buffer::Type, sg_buffer_type> Buffer::sTYPE = {
			{ harmony::Buffer::Type::eVERTEX_BUFFER, sg_buffer_type::SG_BUFFERTYPE_VERTEXBUFFER },
			{ harmony::Buffer::Type::eINDEX_BUFFER, sg_buffer_type::SG_BUFFERTYPE_INDEXBUFFER }
	};
	const std::unordered_map<harmony::Buffer::Usage, sg_usage> Buffer::sUSAGE = {
			{ harmony::Buffer::Usage::eIMMUTABLE, sg_usage::SG_USAGE_IMMUTABLE },
			{ harmony::Buffer::Usage::eDYNAMIC, sg_usage::SG_USAGE_DYNAMIC },
			{ harmony::Buffer::Usage::eSTREAM, sg_usage::SG_USAGE_STREAM }
	};

	const std::unordered_map<harmony::Shader::ConstantType, sg_uniform_type> Pipeline::sCONSTANT_TYPE = {
		{ harmony::Shader::ConstantType::eFLOAT, sg_uniform_type::SG_UNIFORMTYPE_FLOAT },
		{ harmony::Shader::ConstantType::eFLOAT2, sg_uniform_type::SG_UNIFORMTYPE_FLOAT2 },
		{ harmony::Shader::ConstantType::eFLOAT3, sg_uniform_type::SG_UNIFORMTYPE_FLOAT3 },
		{ harmony::Shader::ConstantType::eFLOAT4, sg_uniform_type::SG_UNIFORMTYPE_FLOAT4 },
		{ harmony::Shader::ConstantType::eINT, sg_uniform_type::SG_UNIFORMTYPE_INT },
		{ harmony::Shader::ConstantType::eINT2, sg_uniform_type::SG_UNIFORMTYPE_INT2 },
		{ harmony::Shader::ConstantType::eINT3, sg_uniform_type::SG_UNIFORMTYPE_INT3 },
		{ harmony::Shader::ConstantType::eINT4, sg_uniform_type::SG_UNIFORMTYPE_INT4 },
		{ harmony::Shader::ConstantType::eMAT4, sg_uniform_type::SG_UNIFORMTYPE_MAT4 }
	};

	const std::unordered_map<harmony::Pipeline::VertexFormat, sg_vertex_format> Pipeline::sVERTEX_FORMAT = {
		{ harmony::Pipeline::VertexFormat::eFLOAT, sg_vertex_format::SG_VERTEXFORMAT_FLOAT },
		{ harmony::Pipeline::VertexFormat::eFLOAT2, sg_vertex_format::SG_VERTEXFORMAT_FLOAT2 },
		{ harmony::Pipeline::VertexFormat::eFLOAT3, sg_vertex_format::SG_VERTEXFORMAT_FLOAT3 },
		{ harmony::Pipeline::VertexFormat::eFLOAT4, sg_vertex_format::SG_VERTEXFORMAT_FLOAT4 },
		{ harmony::Pipeline::VertexFormat::eBYTE4, sg_vertex_format::SG_VERTEXFORMAT_BYTE4 },
		{ harmony::Pipeline::VertexFormat::eBYTE4N, sg_vertex_format::SG_VERTEXFORMAT_BYTE4N },
		{ harmony::Pipeline::VertexFormat::eUBYTE4, sg_vertex_format::SG_VERTEXFORMAT_UBYTE4 },
		{ harmony::Pipeline::VertexFormat::eUBYTE4N, sg_vertex_format::SG_VERTEXFORMAT_UBYTE4N },
		{ harmony::Pipeline::VertexFormat::eSHORT2, sg_vertex_format::SG_VERTEXFORMAT_SHORT2 },
		{ harmony::Pipeline::VertexFormat::eSHORT2N, sg_vertex_format::SG_VERTEXFORMAT_SHORT2N },
		{ harmony::Pipeline::VertexFormat::eUSHORT2N, sg_vertex_format::SG_VERTEXFORMAT_USHORT2N },
		{ harmony::Pipeline::VertexFormat::eSHORT4, sg_vertex_format::SG_VERTEXFORMAT_SHORT4 },
		{ harmony::Pipeline::VertexFormat::eSHORT4N, sg_vertex_format::SG_VERTEXFORMAT_SHORT4N },
		{ harmony::Pipeline::VertexFormat::eUSHORT4N, sg_vertex_format::SG_VERTEXFORMAT_USHORT4N },
		{ harmony::Pipeline::VertexFormat::eUINT10_N2, sg_vertex_format::SG_VERTEXFORMAT_UINT10_N2 },
		{ harmony::Pipeline::VertexFormat::eHALF2, sg_vertex_format::SG_VERTEXFORMAT_HALF2 },
		{ harmony::Pipeline::VertexFormat::eHALF4, sg_vertex_format::SG_VERTEXFORMAT_HALF4 }
	};
	const std::unordered_map<harmony::Pipeline::PrimitiveType, sg_primitive_type> Pipeline::sPRIMITIVE_TYPE = {
		{ harmony::Pipeline::PrimitiveType::ePOINTS, sg_primitive_type::SG_PRIMITIVETYPE_POINTS },
		{ harmony::Pipeline::PrimitiveType::eLINES, sg_primitive_type::SG_PRIMITIVETYPE_LINES },
		{ harmony::Pipeline::PrimitiveType::eLINE_STRIP, sg_primitive_type::SG_PRIMITIVETYPE_LINE_STRIP },
		{ harmony::Pipeline::PrimitiveType::eTRIANGLES, sg_primitive_type::SG_PRIMITIVETYPE_TRIANGLES },
		{ harmony::Pipeline::PrimitiveType::eTRIANGLE_STRIP, sg_primitive_type::SG_PRIMITIVETYPE_TRIANGLE_STRIP }
	};
	const std::unordered_map<harmony::Pipeline::CullMode, sg_cull_mode> Pipeline::sCULL_MODE = {
		{ harmony::Pipeline::CullMode::eNONE, sg_cull_mode::SG_CULLMODE_NONE },
		{ harmony::Pipeline::CullMode::eFRONT, sg_cull_mode::SG_CULLMODE_FRONT },
		{ harmony::Pipeline::CullMode::eBACK, sg_cull_mode::SG_CULLMODE_BACK }
	};
	const std::unordered_map<harmony::Pipeline::FaceWinding, sg_face_winding> Pipeline::sFACE_WINDING = {
		{ harmony::Pipeline::FaceWinding::eCOUNTER_CLOCKWISE, sg_face_winding::SG_FACEWINDING_CCW },
		{ harmony::Pipeline::FaceWinding::eCLOCKWISE, sg_face_winding::SG_FACEWINDING_CW }
	};
	const std::unordered_map<harmony::Pipeline::IndexType, sg_index_type> Pipeline::sINDEX_TYPE = {
		{ harmony::Pipeline::IndexType::eNO_INDEX_BUFFER, sg_index_type::SG_INDEXTYPE_NONE },
		{ harmony::Pipeline::IndexType::eUINT16, sg_index_type::SG_INDEXTYPE_UINT16 },
		{ harmony::Pipeline::IndexType::eUINT32, sg_index_type::SG_INDEXTYPE_UINT32 }
	};
	const std::unordered_map<harmony::Pipeline::ShaderStageType, sg_shader_stage> Pipeline::sSTAGE_TYPE = {
		{ harmony::Pipeline::ShaderStageType::eVERTEX_SHADER, sg_shader_stage::SG_SHADERSTAGE_VS },
		{ harmony::Pipeline::ShaderStageType::eFRAGMENT_SHADER, sg_shader_stage::SG_SHADERSTAGE_FS }
	};

	const std::unordered_map<harmony::Sampler::Filter, sg_filter> Sampler::sFILTER = {
		{ harmony::Sampler::Filter::eLINEAR, sg_filter::SG_FILTER_LINEAR },
		{ harmony::Sampler::Filter::eNEAREST, sg_filter::SG_FILTER_NEAREST }
	};
}