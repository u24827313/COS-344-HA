#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
out vec3 fragmentColor;

void main(){

    gl_Position.xyzw = vertexPosition_modelspace;
    
    fragmentColor.x = vertexColor.x * 0.393 + vertexColor.y * 0.769 + vertexColor.z * 0.189;
    fragmentColor.y = vertexColor.x * 0.349 + vertexColor.y * 0.686 + vertexColor.z * 0.168;
    fragmentColor.z = vertexColor.x * 0.272 + vertexColor.y * 0.534 + vertexColor.z * 0.131;
}