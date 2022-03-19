#include "Shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath, const GLchar* computePath)
{
	// 1. ���ļ�·���л�ȡ����/Ƭ����ɫ��
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::string computeCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	std::ifstream cShaderFile;
	// ��֤ifstream��������׳��쳣�� 
	vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit); 
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// ���ļ�
		if (vertexPath != nullptr&& fragmentPath!=nullptr)
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream; // ��ȡ�ļ��Ļ������ݵ��������� 
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// �ر��ļ������� 
			vShaderFile.close();
			fShaderFile.close();
			// ת����������string 
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
		if (computePath != nullptr)
		{
			cShaderFile.open(computePath);
			std::stringstream cShaderStream;
			cShaderStream << cShaderFile.rdbuf();
			cShaderFile.close();
			computeCode = cShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str(); 
	const char* fShaderCode = fragmentCode.c_str();


	// 2. ������ɫ��
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];
	if (vertexPath != nullptr && fragmentPath != nullptr)
	{
		// ������ɫ�� 
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		// ��ӡ�����������еĻ��� 
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog); std::cout << vertexPath <<"---" << "ERROR::SHADER::vertex::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
		// Ƭ����ɫ��
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		// ��ӡ�����������еĻ��� 
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog); std::cout << fragmentPath << "---" << "ERROR::SHADER::fragment::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
	}
	//������ɫ��
	unsigned int geometry;
	if (geometryPath != nullptr)
	{
		const char* gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(geometry, 512, NULL, infoLog); std::cout << geometryPath << "---" << "ERROR::SHADER::fragment::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
	}
	unsigned int compute;
	if (computePath != nullptr)
	{
		const char* cShaderCode = computeCode.c_str();
		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &cShaderCode, NULL);
		glCompileShader(compute);
		glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(compute, 512, NULL, infoLog); std::cout << computePath << "---" << "ERROR::SHADER::compute::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
	}
	// ��ɫ������
	ID = glCreateProgram(); 
	if (vertexPath != nullptr && fragmentPath != nullptr)
	{
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
	}
	if (geometryPath != nullptr)
		glAttachShader(ID, geometry);
	if (computePath != nullptr)
		glAttachShader(ID, compute);
	glLinkProgram(ID);
	// ��ӡ���Ӵ�������еĻ��� 
	glGetProgramiv(ID, GL_LINK_STATUS, &success); 
	if(!success) 
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog); std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// ɾ����ɫ���������Ѿ����ӵ����ǵĳ������ˣ��Ѿ�������Ҫ�� 
	if (vertexPath != nullptr && fragmentPath != nullptr)
	{
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	if (geometryPath != nullptr)
		glDeleteShader(geometry);
	if (computePath != nullptr)
		glDeleteShader(compute);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// ------------------------------------------------------------------------
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniform3f(const char* paraNameString, glm::vec3 param)
{
	glUniform3f(glGetUniformLocation(ID, paraNameString), param.x, param.y, param.z);
}

void Shader::SetUniform1f(const char* paraNameString, float param)
{
	glUniform1f(glGetUniformLocation(ID, paraNameString), param);
}

void Shader::SetUniform2f(const char* paraNameString, float param1,float param2)
{
	glUniform2f(glGetUniformLocation(ID, paraNameString),param1,param2);
}

void Shader::SetUniform1i(const char* paraNameString, int slot)
{
	glUniform1i(glGetUniformLocation(ID, paraNameString), slot);
}