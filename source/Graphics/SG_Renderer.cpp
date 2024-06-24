#include "SG_Renderer.h"

#include "Harmony.h"
#include "Helpers/HLog.h"

#include <sokol_glue.h>

namespace sokol
{
	Renderer::~Renderer()
	{
		sg_shutdown();
	}

	void Renderer::initialize()
	{
		sg_desc createDesc = { 0 };
		createDesc.environment = sglue_environment();
		// createDesc.logger.func = slog_func;
		sg_setup(createDesc);
	}

	void Renderer::deinitialize()
	{
	}
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
	sokol::Drawable::Drawable(const harmony::Drawable::Description& description)
		: mDescription(description), mBind{ 0 } {}

	const harmony::Drawable::Description& sokol::Drawable::getDescription() const
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
	}
	
	void Drawable::bind() const
	{
		sg_apply_bindings(&mBind);
	}
	
	void Drawable::draw(int baseElement, int numElements, int numInstances) const
	{
		sg_draw(baseElement, numElements, numInstances);
	}

	void Drawable::unbind() const
	{
		sg_bindings unbind = { 0 };
		sg_apply_bindings(unbind);
	}

	void Drawable::deinitialize()
	{
		unbind();

		mDescription.vertexBuffer->deinitialize();
		if (mDescription.indexBuffer)
			mDescription.indexBuffer->deinitialize();
	}

	Drawable::~Drawable()
	{
		delete mDescription.vertexBuffer;
		delete mDescription.indexBuffer;
		mBind = { 0 };
	}
}

namespace sokol
{
	sokol::Shader::Shader(const harmony::Shader::Description& description)
		: mDescription(description), mShader{0} {}

	const void* sokol::Shader::getID() const
	{
		return &mShader;
	}

	const harmony::Shader::Description& Shader::getDescription() const
	{
		return mDescription;
	}

	void Shader::initialize()
	{
		sg_shader_desc createDesc = { 0 };

		createDesc.label = mDescription.label;

		for (unsigned int i = 0; i < mDescription.attributesCount; ++i)
		{
			createDesc.attrs[i].sem_name = mDescription.attrs[i].name;
			if (mDescription.attrs[i].index >= 0)
				createDesc.attrs[i].sem_index = mDescription.attrs[i].index;
		}

		createDesc.vs.source = mDescription.vs.source;
		for (unsigned int i = 0; i < mDescription.vs.constantBlocksCount; ++i)
		{
			createDesc.vs.uniform_blocks[i].size = mDescription.vs.constantBlocks[i].size;
			for (unsigned int j = 0; j < mDescription.vs.constantBlocks[i].constantCount; ++j)
			{
				createDesc.vs.uniform_blocks[i].uniforms[j].name = mDescription.vs.constantBlocks[i].constants[j].name;
				createDesc.vs.uniform_blocks[i].uniforms[j].type = sCONSTANT_TYPE.at(mDescription.vs.constantBlocks[i].constants[j].type);
			}
		}

		createDesc.fs.source = mDescription.fs.source;

		mShader = sg_make_shader(createDesc);

	}

	void Shader::deinitialize()
	{
		sg_dealloc_shader(mShader);
	}

	Shader::~Shader()
	{
		sg_destroy_shader(mShader);
	}
}

namespace sokol
{
	Pipeline::Pipeline(const harmony::Pipeline::Description& description)
		: mDescription(description), mPipeline{0} {}

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

		mDescription.shaderProgram->initialize();
		createDesc.shader = *((sg_shader*)mDescription.shaderProgram->getID());

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
		sg_pipeline unbind = { 0 };
		sg_apply_pipeline(unbind);
	}

	void Pipeline::deinitialize()
	{
		mDescription.shaderProgram->deinitialize();
		sg_dealloc_pipeline(mPipeline);
	}

	Pipeline::~Pipeline()
	{
		sg_destroy_pipeline(mPipeline);
		delete mDescription.shaderProgram;
		mDescription = { 0 };
	}
}

namespace sokol
{
	RenderPass::RenderPass(const harmony::RenderPass::Description& description)
		: mDescription(description), mPass{0} {}

	const void* RenderPass::getID() const
	{
		return &mPass;
	}

	const harmony::RenderPass::Description& RenderPass::getDescription() const
	{
		return mDescription;
	}

	void RenderPass::initialize()
	{
		mPass.label = mDescription.label;

		mPass.action.colors->load_action = SG_LOADACTION_CLEAR;
		mPass.action.colors[0].clear_value = { 
			mDescription.action.colors.value[0],
			mDescription.action.colors.value[1],
			mDescription.action.colors.value[2],
			mDescription.action.colors.value[3]
		};

	}

	void RenderPass::begin()
	{
		mPass.swapchain = sglue_swapchain();
		sg_begin_pass(mPass);
	}

	void RenderPass::end() const
	{
		sg_end_pass();
		sg_commit();
	}

	void RenderPass::deinitialize() {}

	RenderPass::~RenderPass()
	{

		mPass = { 0 };
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

	const std::unordered_map<harmony::Shader::ConstantType, sg_uniform_type> Shader::sCONSTANT_TYPE = {
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
}