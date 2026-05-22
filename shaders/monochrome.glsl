#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
out vec3 fragmentColor;

void main(){

    gl_Position.xyzw = vertexPosition_modelspace;
    
    fragmentColor.x = (vertexColor.x + vertexColor.y + vertexColor.z) / 3;
    fragmentColor.y = (vertexColor.x + vertexColor.y + vertexColor.z) / 3;
    fragmentColor.z = (vertexColor.x + vertexColor.y + vertexColor.z) / 3;
}