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

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include "Resources/ResourceManager.h"

using namespace nu;

static const char* kModelPath = "models/sphere.obj";
static const char* kTexturePath = "textures/spot_diffuse.png";

struct VertexUniform
{
    glm::mat4 model;
    glm::mat4 viewProjection;
};
static_assert(sizeof(VertexUniform) == 128, "VertexUniform must match the cbuffer in phong.vert.hlsl");

struct FragmentUniform
{
    glm::vec4 cameraPosition;   
    glm::vec4 lightPosition;    
    glm::vec4 lightColor;       
    glm::vec4 ambientColor;     
    glm::vec4 material;         
};
static_assert(sizeof(FragmentUniform) == 80, "FragmentUniform must match the cbuffer in phong.frag.hlsl");

int main()
{
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
    Input& input = Engine::Instance().GetInput();

    {
        auto model = Resources().Get<Model>(kModelPath, renderer);
        auto texture = Resources().Get<Texture>(kTexturePath, renderer);

        auto vshader = Resources().Get<nu::Shader>("shaders/phong.vert", renderer, 1);
        auto fshader = Resources().Get<nu::Shader>("shaders/phong.frag", renderer, 1, 1);

        if (!model || !texture || !vshader || !fshader)
        {
            std::cerr << "Could not load the model, texture or shaders" << std::endl;
            return 1;
        }

        auto pipeline = std::make_shared<Pipeline>();
        pipeline->AddVertexBuffer(sizeof(Model::Vertex));
        pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
        pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
        pipeline->AddVertexAttribute(2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, normal));
        if (!pipeline->Create(*vshader.get(), *fshader.get(), renderer.GetGPUDevice(), renderer.GetWindow()))
        {
            std::cerr << "Could not create pipeline" << std::endl;
            return 1;
        }

        glm::vec3 cameraPosition{ 0.0f, 0.5f, 3.0f };
        float aspect = static_cast<float>(renderer.GetWidth()) / static_cast<float>(renderer.GetHeight());
        glm::mat4 view = glm::lookAt(cameraPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspectiveRH_ZO(glm::radians(60.0f), aspect, 0.1f, 100.0f);

        glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };
        float lightIntensity = 1.0f;
        glm::vec3 ambientColor{ 0.15f, 0.15f, 0.15f };
        float shininess = 64.0f;
        bool blinnPhong = true;

        bool animate = true;
        float animTime = 0.0f;
        std::cout << "Controls: Up/Down = shininess, B = Blinn-Phong / Phong, Space = pause" << std::endl;
        std::cout << "Specular: Blinn-Phong, Shininess: " << shininess << std::endl;

        VertexUniform vertexUniform{};
        FragmentUniform fragmentUniform{};

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

            if (input.GetKeyPressed(SDL_SCANCODE_UP))
            {
                shininess = std::min(shininess * 2.0f, 1024.0f);
                std::cout << "Shininess: " << shininess << std::endl;
            }
            if (input.GetKeyPressed(SDL_SCANCODE_DOWN))
            {
                shininess = std::max(shininess * 0.5f, 2.0f);
                std::cout << "Shininess: " << shininess << std::endl;
            }
            if (input.GetKeyPressed(SDL_SCANCODE_B))
            {
                blinnPhong = !blinnPhong;
                std::cout << "Specular: " << (blinnPhong ? "Blinn-Phong" : "Phong") << std::endl;
            }
            if (input.GetKeyPressed(SDL_SCANCODE_SPACE)) animate = !animate;

            if (animate) animTime += Engine::Instance().GetTime().GetDeltaTime();

            glm::vec3 lightPosition{ std::cos(animTime) * 2.5f, 1.5f, std::sin(animTime) * 2.5f };

            glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(animTime * 15.0f), glm::vec3(0, 1, 0));

            vertexUniform.model = modelMatrix;
            vertexUniform.viewProjection = projection * view;

            fragmentUniform.cameraPosition = glm::vec4(cameraPosition, 1.0f);
            fragmentUniform.lightPosition = glm::vec4(lightPosition, 1.0f);
            fragmentUniform.lightColor = glm::vec4(lightColor, lightIntensity);
            fragmentUniform.ambientColor = glm::vec4(ambientColor, 1.0f);
            fragmentUniform.material = glm::vec4(shininess, blinnPhong ? 1.0f : 0.0f, 0.0f, 0.0f);

            // RENDER
            renderer.BeginFrame();

            renderer.SetPipeline(*pipeline);
            renderer.SetVertexUniform(0, &vertexUniform, sizeof(vertexUniform));
            renderer.SetFragmentUniform(0, &fragmentUniform, sizeof(fragmentUniform));
            renderer.SetFragmentTexture(0, *texture);

            model->Draw(renderer);

            renderer.EndFrame();
        }

        
        SDL_WaitForGPUIdle(renderer.GetGPUDevice());
    }

    
    Resources().Clear();

    // SHUTDOWN
    Engine::Instance().Shutdown();

    return 0;
}