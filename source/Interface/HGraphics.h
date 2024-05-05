#pragma once

#include <functional>

namespace harmony
{
	class Buffer
	{
	public:
        enum Type
        {
            eVERTEX_BUFFER,
            eINDEX_BUFFER
        };

        enum Usage
        {
            eIMMUTABLE,
            eDYNAMIC,
            eSTREAM
        };

        struct Description
        {
            const char* label;
            Type        type    = Type::eVERTEX_BUFFER;
            Usage       usage   = Usage::eIMMUTABLE;
            size_t      size;
            const void* data;
        };


        virtual ~Buffer() = default;
    public:
        virtual const void* getID() const = 0;
        virtual const Description& getDescription() const = 0;

        virtual void initialize() = 0;
        virtual void deinitialize() = 0;
	};

    class Drawable
    {
    public:
        struct Description
        {
            int baseElement;
            int numElements;
            int numInstances = 1;
        };

        virtual ~Drawable() = default;

    public:
        virtual const Description& getDescription() const = 0;
        virtual void setDescription(const Description& description) = 0;

        virtual void addBuffer(Buffer*& buffer) = 0;

        virtual void initialize() = 0;

        virtual void bind() const = 0;
        
        virtual void draw() = 0;

        virtual void unbind() const = 0;
        
        virtual void deinitialize() = 0;
    };

    class Shader
    {
    public:
        struct AttributeDescription
        {
            const char* name;
            int index;
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
        struct ConstantDescription
        {
            const char* name;
            ConstantType type;
        };
        struct ConstantBlock
        {
            size_t size;
            ConstantDescription constants[16];
            unsigned int constantsSize;
        };

        struct StageDescription
        {
            const char* source;
            // const void* bytecode;
            ConstantBlock constantBlocks[4];
            unsigned int constantBlocksCount;
        };

        struct Description
        {
            const char* label;
            AttributeDescription attrs[16];
            unsigned int attrCount;
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

    class Pipeline
    {
    public:
        enum VertexStep
        {
            ePER_VERTEX,
            ePER_INSTANCE,
        };

        struct VertexBufferLayout
        {
            int stride;
            VertexStep stepFunction;
            int stepRate;
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
            int bufferIndex;
            int offset;
            VertexFormat format;
        };

        struct VertexLayout
        {
            VertexBufferLayout buffers[8];
            unsigned int buffersCount;
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

        enum IndexType
        {
            eUINT16,
            eUINT32
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
            Shader* shader;
            VertexLayout layout;
            PrimitiveType primitiveType = PrimitiveType::eTRIANGLES;
            IndexType indexType = IndexType::eUINT16;
            CullMode cullMode = CullMode::eNONE;
            FaceWinding faceWinding = FaceWinding::eCLOCKWISE;
            int sampleCount;
            const char* label;
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

    class RenderPass
    {
    public:
        struct Action
        {
            enum Load
            {
                eCLEAR,
                eLOAD,
                eDONT_CARE
            };

            enum Store
            {
                eSTORE
            };

            struct Attachment
            {
                Action::Load loadAction = Load::eCLEAR;
                Action::Store storeAction = Store::eSTORE;
                float clearValue[4];
            };

            Attachment colors[4];
        };

        struct Description
        {
            const char* label;
            Action action;
        };

        virtual ~RenderPass() = default;

    public:
        virtual const void* getID() const = 0;
        virtual const Description& getDescription() const = 0;

        virtual void initialize() = 0;

        virtual void begin() = 0;
        virtual void end() const = 0;

        virtual void deinitialize() = 0;
    };

    class Renderer
    {
    public:
        virtual void initialize() = 0;

        std::function<void()> frame;

        virtual void render() = 0;

        virtual void deinitialize() = 0;
    };
}
