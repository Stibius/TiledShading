#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <geSG/AABB.h>

namespace ge
{
	namespace sg
	{
		class Scene;

		class SceneAABB : public ge::sg::AABB
		{
		public:

			SceneAABB() = default;

			explicit SceneAABB(const Scene& scene);

			virtual ~SceneAABB() = default;

			bool compute(const Scene& scene);

			float getLongestSide() const;

			glm::vec3 getCenter() const;
		};
	}
}
