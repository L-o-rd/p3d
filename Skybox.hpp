#ifndef P3D_SKYBOX
#define P3D_SKYBOX

struct Skybox final {
	unsigned vao, vbo, ebo, tex;
	auto Destroy(void) -> void;
	auto Create(void) -> void;
};

#endif