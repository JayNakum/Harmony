#pragma once

namespace harmony
{
    class Shader
    {
    public:
        struct AttributeDescription
        {
            const char* name;
            int index;
        };

        struct StageDescription
        {
            const char* source      = nullptr;
            const void* bytecode    = nullptr;
        };

        struct Description
        {
            const char* label = "default shader program";
            AttributeDescription attrs[16];
            unsigned int attributesCount;
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