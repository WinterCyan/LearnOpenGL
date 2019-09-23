//
//  Camera.h
//  OpenGL
//
//  Created by Winter Cyan on 2019/7/10.
//  Copyright © 2019 Winter Cyan. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

/*
 camera attributes:
    about movement: translation, speed, sensitivity
    about attitude: position, pitch, yaw
 
 function:
    setup attributes, get&update view-matrix, handle mouse&keyboard input.
*/

enum camera_movement{
    FORWARD, BACKWARD, LEFT, RIGHT
};
const float YAW = -90.f;
const float PITCH = 0.f;
const float SPEED = 2.5f;
const float SENSITIVITY = .1f;
const float ZOOM = 45.f;

class MyCamera{
public:
    glm::vec3 Position, Front, Up, Right, WorldUp;
    float Yaw, Pitch, MovementSpeed, MouseSensitivity, Zoom;
    
    MyCamera(glm::vec3 position = glm::vec3(.0f, .0f, .0f), glm::vec3 up = glm::vec3(.0f, 1.f, .0f), float yaw = YAW, float pitch = PITCH): Front(glm::vec3(.0f, .0f, -1.f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    MyCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch): Front(glm::vec3(.0f, .0f, -1.f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM){
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    glm::mat4 getViewMatrix(){
        return glm::lookAt(Position, Position+Front, Up);
    }
    void ProcessKeyboard(camera_movement direction, float deltaTime, float speed){
        float velocity = MovementSpeed * deltaTime * speed;
        if (direction == FORWARD) {
            Position += Front*velocity;
        }
        if (direction == BACKWARD) {
            Position -= Front*velocity;
        }
        if (direction == LEFT) {
            Position -= Right*velocity;
        }
        if (direction == RIGHT) {
            Position += Right*velocity;
        }
    }
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true){
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;
        
        Yaw += xoffset;
        Pitch += yoffset;
        if (constrainPitch) {
            if (Pitch >= 89.f) {
                Pitch = 89.f;
            }
            if (Pitch <= -89.f) {
                Pitch = -89.f;
            }
        }
//        std::cout<<"Pitch: "<<Pitch<<", Yaw: "<<Yaw<<std::endl;
        updateCameraVectors();
    }
    void ProcessMouseScroll(float yoffset){
        if (Zoom>=1.f && Zoom <=89.f) {
            Zoom -= yoffset;
        }
        if (Zoom <= 1.f) {
            Zoom = 1.f;
        }
        if (Zoom >= 89.f) {
            Zoom = 89.f;
        }
    }
    
private:
    void updateCameraVectors(){
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw))*cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = cos(glm::radians(Pitch))*sin(glm::radians(Yaw));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

#endif /* Camera_h */
