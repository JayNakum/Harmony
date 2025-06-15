#pragma once

#include "common.h"
#include "Log.h"

#include "Maths.h"

namespace engine
{
    class GeometryGenerator
    {
    public:
        struct Vertex
        {
            Vertex() = default;

            Vertex(const vec3f& p, const vec3f& n, const vec3f& t, const vec2f& uv)
                : position(p), normal(n), tangentU(t), textureCoord(uv)
            {}

            Vertex(
                float px, float py, float pz,
                float nx, float ny, float nz,
                float tx, float ty, float tz,
                float u, float v)
                : position(px, py, pz), normal(nx, ny, nz), tangentU(tx, ty, tz), textureCoord(u, v)
            {}

            vec3f position;
            vec3f normal;
            vec3f tangentU;
            vec2f textureCoord;
        };

        struct MeshData
        {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices32;

            inline std::vector<uint16_t>& getIndices16()
            {
                if (mIndices16.empty())
                {
                    mIndices16.resize(indices32.size());
                    for (size_t i = 0; i < indices32.size(); ++i)
                    {
                        mIndices16[i] = static_cast<uint16_t>(indices32[i]);
                    }
                }

                return mIndices16;
            }

        private:
            std::vector<uint16_t> mIndices16;
        };

        MeshData createBox(float width, float height, float depth, uint32_t numSubdivisions);

        MeshData createSphere(float radius, uint32_t sliceCount, uint32_t stackCount);

        MeshData createGeosphere(float radius, uint32_t numSubdivisions);

        MeshData createCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount);

        MeshData createGrid(float width, float depth, uint32_t m, uint32_t n);

        MeshData createQuad(float x, float y, float w, float h, float depth);

    private:
        void subdivide(MeshData& meshData);
        Vertex midPoint(const Vertex& v0, const Vertex& v1);
        void buildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData);
        void buildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData);
    };
}
