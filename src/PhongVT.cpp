#include <PhongVT.h>
#include <PointLight.h>

#include <glsg/GLScene.h>
#include <geGL/VertexArray.h>
#include <geGL/Program.h>
#include <geGL/Buffer.h>
#include <geGL/Texture.h>
#include <glsg/EnumToGL.h>

void ts::PhongVT::setShaders(const std::string& vsSource, const std::string& fsSource)
{
	
	m_vsSource = vsSource;
	m_fsSource = fsSource;

	m_needToCompileShaders = true;
}

void ts::PhongVT::setLights(const std::vector<ge::sg::PointLight>& pointLights)
{
	if (!m_shaderProgram)
	{
		return;
	}

	m_shaderProgram->set1i("lightCount", pointLights.size());

	if (pointLights.size() != 0)
	{
		m_lightsBuffer = std::make_unique<ge::gl::Buffer>(pointLights.size() * sizeof(ge::sg::PointLight), pointLights.data(), GL_DYNAMIC_DRAW);
		m_lightsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void ts::PhongVT::setProjectionMatrix(glm::mat4 projectionMatrix)
{
	if (!m_shaderProgram)
	{
		return;
	}

	m_shaderProgram->setMatrix4fv("projection", glm::value_ptr(projectionMatrix));
}

void ts::PhongVT::setViewMatrix(glm::mat4 viewMatrix)
{
	if (!m_shaderProgram)
	{
		return;
	}

	m_shaderProgram->setMatrix4fv("view", glm::value_ptr(viewMatrix));
	m_shaderProgram->set3fv("viewPos", glm::value_ptr(glm::inverse(viewMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
}

void ts::PhongVT::drawSetup()
{

}

void ts::PhongVT::draw()
{
	if (m_needToCompileShaders)
	{
		compileShaders();
		m_needToCompileShaders = false;
	}

	if (!m_glContext || !m_shaderProgram || !m_glScene || !m_sceneProcessed)
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
			m_shaderProgram->set1i("material.ambientTex", 0);
			ambientTex->bind(0);
		}

		ge::gl::Texture* diffuseTex = textureContainer[meshIt.first][ge::sg::MaterialImageComponent::Semantic::diffuseTexture].get();
		m_shaderProgram->set1i("material.hasDiffuseTex", diffuseTex != nullptr);
		if (diffuseTex)
		{
			m_shaderProgram->set1i("material.diffuseTex", 1);
			diffuseTex->bind(1);
		}

		ge::gl::Texture* specularTex = textureContainer[meshIt.first][ge::sg::MaterialImageComponent::Semantic::specularTexture].get();
		m_shaderProgram->set1i("material.hasSpecularTex", specularTex != nullptr);
		if (specularTex)
		{
			m_shaderProgram->set1i("material.specularTex", 2);
			specularTex->bind(2);
		}

		ge::gl::Texture* emissiveTex = textureContainer[meshIt.first][ge::sg::MaterialImageComponent::Semantic::emissiveTexture].get();
		m_shaderProgram->set1i("material.hasEmissiveTex", emissiveTex != nullptr);
		if (emissiveTex)
		{
			m_shaderProgram->set1i("material.emissiveTex", 0);
			emissiveTex->bind(0);
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

void ts::PhongVT::compileShaders()
{
	std::shared_ptr<ge::gl::Shader> vs(std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, m_vsSource));
	std::shared_ptr<ge::gl::Shader> fs(std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, m_fsSource));
	m_shaderProgram = std::make_shared<ge::gl::Program>(vs, fs);
}
