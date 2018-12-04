#pragma once

#include <geSG/Light.h>

namespace ge
{
	namespace sg
	{
		struct PointLight : public Light
		{
			float radius;
			glm::vec3 padding; //needed for passing to shader storage buffer
		};
	}
}
