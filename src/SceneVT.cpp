#include <SceneVT.h>

#include <glsg/GLScene.h>
#include <geGL/VertexArray.h>
#include <geGL/Texture.h>
#include <glsg/EnumToGL.h>

void ts::SceneVT::setScene(std::shared_ptr<ge::glsg::GLScene> scene)
{
	m_glScene = scene;
	m_sceneProcessed = false;
	processScene();
}

void ts::SceneVT::processScene()
{
	if (!m_glScene || !m_glContext)
	{
		return;
	}

	for (auto& meshIt : m_glScene->GLMeshes)
	{
		//mesh geometry
		std::shared_ptr<ge::gl::VertexArray> VAO = std::make_shared<ge::gl::VertexArray>(m_glContext->getFunctionTable());
		for (auto& glattrib : meshIt.second)
		{
			if (glattrib.attributeDescriptor->semantic == ge::sg::AttributeDescriptor::Semantic::indices)
			{
				VAO->addElementBuffer(glattrib.BO);
			}
			else
			{
				int attribLocation = semantic2Attribute(glattrib.attributeDescriptor->semantic);

				if (attribLocation != -1)
				{
					VAO->addAttrib(glattrib.BO, attribLocation, glattrib.attributeDescriptor->numComponents, ge::glsg::translateEnum(glattrib.attributeDescriptor->type), (GLsizei)glattrib.attributeDescriptor->stride);
				}
			}
		}
		VAOContainer[meshIt.first] = VAO;

		//material
		ge::sg::Material* mat = meshIt.first->material.get();

		for (auto& comp : mat->materialComponents)
		{
			switch (comp->getType())
			{
			case ge::sg::MaterialComponent::ComponentType::SIMPLE:
			{
				auto simpleComponent = static_cast<ge::sg::MaterialSimpleComponent*>(comp.get());
				unsigned compsizeinbytes = simpleComponent->getSize(simpleComponent->dataType);
				unsigned sizeinbytes = simpleComponent->size * compsizeinbytes;
				colorContainer[meshIt.first][simpleComponent->semantic] = std::make_unique<unsigned char[]>(sizeinbytes);
				std::copy_n(simpleComponent->data.get(), sizeinbytes, colorContainer[meshIt.first][simpleComponent->semantic].get());
			}
			break;
			case ge::sg::MaterialComponent::ComponentType::IMAGE:
			{
				auto imageComponent = static_cast<ge::sg::MaterialImageComponent*>(comp.get());
				textureContainer[meshIt.first][imageComponent->semantic] = m_glScene->textures[imageComponent];
			}
			break;
			default:
				break;
			}
		}
	}

	m_sceneProcessed = true;
}

int ts::SceneVT::semantic2Attribute(ge::sg::AttributeDescriptor::Semantic semantic)
{
	switch (semantic)
	{
	case ge::sg::AttributeDescriptor::Semantic::position: return 0;
	case ge::sg::AttributeDescriptor::Semantic::normal: return 1;
	case ge::sg::AttributeDescriptor::Semantic::texcoord: return 2;
	default: return -1;
	}
}

void ts::SceneVT::setProjectionMatrix(glm::mat4 projectionMatrix)
{
	m_projectionMatrix = projectionMatrix;
	m_needToSetupUniforms = true;
}

void ts::SceneVT::setViewMatrix(glm::mat4 viewMatrix)
{
	m_viewMatrix = viewMatrix;
	m_needToSetupUniforms = true;
}

void ts::SceneVT::setModelMatrix(glm::mat4 modelMatrix)
{
	m_modelMatrix = modelMatrix;
	m_needToSetupUniforms = true;
}
