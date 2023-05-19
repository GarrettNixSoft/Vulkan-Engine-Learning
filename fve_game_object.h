#pragma once

#include "fve_model.h"

#include <memory>

namespace fve {

	struct Transform2dComponent {
		glm::vec2 translation{};
		glm::vec2 scale{1.0f, 1.0f};
		float rotation;

		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ {c, s}, {-s, c} };
			glm::mat2 scaleMat{ {scale.x, 0.0f}, {0.0f, scale.y} };
			return rotMatrix * scaleMat;
		}
	};

	class FveGameObject {
	public:
		using id_t = unsigned int;

		static FveGameObject createGameObject() {
			static id_t currentId = 0;
			return FveGameObject{ currentId++ };
		}

		id_t getId() { return id; }

		FveGameObject(const FveGameObject&) = delete;
		FveGameObject& operator=(const FveGameObject&) = delete;
		FveGameObject(FveGameObject&&) = default;
		FveGameObject& operator=(FveGameObject&&) = default;

		std::shared_ptr<FveModel> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d;

	private:
		id_t id;

		FveGameObject(id_t objId) : id{ objId } {}
	};
}