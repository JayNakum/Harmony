#pragma once

namespace harmony
{
    class Shader
    {
    public:
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
            Constant constants[16] = { 0 };
            unsigned int constantCount = 0;
        };

        struct Texture
        {
            bool used = false;
            unsigned int imageSlot = 0;
            unsigned int samplerSlot = 0;
        };

        enum Type
        {
            eVERTEX,
            eFRAGMENT
        };

        struct Description
        {
            const char* label = "default shader program";
            
            // TODO: use reflections to get these
            const char* source = "default shader";
            Type type = eVERTEX;
            ConstantBlock constantBlocks[4] = { 0 };
            unsigned int constantBlocksCount = 0;
            Texture textures[1] = { 0 }; // TODO: query the size of this array
            unsigned int texturesCount = 0;
        };

        virtual ~Shader() = default;
    public:
        virtual const void* getID() const = 0;
        virtual const Description& getDescription() const = 0;
        virtual void initialize() = 0;
        virtual void deinitialize() = 0;
    };
}