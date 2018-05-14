#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 定义相机移动的几个可能的选项。 用作抽象来远离窗口系统特定的输入方法。
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    DOWN,
    UP
};

// 默认相机值：
const GLfloat YAW        = -90.0f; // 偏航角
const GLfloat PITCH      =  0.0f;  // 俯仰角
const GLfloat SPEED      =  3.0f;  // 相机速度
const GLfloat SENSITIVTY =  0.25f; // 鼠标灵敏度
const GLfloat ZOOM       =  45.0f; // 视野远近

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    GLfloat Yaw;
    GLfloat Pitch;

    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
        this->Position = position;
        this->WorldUp = up;
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }

    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
        this->Position = glm::vec3(posX, posY, posZ);
        this->WorldUp = glm::vec3(upX, upY, upZ);
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }

    // 获取欧拉角和LookAt矩阵
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }

    // 处理从任何键盘输入系统接收到的输入。 以摄像机定义的ENUM形式接受输入参数（从窗口系统中抽象）。
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
        GLfloat velocity = this->MovementSpeed * deltaTime;
        if (direction == FORWARD)
            this->Position += this->Front * velocity;
        if (direction == BACKWARD)
            this->Position -= this->Front * velocity;
        if (direction == LEFT)
            this->Position -= this->Right * velocity;
        if (direction == RIGHT)
            this->Position += this->Right * velocity;
        if (direction == DOWN)
            this->Position -= this->Up * velocity;
        if (direction == UP)
            this->Position += this->Up * velocity;
        
    }

    // 处理来自鼠标输入系统的输入。 期望在x和y方向上的偏移值。
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->Yaw   += xoffset;
        this->Pitch += yoffset;

        // 确保当俯仰角超出界限（89°）时，屏幕不会翻转。
        if (constrainPitch) {
            if (this->Pitch > 89.0f)
                this->Pitch = 89.0f;
            if (this->Pitch < -89.0f)
                this->Pitch = -89.0f;
        }

        // 使用更新的欧拉角更新前，右和上向量。
        this->updateCameraVectors();
    }

    // 处理从鼠标滚轮事件接收到的输入。 只需要在垂直轮轴上输入。
    void ProcessMouseScroll(GLfloat yoffset) {
        if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
            this->Zoom -= yoffset;
        if (this->Zoom <= 1.0f)
            this->Zoom = 1.0f;
        if (this->Zoom >= 45.0f)
            this->Zoom = 45.0f;
    }

private:
    // 使用更新的欧拉角更新前，右和上向量。
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        this->Front = glm::normalize(front);
        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
        this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
    }
};