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
#include <memory>
#include "Resources/ResourceManager.h"

using namespace nu;

static const char* kModelPath = "models/spot.obj";
static const char* kTexturePath = "textures/spot_diffuse.png";

static constexpr int MAX_LIGHTS = 4;

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

    float outerCutoff;   
    float range;         
    float padding0;
    float padding1;
};
static_assert(sizeof(Light) == 64, "Light must match the Light struct in lighting.hlsli");

struct LightBlock
{
    glm::vec3 cameraPosition;
    float shininess;

    glm::vec3 ambient;
    int32_t lightCount;      

    Light lights[MAX_LIGHTS];
};
static_assert(offsetof(LightBlock, ambient) == 16 && offsetof(LightBlock, lights) == 32, "LightBlock members must line up with the hlsl packing");
static_assert(sizeof(LightBlock) == 32 + 64 * MAX_LIGHTS, "LightBlock must match the LightBlock struct in lighting.hlsli");

struct GouraudVertexUniform
{
    glm::mat4 model;
    glm::mat4 viewProjection;
    LightBlock lighting;
};
static_assert(offsetof(GouraudVertexUniform, lighting) == 128, "GouraudVertexUniform must match the cbuffer in gouraud.vert.hlsl");

struct PhongVertexUniform
{
    glm::mat4 model;
    glm::mat4 viewProjection;
};
static_assert(sizeof(PhongVertexUniform) == 128, "PhongVertexUniform must match the cbuffer in phong.vert.hlsl");

static Light MakeDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
{
    Light light{};
    light.type = LightDirectional;
    light.direction = glm::normalize(direction);   
    light.color = color;
    light.intensity = intensity;
    return light;
}

static Light MakePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float range)
{
    Light light{};
    light.type = LightPoint;
    light.position = position;
    light.color = color;
    light.intensity = intensity;
    light.range = range;
    return light;
}

static Light MakeSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float intensity,
    float innerDegrees, float outerDegrees, float range)
{
    Light light{};
    light.type = LightSpot;
    light.position = position;
    light.direction = glm::normalize(direction);   
    light.color = color;
    light.intensity = intensity;
    light.innerCutoff = std::cos(glm::radians(innerDegrees));   
    light.outerCutoff = std::cos(glm::radians(outerDegrees));
    light.range = range;
    return light;
}

enum class Shading { Gouraud, Phong };

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

        auto gouraudVertex = Resources().Get<nu::Shader>("shaders/gouraud.vert", renderer, 1);
        auto gouraudFragment = Resources().Get<nu::Shader>("shaders/gouraud.frag", renderer, 0, 1);

        auto phongVertex = Resources().Get<nu::Shader>("shaders/phong.vert", renderer, 1);
        auto phongFragment = Resources().Get<nu::Shader>("shaders/phong.frag", renderer, 1, 1);

        if (!model || !texture || !gouraudVertex || !gouraudFragment || !phongVertex || !phongFragment)
        {
            std::cerr << "Could not load the model, texture or shaders" << std::endl;
            return 1;
        }

        auto createPipeline = [&](const nu::Shader& vertexShader, const nu::Shader& fragmentShader) -> std::shared_ptr<Pipeline>
            {
                auto pipeline = std::make_shared<Pipeline>();
                pipeline->AddVertexBuffer(sizeof(Model::Vertex));
                pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
                pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
                pipeline->AddVertexAttribute(2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, normal));
                if (!pipeline->Create(vertexShader, fragmentShader, renderer.GetGPUDevice(), renderer.GetWindow()))
                {
                    return nullptr;
                }
                return pipeline;
            };

        auto gouraudPipeline = createPipeline(*gouraudVertex, *gouraudFragment);
        auto phongPipeline = createPipeline(*phongVertex, *phongFragment);
        if (!gouraudPipeline || !phongPipeline)
        {
            std::cerr << "Could not create the pipelines" << std::endl;
            return 1;
        }

        glm::vec3 cameraPosition{ 0.0f, 0.5f, 3.0f };
        float aspect = static_cast<float>(renderer.GetWidth()) / static_cast<float>(renderer.GetHeight());
        glm::mat4 view = glm::lookAt(cameraPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspectiveRH_ZO(glm::radians(60.0f), aspect, 0.1f, 100.0f);

        // settings
        Shading shading = Shading::Gouraud;
        float shininess = 64.0f;
        bool lightEnabled[3] = { true, true, true };

        float innerDegrees = 12.0f;
        float outerDegrees = 22.0f;

        std::cout << "Controls:\n"
            << "  G / P = Gouraud / Phong shading\n"
            << "  1 / 2 / 3 = turn the directional / point / spot light on or off\n"
            << "  Up / Down = shininess\n"
            << "  I / K = spot inner angle +/-   O / L = spot outer angle +/-\n"
            << "  Space = pause" << std::endl;
        std::cout << "Shading: Gouraud" << std::endl;

        bool animate = true;
        float animTime = 0.0f;

        GouraudVertexUniform gouraudUniform{};
        PhongVertexUniform phongUniform{};

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

            if (input.GetKeyPressed(SDL_SCANCODE_G)) { shading = Shading::Gouraud; std::cout << "Shading: Gouraud" << std::endl; }
            if (input.GetKeyPressed(SDL_SCANCODE_P)) { shading = Shading::Phong; std::cout << "Shading: Phong" << std::endl; }

            const char* lightNames[3] = { "Directional", "Point", "Spot" };
            const SDL_Scancode lightKeys[3] = { SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3 };
            for (int i = 0; i < 3; i++)
            {
                if (input.GetKeyPressed(lightKeys[i]))
                {
                    lightEnabled[i] = !lightEnabled[i];
                    std::cout << lightNames[i] << " light: " << (lightEnabled[i] ? "on" : "off") << std::endl;
                }
            }

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

            glm::vec3 pointPosition{ std::cos(animTime * 0.8f) * 2.2f, 0.5f, std::sin(animTime * 0.8f) * 2.2f };
            glm::vec3 spotPosition{ -2.0f, 1.5f, 2.0f };

            Light allLights[3] = {
                MakeDirectionalLight(glm::vec3(-1.0f, -0.6f, -0.5f), glm::vec3(1.0f, 0.95f, 0.85f), 0.6f),
                MakePointLight(pointPosition, glm::vec3(1.0f, 0.1f, 0.1f), 1.5f, 10.0f),
                MakeSpotLight(spotPosition, -spotPosition, glm::vec3(0.2f, 0.3f, 1.0f), 2.0f, innerDegrees, outerDegrees, 10.0f)
            };

            LightBlock lighting{};
            lighting.cameraPosition = cameraPosition;
            lighting.shininess = shininess;
            lighting.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
            lighting.lightCount = 0;
            for (int i = 0; i < 3; i++)
            {
                if (lightEnabled[i]) lighting.lights[lighting.lightCount++] = allLights[i];
            }

            glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(animTime * 15.0f), glm::vec3(0, 1, 0));
            glm::mat4 viewProjection = projection * view;

            // RENDER
            renderer.BeginFrame();

            if (shading == Shading::Gouraud)
            {
                gouraudUniform.model = modelMatrix;
                gouraudUniform.viewProjection = viewProjection;
                gouraudUniform.lighting = lighting;

                renderer.SetPipeline(*gouraudPipeline);
                renderer.SetVertexUniform(0, &gouraudUniform, sizeof(gouraudUniform));
            }
            else
            {
                phongUniform.model = modelMatrix;
                phongUniform.viewProjection = viewProjection;

                renderer.SetPipeline(*phongPipeline);
                renderer.SetVertexUniform(0, &phongUniform, sizeof(phongUniform));
                renderer.SetFragmentUniform(0, &lighting, sizeof(lighting));
            }

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