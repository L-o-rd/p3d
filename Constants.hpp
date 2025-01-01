#ifndef P3D_CONSTANTS
#define P3D_CONSTANTS

#include <glm/glm.hpp>

namespace Constants {
	constexpr static auto pi = 3.141592653589793238462f;
	constexpr static auto Timestep = 1000.f / 60.f;
	constexpr static auto FOV = glm::radians(60.f);
	constexpr static auto MouseSensitivity = 0.15f;
	constexpr static auto pih = pi * 0.5f;
	constexpr static auto Title = "P3D";
	constexpr static auto Near = .1f;
}

namespace Globals {
	extern int Height;
	extern int  Width;
}

#endif