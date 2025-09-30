#pragma once

#include "common.h"
#include "Log.h"
#include "Maths.h"

#include "GeometryGenerator.h"

#include "ResourceManager.h"
#include "CommandContext.h"

#include "ShaderInput.h"
#include "RenderItem.h"

#include "DebugUI.h"

namespace engine
{
    /*
    * Common root constants that every pass is suggested to have.
    */

    struct alignas(16) FrameConstants
    {
        float totalTime = 0.0f;
        float deltaTime = 0.0f;
        const uint32_t _framePadding[2] = { 0x1deadb0b, 0x1deadb0b };
    };

    struct alignas(16) PassConstants
    {
        mat4f view;
        mat4f invView;

        vec3f eyePosition;
        const uint32_t _passPadding0 = 0x1deadb0b;

        mat4f projection;
        mat4f invProjection;

        float nearZ = 0.0f;
        float farZ = 0.0f;
        const uint32_t _passPadding1 = 0x1deadb0b;
        const uint32_t _passPadding2 = 0x1deadb0b;

        mat4f viewProjection;
        mat4f invViewProjection;

        vec2f renderTargetSize;
        vec2f invRenderTargetSize;
        const uint32_t _passPadding[5] = { 0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b };
    };
}

namespace engine
{
    class PassData
    {
    public:
        PassData() { DebugUI::Register([this] { debugUIRender(); }); }

        inline bool getIsDirty() const { return mIsDirty; }
        inline void setIsDirty(const bool value) { mIsDirty = value; }

        inline void setFrameConstants(const float totalTime = 0.0f, const float deltaTime = 0.0f)
        {
            mFrameConstants.totalTime = totalTime;
            mFrameConstants.deltaTime = deltaTime;
        }
        inline void setFrameConstants(const FrameConstants& frameConstants)
        {
            setFrameConstants(frameConstants.totalTime, frameConstants.deltaTime);
        }

        inline const FrameConstants getFrameConstants() const
        {
            return mFrameConstants;
        }

        inline void setViewMatrix(const mat4f viewMatrix = mat4f())
        {
            mPassConstants.view = viewMatrix;
            mPassConstants.invView = inverse(mPassConstants.view);

            mPassConstants.viewProjection = mPassConstants.projection * mPassConstants.view;
            mPassConstants.invViewProjection = inverse(mPassConstants.viewProjection);
        }

        inline void setEyePosition(const vec3f eyePosition = vec3f())
        {
            mPassConstants.eyePosition = eyePosition;
        }

        inline void setProjectionMatrix(const mat4f projectionMatrix = mat4f(), const float nearZ = 0.0f, const float farZ = 0.0f)
        {
            mPassConstants.projection = projectionMatrix;
            mPassConstants.invProjection = inverse(projectionMatrix);

            mPassConstants.nearZ = nearZ;
            mPassConstants.farZ = farZ;

            mPassConstants.viewProjection = mPassConstants.projection * mPassConstants.view;
            mPassConstants.invViewProjection = inverse(mPassConstants.viewProjection);
        }

        inline void setRenderTargetSize(const unsigned int width = 0, const unsigned int height = 0)
        {
            mPassConstants.renderTargetSize.x = (float)width;
            mPassConstants.renderTargetSize.y = (float)height;
            mPassConstants.invRenderTargetSize = 1 / mPassConstants.renderTargetSize;
        }

        inline void setPassConstants(const PassConstants& passConstants)
        {
            setViewMatrix(passConstants.view);
            setEyePosition(passConstants.eyePosition);
            setProjectionMatrix(passConstants.projection, passConstants.nearZ, passConstants.farZ);
            setRenderTargetSize(passConstants.renderTargetSize.x, passConstants.renderTargetSize.y);
        }

        inline const PassConstants getPassConstants() const
        {
            return mPassConstants;
        }

        virtual void debugUIRender()
        {
            ImGui::Begin("Default Pass");
            debugUICommonParams();
            ImGui::End();
        }

        virtual void reset() = 0;
        virtual ~PassData() = default;

        void debugUICommonParams()
        {
            if (ImGui::CollapsingHeader("Pass Constants"))
            {
                ImGui::DragFloat3("Eye Position", (float*)&mPassConstants.eyePosition);

                DebugUI::matrix("View Matrix", mPassConstants.view);
                DebugUI::matrix("Inverse View Matrix", mPassConstants.invView);

                DebugUI::matrix("Projection Matrix", mPassConstants.projection);
                DebugUI::matrix("Inverse Projection Matrix", mPassConstants.invProjection);

                DebugUI::matrix("ViewProj Matrix", mPassConstants.viewProjection);
                DebugUI::matrix("Inverse ViewProj Matrix", mPassConstants.invViewProjection);

                ImGui::Text("Near Z: %.3f", &mPassConstants.nearZ);
                ImGui::Text("Far Z: %.3f", &mPassConstants.farZ);

                ImGui::Text("RT Size: %.3f, %.3f", mPassConstants.renderTargetSize.x, mPassConstants.renderTargetSize.y);
                ImGui::Text("Inv RT Size: %f, %f", mPassConstants.invRenderTargetSize.x, mPassConstants.invRenderTargetSize.y);
            }
        }

        void debugUISave()
        {
            if (ImGui::Button("SAVE"))
            {
                mIsDirty = true;
            }

            if (ImGui::Button("RESET"))
            {
                reset();
            }
        }

    private:
        FrameConstants mFrameConstants;
        PassConstants mPassConstants;
        bool mIsDirty = true;
    };
}

namespace engine
{
    /*
    * A pass on graphics queue.
    */
    class RenderPass
    {
    protected:
        virtual bool needsCopyDataToGPU() const { return false; }
        virtual void copyDataToGPU(graphics::CopyContext& context, ResourceManager& resourceManager) = 0;

        virtual void freeGPUUploadData(ResourceManager& resourceManager) = 0;

        virtual void resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager) = 0;
        virtual void update(const double timeStep) = 0;

        virtual void getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager) = 0;
        virtual void render(graphics::GraphicContext& context, const FrameConstants& frameConstants) = 0;

        virtual ~RenderPass() = default;
    };
}

namespace engine
{
    /*
    * A pass on compute queue.
    */
    class DispatchPass
    {
    protected:
        virtual bool needsCopyDataToGPU() const { return false; }
        virtual void copyDataToGPU(graphics::CopyContext& context, ResourceManager& resourceManager) = 0;

        virtual void freeGPUUploadData(ResourceManager& resourceManager) = 0;

        virtual void resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager) = 0;
        virtual void update(const double timeStep) = 0;

        virtual void getInputOutputs(graphics::ComputeContext& context, ResourceManager& resourceManager) = 0;
        virtual void dispatch(graphics::ComputeContext& context, const FrameConstants& frameConstants) = 0;

        virtual ~DispatchPass() = default;
    };
}
