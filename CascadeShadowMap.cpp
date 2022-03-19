#include "CascadeShadowMap.h"

int CascadeShadowMap::GetCascadeNum() const
{
    return CascadeNum;
}

CascadeShadowMap::CascadeShadowMap(int _CascadeNum, Camera& camera)
{
    CascadeNum = _CascadeNum;
    CalcClipDistance(camera);
}

void CascadeShadowMap::CalcClipDistance(Camera& camera)
{
    
    for (int i = 1; i < CascadeNum; i++)
    {
        float k = (float)i / (float)CascadeNum;
        CascadeFarPlaneDistantList.push_back(0.8f * (camera.Near * powf(camera.Fov, k)) + (1 - 0.8f) * (camera.Near + (camera.Far - camera.Near) * k));
    }
    CascadeFarPlaneDistantList.push_back(camera.Far);

    //float result= powf(camera.Far / camera.Near, 1.0 / float(CascadeNum));
    //for (size_t i = 1; i < CascadeNum; i++)
    //{
    //    CascadeFarPlaneDistantList.push_back(camera.Near+powf(result,i));
    //}

    for (auto& v : CascadeFarPlaneDistantList)
    {
        cout << v << endl;
    }
}


void CascadeShadowMap::GetClipPointByCamera(Camera& camera)
{
    float BoundingNearDistance = camera.Near;
    float BoundingFarDistance = CascadeFarPlaneDistantList[0];   
    float aspect = camera.Width / camera.Height;
    glm::vec4 CameraPosition = glm::vec4(camera.Position.x, camera.Position.y, camera.Position.z, 1.f);
    glm::vec4 CameraFront = glm::normalize(glm::vec4(camera.Front.x, camera.Front.y, camera.Front.z, 0.f));
    glm::vec4 CameraUp = glm::vec4(camera.Up.x, camera.Up.y, camera.Up.z, 0.f);
    glm::vec4 CameraRight = glm::vec4(camera.Right.x, camera.Right.y, camera.Right.z, 0.f);
    for (size_t i = 1; i < CascadeFarPlaneDistantList.size(); i++)
    {

        std::vector<glm::vec4> singleClipPointList;
  
        float nearHeight = tanf(camera.Fov / 2.0f) * BoundingNearDistance;
        float nearWidth = nearHeight * aspect;
        float farHeight = tanf(camera.Fov / 2.0f) * BoundingFarDistance;
        float farWidth = farHeight * aspect;
        glm::vec4 nearCenter = CameraPosition + CameraFront * BoundingNearDistance;
        glm::vec4 farCenter = CameraPosition + CameraFront * BoundingFarDistance;

        //近平面左下点
        singleClipPointList.push_back(nearCenter - CameraUp * nearHeight - CameraRight * nearWidth);
        //远平面右上点
        singleClipPointList.push_back(farCenter + CameraUp * farHeight + CameraRight * farWidth);

        ClipPointList.push_back(singleClipPointList);
        if (i < CascadeFarPlaneDistantList.size())
        {
            BoundingNearDistance = BoundingFarDistance;
            BoundingFarDistance = CascadeFarPlaneDistantList[i];
        }
    }
}

void CascadeShadowMap::GetClipPointByNDC(Camera& camera)
{
    float BoundingNearDistance = camera.Near;
    float BoundingFarDistance = CascadeFarPlaneDistantList[0];
    for (size_t i = 1; i < CascadeFarPlaneDistantList.size(); i++)
    {
        glm::mat4 projectionMat = glm::perspective(
            glm::radians(camera.Fov), (float)1920 / (float)1080, BoundingNearDistance,
            BoundingFarDistance);
        const auto inverseMat = glm::inverse(projectionMat *camera.GetViewMatrix());

        std::vector<glm::vec4> singleClipPointList;
        //NDC空间反推世界空间
        for (unsigned int x = 0; x < 2; ++x)
        {
            for (unsigned int y = 0; y < 2; ++y)
            {
                for (unsigned int z = 0; z < 2; ++z)
                {
                    const glm::vec4 inverseNdcPoint = inverseMat * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                    singleClipPointList.push_back(inverseNdcPoint / inverseNdcPoint.w);
                }
            }
        }
        ClipPointList.push_back(singleClipPointList);
        if (i < CascadeFarPlaneDistantList.size())
        {
            BoundingNearDistance = BoundingFarDistance;
            BoundingFarDistance = CascadeFarPlaneDistantList[i];
        }
    }
    

}





CascadeShadowMap::~CascadeShadowMap()
{
}

void CascadeShadowMap::UpdateCsm(Camera& camera, glm::vec4 lightPos, glm::vec4 lightDirection)
{
    ClipPointList.clear();
    LightProViewMat4List.clear();
    GetClipPointByCamera(camera);
    GetNewLightViewMat4(camera, lightPos, lightDirection);
}

void CascadeShadowMap::GetNewLightViewMat4(Camera& camera,glm::vec4 lightPos,glm::vec4 lightDirection)
{
    glm::vec3 lightDirectionVec3 = glm::vec3(lightDirection);
    glm::vec3 lightPosVec3 = glm::vec3(lightPos);

    glm::mat4 lightViewSpaceMat4 = glm::lookAt(lightPosVec3, lightPosVec3 + lightDirectionVec3, glm::vec3(0,1,0) );

    for (size_t i = 0; i < ClipPointList.size(); i++)
    {
        glm::vec4 ClipPointViewSpace = lightViewSpaceMat4 * ClipPointList[i][0];
        float minX = ClipPointViewSpace.x;
        float maxX = ClipPointViewSpace.x;

        float minY = ClipPointViewSpace.y;
        float maxY = ClipPointViewSpace.y;

        float minZ = ClipPointViewSpace.z;
        float maxZ = ClipPointViewSpace.z;


        glm::vec3 CurrentBoundingBoxCenter = glm::vec3(0.0f, 0.0f, 0.0f);
        for (size_t j = 0; j < ClipPointList[i].size(); j++)
        {
            maxZ = std::max(maxZ, (lightViewSpaceMat4 * ClipPointList[i][j]).z);
            minZ = std::max(minZ, (lightViewSpaceMat4 * ClipPointList[i][j]).z);
            CurrentBoundingBoxCenter +=glm::vec3(ClipPointList[i][j]);
        }
        CurrentBoundingBoxCenter /= ClipPointList[i].size();
        
        glm::mat4 ShadowCameraViewMat4 = glm::lookAt(CurrentBoundingBoxCenter+lightDirectionVec3, CurrentBoundingBoxCenter, glm::vec3(0, 1, 0));
        // glm::mat4 ShadowCameraViewMat4 = glm::lookAt(CurrentBoundingBoxCenter-glm::normalize(lightDirectionVec3)*(maxZ-minZ)/2.0f, CurrentBoundingBoxCenter, glm::vec3(0, 1, 0));
        ClipPointViewSpace = ShadowCameraViewMat4 * ClipPointList[i][0];
        minX = ClipPointViewSpace.x;
        maxX = ClipPointViewSpace.x;

        minY = ClipPointViewSpace.y;
        maxY = ClipPointViewSpace.y;
        minZ = ClipPointViewSpace.z;
        maxZ = ClipPointViewSpace.z;
        for (size_t j = 0; j < ClipPointList[i].size(); j++)
        {
            ClipPointViewSpace = ShadowCameraViewMat4 * ClipPointList[i][j];
            minX = std::min(minX, ClipPointViewSpace.x);
            maxX = std::max(maxX, ClipPointViewSpace.x);

            minY = std::min(minY, ClipPointViewSpace.y);
            maxY = std::max(maxY, ClipPointViewSpace.y);

            minZ = std::min(minZ, ClipPointViewSpace.z);
            maxZ = std::max(maxZ, ClipPointViewSpace.z);
        }
        constexpr float zMult = 10.0f;
        if (minZ < 0)
        {
            minZ *= zMult;
        }
        else
        {
            minZ /= zMult;
        }
        if (maxZ < 0)
        {
            maxZ /= zMult;
        }
        else
        {
            maxZ *= zMult;
        }
        glm::mat4 ShadowCameraProjectionMat4 = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
        LightProViewMat4List.push_back(ShadowCameraProjectionMat4 * ShadowCameraViewMat4);
    }
}
