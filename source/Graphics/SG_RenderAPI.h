#pragma once

#include "Graphics/HBuffer.h"
#include "Graphics/HTexture.h"
#include "Graphics/HDrawable.h"
#include "Graphics/HShader.h"
#include "Graphics/HPipeline.h"
#include "Graphics/HRenderPass.h"
#include "Graphics/HSwapchain.h"
#include "Graphics/HRenderTarget.h"
#include "Graphics/HRenderAPI.h"

#include <sokol_gfx.h>

#include <unordered_map>

namespace sokol
{
    class RenderAPI : public harmony::RenderAPI
    {
    public:
        RenderAPI(const harmony::RenderAPI::Description& description);
        ~RenderAPI();

        const harmony::RenderAPI::Description& getDescription() const override;
        void initialize() override;
        void frameBegin() override;
        void frameEnd() override;
        void present() override;
        void deinitialize() override;

    private:
		harmony::RenderAPI::Description mDescription;
    };
}

namespace sokol
{
    class RenderTarget : public harmony::RenderTarget
    {
        friend class Swapchain;
        RenderTarget(const harmony::RenderTarget::Description& description = {});
        ~RenderTarget() = default;
    public:
        const harmony::RenderTarget::Description& getDescription() const override;

        void initialize() override;
        const void* getID() const;
        void bind(const Action& action) override;
        void unbind() override;
        void deinitialize() override;

    private:
        sg_pass_action mAction;
        harmony::RenderTarget::Description mDescription;
    };
}

namespace sokol
{
    class Swapchain : public harmony::Swapchain
    {
    public:
        Swapchain(const harmony::Swapchain::Description& description);
        ~Swapchain();

        const void* getID() const override;
        const harmony::Swapchain::Description& getDescription() const override;
        harmony::RenderTarget* getCurrentRenderTarget() const override;

        void initialize() override;
        void prepare() override;
        void resize(unsigned int width, unsigned int height) override;
        void present() override;
        void deinitialize() override;

    private:
        harmony::Swapchain::Description mDescription;
        sg_pass mPass;
        harmony::RenderTarget* mRenderTarget = nullptr;
    };
}

namespace sokol
{
    class Buffer : public harmony::Buffer
    {
    public:
        Buffer(const harmony::Buffer::Description& description);
        ~Buffer();

        const void* getID() const override;
        const Description& getDescription() const override;

        void initialize() override;
        void deinitialize() override;

    private:
        harmony::Buffer::Description mDescription;
        sg_buffer mBuffer;

    private:
        static const std::unordered_map<harmony::Buffer::Type, sg_buffer_type> sTYPE;
        static const std::unordered_map<harmony::Buffer::Usage, sg_usage> sUSAGE;
    };
}

namespace sokol
{
    class Image : public harmony::Image
    {
    public:
        Image(const harmony::Image::Description& description);
        ~Image();

        const void* getID() const;
        const harmony::Image::Description& getDescription() const;
        void initialize();
        void deinitialize();

    private:
        harmony::Image::Description mDescription;
        sg_image mImage;
    };

    class Sampler : public harmony::Sampler
    {
    public:
        Sampler(const harmony::Sampler::Description& description);
        ~Sampler();

    public:
        const void* getID() const override;
        const harmony::Sampler::Description& getDescription() const override;
        void initialize() override;
        void deinitialize() override;

    private:
        harmony::Sampler::Description mDescription;
        sg_sampler mSampler;

    private:
        static const std::unordered_map<harmony::Sampler::Filter, sg_filter> sFILTER;
    };
}

namespace sokol
{
    class Drawable : public harmony::Drawable
    {
    public:
        Drawable(const harmony::Drawable::Description& description);
        ~Drawable();

    public:
        const harmony::Drawable::Description& getDescription() const override;

        void initialize() override;
        void bind(unsigned int slot = 0) const override; // Slot is unused in sokol
        void draw() const override;
        void unbind() const override;
        void deinitialize() override;

    private:
        harmony::Drawable::Description mDescription;
        sg_bindings mBind;
    };
}

namespace sokol
{
    class Shader : public harmony::Shader
    {
    public:
        Shader(const harmony::Shader::Description& description);
        ~Shader();
    public:
        const void* getID() const override;
        const harmony::Shader::Description& getDescription() const override;
        void initialize() override;
        void deinitialize() override;

    private:
        harmony::Shader::Description mDescription;
    };
}

namespace sokol
{
    class Pipeline : public harmony::Pipeline
    {
    public:
        Pipeline(const harmony::Pipeline::Description& description);
        ~Pipeline();

    public:
        const void* getID() const override;
        const harmony::Pipeline::Description& getDescription() const override;
        void initialize() override;
        void bind() const override;
        void applyConstants(unsigned int constantBlockIndex, const void* data, size_t size, const harmony::Pipeline::ShaderStageType type = harmony::Pipeline::ShaderStageType::eVERTEX_SHADER) const override;
        void unbind() const override;
        void deinitialize() override;

    private:
        harmony::Pipeline::Description mDescription;
        sg_pipeline mPipeline;
        sg_shader mShaderProgram;
	private:
		static const std::unordered_map<harmony::Shader::ConstantType, sg_uniform_type> sCONSTANT_TYPE;

        static const std::unordered_map<harmony::Pipeline::VertexFormat, sg_vertex_format> sVERTEX_FORMAT;
        static const std::unordered_map<harmony::Pipeline::PrimitiveType, sg_primitive_type> sPRIMITIVE_TYPE;
        static const std::unordered_map<harmony::Pipeline::CullMode, sg_cull_mode> sCULL_MODE;
        static const std::unordered_map<harmony::Pipeline::FaceWinding, sg_face_winding> sFACE_WINDING;
        static const std::unordered_map<harmony::Pipeline::IndexType, sg_index_type> sINDEX_TYPE;
        static const std::unordered_map<harmony::Pipeline::ShaderStageType, sg_shader_stage> sSTAGE_TYPE;
    };
}
