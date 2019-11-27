#pragma once

#include <memory>

namespace ge
{
	namespace sg
	{
		class Scene;
	}
}

namespace ts
{
	class AABB;

	class Scene
	{
	public:

		Scene() = default;

		explicit Scene(const ge::sg::Scene& scene);

		Scene(const Scene& other);

		Scene(Scene&& other) = default;

		Scene& operator=(const Scene& other);

		Scene& operator=(Scene&& other) = default;

		virtual ~Scene() = default;

		void setScene(const ge::sg::Scene& scene);

		const ge::sg::Scene& getScene() const;

		const AABB& getBoundingBox() const;

	protected:

		std::unique_ptr<ge::sg::Scene> m_scene = std::make_unique<ge::sg::Scene>();
		std::unique_ptr<AABB> m_boundingBox = std::make_unique<ts::AABB>();

	};
}
