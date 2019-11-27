#pragma once

#include <memory>

namespace ge
{
	namespace util
	{
		class PerspectiveCamera;
		class FreeLookCamera;
		class OrbitCamera;
	}
}

namespace ts
{
	class Camera
	{
	public:

		Camera();

		Camera(const Camera& other);

		Camera(Camera&& other) = default;

		Camera& operator=(const Camera& other);

		Camera& operator=(Camera&& other) = default;

		virtual ~Camera() = default;

		std::unique_ptr<ge::util::PerspectiveCamera> m_perspective = std::make_unique<ge::util::PerspectiveCamera>();
		std::unique_ptr<ge::util::OrbitCamera> m_orbit = std::make_unique<ge::util::OrbitCamera>();
		std::unique_ptr<ge::util::FreeLookCamera> m_freeLook;
	};
}
