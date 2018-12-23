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

		Camera(const Camera& camera);

		Camera(Camera&& camera) = default;

		Camera& operator=(const Camera& camera);

		Camera& operator=(Camera&& camera) = default;

		virtual ~Camera() = default;

		std::unique_ptr<ge::util::PerspectiveCamera> m_perspective = std::make_unique<ge::util::PerspectiveCamera>();
		std::unique_ptr<ge::util::OrbitCamera> m_orbit = std::make_unique<ge::util::OrbitCamera>();
		std::unique_ptr<ge::util::FreeLookCamera> m_freeLook = nullptr;
	};
}
