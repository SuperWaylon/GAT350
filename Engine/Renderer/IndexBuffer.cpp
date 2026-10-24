#include "pch.h"
#include "IndexBuffer.h"

namespace nu
{
	IndexBuffer::~IndexBuffer()
	{
		// if both the gpu buffer and the gpu device are valid (not null),
		// release the buffer with SDL_ReleaseGPUBuffer(device, buffer)
		if (m_gpuBuffer && m_gpuDevice)
		{
			SDL_ReleaseGPUBuffer(m_gpuDevice, m_gpuBuffer);
		}
	}

	bool IndexBuffer::Create(uint32_t indexCount, const uint32_t* data, SDL_GPUDevice* gpuDevice)
	{
		// store the index count in m_indexCount (needed later when drawing)
		// store the gpu device in m_gpuDevice so the destructor can release the buffer later
		m_indexCount = indexCount;
		m_gpuDevice = gpuDevice;

		// calculate the total size in bytes of all the vertices (number of vertices * size of one index)
		uint32_t indicesSize = indexCount * sizeof(uint32_t);

		// --- create the index buffer ---
		// this buffer lives in gpu memory, the cpu can't write to it directly

		// fill out the index buffer settings
		SDL_GPUBufferCreateInfo indexBufferCreateInfo{
			.usage = SDL_GPU_BUFFERUSAGE_INDEX, // todo: this buffer holds index data (SDL_GPU_BUFFERUSAGE_...)
			.size = indicesSize // todo: the total size in bytes of the vertices
		};

		// create the buffer with SDL_CreateGPUBuffer(device, &createInfo) and store it in m_gpuBuffer
		m_gpuBuffer = SDL_CreateGPUBuffer(m_gpuDevice, &indexBufferCreateInfo);
		if (m_gpuBuffer == nullptr)
		{
			std::cerr << "Could not create index buffer: " << SDL_GetError() << std::endl;

			return false;
		}

		// --- create the transfer buffer ---
		// a transfer buffer is a staging area the cpu can write to, it is used to copy data into the index buffer

		// fill out the transfer buffer settings
		SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo{
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, // todo: this buffer uploads data from the cpu to the gpu (SDL_GPU_TRANSFERBUFFERUSAGE_...)
			.size = indicesSize // todo: the total size in bytes of the vertices
		};

		// create the transfer buffer with SDL_CreateGPUTransferBuffer(device, &createInfo)
		SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(m_gpuDevice, &transferBufferCreateInfo);
		if (transferBuffer == nullptr)
		{
			std::cerr << "Could not create transfer buffer: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUBuffer(gpuDevice, m_gpuBuffer);
			m_gpuBuffer = nullptr;

			return false;
		}

		// --- copy the index data into the transfer buffer ---

		// map the transfer buffer with SDL_MapGPUTransferBuffer(device, transferBuffer, false)
		// mapping gives the cpu a pointer it can write to
		// the function returns void*, so static_cast it to uint8_t*
		uint8_t* transferData = (uint8_t*)SDL_MapGPUTransferBuffer(m_gpuDevice, transferBuffer, false);
		if (transferData == nullptr)
		{
			std::cerr << "Could not map transfer buffer: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUTransferBuffer(gpuDevice, transferBuffer);
			SDL_ReleaseGPUBuffer(gpuDevice, m_gpuBuffer);
			m_gpuBuffer = nullptr;

			return false;
		}

		// copy the index data into transferData with memcpy(destination, source, size)
		memcpy(transferData, data, indicesSize);

		// unmap the transfer buffer with SDL_UnmapGPUTransferBuffer(device, transferBuffer)
		// the cpu is done writing, so the gpu can now use it
		SDL_UnmapGPUTransferBuffer(m_gpuDevice, transferBuffer);

		// --- upload the transfer buffer to the index buffer ---
		// the gpu does the copy, so it has to be recorded in a command buffer and submitted

		// get a command buffer with SDL_AcquireGPUCommandBuffer(device)
		SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(m_gpuDevice);
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
		SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);

		// where to copy from: the start of the transfer buffer
		SDL_GPUTransferBufferLocation bufferLocation{
			.transfer_buffer = transferBuffer, // todo: the transfer buffer
			.offset = 0
		};

		// where to copy to: the index buffer, starting at the beginning
		SDL_GPUBufferRegion bufferRegion{
			.buffer = m_gpuBuffer, // todo: the index buffer
			.offset = 0,
			.size = indicesSize // todo: the total size in bytes of the vertices
		};

		// record the copy with SDL_UploadToGPUBuffer(copyPass, &location, &region, false)
		SDL_UploadToGPUBuffer(copyPass, &bufferLocation, &bufferRegion, false);

		// end the copy pass with SDL_EndGPUCopyPass(copyPass)
		SDL_EndGPUCopyPass(copyPass);

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
		SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);

		// index buffer created and uploaded successfully
		return true;
	}
}