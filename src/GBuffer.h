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

		std::shared_ptr<ge::gl::Context> m_glContext;

		GBuffer() = default;

		GBuffer(const GBuffer& other) = delete;

		GBuffer(GBuffer&& other) = default;

		GBuffer& operator=(const GBuffer& other) = delete;

		GBuffer& operator=(GBuffer&& other) = default;

		virtual ~GBuffer() = default;

		void init(int width, int height);

		void drawBuffers(GLsizei n, ...) const;

		void drawBuffers(const std::vector<Buffers> buffers) const;

		void readBuffer(Buffers buffer) const;

		void bindTexture(Buffers texture, GLuint unit) const;

	protected:

		int m_screenWidth;
		int m_screenHeight;

		std::unique_ptr<ge::gl::Framebuffer> m_gBuffer;
		std::shared_ptr<ge::gl::Texture> m_textures[NUM_TEXTURES];

		GLenum bufferToAttachment(Buffers buffer) const;
	};
}



