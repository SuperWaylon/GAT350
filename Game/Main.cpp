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
#include <cstdint>
#include <iostream>
#include "Resources/ResourceManager.h"

using namespace nu;

static const char* kModelPath = "models/spot.obj";
static const char* kTexturePath = "textures/spot_diffuse.png";

static const float kOrbitRadius = 2.5f;
static const float kOrbitHeight = 1.5f;

enum LightType : int32_t
{
    LightDirectional = 0,
    LightPoint = 1,
    LightSpot = 2
};

struct Light
{
    glm::vec3 position;
    int32_t type;

    glm::vec3 direction;
    float intensity;

    glm::vec3 color;
    float innerCutoff;   

    glm::vec3 ambient;
    float outerCutoff;  
};
static_assert(sizeof(Light) == 64, "Light must match the Light struct in lighting.hlsli");
static_assert(offsetof(Light, type) == 12 && offsetof(Light, direction) == 16 && offsetof(Light, color) == 32 && offsetof(Light, ambient) == 48,
    "Light members must line up with the hlsl packing");

struct VertexUniform
{
    glm::mat4 model;
    glm::mat4 viewProjection;
    glm::vec3 cameraPosition;
    float shininess;
    Light light;
};
static_assert(sizeof(VertexUniform) == 208, "VertexUniform must match the cbuffer in gouraud.vert.hlsl");

static const char* LightTypeName(int32_t type)
{
    switch (type)
    {
    case LightDirectional: return "Directional";
    case LightPoint: return "Point";
    default: return "Spot";
    }
}

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

        auto vshader = Resources().Get<nu::Shader>("shaders/gouraud.vert", renderer, 1);
        auto fshader = Resources().Get<nu::Shader>("shaders/gouraud.frag", renderer, 0, 1);

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

        int32_t lightType = LightPoint;
        float shininess = 64.0f;

        float innerDegrees = 10.0f;
        float outerDegrees = 18.0f;

        // controls
        std::cout << "Controls:\n"
            << "  1 / 2 / 3 = directional / point / spot light\n"
            << "  Up / Down = shininess\n"
            << "  I / K = inner cone angle +/-   O / L = outer cone angle +/-\n"
            << "  Space = pause" << std::endl;
        std::cout << "Light: " << LightTypeName(lightType) << std::endl;

        bool animate = true;
        float animTime = 0.0f;

        VertexUniform uniform{};

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

            if (input.GetKeyPressed(SDL_SCANCODE_1)) { lightType = LightDirectional; std::cout << "Light: Directional" << std::endl; }
            if (input.GetKeyPressed(SDL_SCANCODE_2)) { lightType = LightPoint; std::cout << "Light: Point" << std::endl; }
            if (input.GetKeyPressed(SDL_SCANCODE_3)) { lightType = LightSpot; std::cout << "Light: Spot" << std::endl; }

            if (input.GetKeyPressed(SDL_SCANCODE_UP)) { shininess = std::min(shininess * 2.0f, 1024.0f); std::cout << "Shininess: " << shininess << std::endl; }
            if (input.GetKeyPressed(SDL_SCANCODE_DOWN)) { shininess = std::max(shininess * 0.5f, 2.0f); std::cout << "Shininess: " << shininess << std::endl; }

            bool coneChanged = false;
            if (input.GetKeyPressed(SDL_SCANCODE_I)) { innerDegrees += 2.0f; coneChanged = true; }
            if (input.GetKeyPressed(SDL_SCANCODE_K)) { innerDegrees -= 2.0f; coneChanged = true; }
            if (input.GetKeyPressed(SDL_SCANCODE_O)) { outerDegrees += 2.0f; coneChanged = true; }
            if (input.GetKeyPressed(SDL_SCANCODE_L)) { outerDegrees -= 2.0f; coneChanged = true; }
            if (coneChanged)
            {
                outerDegrees = std::clamp(outerDegrees, 2.0f, 80.0f);
                innerDegrees = std::clamp(innerDegrees, 1.0f, outerDegrees - 1.0f);
                std::cout << "Spot cone: inner " << innerDegrees << ", outer " << outerDegrees << " degrees" << std::endl;
            }

            if (input.GetKeyPressed(SDL_SCANCODE_SPACE)) animate = !animate;
            if (animate) animTime += Engine::Instance().GetTime().GetDeltaTime();

            glm::vec3 orbit{ std::cos(animTime) * kOrbitRadius, kOrbitHeight, std::sin(animTime) * kOrbitRadius };

            Light light{};
            light.type = lightType;
            light.position = orbit;
            light.direction = glm::normalize(-orbit);
            light.color = glm::vec3(1.0f, 1.0f, 1.0f);
            light.intensity = 1.0f;
            light.ambient = glm::vec3(0.15f, 0.15f, 0.15f);
            light.innerCutoff = std::cos(glm::radians(innerDegrees));
            light.outerCutoff = std::cos(glm::radians(outerDegrees));

            glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(animTime * 15.0f), glm::vec3(0, 1, 0));

            uniform.model = modelMatrix;
            uniform.viewProjection = projection * view;
            uniform.cameraPosition = cameraPosition;
            uniform.shininess = shininess;
            uniform.light = light;

            // RENDER
            renderer.BeginFrame();

            renderer.SetPipeline(*pipeline);
            renderer.SetVertexUniform(0, &uniform, sizeof(uniform));
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