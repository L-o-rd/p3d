#include <windows.h>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <mmsystem.h>

#include "Constants.hpp"
#include "objloader.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Skybox.hpp"

int Globals::Width = 1024;
int Globals::Height = 768;

static glm::vec3 SunPosition = glm::vec3(0.f, 50.f, -50.f);
Shader master, skybox_shader, tess_shader;
static bool Keymap[256], flook = true;
static glm::mat4 Projection;
static glm::vec2 LastMouse;
static glm::mat4 Model;
static glm::mat4 View;
auto MoveSpeed = .5f;
static GLint WinID;
Skybox skybox;
Camera camera;

static std::vector<glm::vec3> tree_colrs;
static std::vector<glm::vec3> tree_verts;
static std::vector<glm::vec3> tree_norms;
GLuint tree_vao, tree_vbo;

static std::vector<glm::vec3> ff_colrs;
static std::vector<glm::vec3> ff_verts;
static std::vector<glm::vec3> ff_norms;
GLuint ff_vao, ff_vbo;

auto ProcessKeydown(GLubyte key, int, int) -> void {
	Keymap[key] = true;
	if (key == VK_SPACE) {
		std::cout << "glm::vec3(" << camera.eye.x << "f, " << camera.eye.y << "f, " << camera.eye.z << "f)," << std::endl;
	}

	if (key == 27) {
		glutDestroyWindow(WinID);
		std::exit(0);
		return;
	}
}

auto ProcessKeyup(GLubyte key, int, int) -> void {
	Keymap[key] = false;
}

auto MouseButton(GLint button, GLint state, GLint, GLint) {
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			glutSetCursor(GLUT_CURSOR_NONE);
		} else {
			glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		}
	}
}

auto Resize(int width, int height) -> void {
	glViewport(0, 0, width, height);
	Globals::Height = height;
	Globals::Width = width;

	Projection = glm::infinitePerspective(Constants::FOV, static_cast<float>(Globals::Width) / Globals::Height, Constants::Near);
}

auto MouseMovement(int x, int y) -> void {
	//if (flook) {
	//	LastMouse = glm::vec2(static_cast<float>(x), static_cast<float>(y));
	//	flook = false;
	//}
	//
	//glm::vec2 delta = glm::vec2(
	//	(static_cast<float>(x) - LastMouse.x) * Constants::MouseSensitivity,
	//	(static_cast<float>(y) - LastMouse.y) * Constants::MouseSensitivity
	//);
	//
	//LastMouse = glm::vec2(static_cast<float>(x), static_cast<float>(y));
	//camera.pitch += -delta.y;
	//camera.yaw += -delta.x;
	//
	//std::cout << camera.yaw << std::endl;
	//
	//camera.pitch = glm::clamp(camera.pitch, -89.5f, 89.5f);
	//glm::quat yaw = glm::angleAxis(glm::radians(camera.yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::quat pitch = glm::angleAxis(glm::radians(camera.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	//camera.orientation = glm::normalize(yaw * pitch);
	//camera.Compute();

	glm::vec2 delta = glm::vec2(
		(static_cast<float>(x) - (Globals::Width * 0.5f)) * Constants::MouseSensitivity,
		(static_cast<float>(y) - (Globals::Height * 0.5f)) * Constants::MouseSensitivity
	);

	camera.pitch += -delta.y;
	camera.yaw += -delta.x;
	
	camera.pitch = glm::clamp(camera.pitch, -89.f, 89.f);
	glm::quat yaw = glm::angleAxis(glm::radians(camera.yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat pitch = glm::angleAxis(glm::radians(camera.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	camera.orientation = glm::normalize(yaw * pitch);
	camera.Compute();

	glutWarpPointer(Globals::Width / 2, Globals::Height / 2);
}

typedef std::chrono::high_resolution_clock Clock;
auto ThenTime = Clock::now();
auto elapsed = 0.f;
float dt = 0.f;

auto Tick(int) -> void {
	auto NowTime = Clock::now();
	elapsed += std::chrono::duration_cast<std::chrono::milliseconds> (
		NowTime - ThenTime).count();
	ThenTime = NowTime;

	while (elapsed >= Constants::Timestep) {
		if (Keymap['w']) camera.Move(camera.forward * MoveSpeed);
		if (Keymap['s']) camera.Move(camera.forward * -MoveSpeed);
		if (Keymap['a']) camera.Move(camera.right * -MoveSpeed);
		if (Keymap['d']) camera.Move(camera.right * MoveSpeed);
		elapsed -= Constants::Timestep;
		dt += 0.1f;
	}

	glutPostRedisplay();
	glutTimerFunc((unsigned)(Constants::Timestep), ::Tick, 0);
}

std::vector<glm::vec2> tverts;
std::vector<glm::vec3> trees = {
	glm::vec3(45.4649f, -28.1704f, 11.2697f),
	glm::vec3(42.2271f, -29.9265f, 2.49888f),
	glm::vec3(28.2875f, -30.524f, 22.4994f),
	glm::vec3(-7.50374f, -22.7094f, 9.64357f),
	glm::vec3(44.8195f, -35.1529f, -42.2467f),
	glm::vec3(62.5392f, -33.1246f, -36.5159f),
	glm::vec3(119.209f, -32.9711f, 31.0922f),
	glm::vec3(119.885f, -31.0734f, 38.8321f),
	glm::vec3(122.026f, -30.0359f, 45.4041f),
	glm::vec3(125.888f, -28.7711f, 50.1443f),
	glm::vec3(62.5337f, -28.5039f, 107.862f),
	glm::vec3(48.0085f, -28.5666f, 103.063f),
	glm::vec3(17.1894f, -31.0814f, 113.204f),
	glm::vec3(-0.382649f, -25.6684f, 122.957f),
	glm::vec3(-31.7291f, -24.5964f, 115.636f),
	glm::vec3(-38.2111f, -23.0785f, 111.705f),
	glm::vec3(-51.0588f, -25.1304f, 92.1662f),
	glm::vec3(-77.8962f, -31.3844f, 42.7911f),
	glm::vec3(-86.2296f, -30.397f, 37.7538f),
	glm::vec3(-63.1079f, -30.478f, 21.3483f),
	glm::vec3(14.1416f, -36.7562f, -30.7414f),
	glm::vec3(14.824f, -33.0264f, -24.3304f),
	glm::vec3(12.6574f, -30.3147f, -16.255f),
	glm::vec3(3.00165f, -26.9095f, -6.86364f),
	glm::vec3(45.281f, -38.5177f, -14.3053f),
	glm::vec3(109.937f, -38.4736f, -25.1244f),
	glm::vec3(120.563f, -36.5647f, -20.0041f),
	glm::vec3(137.632f, -27.3371f, -12.0241f),
	glm::vec3(150.552f, -26.2514f, 7.55797f),
	glm::vec3(148.46f, -35.1153f, 47.6612f),
	glm::vec3(146.673f, -39.8081f, 65.4343f),
	glm::vec3(152.581f, -39.9149f, 88.7019f),
	glm::vec3(141.405f, -41.045f, 106.009f),
	glm::vec3(155.079f, -36.6432f, 129.779f),
	glm::vec3(139.112f, -38.3711f, 139.498f),
	glm::vec3(117.885f, -31.7281f, 145.751f),
	glm::vec3(92.9287f, -25.2855f, 147.839f),
	glm::vec3(69.5792f, -30.8468f, 145.183f),
	glm::vec3(44.2022f, -36.0674f, 150.329f),
	glm::vec3(3.71151f, -29.9314f, 150.705f),
	glm::vec3(-11.5857f, -22.0514f, 153.207f),
	glm::vec3(-22.7676f, -23.9836f, 151.494f),
	glm::vec3(-56.825f, -27.786f, 149.079f),
	glm::vec3(-67.5416f, -30.544f, 127.585f),
	glm::vec3(-91.5495f, -37.3099f, 107.834f),
	glm::vec3(-81.358f, -32.566f, 82.9724f),
	glm::vec3(-97.0941f, -34.9507f, 54.3177f),
	glm::vec3(-96.4157f, -44.7605f, 16.8833f),
	glm::vec3(-92.8002f, -47.3462f, -20.8982f),
	glm::vec3(-87.7976f, -38.8592f, -30.9746f),
	glm::vec3(-89.4077f, -40.7672f, -53.54f),
	glm::vec3(-82.7431f, -36.3359f, -60.0988f),
	glm::vec3(-77.3671f, -34.7546f, -67.0999f),
	glm::vec3(-67.9846f, -31.4843f, -72.758f),
	glm::vec3(-63.0962f, -29.3194f, -84.9066f),
	glm::vec3(-51.876f, -26.3173f, -95.5581f),
	glm::vec3(-25.3877f, -39.1511f, -91.9587f),
	glm::vec3(-9.41106f, -46.9141f, -87.0144f),
	glm::vec3(12.4711f, -47.4944f, -84.9599f),
	glm::vec3(25.8707f, -37.5962f, -88.2047f),
	glm::vec3(32.4153f, -34.4793f, -89.9958f),
	glm::vec3(42.9943f, -30.9979f, -94.1014f),
	glm::vec3(69.4307f, -29.4465f, -83.3077f),
	glm::vec3(87.4624f, -33.1896f, -70.5471f),
	glm::vec3(97.4218f, -30.9868f, -63.6535f),
	glm::vec3(115.173f, -30.3498f, -55.931f),
	glm::vec3(135.665f, -29.5697f, -56.1692f),
	glm::vec3(145.875f, -33.2341f, -49.6728f),
	glm::vec3(134.651f, -28.8018f, -15.1949f),
	glm::vec3(-17.1698f, -30.1789f, 50.1849f),
	glm::vec3(-10.6144f, -34.0512f, 61.7213f),
	glm::vec3(8.60031f, -44.9461f, 75.1718f),
	glm::vec3(7.22576f, -46.0752f, 96.0888f),
	glm::vec3(-6.76541f, -50.5446f, 98.8971f),
};
std::vector<glm::vec3> flies = {
	glm::vec3(-2.77354f, -47.2393f, 89.4051f),
	glm::vec3(2.20493f, -41.376f, 84.2381f),
	glm::vec3(-20.7421f, -38.6285f, 95.2088f),
	glm::vec3(-30.9451f, -39.5608f, 77.4653f),
};

GLuint tvao, tvbo;

auto Display(void) -> void {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	(void) skybox_shader.Use();
	View = glm::mat4(glm::mat3(camera.View())); /* Ignore translation. */
	skybox_shader.SetMatrix("view", glm::value_ptr(View));
	skybox_shader.SetMatrix("projection", glm::value_ptr(Projection));
	
	glBindVertexArray(skybox.vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.tex);
	glBindBuffer(GL_ARRAY_BUFFER, skybox.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox.ebo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	(void) master.Use();

	View = camera.View();
	master.SetMatrix("view", glm::value_ptr(View));
	master.SetMatrix("projection", glm::value_ptr(Projection));
	master.SetVec3f("lightPos", SunPosition.x, SunPosition.y, SunPosition.z);
	master.SetVec3f("viewPos", camera.eye.x, camera.eye.y, camera.eye.z);

	glBindVertexArray(tree_vao);
	glBindBuffer(GL_ARRAY_BUFFER, tree_vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(tree_verts.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)((tree_verts.size() * sizeof(glm::vec3)) + (tree_norms.size() * sizeof(glm::vec3))));
	
	for (auto&& t : trees) {
		Model = glm::translate(glm::mat4(1.f), t) *
			glm::scale(glm::mat4(1.f), glm::vec3(2.f, 2.f, 2.f));
		master.SetMatrix("model", glm::value_ptr(Model));
		glDrawArrays(GL_TRIANGLES, 0, tree_verts.size());
	}

	glBindVertexArray(ff_vao);
	glBindBuffer(GL_ARRAY_BUFFER, ff_vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(ff_verts.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)((ff_verts.size() * sizeof(glm::vec3)) + (ff_norms.size() * sizeof(glm::vec3))));

	master.SetInteger("fly", 1);
	glm::vec3 flights[4];
	for (size_t i = 0; i < flies.size(); ++i) {
		auto&& f = flies[i];
		auto dir = i & 1 ? -1 : 1;
		auto sf = glm::vec3(std::sin(dt), std::sin(dt), std::sin(dt)) * .1f;
		auto ff_pos = f + glm::vec3(std::sin(-dir * dt), std::sin(std::sqrt(std::abs(dt))), std::cos(dir * dt)) * 3.f;
		flights[i] = ff_pos;
		Model = glm::translate(glm::mat4(1.f), ff_pos) * glm::scale(glm::mat4(1.f), glm::vec3(.175f, .175f, .175f) + sf);
		master.SetMatrix("model", glm::value_ptr(Model));
		glDrawArrays(GL_TRIANGLES, 0, ff_verts.size());
	}

	glm::mat4 lights = glm::mat4(
		glm::vec4(flights[0], 1.f),
		glm::vec4(flights[1], 1.f),
		glm::vec4(flights[2], 1.f),
		glm::vec4(flights[3], 1.f)
	);

	master.SetMatrix("lights", glm::value_ptr(lights));
	master.SetInteger("fly", 0);
	(void) tess_shader.Use();
	View = camera.View();
	Model = glm::translate(glm::mat4(1.f), glm::vec3(-100.f, -74.f, -100.f)) *
		glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
	tess_shader.SetMatrix("view", glm::value_ptr(View));
	tess_shader.SetMatrix("model", glm::value_ptr(Model));
	tess_shader.SetMatrix("projection", glm::value_ptr(Projection));
	tess_shader.SetVec3f("lightPos", SunPosition.x, SunPosition.y, SunPosition.z);
	tess_shader.SetVec3f("viewPos", camera.eye.x, camera.eye.y, camera.eye.z);
	tess_shader.SetMatrix("lights", glm::value_ptr(lights));
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glBindVertexArray(tvao);
	glBindBuffer(GL_ARRAY_BUFFER, tvbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_PATCHES, 0, tverts.size());
	glBindVertexArray(0);

	glDisable(GL_DEPTH_TEST);
	glutSwapBuffers();
}

auto Fly(void) -> void {
	std::vector<glm::vec2> uvs;
	loadOBJ("./Assets/ff.obj", ff_verts, uvs, ff_norms);

	for (auto&& v : ff_verts) {
		ff_colrs.push_back(glm::vec3(.9f, .9f, .1f));
	}

	glGenVertexArrays(1, &ff_vao);
	glBindVertexArray(ff_vao);

	glGenBuffers(1, &ff_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ff_vbo);
	glBufferData(GL_ARRAY_BUFFER, (ff_verts.size() * sizeof(glm::vec3)) + (ff_norms.size() * sizeof(glm::vec3)) + (ff_colrs.size() * sizeof(glm::vec3)), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ff_verts.size() * sizeof(glm::vec3), ff_verts.data());
	glBufferSubData(GL_ARRAY_BUFFER, ff_verts.size() * sizeof(glm::vec3), ff_norms.size() * sizeof(glm::vec3), ff_norms.data());
	glBufferSubData(GL_ARRAY_BUFFER, (ff_verts.size() * sizeof(glm::vec3)) + (ff_norms.size() * sizeof(glm::vec3)), ff_colrs.size() * sizeof(glm::vec3), ff_colrs.data());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(ff_verts.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)((ff_verts.size() * sizeof(glm::vec3)) + (ff_norms.size() * sizeof(glm::vec3))));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

auto Tree(void) -> void {
	std::vector<glm::vec2> uvs;
	loadOBJ("./Assets/fir.obj", tree_verts, uvs, tree_norms);

	for (auto&& v : tree_verts) {
		float dst = glm::distance(glm::vec3(0.f, 0.f, 0.f), v);
		auto col = dst <= 1.f ? glm::vec3(.36f, .25f, .2f) : glm::vec3(.1f, .9f, .12f);
		tree_colrs.push_back(col);
	}

	glGenVertexArrays(1, &tree_vao);
	glBindVertexArray(tree_vao);

	glGenBuffers(1, &tree_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tree_vbo);
	glBufferData(GL_ARRAY_BUFFER, (tree_verts.size() * sizeof(glm::vec3)) + (tree_norms.size() * sizeof(glm::vec3)) + (tree_colrs.size() * sizeof(glm::vec3)), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, tree_verts.size() * sizeof(glm::vec3), tree_verts.data());
	glBufferSubData(GL_ARRAY_BUFFER, tree_verts.size() * sizeof(glm::vec3), tree_norms.size() * sizeof(glm::vec3), tree_norms.data());
	glBufferSubData(GL_ARRAY_BUFFER, (tree_verts.size() * sizeof(glm::vec3)) + (tree_norms.size() * sizeof(glm::vec3)), tree_colrs.size() * sizeof(glm::vec3), tree_colrs.data());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) (tree_verts.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*) ((tree_verts.size() * sizeof(glm::vec3)) + (tree_norms.size() * sizeof(glm::vec3))));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

auto Terrain(void) -> void {
	tess_shader.Create("./Assets/tess.tes", GL_TESS_EVALUATION_SHADER);
	tess_shader.Create("./Assets/tess.tcs", GL_TESS_CONTROL_SHADER);
	tess_shader.Create("./Assets/tess.fs", GL_FRAGMENT_SHADER);
	tess_shader.Create("./Assets/tess.vs", GL_VERTEX_SHADER);
	tess_shader.Compile();

	glGenVertexArrays(1, &tvao);
	glBindVertexArray(tvao);

	glGenBuffers(1, &tvbo);
	glBindBuffer(GL_ARRAY_BUFFER, tvbo);

	for (int i = 0; i < 256; ++i) {
		for (int j = 0; j < 256; ++j) {
			glm::vec2 left = glm::vec2(j * 1.f, i * 1.f);
			tverts.push_back(left + glm::vec2(0.f, 0.f));
			tverts.push_back(left + glm::vec2(1.f, 0.f));
			tverts.push_back(left + glm::vec2(0.f, 1.f));
			tverts.push_back(left + glm::vec2(1.f, 1.f));
		}
	}

	glBufferData(GL_ARRAY_BUFFER, tverts.size() * sizeof(glm::vec2), tverts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
	glEnableVertexAttribArray(0);

	tess_shader.Use();
	tess_shader.SetFloat("tess_Level", 5.f);
}

auto Cleanup(void) -> void {
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &tree_vbo);
	glDeleteBuffers(1, &ff_vbo);
	glDeleteBuffers(1, &tvbo);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &tree_vao);
	glDeleteVertexArrays(1, &ff_vao);
	glDeleteVertexArrays(1, &tvao);
	skybox.Destroy();
}

auto main(int argc, char ** argv) -> int {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(Globals::Width, Globals::Height);
	const int sy = (GetSystemMetrics(SM_CYSCREEN) - Globals::Height) >> 1;
	const int sx = (GetSystemMetrics(SM_CXSCREEN) - Globals::Width) >> 1;
	glutInitWindowPosition(sx, sy);
	WinID = glutCreateWindow(Constants::Title);
	
	if (glewInit() != GLEW_OK) {
		std::cerr << "fatal: failed to initialize glew." << std::endl;
		return 1;
	}

	Projection = glm::infinitePerspective(Constants::FOV, static_cast<float>(Globals::Width) / Globals::Height, Constants::Near);
	master.Create("./Assets/master.fs", GL_FRAGMENT_SHADER);
	master.Create("./Assets/master.vs", GL_VERTEX_SHADER);
	master.Compile();

	skybox_shader.Create("./Assets/skybox.fs", GL_FRAGMENT_SHADER);
	skybox_shader.Create("./Assets/skybox.vs", GL_VERTEX_SHADER);
	skybox_shader.Compile();

	/**
		Use texture 0.
	*/

	(void) skybox_shader.Use();
	skybox_shader.SetInteger("skybox", 0);
	skybox.Create();

	Terrain();
	Tree();
	Fly();

	DWORD dwVolume = ((DWORD) 0x4000 & 0x0000ffff) | ((DWORD) 0x4000 << 16);
	waveOutSetVolume(0, dwVolume);

	PlaySound(L"./Assets/night.wav", nullptr, SND_FILENAME | SND_LOOP | SND_ASYNC);
	glClearColor(0.086f, 0.090f, 0.090f, 1.0f);
	glViewport(0, 0, Globals::Width, Globals::Height);
	glutTimerFunc((unsigned)(Constants::Timestep), ::Tick, 0);
	glutWarpPointer(Globals::Width / 2, Globals::Height / 2);
	glutMotionFunc(::MouseMovement);
	glutKeyboardFunc(::ProcessKeydown);
	glutKeyboardUpFunc(::ProcessKeyup);
	glutMouseFunc(::MouseButton);
	glutDisplayFunc(::Display);
	glutReshapeFunc(::Resize);
	glutCloseFunc(::Cleanup);
	glutMainLoop();
	return 0;
}