#pragma once

#include "Graphics/HBuffer.h"
#include "Graphics/HDrawable.h"
#include "Graphics/HShader.h"
#include "Graphics/HPipeline.h"
#include "Graphics/HRenderPass.h"
#include "Graphics/HRenderer.h"

#include <sokol_gfx.h>

#include <unordered_map>

namespace sokol
{
    class Renderer : public harmony::Renderer
    {
    public:
        Renderer() = default;
        ~Renderer();

        void initialize() override;
        
        void deinitialize() override;
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
    class Drawable : public harmony::Drawable
    {
    public:

        Drawable(const harmony::Drawable::Description& description);
        ~Drawable();

    public:
        const harmony::Drawable::Description& getDescription() const override;

        void initialize() override;
        void bind() const override;
        void draw(int baseElement, int numElements, int numInstances = 1) const override;
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
        sg_shader mShader;
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
        void unbind() const override;
        void deinitialize() override;

    private:
        harmony::Pipeline::Description mDescription;
        sg_pipeline mPipeline;

    private:
        static const std::unordered_map<harmony::Pipeline::VertexFormat, sg_vertex_format> mVERTEX_FORMAT;
        static const std::unordered_map<harmony::Pipeline::PrimitiveType, sg_primitive_type> mPRIMITIVE_TYPE;
        static const std::unordered_map<harmony::Pipeline::CullMode, sg_cull_mode> mCULL_MODE;
        static const std::unordered_map<harmony::Pipeline::FaceWinding, sg_face_winding> mFACE_WINDING;
        static const std::unordered_map<harmony::Pipeline::IndexType, sg_index_type> mINDEX_TYPE;
    };
}

namespace sokol
{
    class RenderPass : public harmony::RenderPass
    {
    public:
        RenderPass(const harmony::RenderPass::Description& description);
        ~RenderPass();

    public:
        const void* getID() const override;
        const harmony::RenderPass::Description& getDescription() const override;
        void initialize() override;
        void begin() override;
        void end() const override;
        void deinitialize() override;

    private:
        harmony::RenderPass::Description mDescription;
        sg_pass mPass;
    };
}
