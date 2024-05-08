#pragma once

namespace harmony
{
    class Shader
    {
    public:
        struct StageDescription
        {
            const char* source      = nullptr;
            const void* bytecode    = nullptr;
        };

        struct Description
        {
            const char* label;
            StageDescription vs;
            StageDescription fs;
        };

        virtual ~Shader() = default;
    public:
        virtual const void* getID() const = 0;
        virtual const Description& getDescription() const = 0;

        virtual void initialize() = 0;
        virtual void deinitialize() = 0;
    };
}