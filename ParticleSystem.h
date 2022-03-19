#pragma once

#include "Shader.h"
#include <glm.hpp>
#include "utils.h"
#include <vector>
using namespace std;

struct Particle
{
	Particle():
		position(utils::gen_random(0.f,1920.f), utils::gen_random(0.f, 1080.f)),
		velocity(1.f),
		scale(utils::gen_random(10.f, 16.f)),
		mass(2.0f)
	{

	}

	glm::vec2 position;
	glm::vec2 velocity;
	float scale;
	float mass;
};

class ParticleSystem
{
private:
	GLuint vao;
	GLuint Pos_ssbo;
	GLuint Vel_ssbo;
	GLuint particle_ssbo;
	std::vector<Particle> particles;

public:
	ParticleSystem(int num_particles = 10 ,const GLchar * vertexPath = nullptr, const GLchar * fragmentPath = nullptr, const char* geometryPath = nullptr, const char* computePath = nullptr) : particles()
	{
		for (int i = 0; i < num_particles; ++i) {
			particles.push_back(Particle());
		}
		init_particle_system(vertexPath, fragmentPath, geometryPath, computePath);

	}
	virtual ~ParticleSystem();
	Shader* compute_shader;
	Shader* render_shader;

	void init_particle_system(const GLchar* vertexPath = nullptr, const GLchar* fragmentPath = nullptr, const char* geometryPath = nullptr, const char* computePath = nullptr);
	void draw_particle_system(float Runtime);
	void destroy_particle_system();
	glm::mat4 orthographic_perspective(float w, float h, float nZ, float fZ);
	std::string print_compute_shader_info();

};

