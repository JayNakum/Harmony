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
        void render() override;
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

