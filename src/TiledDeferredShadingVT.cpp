#include <math.h>

#include <TiledDeferredShadingVT.h>
#include <Sphere.h>
#include <PointLight.h> 

#include <glsg/GLScene.h>
#include <glsg/EnumToGL.h>


#include <QDebug>

void ts::TiledDeferredShadingVT::setViewportSize(int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;

	if (m_gBuffer)
	{
		m_gBuffer->init(width, height);
	}

	if (m_outputTexture)
	{
		int textureWidth = m_screenWidth;
		int textureHeight = m_screenHeight;

		m_outputTexture = std::make_unique<ge::gl::Texture>(GL_TEXTURE_2D, GL_RGBA32F, 0, textureWidth, textureHeight, 0);
		m_outputTexture->setData2D(nullptr, GL_RGBA, GL_FLOAT, 0, GL_TEXTURE_2D);
		m_outputTexture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		m_outputTexture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	m_needToSetupUniforms = true;
}

void ts::TiledDeferredShadingVT::setProjectionMatrix(glm::mat4 projectionMatrix)
{
	m_projectionMatrix = projectionMatrix;

	m_near = (2.0f * projectionMatrix[3][2]) / (2.0f * projectionMatrix[2][2] - 2.0f);
	m_far = (m_near * (projectionMatrix[2][2] - 1.0f)) / (projectionMatrix[2][2] + 1.0f);

	m_needToSetupUniforms = true;
}

void ts::TiledDeferredShadingVT::setTileSize(int size)
{
	size_t pos1 = m_lightingPassCSSource.find("const uint WORK_GROUP_SIZE");
	if (pos1 == std::string::npos) return;
	size_t pos2 = m_lightingPassCSSource.find(";", pos1);
	if (pos2 == std::string::npos) return;
	m_lightingPassCSSource.replace(pos1, pos2 - pos1, "const uint WORK_GROUP_SIZE = " + std::to_string(size));

	m_tileSize = size;
	m_needToCompileShaders = true;
}

void ts::TiledDeferredShadingVT::setMaxLightsPerTile(int maxLightsPerTile)
{
	size_t pos1 = m_lightingPassCSSource.find("const uint MAX_LIGHTS_PER_TILE");
	if (pos1 == std::string::npos) return;
	size_t pos2 = m_lightingPassCSSource.find(";", pos1);
	if (pos2 == std::string::npos) return;
	m_lightingPassCSSource.replace(pos1, pos2 - pos1, "const uint MAX_LIGHTS_PER_TILE = " + std::to_string(maxLightsPerTile));

	m_needToCompileShaders = true;
}

void ts::TiledDeferredShadingVT::showTiles(bool showTiles)
{
	m_showTiles = showTiles;

	m_needToSetupUniforms = true;
}

void ts::TiledDeferredShadingVT::setShaders(
	const std::string& geometryPassVSSource, 
	const std::string& geometryPassFSSource, 
	const std::string& lightingPassCSSource,
	const std::string& renderPassVSSource,
	const std::string& renderPassFSSource)
{
	m_geometryPassVSSource = geometryPassVSSource;
	m_geometryPassFSSource = geometryPassFSSource;
	m_lightingPassCSSource = lightingPassCSSource;
	m_renderPassVSSource = renderPassVSSource;
	m_renderPassFSSource = renderPassFSSource;

	m_needToCompileShaders = true;
}

void ts::TiledDeferredShadingVT::drawSetup()
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

		std::shared_ptr<ge::gl::Shader> lightingCS(std::make_shared<ge::gl::Shader>(GL_COMPUTE_SHADER, m_lightingPassCSSource));
		m_lightingPassShaderProgram = std::make_unique<ge::gl::Program>(lightingCS);

		std::shared_ptr<ge::gl::Shader> renderVS(std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, m_renderPassVSSource));
		std::shared_ptr<ge::gl::Shader> renderFS(std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, m_renderPassFSSource));
		m_renderPassShaderProgram = std::make_unique<ge::gl::Program>(renderVS, renderFS);

		m_lightingPassShaderProgram->set1i("gBufferPosition", 4);
		m_lightingPassShaderProgram->set1i("gBufferNormal", 5);
		m_lightingPassShaderProgram->set1i("gBufferAmbient", 6);
		m_lightingPassShaderProgram->set1i("gBufferDiffuse", 7);
		m_lightingPassShaderProgram->set1i("gBufferSpecular", 8);
		m_lightingPassShaderProgram->set1i("gBufferEmissive", 9);
		m_lightingPassShaderProgram->set1i("gBufferShininess", 10);

		m_needToCompileShaders = false;
		m_needToSetupUniforms = true;
		m_needToSetupLights = true;
	}

	if (m_needToSetupLights)
	{
		if (m_pointLights && m_pointLights->size() != 0)
		{
			m_lightingPassShaderProgram->set1ui("lightCount", m_pointLights->size());
			m_lightsShaderStorageBuffer = std::make_unique<ge::gl::Buffer>(m_pointLights->size() * sizeof(ge::sg::PointLight), m_pointLights->data(), GL_DYNAMIC_DRAW);
			m_lightsShaderStorageBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
		}
		else
		{
			m_lightingPassShaderProgram->set1ui("lightCount", 0);
		}

		m_needToSetupLights = false;
	}

	if (m_needToSetupUniforms)
	{
		m_geometryPassShaderProgram->setMatrix4fv("projectionMatrix", glm::value_ptr(m_projectionMatrix));
		m_geometryPassShaderProgram->setMatrix4fv("viewMatrix", glm::value_ptr(m_viewMatrix));
		m_geometryPassShaderProgram->setMatrix4fv("modelMatrix", glm::value_ptr(m_modelMatrix));

		m_lightingPassShaderProgram->set3fv("cameraPos", glm::value_ptr(glm::inverse(m_viewMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		m_lightingPassShaderProgram->set1f("fov", atan(1.0f / m_projectionMatrix[1][1]) * 2.0f);
		m_lightingPassShaderProgram->set1f("near", m_near);
		m_lightingPassShaderProgram->set1f("far", m_far);
		m_lightingPassShaderProgram->setMatrix4fv("viewMatrix", glm::value_ptr(m_viewMatrix));
		m_lightingPassShaderProgram->set2uiv("screenSize", glm::value_ptr(glm::uvec2(m_screenWidth, m_screenHeight)));
		m_lightingPassShaderProgram->set1i("showTiles", m_showTiles);

		m_renderPassShaderProgram->set2uiv("screenSize", glm::value_ptr(glm::uvec2(m_screenWidth, m_screenHeight)));

		m_needToSetupUniforms = false;
	}

	if (!m_gBuffer)
	{
		m_gBuffer = std::make_unique<GBuffer>();
		m_gBuffer->m_glContext = m_glContext;
		m_gBuffer->init(m_screenWidth, m_screenHeight);
	}

	if (!m_outputTexture)
	{
		int textureWidth = m_screenWidth;
		int textureHeight = m_screenHeight;

		m_outputTexture = std::make_unique<ge::gl::Texture>(GL_TEXTURE_2D, GL_RGBA32F, 0, textureWidth, textureHeight, 0);
		m_outputTexture->setData2D(nullptr, GL_RGBA, GL_FLOAT, 0, GL_TEXTURE_2D);
		m_outputTexture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		m_outputTexture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	if (!m_dummyVAO)
	{
		m_dummyVAO = std::make_unique<ge::gl::VertexArray>();
	}
}

void ts::TiledDeferredShadingVT::draw()
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
	m_glContext->glDisable(GL_DEPTH_TEST);

	m_outputTexture->bindImage(0, 0, GL_RGBA32F, GL_READ_WRITE, GL_FALSE, 0);

	lightingPass();

	m_glContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	m_dummyVAO->bind();
	renderPass();
	m_dummyVAO->unbind();
}

void ts::TiledDeferredShadingVT::geometryPass()
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

void ts::TiledDeferredShadingVT::lightingPass()
{
	m_gBuffer->bindTexture(GBuffer::Buffers::POSITION_TEXTURE, 4);
	m_gBuffer->bindTexture(GBuffer::Buffers::NORMAL_TEXTURE, 5);
	m_gBuffer->bindTexture(GBuffer::Buffers::AMBIENT_TEXTURE, 6);
	m_gBuffer->bindTexture(GBuffer::Buffers::DIFFUSE_TEXTURE, 7);
	m_gBuffer->bindTexture(GBuffer::Buffers::SPECULAR_TEXTURE, 8);
	m_gBuffer->bindTexture(GBuffer::Buffers::EMISSIVE_TEXTURE, 9);
	m_gBuffer->bindTexture(GBuffer::Buffers::SHININESS_TEXTURE, 10);

	m_lightingPassShaderProgram->dispatch(ceil(static_cast<float>(m_screenWidth) / m_tileSize), ceil(static_cast<float>(m_screenHeight) / m_tileSize), 1);
}

void ts::TiledDeferredShadingVT::renderPass()
{
	m_renderPassShaderProgram->use();

	m_glContext->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
