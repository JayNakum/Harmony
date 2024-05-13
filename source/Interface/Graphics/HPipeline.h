#pragma once

#include "HShader.h"

namespace harmony
{
    class Pipeline
    {
    public:
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

        struct Description
        {
            const char* label           = "default pipeline";
            Shader* shaderProgram;
            VertexLayout layout;
            PrimitiveType primitiveType = PrimitiveType::eTRIANGLES;
            CullMode cullMode           = CullMode::eNONE;
            FaceWinding faceWinding     = FaceWinding::eCLOCKWISE;
        };

        virtual ~Pipeline() = default;

    public:
        virtual const void* getID() const = 0;
        virtual const Description& getDescription() const = 0;

        virtual void initialize() = 0;
        virtual void bind() const = 0;
        virtual void unbind() const = 0;
        virtual void deinitialize() = 0;
    };
}