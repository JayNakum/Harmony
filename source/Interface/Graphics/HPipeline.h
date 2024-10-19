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
            eFLOAT,     // 4 bytes
            eFLOAT2,    // 8 bytes
            eFLOAT3,    // 12 bytes
            eFLOAT4,    // 16 bytes
            eBYTE4,     // 4 bytes
            eBYTE4N,    // 4 bytes
            eUBYTE4,    // 4 bytes
            eUBYTE4N,   // 4 bytes
            eSHORT2,    // 4 bytes
            eSHORT2N,   // 4 bytes
            eUSHORT2N,  // 4 bytes
            eSHORT4,    // 8 bytes
            eSHORT4N,   // 8 bytes
            eUSHORT4N,  // 8 bytes
            eUINT10_N2, // 4 bytes
            eHALF2,     // 4 bytes
            eHALF4      // 8 bytes
        };

        unsigned int getByteSize(const VertexFormat format) const
        {
            switch (format)
            {
            default:
                return 0;

            case eFLOAT:
			case eBYTE4:
			case eBYTE4N:
			case eUBYTE4:
			case eUBYTE4N:
			case eSHORT2:
			case eSHORT2N:
            case eUSHORT2N:
            case eUINT10_N2:
            case eHALF2:
                return 4;

            case eFLOAT2:
            case eSHORT4:
            case eSHORT4N:
            case eUSHORT4N:
            case eHALF4:
                return 8;

            case eFLOAT3:
                return 12;

            case eFLOAT4:
                return 16;
            }
        }

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
        virtual void bind() = 0;
        virtual void applyConstants(unsigned int constantBlockIndex, const void* data, size_t size, const ShaderStageType type = ShaderStageType::eVERTEX_SHADER) = 0;
        virtual void unbind() const = 0;
        virtual void deinitialize() = 0;
    };
}