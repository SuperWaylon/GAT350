#include "pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Core/File.h"

namespace nu
{
	Shader::~Shader()
	{
		// if both the gpu device and the gpu shader are valid (not null),
		// release the shader with SDL_ReleaseGPUShader(device, shader)
	}

	bool Shader::Load(const std::string& filename, Renderer& renderer)
	{
		// store the renderer's gpu device in m_gpuDevice so the destructor can release the shader later

		// figure out the shader stage from the filename
		// if the filename contains ".vert" set stage to SDL_GPU_SHADERSTAGE_VERTEX
		// if the filename contains ".frag" set stage to SDL_GPU_SHADERSTAGE_FRAGMENT
		// otherwise print an error with std::cerr and return false
		// hint: filename.find(".vert") != std::string::npos is true when ".vert" is in the filename
		SDL_GPUShaderStage stage{};

		// the shader format and filename depend on which graphics backend sdl is using
		// (vulkan uses spirv, direct3d 12 uses dxil)
		SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
		std::string shaderFilename = filename;

		// the name of the function the shader starts in
		const char* entrypoint = "main";

		// get the shader formats supported by the gpu device
		// the result is a set of bit flags, use & to check if a format is supported
		SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(renderer.m_gpuDevice);
		if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV)
		{
			// add ".spv" to the end of shaderFilename and set format to SDL_GPU_SHADERFORMAT_SPIRV
		}
		else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL)
		{
			// add ".dxil" to the end of shaderFilename and set format to SDL_GPU_SHADERFORMAT_DXIL
		}
		else
		{
			// no supported format, print an error with std::cerr and return false
		}

		// read the compiled shader file into a vector named 'bytes' using ReadBinaryFile()
		// if the vector is empty the read failed, print an error with std::cerr and return false
		std::vector<uint8_t> bytes;

		// fill out the shader settings
		// each field below needs a value, replace each {} using the comment beside it
		SDL_GPUShaderCreateInfo shaderInfo{
			.code_size = {}, // todo: the number of bytes in the shader code
			.code = {}, // todo: pointer to the shader code (bytes.data())
			.entrypoint = {}, // todo: the entry function name
			.format = {}, // todo: the shader format found above
			.stage = {} // todo: the shader stage found above
		};

		// create the shader with SDL_CreateGPUShader(device, &createInfo) and store it in m_gpuShader

		// if the shader is null, creation failed
		// print an error with std::cerr that includes SDL_GetError() and return false

		// shader loaded successfully
		return true;
	}
}