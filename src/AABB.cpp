#include <AABB.h>

#include <geSG/Scene.h>

ts::AABB::AABB(const ge::sg::Scene& scene)
{
	compute(scene);
}

bool ts::AABB::compute(const ge::sg::Scene& scene)
{
	min = glm::vec3(0.0f, 0.0f, 0.0f);
	max = glm::vec3(0.0f, 0.0f, 0.0f);

	if (scene.models.empty())
	{
		return false;
	}

	AABB bbox;
	bbox.min = glm::vec3(std::numeric_limits<float>::max());
	bbox.max = glm::vec3(std::numeric_limits<float>::min());

	for (std::shared_ptr<ge::sg::Model> model : scene.models)
	{
		if (model->meshes.empty()) return false;
		for (std::shared_ptr<ge::sg::Mesh> mesh : model->meshes)
		{
			std::shared_ptr<ge::sg::AttributeDescriptor> attDesc = mesh->getAttribute(ge::sg::AttributeDescriptor::Semantic::position);
			if (attDesc->numComponents != 3 || attDesc->offset >= attDesc->size) return false;

			size_t stride = attDesc->stride != 0 ? attDesc->stride : 3 * sizeof(float);
			if (stride == 0) return false;

			for (size_t i = attDesc->offset / sizeof(float); i < attDesc->size / sizeof(float); i += stride / sizeof(float))
			{
				bbox.min.x = std::min(bbox.min.x, (static_cast<float*>(attDesc->data.get()))[i]);
				bbox.min.y = std::min(bbox.min.y, (static_cast<float*>(attDesc->data.get()))[i + 1]);
				bbox.min.z = std::min(bbox.min.z, (static_cast<float*>(attDesc->data.get()))[i + 2]);
				bbox.max.x = std::max(bbox.max.x, (static_cast<float*>(attDesc->data.get()))[i]);
				bbox.max.y = std::max(bbox.max.y, (static_cast<float*>(attDesc->data.get()))[i + 1]);
				bbox.max.z = std::max(bbox.max.z, (static_cast<float*>(attDesc->data.get()))[i + 2]);
			}
		}
	}

	*this = bbox;

	return true;
}

float ts::AABB::getLongestSide() const
{
	float xSize = max.x - min.x;
	float ySize = max.y - min.y;
	float zSize = max.z - min.z;

	if (xSize > ySize && xSize > zSize)
	{
		return xSize;
	}
	else if (ySize > xSize && ySize > zSize)
	{
		return ySize;
	}
	else
	{
		return zSize;
	}
}

glm::vec3 ts::AABB::getCenter() const
{
	return glm::vec3(
		0.5f * (max.x + min.x),
		0.5f * (max.y + min.y),
		0.5f * (max.z + min.z));
}
