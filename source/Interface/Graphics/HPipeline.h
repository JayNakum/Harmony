#pragma once

#include "HShader.h"

namespace harmony
{
    class Pipeline
    {
    public:
        enum ShaderStageType
        {
            eVERTEX_SHADER,
            eFRAGMENT_SHADER,
        };

        enum VertexFormat
        {
            eFLOAT,
            eFLOAT2,
            eFLOAT3,
            eFLOAT4,
            eBYTE4,
            eBYTE4N,
            eUBYTE4,
            eUBYTE4N,
            eSHORT2,
            eSHORT2N,
            eUSHORT2N,
            eSHORT4,
            eSHORT4N,
            eUSHORT4N,
            eUINT10_N2,
            eHALF2,
            eHALF4
        };

        struct VertexAttribute
        {
            VertexFormat format;
			const char* name;
			int index = -1;
        };

        struct VertexLayout
        {
            VertexAttribute attributes[16];
            unsigned int attributesCount;
        };

        enum PrimitiveType
        {
            ePOINTS,
            eLINES,
            eLINE_STRIP,
            eTRIANGLES,
            eTRIANGLE_STRIP
        };

        enum CullMode
        {
            eNONE,
            eFRONT,
            eBACK
        };

        enum FaceWinding
        {
            eCOUNTER_CLOCKWISE,
            eCLOCKWISE
        };

        enum IndexType
        {
            eNO_INDEX_BUFFER,
            eUINT16,
            eUINT32
        };

        struct Description
        {
            const char* label           = "default pipeline";
            
            Shader* vertexShader        = nullptr;
            Shader* fragmentShader      = nullptr;

            VertexLayout layout;
            PrimitiveType primitiveType = PrimitiveType::eTRIANGLES;
            CullMode cullMode           = CullMode::eNONE;
            FaceWinding faceWinding     = FaceWinding::eCLOCKWISE;
            IndexType indexType         = IndexType::eNO_INDEX_BUFFER;

            bool depthBufferEnabled     = false;
        };

        virtual ~Pipeline() = default;

    public:
        virtual const void* getID() const = 0;
        virtual const Description& getDescription() const = 0;

        virtual void initialize() = 0;
        virtual void bind() const = 0;
        virtual void applyConstants(unsigned int constantBlockIndex, const void* data, size_t size, const ShaderStageType type = ShaderStageType::eVERTEX_SHADER) const = 0;
        virtual void unbind() const = 0;
        virtual void deinitialize() = 0;
    };
}