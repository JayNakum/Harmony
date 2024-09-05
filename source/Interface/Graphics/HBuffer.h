#pragma once

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
            const char* label   = "default immutable vertex buffer";
            Type        type    = Type::eVERTEX_BUFFER;
            Usage       usage   = Usage::eIMMUTABLE;
            size_t      size    = 0;
            size_t      stride  = 0;
            const void* data    = nullptr;
        };


        virtual ~Buffer() = default;
    public:
        virtual const void* getID() const = 0;
        virtual const Description& getDescription() const = 0;

        virtual void initialize() = 0;
        virtual void deinitialize() = 0;
    };
}