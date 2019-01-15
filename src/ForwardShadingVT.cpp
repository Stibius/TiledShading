#include <ForwardShadingVT.h>
#include <PointLight.h>

#include <glsg/GLScene.h>
#include <geGL/VertexArray.h>
#include <geGL/Program.h>
#include <geGL/Buffer.h>
#include <geGL/Texture.h>
#include <glsg/EnumToGL.h>

void ts::ForwardShadingVT::setViewportSize(int width, int height)
{
}

void ts::ForwardShadingVT::setShaders(const std::string& vsSource, const std::string& fsSource)
{
	m_vsSource = vsSource;
	m_fsSource = fsSource;

	m_needToCompileShaders = true;
}

void ts::ForwardShadingVT::drawSetup()
{
	if (m_needToCompileShaders)
	{
		std::shared_ptr<ge::gl::Shader> vs(std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, m_vsSource));
		std::shared_ptr<ge::gl::Shader> fs(std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, m_fsSource));
		m_shaderProgram = std::make_unique<ge::gl::Program>(vs, fs);

		m_shaderProgram->set1i("material.ambientTex", 0);
		m_shaderProgram->set1i("material.diffuseTex", 1);
		m_shaderProgram->set1i("material.specularTex", 2);
		m_shaderProgram->set1i("material.emissiveTex", 3);

		m_needToCompileShaders = false;
	}

	if (m_needToSetupLights)
	{
		if (m_pointLights && m_pointLights->size() != 0)
		{
			m_shaderProgram->set1ui("lightCount", m_pointLights->size());
			m_lightsShaderStorageBuffer = std::make_unique<ge::gl::Buffer>(m_pointLights->size() * sizeof(ge::sg::PointLight), m_pointLights->data(), GL_DYNAMIC_DRAW);
			m_lightsShaderStorageBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
		}
		else
		{
			m_shaderProgram->set1ui("lightCount", 0);
		}

		m_needToSetupLights = false;
	}

	if (m_needToSetupUniforms)
	{
		m_shaderProgram->setMatrix4fv("projectionMatrix", glm::value_ptr(m_projectionMatrix));
		m_shaderProgram->setMatrix4fv("viewMatrix", glm::value_ptr(m_viewMatrix));
		m_shaderProgram->setMatrix4fv("modelMatrix", glm::value_ptr(m_modelMatrix));
		m_shaderProgram->set3fv("cameraPos", glm::value_ptr(glm::inverse(m_viewMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

		m_needToSetupUniforms = false;
	}
}

void ts::ForwardShadingVT::draw()
{
	if (!m_glScene || !m_sceneProcessed)
	{
		return;
	}

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
}
