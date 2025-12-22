#pragma once

#include "common.h"

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

        graphics::Buffer* vertexBuffer = nullptr;
        graphics::Buffer* indexBuffer = nullptr;

        unsigned int vertexByteStride = 0;
        bool is32BitIndices = false;

        std::unordered_map<std::string, SubmeshGeometry> drawArgs;

        inline const graphics::BufferView vertexBufferView() const
        {
            HASSERT(vertexBuffer != nullptr, "Vertex buffer is not set for mesh geometry ", name.c_str());
            return vertexBuffer->vertexBufferView(vertexByteStride);
        }

        inline const graphics::BufferView indexBufferView() const
        {
            HASSERT(indexBuffer != nullptr, "Index buffer is not set for mesh geometry ", name.c_str());
            return indexBuffer->indexBufferView(0, is32BitIndices);
        }
    };

    struct RenderItem
    {
        RenderItem(const MeshGeometry& _geometry = MeshGeometry(), const mat4f _world = mat4f(), const graphics::PrimitiveTopology _primitiveTopology = graphics::PrimitiveTopology::TriangleList)
            : geometry(_geometry), world(_world), primitiveTopology(_primitiveTopology) {
        }

        MeshGeometry geometry;
        mat4f world;

        graphics::PrimitiveTopology primitiveTopology;
    };
}
