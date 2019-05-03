#include <GBufferVT.h>

#include <glsg/GLScene.h>
#include <geGL/VertexArray.h>
#include <geGL/Program.h>
#include <geGL/Buffer.h>
#include <geGL/Framebuffer.h>
#include <geGL/Renderbuffer.h>
#include <geGL/Texture.h>
#include <glsg/EnumToGL.h>

#include <QDebug>

void ts::GBufferVT::setViewportSize(int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;

	if (m_gBuffer)
	{
		m_gBuffer->init(width, height);
	}
}

void ts::GBufferVT::setShaders(
	const std::string& VSSource,
	const std::string& FSSource)
{
	m_VSSource = VSSource;
	m_FSSource = FSSource;

	m_needToCompileShaders = true;
}

void ts::GBufferVT::setDrawBuffer(GBuffer::Buffers buffer) 
{
	m_drawBuffer = buffer;
}

void ts::GBufferVT::drawSetup()
{
	if (m_needToCompileShaders)
	{
		std::shared_ptr<ge::gl::Shader> VS(std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, m_VSSource));
		std::shared_ptr<ge::gl::Shader> FS(std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, m_FSSource));
		m_shaderProgram = std::make_unique<ge::gl::Program>(VS, FS);

		m_shaderProgram->set1i("material.ambientTex", 0);
		m_shaderProgram->set1i("material.diffuseTex", 1);
		m_shaderProgram->set1i("material.specularTex", 2);
		m_shaderProgram->set1i("material.emissiveTex", 3);

		m_needToCompileShaders = false;
	}

	if (m_needToSetupUniforms)
	{
		m_shaderProgram->setMatrix4fv("projectionMatrix", glm::value_ptr(m_projectionMatrix));
		m_shaderProgram->setMatrix4fv("viewMatrix", glm::value_ptr(m_viewMatrix));
		m_shaderProgram->setMatrix4fv("modelMatrix", glm::value_ptr(m_modelMatrix));

		m_needToSetupUniforms = false;
	}

	if (!m_gBuffer)
	{
		m_gBuffer = std::make_unique<GBuffer>();
		m_gBuffer->m_glContext = m_glContext;
		m_gBuffer->init(m_screenWidth, m_screenHeight);
	}
}

void ts::GBufferVT::draw()
{
	if (!m_glScene || !m_sceneProcessed)
	{
		return;
	}

	m_gBuffer->drawBuffers(
		8,
		GBuffer::POSITION_TEXTURE,
		GBuffer::NORMAL_TEXTURE,
		GBuffer::AMBIENT_TEXTURE,
		GBuffer::DIFFUSE_TEXTURE,
		GBuffer::SPECULAR_TEXTURE,
		GBuffer::EMISSIVE_TEXTURE,
		GBuffer::SHININESS_TEXTURE,
		GBuffer::OUTPUT_RENDERBUFFER);
	m_glContext->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_glContext->glDisable(GL_BLEND);
	m_glContext->glDepthMask(GL_TRUE);
	m_glContext->glEnable(GL_DEPTH_TEST);

	m_shaderProgram->use();

	for (auto& meshIt : m_glScene->GLMeshes)
	{
		ge::sg::Mesh* mesh = meshIt.first;

		ge::gl::Texture* ambientTex = textureContainer[meshIt.first][ge::sg::MaterialImageComponent::Semantic::ambientTexture].get();
		m_shaderProgram->set1i("material.hasAmbientTex", ambientTex != nullptr);
		if (ambientTex)
		{
			ambientTex->bind(0);
		}

		ge::gl::Texture* diffuseTex = textureContainer[meshIt.first][ge::sg::MaterialImageComponent::Semantic::diffuseTexture].get();
		m_shaderProgram->set1i("material.hasDiffuseTex", diffuseTex != nullptr);
		if (diffuseTex)
		{
			diffuseTex->bind(1);
		}

		ge::gl::Texture* specularTex = textureContainer[meshIt.first][ge::sg::MaterialImageComponent::Semantic::specularTexture].get();
		m_shaderProgram->set1i("material.hasSpecularTex", specularTex != nullptr);
		if (specularTex)
		{
			specularTex->bind(2);
		}

		ge::gl::Texture* emissiveTex = textureContainer[meshIt.first][ge::sg::MaterialImageComponent::Semantic::emissiveTexture].get();
		m_shaderProgram->set1i("material.hasEmissiveTex", emissiveTex != nullptr);
		if (emissiveTex)
		{
			emissiveTex->bind(3);
		}

		m_shaderProgram->set3fv("material.ambient", reinterpret_cast<const float*>(colorContainer[mesh][ge::sg::MaterialSimpleComponent::Semantic::ambientColor].get()));
		m_shaderProgram->set3fv("material.diffuse", reinterpret_cast<const float*>(colorContainer[mesh][ge::sg::MaterialSimpleComponent::Semantic::diffuseColor].get()));
		m_shaderProgram->set3fv("material.specular", reinterpret_cast<const float*>(colorContainer[mesh][ge::sg::MaterialSimpleComponent::Semantic::specularColor].get()));
		m_shaderProgram->set3fv("material.emissive", reinterpret_cast<const float*>(colorContainer[mesh][ge::sg::MaterialSimpleComponent::Semantic::emissiveColor].get()));
		m_shaderProgram->set1fv("material.shininess", reinterpret_cast<const float*>(colorContainer[mesh][ge::sg::MaterialSimpleComponent::Semantic::shininess].get()));

		ge::gl::VertexArray* VAO = VAOContainer[mesh].get();
		VAO->bind();
		m_glContext->glDrawElements(ge::glsg::translateEnum(mesh->primitive), mesh->count, ge::glsg::translateEnum(mesh->getAttribute(ge::sg::AttributeDescriptor::Semantic::indices)->type), 0);
		VAO->unbind();
	}

	m_glContext->glEnable(GL_BLEND);
	m_glContext->glDepthMask(GL_FALSE);
	m_glContext->glDisable(GL_DEPTH_TEST);

	m_gBuffer->readBuffer(m_drawBuffer);

	m_glContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	m_glContext->glBlitFramebuffer(0, 0, m_screenWidth, m_screenHeight, 0, 0, m_screenWidth, m_screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
