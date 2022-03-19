#include "Camera.h"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup ,float near , float far )
{
    Near = near;
    Far = far;
    Fov = FOV;
    Position = position;
    WorldUp = worldup;
    //normalize 归一化变成单位向量
    //Forward 摄影机的“方向”（一个和朝向相反的向量）
    Front = glm::normalize(position - target);
    //Right 它代表Camera的右方，用世界的上方向与摄影机朝向叉乘
    Right = glm::normalize(glm::cross(WorldUp, Front));
    //UP  摄像机的上方向
    Up = glm::cross(Front, Right);
    Width=1920.f;
    Height=1080.f;
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float near, float far) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV), Near(near), Far(far)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
    Width = 1920.f;
    Height = 1080.f;
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float near, float far) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV),Near(near),Far(far)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
    Width = 1920.f;
    Height = 1080.f;
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
    
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == Camera_Movement::FORWARD)
        Position += Front * velocity;
    if (direction == Camera_Movement::BACKWARD)
        Position -= Front * velocity;
    if (direction == Camera_Movement::LEFT)
        Position -= Right * velocity;
    if (direction == Camera_Movement::RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Fov -= (float)yoffset;
    if (Fov < 1.0f)
        Fov = 1.0f;
    if (Fov > 45.0f)
        Fov = 45.0f;
}

/// <summary>
/// upadate CameraVector by Euler
/// </summary>
void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw))* cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    /*
    front.x = cos(glm::radians(Yaw));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    */
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
    FrontQuaternion = glm::quat(Front);
}

/// <summary>
/// update CameraVector by Quat(Maybe have some question)
/// </summary>
/// <param name="xoffset"></param>
/// <param name="yoffset"></param>
/// <param name="constrainPitch"></param>
void Camera::updateCameraVectorsByQuat1( float xoffset, float yoffset)
{
    Yaw += 90;
    xoffset *= 0.008f*MouseSensitivity;
    yoffset *= 0.008f * MouseSensitivity;

    Yaw = xoffset;
    Pitch= yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped

    if (Pitch > 90.0f)
        Pitch -= 90.0f;
    if (Pitch < -90.0f)
        Pitch += 90.0f;

    if (Yaw > 90.0f)
        Yaw -= 90.0f;
    if (Yaw < -90.0f)
        Yaw += 90.0f;
    

    glm::vec3 axis = glm::cross(Front, Up);
    glm::quat pitchQuat = glm::angleAxis(Pitch, axis);
    //determine heading quaternion from the camera up vector and the heading angle
    axis = glm::cross(Front, axis);
    glm::quat yawQuat = glm::angleAxis(Yaw, Up);
    //add the two quaternions
    glm::quat combinedRotation = pitchQuat*yawQuat;

    Front = glm::rotate(combinedRotation, Front);


    Front = glm::normalize(Front);
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));

    Yaw -= 90;
}

/// <summary>
/// update CameraVector by Quat (right)
/// </summary>
/// <param name="xoffset"></param>
/// <param name="yoffset"></param>
void Camera::updateCameraVectorsByQuat2(float xoffset, float yoffset)
{
    glm::vec3  ViewDest = Front + Right * xoffset * 0.008f * MouseSensitivity + Up * yoffset * 0.008f * MouseSensitivity;

    glm::quat rot1 = RotationBetweenVectors(Front, ViewDest);

    Front = glm::normalize(glm::rotate(rot1, Front));
 
    Right = glm::normalize(glm::cross(Front, Up));
   
    Up = glm::normalize(glm::cross(Right, Front));


}

/// <summary>
/// get rotation between two vectors
/// </summary>
/// <param name="start"></param>
/// <param name="dest"></param>
/// <returns></returns>
glm::quat Camera::RotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
    start = normalize(start);
    dest = normalize(dest);

    float cosTheta = dot(start, dest);
    glm::vec3 rotationAxis;

    if (cosTheta < -1 + 0.001f) {
        // special case when vectors in opposite directions:
        // there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
        if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
            rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

        rotationAxis = normalize(rotationAxis);
        return glm::angleAxis(180.0f, rotationAxis);
    }

    rotationAxis = cross(start, dest);

    float s = sqrt((1 + cosTheta) * 2);
    float invs = 1 / s;

    return glm::quat(
        s * 0.5f,
        rotationAxis.x * invs,
        rotationAxis.y * invs,
        rotationAxis.z * invs
    );

}

void Camera::ResetYawAndPitch()
{
    Yaw = YAW;
    Pitch = PITCH;
}
