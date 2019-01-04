#include <GBuffer.h>

#include <geGL/Framebuffer.h>
#include <geGL/Renderbuffer.h>
#include <geGL/Texture.h>
#include <geGL/OpenGLContext.h>

void ts::GBuffer::init(int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;

	m_gBuffer = std::make_unique<ge::gl::Framebuffer>();
	m_gBuffer->bind(GL_FRAMEBUFFER);

	//position texture
	m_textures[Buffers::POSITION_TEXTURE] = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_RGB16F, 0, m_screenWidth, m_screenHeight, 0);
	m_textures[Buffers::POSITION_TEXTURE]->setData2D(nullptr, GL_RGB, GL_FLOAT, 0, GL_TEXTURE_2D);
	m_textures[Buffers::POSITION_TEXTURE]->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_textures[Buffers::POSITION_TEXTURE]->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer->attachTexture(bufferToAttachment(Buffers::POSITION_TEXTURE), m_textures[Buffers::POSITION_TEXTURE]);

	//normal texture
	m_textures[Buffers::NORMAL_TEXTURE] = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_RGB16F, 0, m_screenWidth, m_screenHeight, 0);
	m_textures[Buffers::NORMAL_TEXTURE]->setData2D(nullptr, GL_RGB, GL_FLOAT, 0, GL_TEXTURE_2D);
	m_textures[Buffers::NORMAL_TEXTURE]->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_textures[Buffers::NORMAL_TEXTURE]->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer->attachTexture(bufferToAttachment(Buffers::NORMAL_TEXTURE), m_textures[Buffers::NORMAL_TEXTURE]);

	//ambient texture
	m_textures[Buffers::AMBIENT_TEXTURE] = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_RGB, 0, m_screenWidth, m_screenHeight, 0);
	m_textures[Buffers::AMBIENT_TEXTURE]->setData2D(nullptr, GL_RGB, GL_UNSIGNED_BYTE, 0, GL_TEXTURE_2D);
	m_textures[Buffers::AMBIENT_TEXTURE]->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_textures[Buffers::AMBIENT_TEXTURE]->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer->attachTexture(bufferToAttachment(Buffers::AMBIENT_TEXTURE), m_textures[Buffers::AMBIENT_TEXTURE]);

	//diffuse texture
	m_textures[Buffers::DIFFUSE_TEXTURE] = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_RGB, 0, m_screenWidth, m_screenHeight, 0);
	m_textures[Buffers::DIFFUSE_TEXTURE]->setData2D(nullptr, GL_RGB, GL_UNSIGNED_BYTE, 0, GL_TEXTURE_2D);
	m_textures[Buffers::DIFFUSE_TEXTURE]->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_textures[Buffers::DIFFUSE_TEXTURE]->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer->attachTexture(bufferToAttachment(Buffers::DIFFUSE_TEXTURE), m_textures[Buffers::DIFFUSE_TEXTURE]);

	//specular texture
	m_textures[Buffers::SPECULAR_TEXTURE] = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_RGB, 0, m_screenWidth, m_screenHeight, 0);
	m_textures[Buffers::SPECULAR_TEXTURE]->setData2D(nullptr, GL_RGB, GL_UNSIGNED_BYTE, 0, GL_TEXTURE_2D);
	m_textures[Buffers::SPECULAR_TEXTURE]->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_textures[Buffers::SPECULAR_TEXTURE]->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer->attachTexture(bufferToAttachment(Buffers::SPECULAR_TEXTURE), m_textures[Buffers::SPECULAR_TEXTURE]);

	//emissive texture
	m_textures[Buffers::EMISSIVE_TEXTURE] = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_RGB, 0, m_screenWidth, m_screenHeight, 0);
	m_textures[Buffers::EMISSIVE_TEXTURE]->setData2D(nullptr, GL_RGB, GL_UNSIGNED_BYTE, 0, GL_TEXTURE_2D);
	m_textures[Buffers::EMISSIVE_TEXTURE]->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_textures[Buffers::EMISSIVE_TEXTURE]->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer->attachTexture(bufferToAttachment(Buffers::EMISSIVE_TEXTURE), m_textures[Buffers::EMISSIVE_TEXTURE]);

	//shininess texture
	m_textures[Buffers::SHININESS_TEXTURE] = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_R16F, 0, m_screenWidth, m_screenHeight, 0);
	m_textures[Buffers::SHININESS_TEXTURE]->setData2D(nullptr, GL_RED, GL_UNSIGNED_BYTE, 0, GL_TEXTURE_2D);
	m_textures[Buffers::SHININESS_TEXTURE]->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_textures[Buffers::SHININESS_TEXTURE]->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer->attachTexture(bufferToAttachment(Buffers::SHININESS_TEXTURE), m_textures[Buffers::SHININESS_TEXTURE]);

	std::shared_ptr<ge::gl::Renderbuffer> outputRenderBuffer = std::make_shared<ge::gl::Renderbuffer>();
	m_glContext->glNamedRenderbufferStorageMultisample(outputRenderBuffer->getId(), 0, GL_RGB16F, m_screenWidth, m_screenHeight);
	m_gBuffer->attachRenderbuffer(bufferToAttachment(Buffers::OUTPUT_RENDERBUFFER), outputRenderBuffer);

	std::shared_ptr<ge::gl::Renderbuffer> depthRenderBuffer = std::make_shared<ge::gl::Renderbuffer>();
	m_glContext->glNamedRenderbufferStorageMultisample(depthRenderBuffer->getId(), 0, GL_DEPTH_STENCIL, m_screenWidth, m_screenHeight);
	m_gBuffer->attachRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, depthRenderBuffer);

	m_gBuffer->unbind(GL_FRAMEBUFFER);
}

void ts::GBuffer::drawBuffers(GLsizei n, ...) const
{
	if (n == 0)
	{
		m_gBuffer->drawBuffer(GL_NONE);

		return;
	}

	m_gBuffer->bind(GL_DRAW_FRAMEBUFFER);

	GLenum* attachments = new GLenum[n];

	va_list args;
	va_start(args, n);
	for (GLsizei i = 0; i < n; i++)
	{
		Buffers buffer = (Buffers)va_arg(args, Buffers);
		attachments[i] = bufferToAttachment(buffer);
	}
	va_end(args);

	m_gBuffer->drawBuffers(n, attachments);

	delete[] attachments;
}

void ts::GBuffer::readBuffer(Buffers buffer) const
{
	m_gBuffer->bind(GL_READ_FRAMEBUFFER);
	m_glContext->glReadBuffer(bufferToAttachment(buffer));
}

void ts::GBuffer::bindTexture(Buffers texture, GLuint unit) const
{
	if (texture < NUM_TEXTURES)
	{
		m_textures[texture]->bind(unit);
	}
}

GLenum ts::GBuffer::bufferToAttachment(Buffers buffer) const
{
	switch (buffer)
	{
	case Buffers::POSITION_TEXTURE:
		return GL_COLOR_ATTACHMENT0;
	case Buffers::NORMAL_TEXTURE:
		return GL_COLOR_ATTACHMENT1;
	case Buffers::AMBIENT_TEXTURE:
		return GL_COLOR_ATTACHMENT2;
	case Buffers::DIFFUSE_TEXTURE:
		return GL_COLOR_ATTACHMENT3;
	case Buffers::SPECULAR_TEXTURE:
		return GL_COLOR_ATTACHMENT4;
	case Buffers::EMISSIVE_TEXTURE:
		return GL_COLOR_ATTACHMENT5;
	case Buffers::SHININESS_TEXTURE:
		return GL_COLOR_ATTACHMENT6;
	case Buffers::OUTPUT_RENDERBUFFER:
		return GL_COLOR_ATTACHMENT7;
	default:
		return 0;
	}
}
