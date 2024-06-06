#pragma once

namespace harmony
{
    class Shader
    {
    public:
        struct AttributeDescription
        {
            const char* name;
            int index = -1;
        };

        enum ConstantType
        {
            eFLOAT,
            eFLOAT2,
            eFLOAT3,
            eFLOAT4,
            eINT,
            eINT2,
            eINT3,
            eINT4,
            eMAT4
        };

        struct Constant
        {
            const char* name;
            ConstantType type;
        };

        struct ConstantBlock
        {
            size_t size;
            Constant constants[16];
            unsigned int constantCount = 0;
        };

        struct StageDescription
        {
            const char* source = nullptr;
            ConstantBlock constantBlocks[4];
            unsigned int constantBlocksCount = 0;
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