#include <iostream>

#include "Harmony.h"

int main()
{
    auto renderer = harmony::createSGRenderer();
    renderer->initialize();

    const float vertices[] = {
        // positions            colors
         0.0f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
    };

    harmony::Buffer::Description vbDesc;
    vbDesc.data = vertices;
    vbDesc.size = sizeof(vertices);
    vbDesc.type = harmony::Buffer::Type::eVERTEX_BUFFER;
    vbDesc.label = "TriangleVertexBuffer";
    harmony::Buffer* buffer = harmony::createSGBuffer(vbDesc);
    buffer->initialize();

    harmony::Drawable::Description drawDesc;
    drawDesc.baseElement = 0;
    drawDesc.numElements = 3;

    auto drawable = harmony::createSGDrawable();
    drawable->addBuffer(buffer);
    drawable->initialize();
    drawable->setDescription(drawDesc);

    harmony::Shader::Description shdDesc;
    shdDesc.attrs[0].name = "POS";
    shdDesc.attrs[1].name = "COLOR";
    shdDesc.attrCount = 2;

    shdDesc.vs.source = 
        "struct vs_in {\n"
        "  float4 pos: POS;\n"
        "  float4 color: COLOR;\n"
        "};\n"
        "struct vs_out {\n"
        "  float4 color: COLOR0;\n"
        "  float4 pos: SV_Position;\n"
        "};\n"
        "vs_out main(vs_in inp) {\n"
        "  vs_out outp;\n"
        "  outp.pos = inp.pos;\n"
        "  outp.color = inp.color;\n"
        "  return outp;\n"
        "}\n";

    shdDesc.fs.source =
        "float4 main(float4 color: COLOR0): SV_Target0 {\n"
        "  return color;\n"
        "}\n";
    auto shader = harmony::createSGShader(shdDesc);
    shader->initialize();


    harmony::Pipeline::Description pipDesc;
    pipDesc.shader = shader;
    pipDesc.layout.attributes[0].format = harmony::Pipeline::VertexFormat::eFLOAT3;
    pipDesc.layout.attributes[1].format = harmony::Pipeline::VertexFormat::eFLOAT4;
    pipDesc.layout.attributesCount = 2;
    auto pip = harmony::createSGPipeline(pipDesc);
    pip->initialize();

    harmony::RenderPass::Description passDesc;
    passDesc.action.colors[0].clearValue[0] = 1.0f;
    passDesc.action.colors[0].clearValue[1] = 1.0f;
    passDesc.action.colors[0].clearValue[2] = 1.0f;
    passDesc.action.colors[0].clearValue[3] = 1.0f;
    auto pass = harmony::createSGRenderPass(passDesc);
    pass->initialize();


    renderer->frame = [&]() {
        pass->begin();
        pip->bind();
        drawable->bind();
        drawable->draw();
        pass->end();
        };
    renderer->render();
}
