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

		GBufferVT(const GBufferVT& other) = delete;

		GBufferVT(GBufferVT&& other) = default;

		GBufferVT& operator=(const GBufferVT& other) = delete;

		GBufferVT& operator=(GBufferVT&& other) = default;

		~GBufferVT() override = default;

		void setViewportSize(int width, int height) override;

		void setShaders(
			const std::string& VSSource,
			const std::string& FSSource);

		void setDrawBuffer(GBuffer::Buffers buffer);

		void drawSetup() override;

		void draw() override;

	protected:

		int m_screenWidth;
		int m_screenHeight;

		std::unique_ptr<GBuffer> m_gBuffer;
		GBuffer::Buffers m_drawBuffer;
		std::unique_ptr<ge::gl::Program> m_shaderProgram;
		std::string m_VSSource;
		std::string m_FSSource;

		bool m_needToCompileShaders = false;
	};
}
