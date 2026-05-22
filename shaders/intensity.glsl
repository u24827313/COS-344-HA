#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
out vec3 fragmentColor;

void main(){

    gl_Position.xyzw = vertexPosition_modelspace;
    
    if (vertexColor.x > 0.5) {
        fragmentColor.x = 1.0;
    }
    else {
        fragmentColor.x = vertexColor.x /2;
    }

    if (vertexColor.y > 0.5) {
        fragmentColor.y = 1.0;
    }
    else {
        fragmentColor.y = vertexColor.y /2;
    }

    if (vertexColor.z > 0.5) {
        fragmentColor.z = 1.0;
    }
    else {
        fragmentColor.z = vertexColor.z /2;
    }
}