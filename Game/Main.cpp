#include "Engine.h"
#include "Core/File.h"
#include "Renderer/Shader.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Model.h"
#include "Renderer/Texture.h"
#include "Math/Transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <cmath>
#include <cstddef>
#include <iostream>
#include "Resources/ResourceManager.h"

using namespace nu;

// matches the cbuffer in unlit.vert.hlsl (register b0, space1)
struct VertexUniform
{
    glm::mat4 mvp;
};

int main()
{
    // the working directory must be ...\GAT350\Build (set in Game > Properties > Debugging)
    // so "Assets" resolves to Build\Assets, where the compiled shaders, models and textures are
    std::cout << "CWD before: " << GetWorkingDirectory() << std::endl;
    if (!SetWorkingDirectory("Assets"))
    {
        std::cerr << "Could not find Assets folder. Current directory: " << GetWorkingDirectory() << std::endl;
        return 1;
    }

    // INITIALIZATION
    if (!Engine::Instance().Initialize())
    {
        std::cerr << "Engine failed to initialize" << std::endl;
        return 1;
    }
    Renderer& renderer = Engine::Instance().GetRenderer();

    // scope so the local pointers are released before Shutdown
    {
        // load the model, texture and shaders with the resource manager
        // the vertex shader has 1 uniform buffer (mvp), the fragment shader has 1 uniform buffer (color) and 1 sampler (texture)
        auto model = Resources().Get<Model>("models/spot.obj", renderer);
        auto texture = Resources().Get<Texture>("textures/spot_diffuse.png", renderer);

        auto vshader = Resources().Get<nu::Shader>("shaders/unlit.vert", renderer, 1);
        auto fshader = Resources().Get<nu::Shader>("shaders/unlit.frag", renderer, 1, 1);

        if (!model || !texture || !vshader || !fshader)
        {
            std::cerr << "Could not load the model, texture or shaders" << std::endl;
            return 1;
        }

        // vertex layout comes straight from Model::Vertex (position, texcoord, normal, tangent)
        // locations 0 and 1 match TEXCOORD0 and TEXCOORD1 in unlit.vert.hlsl
        // normal and tangent are in the buffer but the shader doesn't read them
        auto pipeline = std::make_shared<Pipeline>();
        pipeline->AddVertexBuffer(sizeof(Model::Vertex));
        pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
        pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
        if (!pipeline->Create(*vshader.get(), *fshader.get(), renderer.GetGPUDevice(), renderer.GetWindow()))
        {
            std::cerr << "Could not create pipeline" << std::endl;
            return 1;
        }

        // camera: sits on +z looking at the origin
        // perspectiveRH_ZO builds a projection with depth 0..1, which is what SDL_GPU (d3d12/vulkan) expects
        float aspect = static_cast<float>(renderer.GetWidth()) / static_cast<float>(renderer.GetHeight());
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspectiveRH_ZO(glm::radians(60.0f), aspect, 0.1f, 100.0f);

        // fragment uniform (the shader declares it but doesn't use it yet)
        glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };

        VertexUniform vertexUniform{};

        // MAIN LOOP
        bool quit = false;
        while (!quit)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT) quit = true;
                if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) quit = true;
            }

            // ENGINE
            Engine::Instance().Update();
            float seconds = Engine::Instance().GetTime().GetTime();

            // MVP = projection * view * model (rotate around y, 45 degrees per second)
            glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(seconds * 45.0f), glm::vec3(0, 1, 0));
            vertexUniform.mvp = projection * view * modelMatrix;

            // RENDER
            renderer.BeginFrame();

            renderer.SetPipeline(*pipeline);
            renderer.SetVertexUniform(0, &vertexUniform, sizeof(vertexUniform));
            renderer.SetFragmentUniform(0, &color, sizeof(color));
            renderer.SetFragmentTexture(0, *texture);

            model->Draw(renderer);

            renderer.EndFrame();
        }

        // make sure the gpu is done with the buffers, pipeline and shaders before they are released
        SDL_WaitForGPUIdle(renderer.GetGPUDevice());
    }

    // the resource manager still holds the model, texture and shaders, release them while the gpu device is still valid
    // (requires the Clear() function added to ResourceManager.h)
    Resources().Clear();

    // SHUTDOWN
    Engine::Instance().Shutdown();

    return 0;
}