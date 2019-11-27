#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <geSG/AABB.h>

namespace ge
{
	namespace sg
	{
		class Scene;
	}
}

namespace ts
{
	class AABB : public ge::sg::AABB
	{
	public:

		AABB() = default;

		explicit AABB(const ge::sg::Scene& scene);

		AABB(const AABB& other) = default;

		AABB(AABB&& other) = default;

		AABB& operator=(const AABB& other) = default;

		AABB& operator=(AABB&& other) = default;

		virtual ~AABB() = default;

		bool compute(const ge::sg::Scene& scene);

		float getLongestSide() const;

		glm::vec3 getCenter() const;
	};
}

