#include "SG_Renderer.h"

#include "Helpers/HLog.h"
#include "Harmony.h"

#include <sokol_glue.h>

namespace sokol
{
	Renderer::~Renderer()
	{
		sg_shutdown();
	}

	void Renderer::initialize()
	{
		sg_desc desc = { 0 };
		desc.environment = sglue_environment();
		sg_setup(desc);
	}

	void Renderer::render()
	{
		// frame();
	}

	void Renderer::deinitialize()
	{}
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

	Buffer::Buffer(const harmony::Buffer::Description& description)
		: mDescription(description), mBuffer() {}

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
		sg_buffer_desc desc = { 0 };
		desc.label = mDescription.label;
		desc.data.ptr = mDescription.data;
		desc.size = mDescription.size;
		desc.type = Buffer::sTYPE.at(mDescription.type);
		desc.usage = Buffer::sUSAGE.at(mDescription.usage);

		mBuffer = sg_make_buffer(desc);
	}

	void Buffer::deinitialize()
	{
		sg_dealloc_buffer(mBuffer);
	}
}