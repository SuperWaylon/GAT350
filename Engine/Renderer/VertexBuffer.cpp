#include "pch.h"
#include "VertexBuffer.h"

namespace nu
{
	VertexBuffer::~VertexBuffer()
	{
		// if both the gpu buffer and the gpu device are valid (not null),
		// release the buffer with SDL_ReleaseGPUBuffer(device, buffer)
	}

	bool VertexBuffer::Create(uint32_t vertexCount, uint32_t vertexSize, const uint8_t* data, SDL_GPUDevice* gpuDevice)
	{
		// store the vertex count in m_vertexCount (needed later when drawing)
		// store the gpu device in m_gpuDevice so the destructor can release the buffer later

		// calculate the total size in bytes of all the vertices (number of vertices * size of one vertex)
		uint32_t verticesSize = 0;

		// --- create the vertex buffer ---
		// this buffer lives in gpu memory, the cpu can't write to it directly

		// fill out the vertex buffer settings
		SDL_GPUBufferCreateInfo vertexBufferCreateInfo{
			.usage = {}, // todo: this buffer holds vertex data (SDL_GPU_BUFFERUSAGE_...)
			.size = {} // todo: the total size in bytes of the vertices
		};

		// create the buffer with SDL_CreateGPUBuffer(device, &createInfo) and store it in m_gpuBuffer
		if (m_gpuBuffer == nullptr)
		{
			std::cerr << "Could not create vertex buffer: " << SDL_GetError() << std::endl;

			return false;
		}

		// --- create the transfer buffer ---
		// a transfer buffer is a staging area the cpu can write to, it is used to copy data into the vertex buffer

		// fill out the transfer buffer settings
		SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo{
			.usage = {}, // todo: this buffer uploads data from the cpu to the gpu (SDL_GPU_TRANSFERBUFFERUSAGE_...)
			.size = {} // todo: the total size in bytes of the vertices
		};

		// create the transfer buffer with SDL_CreateGPUTransferBuffer(device, &createInfo)
		SDL_GPUTransferBuffer* transferBuffer = nullptr;
		if (transferBuffer == nullptr)
		{
			std::cerr << "Could not create transfer buffer: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUBuffer(gpuDevice, m_gpuBuffer);
			m_gpuBuffer = nullptr;

			return false;
		}

		// --- copy the vertex data into the transfer buffer ---

		// map the transfer buffer with SDL_MapGPUTransferBuffer(device, transferBuffer, false)
		// mapping gives the cpu a pointer it can write to
		// the function returns void*, so static_cast it to uint8_t*
		uint8_t* transferData = nullptr;
		if (transferData == nullptr)
		{
			std::cerr << "Could not map transfer buffer: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUTransferBuffer(gpuDevice, transferBuffer);
			SDL_ReleaseGPUBuffer(gpuDevice, m_gpuBuffer);
			m_gpuBuffer = nullptr;

			return false;
		}

		// copy the vertex data into transferData with memcpy(destination, source, size)

		// unmap the transfer buffer with SDL_UnmapGPUTransferBuffer(device, transferBuffer)
		// the cpu is done writing, so the gpu can now use it

		// --- upload the transfer buffer to the vertex buffer ---
		// the gpu does the copy, so it has to be recorded in a command buffer and submitted

		// get a command buffer with SDL_AcquireGPUCommandBuffer(device)
		SDL_GPUCommandBuffer* commandBuffer = nullptr;
		if (commandBuffer == nullptr)
		{
			std::cerr << "Could not acquire GPU command buffer: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUTransferBuffer(gpuDevice, transferBuffer);
			SDL_ReleaseGPUBuffer(gpuDevice, m_gpuBuffer);
			m_gpuBuffer = nullptr;

			return false;
		}

		// start a copy pass with SDL_BeginGPUCopyPass(commandBuffer)
		// copy commands must be recorded inside a copy pass
		SDL_GPUCopyPass* copyPass = nullptr;

		// where to copy from: the start of the transfer buffer
		SDL_GPUTransferBufferLocation bufferLocation{
			.transfer_buffer = {}, // todo: the transfer buffer
			.offset = 0
		};

		// where to copy to: the vertex buffer, starting at the beginning
		SDL_GPUBufferRegion bufferRegion{
			.buffer = {}, // todo: the vertex buffer
			.offset = 0,
			.size = {} // todo: the total size in bytes of the vertices
		};

		// record the copy with SDL_UploadToGPUBuffer(copyPass, &location, &region, false)

		// end the copy pass with SDL_EndGPUCopyPass(copyPass)

		// submit the command buffer so the gpu runs the copy
		if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
		{
			std::cerr << "Could not submit GPU command buffer: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUTransferBuffer(gpuDevice, transferBuffer);
			SDL_ReleaseGPUBuffer(gpuDevice, m_gpuBuffer);
			m_gpuBuffer = nullptr;

			return false;
		}

		// the transfer buffer is no longer needed,
		// release it with SDL_ReleaseGPUTransferBuffer(device, transferBuffer)

		// vertex buffer created and uploaded successfully
		return true;
	}
}