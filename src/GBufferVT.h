#pragma once

#include <SceneVT.h>
#include <GBuffer.h>

#include <memory>
#include <vector>

namespace ge
{
	namespace gl
	{
		class Program;
	}
}

namespace ts
{
	class GBuffer;

	class GBufferVT : public SceneVT
	{
	public:

		GBufferVT() = default;

		GBufferVT(const GBufferVT& vt) = delete;

		GBufferVT(GBufferVT&& vt) = default;

		GBufferVT& operator=(const GBufferVT& vt) = delete;

		GBufferVT& operator=(GBufferVT&& vt) = default;

		virtual ~GBufferVT() = default;

		virtual void setViewportSize(int width, int height) override;

		void setShaders(
			const std::string& VSSource,
			const std::string& FSSource);

		void setDrawBuffer(GBuffer::Buffers buffer);

		virtual void drawSetup() override;

		virtual void draw() override;

	protected:

		int m_screenWidth;
		int m_screenHeight;

		std::unique_ptr<GBuffer> m_gBuffer = nullptr;
		GBuffer::Buffers m_drawBuffer;
		std::unique_ptr<ge::gl::Program> m_shaderProgram = nullptr;
		std::string m_VSSource;
		std::string m_FSSource;

		bool m_needToCompileShaders = false;
	};
}
