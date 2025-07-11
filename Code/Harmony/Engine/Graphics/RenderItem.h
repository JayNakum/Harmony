#pragma once

#include "dx12/Buffer.h"
#include "PipelineStateEnums.h"

#include "Maths.h"

namespace engine
{
    struct SubmeshGeometry
    {
        unsigned int indexCount = 0;
        unsigned int startIndexLocation = 0;
        int baseVertexLocation = 0;
    };

    struct MeshGeometry
    {
        std::string name;

        graphics::dx12::ByteAddressBuffer* vertexBufferUploader = nullptr;
        graphics::dx12::ByteAddressBuffer* vertexBuffer = nullptr;

        graphics::dx12::ByteAddressBuffer* indexBufferUploader = nullptr;
        graphics::dx12::ByteAddressBuffer* indexBuffer = nullptr;

        unsigned int vertexByteStride = 0;
        bool is32BitIndices = false;

        std::unordered_map<std::string, SubmeshGeometry> drawArgs;

        inline const graphics::dx12::BufferView vertexBufferView() const
        {
            return vertexBuffer->vertexBufferView(vertexByteStride);
        }

        inline const graphics::dx12::BufferView indexBufferView() const
        {
            return indexBuffer->indexBufferView(0, is32BitIndices);
        }

        inline void disposeUploaders()
        {
            delete vertexBufferUploader;
            delete indexBufferUploader;
        }
    };

    struct RenderItem
    {
        RenderItem(const MeshGeometry& _geometry, const mat4f _world = mat4f(), const graphics::PrimitiveTopology _primitiveTopology = graphics::PrimitiveTopology::TriangleList)
            : geometry(_geometry), world(_world), primitiveTopology(_primitiveTopology) {}

        MeshGeometry geometry;
        mat4f world;

        graphics::PrimitiveTopology primitiveTopology;
    };
}
