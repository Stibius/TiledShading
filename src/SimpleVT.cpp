#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SimpleVT.h>

#include <glsg/GLScene.h>
#include <geGL/VertexArray.h>
#include <geGL/Texture.h>
#include <glsg/EnumToGL.h>
#include <geGL/geGL.h>

using namespace std;
using namespace ge::gl;
using namespace ge::sg;
using namespace ge::glsg;

/**
 * Sets the scene to visualize.
 * \param scene GLScene to visualize.
 */
void ts::SimpleVT::setScene(std::shared_ptr<ge::glsg::GLScene> scene)
{
	m_glScene = scene;
}

/**
 * Creates VAOs, preps uniforms and textures that the VT needs for visualization.
 */
void ts::SimpleVT::processScene()
{
	if (!m_glScene || !m_glContext)
	{
		return;
	}

	for (auto& meshIt : m_glScene->GLMeshes)
	{
		//mesh geometry
		shared_ptr<VertexArray> VAO = make_shared<VertexArray>(m_glContext->getFunctionTable());
		for (auto& glattrib : meshIt.second)
		{
			if (glattrib.attributeDescriptor->semantic == AttributeDescriptor::Semantic::indices)
			{
				VAO->addElementBuffer(glattrib.BO);
			}
			else
			{
				int attribLocation = semantic2Attribute(glattrib.attributeDescriptor->semantic);
				if (attribLocation != -1)
				{
					VAO->addAttrib(glattrib.BO, attribLocation, glattrib.attributeDescriptor->numComponents, translateEnum(glattrib.attributeDescriptor->type), (GLsizei)glattrib.attributeDescriptor->stride);
				}
			}
		}
		VAOContainer[meshIt.first] = VAO;

		//material
		Material* mat = meshIt.first->material.get();

		for (auto& comp : mat->materialComponents)
		{
			switch (comp->getType())
			{
			case MaterialComponent::ComponentType::SIMPLE:
			{
				auto simpleComponent = static_cast<MaterialSimpleComponent*>(comp.get());
				unsigned compsizeinbytes = simpleComponent->getSize(simpleComponent->dataType);
				unsigned sizeinbytes = simpleComponent->size * compsizeinbytes;
				colorContainer[meshIt.first][simpleComponent->semantic] = make_unique<unsigned char[]>(sizeinbytes);
				std::copy_n(simpleComponent->data.get(), sizeinbytes, colorContainer[meshIt.first][simpleComponent->semantic].get());
			}
				break;
			case MaterialComponent::ComponentType::IMAGE:
			{
				auto imageComponent = static_cast<MaterialImageComponent*>(comp.get());
				textureContainer[meshIt.first][imageComponent->semantic] = m_glScene->textures[imageComponent];
			}
				break;
			default:
				break;
			}
		}
	}
}

/**
 * Currently does nothing.
 */
void ts::SimpleVT::drawSetup()
{
}

/**
 * Use provided shader and draws the provided scene.
 */
void ts::SimpleVT::draw()
{
	if (!m_shaderProgram || !m_glScene || !m_glContext)
	{
		return;
	}

	m_shaderProgram->use();

	for (auto& meshIt : m_glScene->GLMeshes)
	{
		Mesh* mesh = meshIt.first;
	
		Texture* ambientTex = textureContainer[meshIt.first][MaterialImageComponent::Semantic::ambientTexture].get();
		m_shaderProgram->set1i("material.hasAmbientTex", ambientTex != nullptr);
		if (ambientTex)
		{
			m_shaderProgram->set1i("material.ambientTex", 0);
			ambientTex->bind(0);
		}

		Texture* diffuseTex = textureContainer[meshIt.first][MaterialImageComponent::Semantic::diffuseTexture].get();
		m_shaderProgram->set1i("material.hasDiffuseTex", diffuseTex != nullptr);
		if (diffuseTex)
		{
			m_shaderProgram->set1i("material.diffuseTex", 1);
			diffuseTex->bind(1);
		}

		Texture* specularTex = textureContainer[meshIt.first][MaterialImageComponent::Semantic::specularTexture].get();
		m_shaderProgram->set1i("material.hasSpecularTex", specularTex != nullptr);
		if (specularTex)
		{
			m_shaderProgram->set1i("material.specularTex", 2);
			specularTex->bind(2);
		}

		Texture* emissiveTex = textureContainer[meshIt.first][MaterialImageComponent::Semantic::emissiveTexture].get();
		m_shaderProgram->set1i("material.hasEmissiveTex", emissiveTex != nullptr);
		if (emissiveTex)
		{
			m_shaderProgram->set1i("material.emissiveTex", 0);
			emissiveTex->bind(0);
		}

		m_shaderProgram->set3fv("material.ambient", reinterpret_cast<const float*>(colorContainer[mesh][MaterialSimpleComponent::Semantic::ambientColor].get()));
		m_shaderProgram->set3fv("material.diffuse", reinterpret_cast<const float*>(colorContainer[mesh][MaterialSimpleComponent::Semantic::diffuseColor].get()));
		m_shaderProgram->set3fv("material.specular", reinterpret_cast<const float*>(colorContainer[mesh][MaterialSimpleComponent::Semantic::specularColor].get()));
		m_shaderProgram->set3fv("material.emissive", reinterpret_cast<const float*>(colorContainer[mesh][MaterialSimpleComponent::Semantic::emissiveColor].get()));
		m_shaderProgram->set1fv("material.shininess", reinterpret_cast<const float*>(colorContainer[mesh][MaterialSimpleComponent::Semantic::shininess].get()));
		
		VertexArray * VAO = VAOContainer[mesh].get();
		VAO->bind();
		m_glContext->glDrawElements(translateEnum(mesh->primitive), mesh->count, translateEnum(mesh->getAttribute(AttributeDescriptor::Semantic::indices)->type), 0);
		VAO->unbind();
	}
}

/**
 * Internal helper function that returns attribute position for given semantic or -1
 * if the attrib is not to be used by this VT.
 */
int ts::SimpleVT::semantic2Attribute(AttributeDescriptor::Semantic semantic)
{
	switch (semantic)
	{
	case AttributeDescriptor::Semantic::position: return 0;
	case AttributeDescriptor::Semantic::normal: return 1;
	case AttributeDescriptor::Semantic::texcoord: return 2;
	default: return -1;
	}
}
