#include "ParticleSystem.h"

ParticleSystem::~ParticleSystem()
{
	destroy_particle_system();
}

void ParticleSystem::init_particle_system(const GLchar* vertexPath, const GLchar* fragmentPath, const char* geometryPath, const char* computePath)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	{ // Particle SSBO
		glGenBuffers(1, &particle_ssbo);
		glBindBuffer(GL_ARRAY_BUFFER, particle_ssbo);
		glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), &particles[0], GL_DYNAMIC_DRAW);

	}


	{ // Setup Vertex Attributes
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)(sizeof(float) * 2));
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)(sizeof(float) * 4));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)(sizeof(float) * 5));

		glVertexAttribDivisor(0, 1);
		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
	}

	glBindVertexArray(0);

	{ // Shaders
		compute_shader = new Shader(
			nullptr,
			nullptr,
			nullptr,
			computePath
		);
		compute_shader->SetUniform1f("time", 0.f);
		compute_shader->SetUniform2f("resolution", 1920.0f, 1080.0f);
		compute_shader->SetUniform2f("attractor_radii", 400.f, 200.f);


		render_shader = new Shader(
			vertexPath,
			fragmentPath,
			geometryPath,
			nullptr
		);
		render_shader->SetUniform1f("vertex_count", particles.size());
		render_shader->setMat4("projection", orthographic_perspective(1920.0f, 1080.0f, -1.f, 1.f));
	}
}

void ParticleSystem::draw_particle_system(float Runtime)
{
	// Invoke Compute Shader and wait for all memory access to SSBO to safely finish
	compute_shader->use();
	compute_shader->SetUniform1f("time", Runtime );
	compute_shader->SetUniform2f("resolution", 1920.0f, 1080.0f);
	compute_shader->SetUniform2f("attractor_radii", 400.f, 200.f);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particle_ssbo);
	glDispatchCompute((particles.size() / 128) + 1, 1, 1);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);



	// Render the results
	render_shader->use();
	glBindVertexArray(vao);
	render_shader->SetUniform1f("vertex_count", particles.size());
	render_shader->setMat4("projection", orthographic_perspective(1920.0f, 1080.0f, -1.f, 1.f));



	glPointSize(2.0f);
	glDrawArraysInstanced(GL_POINTS, 0, 1 ,particles.size());
	//glDrawArrays(GL_POINTS, 0, particles.size());
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void ParticleSystem::destroy_particle_system()
{
	delete compute_shader;
	delete render_shader;
	glDeleteBuffers(1, &particle_ssbo);
	glDeleteVertexArrays(1, &vao);
}

std::string ParticleSystem::print_compute_shader_info()
{
	std::stringstream ss;
	ss << "Compute Shader Capabilities:" << std::endl;
	ss << "GL_MAX_COMPUTE_WORK_GROUP_COUNT:" << std::endl;
	for (int i : {0, 1, 2}) {
		int tmp = 0;
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, i, &tmp);
		ss << tmp << std::endl;
	}

	ss << "GL_MAX_COMPUTE_WORK_GROUP_SIZE:" << std::endl;
	for (int i : {0, 1, 2}) {
		int tmp = 0;
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, i, &tmp);
		ss << tmp << std::endl;
	}

	return ss.str();
}

glm::mat4 ParticleSystem::orthographic_perspective(float w, float h, float nZ, float fZ) {
	float z = fZ - nZ;
	float i = -w / w;
	float j = -h / h;
	float k = -(fZ + nZ) / z;

	return glm::mat4(
		glm::vec4(2.f / w, 0.f, 0.f, 0.f),
		glm::vec4(0.f, 2.f / h, 0.f, 0.f),
		glm::vec4(0.f, 0.f, -2.f / z, 0.f),
		glm::vec4(i, j, k, 1.f)
	);
}