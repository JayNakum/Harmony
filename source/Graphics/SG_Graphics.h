#pragma once

#include "HGraphics.h"

#include "SG_d3d11_Entry.h"
#include "sokol_gfx.h"

#include <unordered_map>

namespace Sokol
{
    class Buffer : public harmony::Buffer
    {
    public:
        Buffer(const harmony::Buffer::Description& description);
        ~Buffer();

    public:
        const void* getID() const override;
        const harmony::Buffer::Description& getDescription() const override;

        void initialize() override;
        void deinitialize() override;

    private:
        harmony::Buffer::Description mDescription;
        sg_buffer mBuffer;

    private:
        const std::unordered_map<harmony::Buffer::Type, sg_buffer_type> mTYPE = {
            { harmony::Buffer::Type::eVERTEX_BUFFER, sg_buffer_type::SG_BUFFERTYPE_VERTEXBUFFER },
            { harmony::Buffer::Type::eINDEX_BUFFER, sg_buffer_type::SG_BUFFERTYPE_INDEXBUFFER }
        };
        const std::unordered_map<harmony::Buffer::Usage, sg_usage> mUSAGE = {
            { harmony::Buffer::Usage::eIMMUTABLE, sg_usage::SG_USAGE_IMMUTABLE },
            { harmony::Buffer::Usage::eDYNAMIC, sg_usage::SG_USAGE_DYNAMIC },
            { harmony::Buffer::Usage::eSTREAM, sg_usage::SG_USAGE_STREAM }
        };
    };

    class Drawable : public harmony::Drawable
    {
    public:
        Drawable();
        ~Drawable() = default;

    public:
        const harmony::Drawable::Description& getDescription() const override;
        void setDescription(const harmony::Drawable::Description& description) override;

        void addBuffer(harmony::Buffer*& buffer) override;

        void initialize() override;
        
        void bind() const override;
        
        void draw() override;
        
        void unbind() const override;
        
        void deinitialize() override;

    private:
        sg_bindings mBind;
        unsigned int vertexBufferCount;

        harmony::Drawable::Description mDescription;
    };

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
        sg_shader mShaderProgram;
    };

    class Pipeline : public harmony::Pipeline
    {
    public:
        Pipeline(const harmony::Pipeline::Description& description);
        ~Pipeline();

    public:
        const void* getID() const override;
        const Description& getDescription() const override;

        void initialize() override;

        void bind() const override;
        void unbind() const override;

        void deinitialize() override;

    private:
        harmony::Pipeline::Description mDescription;
        sg_pipeline mPipeline;

    private:
        const std::unordered_map<harmony::Pipeline::VertexStep, sg_vertex_step> mVERTEX_STEP = {
            { harmony::Pipeline::VertexStep::ePER_VERTEX, sg_vertex_step::SG_VERTEXSTEP_PER_VERTEX },
            { harmony::Pipeline::VertexStep::ePER_INSTANCE, sg_vertex_step::SG_VERTEXSTEP_PER_INSTANCE }
        };
        const std::unordered_map<harmony::Pipeline::VertexFormat, sg_vertex_format> mVERTEX_FORMAT = {
            { harmony::Pipeline::VertexFormat::eFLOAT, sg_vertex_format::SG_VERTEXFORMAT_FLOAT },
            { harmony::Pipeline::VertexFormat::eFLOAT2, sg_vertex_format::SG_VERTEXFORMAT_FLOAT2 },
            { harmony::Pipeline::VertexFormat::eFLOAT3, sg_vertex_format::SG_VERTEXFORMAT_FLOAT3 },
            { harmony::Pipeline::VertexFormat::eFLOAT4, sg_vertex_format::SG_VERTEXFORMAT_FLOAT4 },
            { harmony::Pipeline::VertexFormat::eBYTE4, sg_vertex_format::SG_VERTEXFORMAT_BYTE4 },
            { harmony::Pipeline::VertexFormat::eBYTE4N, sg_vertex_format::SG_VERTEXFORMAT_BYTE4N },
            { harmony::Pipeline::VertexFormat::eUBYTE4, sg_vertex_format::SG_VERTEXFORMAT_UBYTE4 },
            { harmony::Pipeline::VertexFormat::eUBYTE4N,sg_vertex_format::SG_VERTEXFORMAT_UBYTE4N },
            { harmony::Pipeline::VertexFormat::eSHORT2, sg_vertex_format::SG_VERTEXFORMAT_SHORT2 },
            { harmony::Pipeline::VertexFormat::eSHORT2N,sg_vertex_format::SG_VERTEXFORMAT_SHORT2N },
            { harmony::Pipeline::VertexFormat::eUSHORT2N, sg_vertex_format::SG_VERTEXFORMAT_USHORT2N },
            { harmony::Pipeline::VertexFormat::eSHORT4, sg_vertex_format::SG_VERTEXFORMAT_SHORT4 },
            { harmony::Pipeline::VertexFormat::eSHORT4N,sg_vertex_format::SG_VERTEXFORMAT_SHORT4N },
            { harmony::Pipeline::VertexFormat::eUSHORT4N, sg_vertex_format::SG_VERTEXFORMAT_USHORT4N },
            { harmony::Pipeline::VertexFormat::eUINT10_N2, sg_vertex_format::SG_VERTEXFORMAT_UINT10_N2 },
            { harmony::Pipeline::VertexFormat::eHALF2, sg_vertex_format::SG_VERTEXFORMAT_HALF2 },
            { harmony::Pipeline::VertexFormat::eHALF4, sg_vertex_format::SG_VERTEXFORMAT_HALF4 }
        };
        const std::unordered_map<harmony::Pipeline::IndexType, sg_index_type> mINDEX_TYPE = {
            { harmony::Pipeline::IndexType::eUINT16, sg_index_type::SG_INDEXTYPE_UINT16 },
            { harmony::Pipeline::IndexType::eUINT32, sg_index_type::SG_INDEXTYPE_UINT32 }
        };
        const std::unordered_map<harmony::Pipeline::CullMode, sg_cull_mode> mCULL_MODE = {
            { harmony::Pipeline::CullMode::eNONE, sg_cull_mode::SG_CULLMODE_NONE },
            { harmony::Pipeline::CullMode::eBACK, sg_cull_mode::SG_CULLMODE_BACK },
            { harmony::Pipeline::CullMode::eFRONT, sg_cull_mode::SG_CULLMODE_FRONT }
        };
    };

    class RenderPass : public harmony::RenderPass
    {
    public:
        RenderPass(const harmony::RenderPass::Description& description);
        ~RenderPass();

    public:
        const void* getID() const override;
        const Description& getDescription() const override;
        
        void initialize() override;

        void begin() override;
        void end() const override;

        void deinitialize() override;

    private:
        harmony::RenderPass::Description mDescription;
        sg_pass mRenderPass;

    private:
        const std::unordered_map<harmony::RenderPass::Action::Load, sg_load_action> mLOAD_ACTION = {
            { harmony::RenderPass::Action::Load::eCLEAR, sg_load_action::SG_LOADACTION_CLEAR },
            { harmony::RenderPass::Action::Load::eLOAD, sg_load_action::SG_LOADACTION_LOAD },
            { harmony::RenderPass::Action::Load::eDONT_CARE, sg_load_action::SG_LOADACTION_DONTCARE }
        };
        const std::unordered_map<harmony::RenderPass::Action::Store, sg_store_action> mSTORE_ACTION = {
            { harmony::RenderPass::Action::Store::eSTORE, sg_store_action::SG_STOREACTION_STORE }
        };
    };

    class Renderer : public harmony::Renderer
    {
    public:
        void initialize() override;

        void render() override;

        void deinitialize() override;
    };
}
