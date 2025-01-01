#ifndef P3D_CAMERA
#define P3D_CAMERA

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

struct Camera final {
	Camera(const glm::vec3& o = glm::vec3(0.f, 0.f,  0.f),
		const glm::vec3& f = glm::vec3(0.f, 0.f, -1.f),
		const glm::vec3& r = glm::vec3(1.f, 0.f,  0.f)) 
		: eye(o), forward(glm::normalize(f)), right(glm::normalize(r)) {
		this->up = glm::cross(right, forward);
		this->orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		this->yaw = this->pitch = 0.f;
		this->Compute();
	}

	inline auto Compute(void) -> void {
		this->forward = glm::normalize(this->orientation * glm::vec3(0.0f, 0.0f, -1.0f));
		this->right = glm::normalize(glm::cross(this->forward, glm::vec3(0.0f, 1.0f, 0.0f)));
		this->up = glm::normalize(glm::cross(this->right, this->forward));
		this->view = glm::lookAt(this->eye, this->eye + this->forward, this->up);
	}

	inline auto View(void) -> glm::mat4& {
		return this->view;
	}

	inline auto Move(const glm::vec3& t) {
		this->eye += t;
		this->Compute();
	}

	inline auto MouseLook(const glm::vec2& mouse) {

	}

	glm::vec3 forward, right;
	glm::quat orientation;
	glm::vec3 eye, up;
	float yaw, pitch;
	glm::mat4 view;
};

#endif