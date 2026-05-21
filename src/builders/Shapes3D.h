#ifndef SHAPES3D_H
#define SHAPES3D_H

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

using namespace glm;
using namespace std;

namespace Shapes3D
{
    mat4 MVPMatrixCreator(mat4 M = mat4(1.0f), mat4 V = mat4(1.0f), mat4 P = mat4(1.0f));
    vector<vec4> applyMatrix(const vector<vec4> &vertices, const mat4 &MVP);
    GLfloat *toGLfloat(const vector<vec3> &v);
    GLfloat *toGLfloat(const vector<vec4> &v);

    class Shape
    {
    protected:
        vector<vec4> vertices;
        vector<vec3> colors;
        vector<Shape *> shapes;

    public:
        Shape();
        virtual ~Shape();

        virtual int numPoints() const;
        virtual int numVertices() const;
        virtual int numColors() const;

        virtual vector<vec4> getVertices() const;
        virtual vector<vec3> getColours() const;
    };

    class Triangle : public Shape
    {
    public:
        Triangle(vec3 = vec3(-0.8, 0.0, 0), vec3 = vec3(0.0, 0.8, 0), vec3 = vec3(0.8f, 0.0, 0), vec3 = vec3(1.0, 0.0, 0.0));
        int numVertices() const;
        int numColors() const;
        int numPoints() const;
    };

    class Square : public Shape
    {
    public:
        Square(vec3 BL = vec3(-0.5, -0.5, 0), vec3 TL = vec3(-0.5, 0.5, 0), vec3 TR = vec3(0.5, 0.5, 0), vec3 BR = vec3(0.5, -0.5, 0), vec3 = vec3(1.0, 0.0, 0.0));
    };

    class Box : public Shape
    {
    public:
        Box(vec3 FLB = vec3(-0.2, -0.2, -0.2), vec3 FLT = vec3(-0.2, 0.2, -0.2), vec3 FRT = vec3(0.2, 0.2, -0.2), vec3 FRB = vec3(0.2, -0.2, -0.2),
            vec3 BLB = vec3(-0.2, -0.2, 0.2), vec3 BLT = vec3(-0.2, 0.2, 0.2), vec3 BRT = vec3(0.2, 0.2, 0.2), vec3 BRB = vec3(0.2, -0.2, 0.2), vec3 = vec3(1.0, 0.0, 0.0));
    };

    class Scene : public Shape
    {
    public:    
        Scene(vec3 cameraPos = vec3(0,0,0));
    };

    class TriMesh : public Shape
    {
    public:
        TriMesh(int width, double yHeight, vec3 color);
    };

} // namespace Shapes3D

#endif /*SHAPES3D_H*/