#include <DeferredShadingVT.h>
#include <PointLight.h>
#include <Sphere.h>
#include <GBuffer.h>

#include <glsg/GLScene.h>
#include <geGL/VertexArray.h>
#include <geGL/Program.h>
#include <geGL/Buffer.h>
#include <geGL/Framebuffer.h>
#include <geGL/Renderbuffer.h>
#include <geGL/Texture.h>
#include <glsg/EnumToGL.h>

const std::string ts::DeferredShadingVT::m_stencilPassVSSource = R".(
#version 450

layout(location = 0) in vec3 position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main()
{ 
   gl_Position = projectionMatrix * viewMatrix * modelMatrix* vec4(position, 1.0);
}
).";

const std::string ts::DeferredShadingVT::m_stencilPassFSSource = R".(
#version 450

void main()
{
}
).";

void ts::DeferredShadingVT::setViewportSize(int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;

	if (m_gBuffer)
	{
		m_gBuffer->init(width, height);
	}
}

void ts::DeferredShadingVT::setShaders(
	const std::string& geometryPassVSSource, 
	const std::string& geometryPassFSSource,
	const std::string& lightingPassVSSource,
	const std::string& lightingPassFSSource)
{
	m_geometryPassVSSource = geometryPassVSSource;
	m_geometryPassFSSource = geometryPassFSSource;
	m_lightingPassVSSource = lightingPassVSSource;
	m_lightingPassFSSource = lightingPassFSSource;

	m_needToCompileShaders = true;
}

void ts::DeferredShadingVT::drawSetup()
{
	if (m_needToCompileShaders)
	{
		std::shared_ptr<ge::gl::Shader> geometryVS(std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, m_geometryPassVSSource));
		std::shared_ptr<ge::gl::Shader> geometryFS(std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, m_geometryPassFSSource));
		m_geometryPassShaderProgram = std::make_unique<ge::gl::Program>(geometryVS, geometryFS);

		m_geometryPassShaderProgram->set1i("material.ambientTex", 0);
		m_geometryPassShaderProgram->set1i("material.diffuseTex", 1);
		m_geometryPassShaderProgram->set1i("material.specularTex", 2);
		m_geometryPassShaderProgram->set1i("material.emissiveTex", 3);

		std::shared_ptr<ge::gl::Shader> stencilVS(std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, m_stencilPassVSSource));
		std::shared_ptr<ge::gl::Shader> stencilFS(std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, m_stencilPassFSSource));
		m_stencilPassShaderProgram = std::make_unique<ge::gl::Program>(stencilVS, stencilFS);

		std::shared_ptr<ge::gl::Shader> lightingVS(std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, m_lightingPassVSSource));
		std::shared_ptr<ge::gl::Shader> lightingFS(std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, m_lightingPassFSSource));
		m_lightingPassShaderProgram = std::make_unique<ge::gl::Program>(lightingVS, lightingFS);

		m_lightingPassShaderProgram->set1i("gBufferPosition", 4);
		m_lightingPassShaderProgram->set1i("gBufferNormal", 5);
		m_lightingPassShaderProgram->set1i("gBufferAmbient", 6);
		m_lightingPassShaderProgram->set1i("gBufferDiffuse", 7);
		m_lightingPassShaderProgram->set1i("gBufferSpecular", 8);
		m_lightingPassShaderProgram->set1i("gBufferEmissive", 9);
		m_lightingPassShaderProgram->set1i("gBufferShininess", 10);

		m_needToCompileShaders = false;
	}

	if (m_needToSetupUniforms)
	{
		m_geometryPassShaderProgram->setMatrix4fv("projectionMatrix", glm::value_ptr(m_projectionMatrix));
		m_geometryPassShaderProgram->setMatrix4fv("viewMatrix", glm::value_ptr(m_viewMatrix));
		m_geometryPassShaderProgram->setMatrix4fv("modelMatrix", glm::value_ptr(m_modelMatrix));

		m_lightingPassShaderProgram->set3fv("cameraPos", glm::value_ptr(glm::inverse(m_viewMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		m_lightingPassShaderProgram->setMatrix4fv("projectionMatrix", glm::value_ptr(m_projectionMatrix));
		m_lightingPassShaderProgram->setMatrix4fv("viewMatrix", glm::value_ptr(m_viewMatrix));

		m_stencilPassShaderProgram->setMatrix4fv("projectionMatrix", glm::value_ptr(m_projectionMatrix));
		m_stencilPassShaderProgram->setMatrix4fv("viewMatrix", glm::value_ptr(m_viewMatrix));
		
		m_needToSetupUniforms = false;
	}

	if (!m_gBuffer)
	{
		m_gBuffer = std::make_unique<GBuffer>();
		m_gBuffer->m_glContext = m_glContext;
		m_gBuffer->init(m_screenWidth, m_screenHeight);
	}

	if (!m_sphereVAO)
	{
		m_sphereVAO = std::make_unique<ge::gl::VertexArray>();

		std::shared_ptr<ge::gl::Buffer> VBO = std::make_shared<ge::gl::Buffer>(sizeof(sphereVertices), sphereVertices);
		m_sphereVAO->addAttrib(VBO, 0, 3, GL_FLOAT, 3 * sizeof(float));

		std::shared_ptr<ge::gl::Buffer> EBO = std::make_shared<ge::gl::Buffer>(sizeof(sphereIndices), sphereIndices);
		m_sphereVAO->addElementBuffer(EBO);
	}
}

void ts::DeferredShadingVT::draw()
{
	if (!m_glScene || !m_sceneProcessed || !m_pointLights || m_pointLights->empty())
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
	
	geometryPass();

	m_glContext->glDepthMask(GL_FALSE);
	m_glContext->glEnable(GL_STENCIL_TEST);
	m_sphereVAO->bind();

	m_lightingPassShaderProgram->set2uiv("screenSize", glm::value_ptr(glm::uvec2(m_screenWidth, m_screenHeight)));

	for (const ge::sg::PointLight& pointLight : *m_pointLights)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(pointLight.position.x, pointLight.position.y, pointLight.position.z));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(pointLight.radius, pointLight.radius, pointLight.radius));
		m_stencilPassShaderProgram->setMatrix4fv("modelMatrix", glm::value_ptr(modelMatrix));
		m_lightingPassShaderProgram->setMatrix4fv("modelMatrix", glm::value_ptr(modelMatrix));

		m_glContext->glDisable(GL_BLEND);
		m_glContext->glEnable(GL_DEPTH_TEST);
		m_glContext->glDisable(GL_CULL_FACE);
		m_glContext->glStencilFunc(GL_ALWAYS, 0, 0);
		m_glContext->glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		m_glContext->glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

		stencilPass(pointLight);

		m_glContext->glEnable(GL_BLEND);
		m_glContext->glBlendEquation(GL_FUNC_ADD);
		m_glContext->glBlendFunc(GL_ONE, GL_ONE);
		m_glContext->glDisable(GL_DEPTH_TEST);
		m_glContext->glEnable(GL_CULL_FACE);
		m_glContext->glCullFace(GL_FRONT);
		m_glContext->glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
		
		lightingPass(pointLight);

		m_glContext->glCullFace(GL_BACK);
	}
	m_sphereVAO->unbind();

	m_glContext->glDisable(GL_STENCIL_TEST);

	m_gBuffer->readBuffer(GBuffer::OUTPUT_RENDERBUFFER);

	m_glContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	m_glContext->glBlitFramebuffer(0, 0, m_screenWidth, m_screenHeight, 0, 0, m_screenWidth, m_screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void ts::DeferredShadingVT::geometryPass()
{
	m_gBuffer->drawBuffers(
		7,
		GBuffer::POSITION_TEXTURE,
		GBuffer::NORMAL_TEXTURE,
		GBuffer::AMBIENT_TEXTURE,
		GBuffer::DIFFUSE_TEXTURE,
		GBuffer::SPECULAR_TEXTURE,
		GBuffer::EMISSIVE_TEXTURE,
		GBuffer::SHININESS_TEXTURE);

	m_geometryPassShaderProgram->use();

	for (auto& meshIt : m_glScene->GLMeshes)
	{
		ge::sg::Mesh* mesh = meshIt.first;

		ge::gl::Texture* ambientTex = textureContainer[meshIt.first][ge::sg::MaterialImageComponent::Semantic::ambientTexture].get();
		m_geometryPassShaderProgram->set1i("material.hasAmbientTex", ambientTex != nullptr);
		if (ambientTex)
		{
			ambientTex->bind(0);
		}

		ge::gl::Texture* diffuseTex = textureContainer[meshIt.first][ge::sg::MaterialImageComponent::Semantic::diffuseTexture].get();
		m_geometryPassShaderProgram->set1i("material.hasDiffuseTex", diffuseTex != nullptr);
		if (diffuseTex)
		{
			diffuseTex->bind(1);
		}

		ge::gl::Texture* specularTex = textureContainer[meshIt.first][ge::sg::MaterialImageComponent::Semantic::specularTexture].get();
		m_geometryPassShaderProgram->set1i("material.hasSpecularTex", specularTex != nullptr);
		if (specularTex)
		{
			specularTex->bind(2);
		}

		ge::gl::Texture* emissiveTex = textureContainer[meshIt.first][ge::sg::MaterialImageComponent::Semantic::emissiveTexture].get();
		m_geometryPassShaderProgram->set1i("material.hasEmissiveTex", emissiveTex != nullptr);
		if (emissiveTex)
		{
			emissiveTex->bind(3);
		}

		m_geometryPassShaderProgram->set3fv("material.ambient", reinterpret_cast<const float*>(colorContainer[mesh][ge::sg::MaterialSimpleComponent::Semantic::ambientColor].get()));
		m_geometryPassShaderProgram->set3fv("material.diffuse", reinterpret_cast<const float*>(colorContainer[mesh][ge::sg::MaterialSimpleComponent::Semantic::diffuseColor].get()));
		m_geometryPassShaderProgram->set3fv("material.specular", reinterpret_cast<const float*>(colorContainer[mesh][ge::sg::MaterialSimpleComponent::Semantic::specularColor].get()));
		m_geometryPassShaderProgram->set3fv("material.emissive", reinterpret_cast<const float*>(colorContainer[mesh][ge::sg::MaterialSimpleComponent::Semantic::emissiveColor].get()));
		m_geometryPassShaderProgram->set1fv("material.shininess", reinterpret_cast<const float*>(colorContainer[mesh][ge::sg::MaterialSimpleComponent::Semantic::shininess].get()));

		ge::gl::VertexArray* VAO = VAOContainer[mesh].get();
		VAO->bind();
		m_glContext->glDrawElements(ge::glsg::translateEnum(mesh->primitive), mesh->count, ge::glsg::translateEnum(mesh->getAttribute(ge::sg::AttributeDescriptor::Semantic::indices)->type), 0);
		VAO->unbind();
	}
}

void ts::DeferredShadingVT::stencilPass(const ge::sg::PointLight& pointLight)
{
	m_gBuffer->drawBuffers(0);

	m_glContext->glClear(GL_STENCIL_BUFFER_BIT);

	m_stencilPassShaderProgram->use();

	m_glContext->glDrawElements(GL_TRIANGLES, sizeof(sphereIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
}

void ts::DeferredShadingVT::lightingPass(const ge::sg::PointLight& pointLight)
{
	m_gBuffer->drawBuffers(1, GBuffer::OUTPUT_RENDERBUFFER);

	m_gBuffer->bindTexture(GBuffer::Buffers::POSITION_TEXTURE, 4);
	m_gBuffer->bindTexture(GBuffer::Buffers::NORMAL_TEXTURE, 5);
	m_gBuffer->bindTexture(GBuffer::Buffers::AMBIENT_TEXTURE, 6);
	m_gBuffer->bindTexture(GBuffer::Buffers::DIFFUSE_TEXTURE, 7);
	m_gBuffer->bindTexture(GBuffer::Buffers::SPECULAR_TEXTURE, 8);
	m_gBuffer->bindTexture(GBuffer::Buffers::EMISSIVE_TEXTURE, 9);
	m_gBuffer->bindTexture(GBuffer::Buffers::SHININESS_TEXTURE, 10);

	m_lightingPassShaderProgram->use();

	m_lightingPassShaderProgram->set4fv("pointLight.position", glm::value_ptr(pointLight.position));
	m_lightingPassShaderProgram->set4fv("pointLight.color", glm::value_ptr(pointLight.color));
	m_lightingPassShaderProgram->set1f("pointLight.radius", pointLight.radius);
	m_lightingPassShaderProgram->set1f("pointLight.constantAttenuationFactor", pointLight.constantAttenuationFactor);
	m_lightingPassShaderProgram->set1f("pointLight.linearAttenuationFactor", pointLight.linearAttenuationFactor);
	m_lightingPassShaderProgram->set1f("pointLight.quadraticAttenuationFactor", pointLight.quadraticAttenuationFactor);
		
	m_glContext->glDrawElements(GL_TRIANGLES, sizeof(sphereIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
}
