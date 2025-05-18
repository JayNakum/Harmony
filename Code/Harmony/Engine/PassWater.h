#pragma once

#include "CommandContext.h"

namespace engine
{
    namespace renderPass
    {
        class Water
        {
        public:
            Water() = default;

            struct InputOutputs
            {

            };
            void render(graphics::GraphicContext& context, InputOutputs& inputOutputs);

            ~Water() = default;
        };
    }
}
