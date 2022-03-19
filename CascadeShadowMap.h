#pragma once
#include "Shader.h"
#include "Camera.h"
#include <glm.hpp>
#include "utils.h"
#include <vector>
using namespace std;

class CascadeShadowMap
{
private:
	int CascadeNum;
	vector<vector<glm::vec4>> ClipPointList;
	//计算划分的距离对数
	void CalcClipDistance(Camera& camera);
	//在world space计算出视野切分的各个点
	void GetClipPoint(Camera& camera);
	//计算每个包围盒的变换矩阵
	void GetNewLightViewMat4(Camera& camera,glm::vec4 lightPos, glm::vec4 lightDirection);
public:
	
	
	vector<float> CascadeFarPlaneDistantList;
	vector<glm::mat4> LightProViewMat4List;
	int GetCascadeNum() const;
	CascadeShadowMap(int _CascadeNum,Camera& camera);
	virtual~CascadeShadowMap();

	void UpdateCsm(Camera& camera, glm::vec4 lightPos, glm::vec4 lightDirection);
};

