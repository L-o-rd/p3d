#include "Shader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::~Shader() {
    std::cout << "Shader(" << this->id << ") destroyed." << std::endl;
	glDeleteProgram(this->id);
}

auto Shader::GetID(void) const -> const GLuint& {
	return this->id;
}

auto Shader::SetVec3f(const char * name, const GLfloat& x, const GLfloat& y, const GLfloat& z) -> void {
    auto address = glGetUniformLocation(this->id, name);
    glUniform3f(address, x, y, z);
}

auto Shader::SetVec2f(const char * name, const GLfloat& x, const GLfloat& y) -> void {
    auto address = glGetUniformLocation(this->id, name);
    glUniform2f(address, x, y);
}

auto Shader::SetMatrix(const char * name, const GLfloat * matrix) -> void {
	auto address = glGetUniformLocation(this->id, name);
	glUniformMatrix4fv(address, 1, GL_FALSE, matrix);
}

auto Shader::SetInteger(const char * name, const GLint& value) -> void {
    auto address = glGetUniformLocation(this->id, name);
    glUniform1i(address, value);
}

auto Shader::SetFloat(const char * name, const GLfloat& value) -> void {
    auto address = glGetUniformLocation(this->id, name);
    glUniform1f(address, value);
}

auto Shader::Use(void) -> Shader& {
	return glUseProgram(this->id), *this;
}

auto Shader::Create(const char * src, const int& type) -> void {
    const char * stype = nullptr;

    switch (type) {
        case GL_VERTEX_SHADER: stype = "Vertex"; break;
        case GL_FRAGMENT_SHADER: stype = "Fragment"; break;
        case GL_TESS_CONTROL_SHADER: stype = "TCS"; break;
        case GL_TESS_EVALUATION_SHADER: stype = "TES"; break;
        default: {
            std::cerr << "Shader::Create(" << type << ") invalid shader type." << std::endl;
            return;
        }
    }

    auto ssm = std::stringstream();
    auto vin = std::ifstream(src);
    ssm << vin.rdbuf();
    vin.close();

    auto ssource = ssm.str();
    auto source = ssource.c_str();
    GLchar info[1024];
    GLint success;

    GLuint pid = glCreateShader(type);
    glShaderSource(pid, 1, &source, nullptr);
    glCompileShader(pid);
    glGetShaderiv(pid, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(pid, sizeof(info), nullptr, &info[0]);
        std::cerr << "Shader::Compile [" << stype << "]: " << std::endl;
        std::cerr << info << std::endl;
    }

    switch (type) {
        case GL_VERTEX_SHADER: {
            this->has_vertex = true;
            this->vid = pid;
            return;
        }

        case GL_FRAGMENT_SHADER: {
            this->has_fragment = true;
            this->fid = pid;
            return;
        }

        case GL_TESS_CONTROL_SHADER: {
            this->has_tcs = true;
            this->tcid = pid;
            return;
        }

        case GL_TESS_EVALUATION_SHADER: {
            this->has_tes = true;
            this->teid = pid;
            return;
        }

        default: return;
    }
}

void Shader::Compile(void) {
    this->id = glCreateProgram();
    if (this->has_vertex) glAttachShader(this->id, this->vid);
    if (this->has_fragment) glAttachShader(this->id, this->fid);
    if (this->has_tcs) glAttachShader(this->id, this->tcid);
    if (this->has_tes) glAttachShader(this->id, this->teid);
    glLinkProgram(this->id);

    GLchar info[1024] = {0};
    GLint success;

    glGetProgramiv(this->id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->id, sizeof(info), nullptr, &info[0]);
        std::cerr << "Shader::Compile [Program]: " << std::endl;
        std::cerr << info << std::endl;
    }

    if (this->has_vertex) glDeleteShader(this->vid);
    if (this->has_fragment) glDeleteShader(this->fid);
    if (this->has_tcs) glDeleteShader(this->tcid);
    if (this->has_tes) glDeleteShader(this->teid);
}