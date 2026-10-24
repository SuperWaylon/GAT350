#include "pch.h"
#include "Texture.h"
#include "Renderer.h"
#include <SDL3_image/SDL_image.h>

namespace nu
{
	Texture::~Texture()
	{
		// if the gpu device is valid (not null), release the sampler and texture
		// with SDL_ReleaseGPUSampler(device, sampler) and SDL_ReleaseGPUTexture(device, texture)
		if (m_gpuDevice)
		{
			if (m_gpuSampler)
			{
				SDL_ReleaseGPUSampler(m_gpuDevice, m_gpuSampler);
			}

			if (m_gpuTexture)
			{
				SDL_ReleaseGPUTexture(m_gpuDevice, m_gpuTexture);
			}
		}
	}

	bool Texture::Load(const std::string& filename, const Renderer& renderer)
	{
		// store the gpu device in m_gpuDevice so the destructor can release the texture later
		m_gpuDevice = renderer.GetGPUDevice();

		// --- load the image into a surface ---
		// a surface is image data in cpu memory

		// load the image file with IMG_Load(filename)
		SDL_Surface* loadedSurface = IMG_Load(filename.c_str());
		if (loadedSurface == nullptr)
		{
			std::cerr << "Could not load image " << filename << ": " << SDL_GetError() << std::endl;

			return false;
		}

		// images can be stored in many pixel formats, convert to rgba (4 bytes per pixel)
		// so it matches the gpu texture format below
		// SDL_PIXELFORMAT_ABGR8888 stores the bytes in r, g, b, a order on little endian machines
		SDL_Surface* surface = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_ABGR8888);
		SDL_DestroySurface(loadedSurface);
		if (surface == nullptr)
		{
			std::cerr << "Could not convert image " << filename << ": " << SDL_GetError() << std::endl;

			return false;
		}

		// store the size, needed when creating and uploading the texture
		m_width = static_cast<uint32_t>(surface->w);
		m_height = static_cast<uint32_t>(surface->h);

		// calculate the total size in bytes of the image (bytes per row * number of rows)
		// pitch is the number of bytes in one row, it can be larger than width * 4 if rows are padded
		uint32_t imageSize = static_cast<uint32_t>(surface->pitch) * m_height;

		// --- create the texture ---
		// this texture lives in gpu memory, the cpu can't write to it directly

		// fill out the texture settings
		SDL_GPUTextureCreateInfo textureCreateInfo{
			.type = SDL_GPU_TEXTURETYPE_2D, // a regular 2d image (SDL_GPU_TEXTURETYPE_...)
			.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, // 4 bytes per pixel, r g b a (SDL_GPU_TEXTUREFORMAT_...)
			.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER, // the texture will be sampled in a shader (SDL_GPU_TEXTUREUSAGE_...)
			.width = m_width,
			.height = m_height,
			.layer_count_or_depth = 1,
			.num_levels = 1
		};

		// create the texture with SDL_CreateGPUTexture(device, &createInfo) and store it in m_gpuTexture
		m_gpuTexture = SDL_CreateGPUTexture(m_gpuDevice, &textureCreateInfo);
		if (m_gpuTexture == nullptr)
		{
			std::cerr << "Could not create texture: " << SDL_GetError() << std::endl;

			SDL_DestroySurface(surface);

			return false;
		}

		// --- create the transfer buffer ---
		// a transfer buffer is a staging area the cpu can write to, it is used to copy data into the texture

		// fill out the transfer buffer settings
		SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo{
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, // this buffer uploads data from the cpu to the gpu (SDL_GPU_TRANSFERBUFFERUSAGE_...)
			.size = imageSize // the total size in bytes of the image
		};

		// create the transfer buffer with SDL_CreateGPUTransferBuffer(device, &createInfo)
		SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(m_gpuDevice, &transferBufferCreateInfo);
		if (transferBuffer == nullptr)
		{
			std::cerr << "Could not create transfer buffer: " << SDL_GetError() << std::endl;

			SDL_DestroySurface(surface);
			SDL_ReleaseGPUTexture(m_gpuDevice, m_gpuTexture);
			m_gpuTexture = nullptr;

			return false;
		}

		// --- copy the pixel data into the transfer buffer ---

		// map the transfer buffer with SDL_MapGPUTransferBuffer(device, transferBuffer, false)
		// mapping gives the cpu a pointer it can write to
		// the function returns void*, so static_cast it to uint8_t*
		uint8_t* transferData = static_cast<uint8_t*>(SDL_MapGPUTransferBuffer(m_gpuDevice, transferBuffer, false));
		if (transferData == nullptr)
		{
			std::cerr << "Could not map transfer buffer: " << SDL_GetError() << std::endl;

			SDL_DestroySurface(surface);
			SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);
			SDL_ReleaseGPUTexture(m_gpuDevice, m_gpuTexture);
			m_gpuTexture = nullptr;

			return false;
		}

		// copy the pixel data into transferData with memcpy(destination, source, size)
		memcpy(transferData, surface->pixels, imageSize);

		// unmap the transfer buffer with SDL_UnmapGPUTransferBuffer(device, transferBuffer)
		// the cpu is done writing, so the gpu can now use it
		SDL_UnmapGPUTransferBuffer(m_gpuDevice, transferBuffer);

		// the pixels are in the transfer buffer now, so the surface is no longer needed
		uint32_t pixelsPerRow = static_cast<uint32_t>(surface->pitch) / 4;
		SDL_DestroySurface(surface);

		// --- upload the transfer buffer to the texture ---
		// the gpu does the copy, so it has to be recorded in a command buffer and submitted

		// get a command buffer with SDL_AcquireGPUCommandBuffer(device)
		SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(m_gpuDevice);
		if (commandBuffer == nullptr)
		{
			std::cerr << "Could not acquire GPU command buffer: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);
			SDL_ReleaseGPUTexture(m_gpuDevice, m_gpuTexture);
			m_gpuTexture = nullptr;

			return false;
		}

		// start a copy pass with SDL_BeginGPUCopyPass(commandBuffer)
		// copy commands must be recorded inside a copy pass
		SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);

		// where to copy from: the start of the transfer buffer
		// pixels_per_row tells the gpu how far apart the rows are (includes any row padding)
		SDL_GPUTextureTransferInfo transferInfo{
			.transfer_buffer = transferBuffer, // the transfer buffer
			.offset = 0,
			.pixels_per_row = pixelsPerRow,
			.rows_per_layer = m_height
		};

		// where to copy to: the whole texture, starting at the top-left corner
		SDL_GPUTextureRegion textureRegion{
			.texture = m_gpuTexture, // the texture
			.mip_level = 0,
			.layer = 0,
			.x = 0,
			.y = 0,
			.z = 0,
			.w = m_width, // the width of the image
			.h = m_height, // the height of the image
			.d = 1
		};

		// record the copy with SDL_UploadToGPUTexture(copyPass, &transferInfo, &region, false)
		SDL_UploadToGPUTexture(copyPass, &transferInfo, &textureRegion, false);

		// end the copy pass with SDL_EndGPUCopyPass(copyPass)
		SDL_EndGPUCopyPass(copyPass);

		// submit the command buffer so the gpu runs the copy
		if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
		{
			std::cerr << "Could not submit GPU command buffer: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);
			SDL_ReleaseGPUTexture(m_gpuDevice, m_gpuTexture);
			m_gpuTexture = nullptr;

			return false;
		}

		// the transfer buffer is no longer needed,
		// release it with SDL_ReleaseGPUTransferBuffer(device, transferBuffer)
		SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);

		// --- create the sampler ---
		// a sampler tells the gpu how to read the texture (filtering and what happens outside 0-1 uvs)

		// fill out the sampler settings
		SDL_GPUSamplerCreateInfo samplerCreateInfo{
			.min_filter = SDL_GPU_FILTER_LINEAR, // smooth when the texture is shrunk (SDL_GPU_FILTER_...)
			.mag_filter = SDL_GPU_FILTER_LINEAR, // smooth when the texture is enlarged (SDL_GPU_FILTER_...)
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT, // tile the texture horizontally (SDL_GPU_SAMPLERADDRESSMODE_...)
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT, // tile the texture vertically (SDL_GPU_SAMPLERADDRESSMODE_...)
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT
		};

		// create the sampler with SDL_CreateGPUSampler(device, &createInfo) and store it in m_gpuSampler
		m_gpuSampler = SDL_CreateGPUSampler(m_gpuDevice, &samplerCreateInfo);
		if (m_gpuSampler == nullptr)
		{
			std::cerr << "Could not create sampler: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUTexture(m_gpuDevice, m_gpuTexture);
			m_gpuTexture = nullptr;

			return false;
		}

		// texture created and uploaded successfully
		return true;
	}
}