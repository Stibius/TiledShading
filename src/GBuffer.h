#pragma once

#include <memory>
#include <vector>

#include <geGL/geGL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ge
{
	namespace gl
	{
		class Framebuffer;
		class Texture;
		class Context;
	}
}

namespace ts
{
	class GBuffer 
	{
	public:

		enum Buffers
		{
			POSITION_TEXTURE,
			NORMAL_TEXTURE,
			AMBIENT_TEXTURE,
			DIFFUSE_TEXTURE,
			SPECULAR_TEXTURE,
			EMISSIVE_TEXTURE,
			SHININESS_TEXTURE,
			NUM_TEXTURES,
			OUTPUT_RENDERBUFFER = NUM_TEXTURES
		};

		std::shared_ptr<ge::gl::Context> m_glContext = nullptr;

		GBuffer() = default;

		GBuffer(const GBuffer& gBuffer) = delete;

		GBuffer(GBuffer&& vt) = default;

		GBuffer& operator=(const GBuffer& gBuffer) = delete;

		GBuffer& operator=(GBuffer&& gBuffer) = default;

		virtual ~GBuffer() = default;

		virtual void init(int width, int height);

		virtual void drawBuffers(GLsizei n, ...) const;

		virtual void drawBuffers(const std::vector<Buffers> buffers) const;

		virtual void readBuffer(Buffers buffer) const;

		virtual void bindTexture(Buffers texture, GLuint unit) const;

	protected:

		int m_screenWidth;
		int m_screenHeight;

		std::unique_ptr<ge::gl::Framebuffer> m_gBuffer = nullptr;
		std::shared_ptr<ge::gl::Texture> m_textures[NUM_TEXTURES];

		virtual GLenum bufferToAttachment(Buffers buffer) const;
	};
}



