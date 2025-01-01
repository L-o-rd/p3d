#ifndef P3D_SHADER
#define P3D_SHADER

#include <GL/glew.h>

class Shader final {
	bool has_vertex, has_fragment, has_tcs, has_tes;
	GLuint id, vid, fid, tcid, teid;
public:
	Shader() : has_vertex(false), has_fragment(false), has_tcs(false), has_tes(false) {}

	auto SetVec3f(const char*, const GLfloat&, const GLfloat&, const GLfloat&) -> void;
	auto SetVec2f(const char*, const GLfloat&, const GLfloat&) -> void;
	auto SetMatrix(const char *, const GLfloat *) -> void;
	auto SetInteger(const char *, const GLint&) -> void;
	auto SetFloat(const char *, const GLfloat&) -> void;
	auto Create(const char *, const int&) -> void;
	auto GetID(void) const -> const GLuint&;
	auto Compile(void) -> void;
	auto Use(void) -> Shader&;
	~Shader();
};

#endif