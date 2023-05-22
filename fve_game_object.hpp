#pragma once

#include "fve_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace fve {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{1.0f, 1.0f, 1.0f};
		glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
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
		TransformComponent transform{};

	private:
		id_t id;

		FveGameObject(id_t objId) : id{ objId } {}
	};
}