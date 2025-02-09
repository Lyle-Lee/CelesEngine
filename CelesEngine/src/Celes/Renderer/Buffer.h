#pragma once

#include "Texture.h"

namespace Celes {

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case Celes::ShaderDataType::Float:
				return 4;
			case Celes::ShaderDataType::Float2:
				return 8;
			case Celes::ShaderDataType::Float3:
				return 12;
			case Celes::ShaderDataType::Float4:
				return 16;
			case Celes::ShaderDataType::Mat3:
				return 36;
			case Celes::ShaderDataType::Mat4:
				return 64;
			case Celes::ShaderDataType::Int:
				return 4;
			case Celes::ShaderDataType::Int2:
				return 8;
			case Celes::ShaderDataType::Int3:
				return 12;
			case Celes::ShaderDataType::Int4:
				return 16;
			case Celes::ShaderDataType::Bool:
				return 1;
			default:
				break;
		}

		CE_CORE_ASSERT(false, "Unknown data type!")
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		//BufferElement() {}
		BufferElement(const std::string& name, ShaderDataType type, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case Celes::ShaderDataType::Float:
					return 1;
				case Celes::ShaderDataType::Float2:
					return 2;
				case Celes::ShaderDataType::Float3:
					return 3;
				case Celes::ShaderDataType::Float4:
					return 4;
				case Celes::ShaderDataType::Mat3:
					return 9;
				case Celes::ShaderDataType::Mat4:
					return 16;
				case Celes::ShaderDataType::Int:
					return 1;
				case Celes::ShaderDataType::Int2:
					return 2;
				case Celes::ShaderDataType::Int3:
					return 3;
				case Celes::ShaderDataType::Int4:
					return 4;
				case Celes::ShaderDataType::Bool:
					return 1;
				default:
					break;
			}
			
			CE_CORE_ASSERT(false, "Unknown data type!")
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout(): m_Stride(0) {}
		BufferLayout(const std::initializer_list<BufferElement>& elements): m_Elements(elements), m_Stride(0) { Fit(); }

		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		inline uint32_t GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void Fit()
		{
			uint32_t offset = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride;
	};

	class CE_API VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class CE_API IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t size);
	};

	struct FrameBufferTextureDescription
	{
		FrameBufferTextureDescription() = default;
		FrameBufferTextureDescription(TextureFormat format, uint32_t samples = 1)
			: TexFormat(format), Samples(samples) {}

		TextureFormat TexFormat = TextureFormat::None;
		uint32_t Samples = 1;
		// TODO: filtering/warp
	};

	struct FrameBufferAttachmentDescription
	{
		FrameBufferAttachmentDescription() = default;
		FrameBufferAttachmentDescription(std::initializer_list<FrameBufferTextureDescription> attachments)
			: Attachments(attachments) {}

		std::vector<FrameBufferTextureDescription> Attachments;
	};

	struct FrameBufferDescription
	{
		uint32_t Width = 0, Height = 0;
		FrameBufferAttachmentDescription AttachmentDesc;
		//uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class CE_API FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void SetRenderBuffer() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void AddAttachment(const FrameBufferTextureDescription& texDesc) = 0;
		virtual uint32_t GetAttachmentBufferID(size_t idx = 0) = 0;
		virtual Ref<Texture2D>& GetAttachmentTexture(size_t idx = 0) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual int ReadPixel(uint32_t attachmentIdx, int x, int y) = 0;
		virtual void ClearAttachment(uint32_t attachmentIdx, int value) = 0;

		static Ref<FrameBuffer> Create(const FrameBufferDescription& fbDesc);
	};

	class CE_API UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		static Ref<UniformBuffer> Create(uint32_t size, uint32_t slot);
	};

}
