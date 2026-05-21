#include "Shapes3D.h"

using namespace Shapes3D;

Shape::Shape()
{
}

Shape::~Shape()
{
    for (vector<Shape *>::iterator it = shapes.begin(); it != shapes.end(); it++)
    {
        delete (*it);
    }
}

int Shape::numPoints() const
{
    int count = 0;
    for (vector<Shape *>::const_iterator it = shapes.begin(); it != shapes.end(); it++)
    {
        count += (*it)->numPoints();
    }
    return count;
}

int Shape::numVertices() const
{
    int count = 0;
    for (vector<Shape *>::const_iterator it = shapes.begin(); it != shapes.end(); it++)
    {
        count += (*it)->numVertices();
    }
    return count;
}

int Shape::numColors() const
{
    int count = 0;
    for (vector<Shape *>::const_iterator it = shapes.begin(); it != shapes.end(); it++)
    {
        count += (*it)->numColors();
    }
    return count;
}

vector<vec4> Shape::getVertices() const
{
    if (!vertices.empty())
    {
        return vertices;
    }
    else
    {
        vector<vec4> result;
        for (vector<Shape *>::const_iterator it = shapes.begin(); it != shapes.end(); it++)
        {
            vector<vec4> tempResults = (*it)->getVertices();
            for (vector<vec4>::iterator it2 = tempResults.begin(); it2 != tempResults.end(); it2++)
            {
                result.push_back((*it2));
            }
        }
        return result;
    }
}

vector<vec3> Shape::getColours() const
{
    if (!colors.empty())
    {
        return colors;
    }
    else
    {
        vector<vec3> result;
        for (vector<Shape *>::const_iterator it = shapes.begin(); it != shapes.end(); it++)
        {
            vector<vec3> tempResults = (*it)->getColours();
            for (vector<vec3>::iterator it2 = tempResults.begin(); it2 != tempResults.end(); it2++)
            {
                result.push_back((*it2));
            }
        }
        return result;
    }
}

mat4 Shapes3D::MVPMatrixCreator(mat4 M, mat4 V, mat4 P)
{
    return (P * (V * M));
}

vector<vec4> Shapes3D::applyMatrix(const vector<vec4> &vertices, const mat4 &MVP)
{
    vector<vec4> worldVertices;

    for (vector<vec4>::const_iterator it = vertices.begin(); it != vertices.end(); it++)
    {
        vec4 v(*it);
        vec4 vPrime = MVP * v;
        worldVertices.push_back(vPrime);
    }

    return worldVertices;
}

GLfloat *Shapes3D::toGLfloat(const vector<vec3> &v)
{
    GLfloat *result = new GLfloat[v.size() * 3];
    int i = 0;
    for (vector<vec3>::const_iterator it = v.begin(); it != v.end(); it++)
    {
        result[i++] = (*it)[0];
        result[i++] = (*it)[1];
        result[i++] = (*it)[2];
    }
    return result;
}

GLfloat *Shapes3D::toGLfloat(const vector<vec4> &v)
{
    GLfloat *result = new GLfloat[v.size() * 4];
    int i = 0;
    for (vector<vec4>::const_iterator it = v.begin(); it != v.end(); it++)
    {
        result[i++] = (*it)[0];
        result[i++] = (*it)[1];
        result[i++] = (*it)[2];
        result[i++] = (*it)[3];
    }
    return result;
}

Box::Box(vec3 FLB, vec3 FLT, vec3 FRT, vec3 FRB, vec3 BLB, vec3 BLT, vec3 BRT, vec3 BRB, vec3 color)
{
    shapes.push_back(new Square(FLB, FLT, FRT, FRB, color));
    shapes.push_back(new Square(FRB, FRT, BRT, BRB, color));
    shapes.push_back(new Square(BRB, BRT, BLT, BLB, color));
    shapes.push_back(new Square(BLB, BLT, FLT, FLB, color));
    shapes.push_back(new Square(FLT, BLT, BRT, FRT, color));
    shapes.push_back(new Square(BLB, FLB, FRB, BRB, color));
}

Square::Square(vec3 BL, vec3 TL, vec3 TR, vec3 BR, vec3 color)
{
    shapes.push_back(new Triangle(TR, TL, BL, color));
    shapes.push_back(new Triangle(BR, TR, BL, color));
}

Triangle::Triangle(vec3 point1, vec3 point2, vec3 point3, vec3 color)
{

    vertices.push_back(vec4(point1, 1.0));
    vertices.push_back(vec4(point2, 1.0));
    vertices.push_back(vec4(point3, 1.0));

    colors.push_back(color);
    colors.push_back(color);
    colors.push_back(color);
}

int Triangle::numVertices() const
{
    return 12;
}

int Triangle::numColors() const
{
    return 9;
}

int Triangle::numPoints() const
{
    return 3;
}

Shapes3D::Scene::Scene(vec3 cameraPos)
{
    vec3 base[8];

    {
        base[0] = vec3(-1.0, -1.0, -1.0);
        base[1] = vec3(-1.0, -0.95, -1.0);
        base[2] = vec3(1.0, -0.95, -1.0);
        base[3] = vec3(1.0, -1.0, -1.0);
        base[4] = vec3(-1.0, -1.0, 1.0);
        base[5] = vec3(-1.0, -0.95, 1.0);
        base[6] = vec3(1.0, -0.95, 1.0);
        base[7] = vec3(1.0, -1.0, 1.0);
    }

    vec3 boxy[8];

    {
        boxy[0] = vec3(0.4, -0.2, -0.2);
        boxy[1] = vec3(0.4, 0.2, -0.2);
        boxy[2] = vec3(0.8, 0.2, -0.2);
        boxy[3] = vec3(0.8, -0.2, -0.2);
        boxy[4] = vec3(0.4, -0.2, 0.2);
        boxy[5] = vec3(0.4, 0.2, 0.2);
        boxy[6] = vec3(0.8, 0.2, 0.2);
        boxy[7] = vec3(0.8, -0.2, 0.2);
    }

    shapes.push_back(new Box(base[0], base[1], base[2], base[3], base[4], base[5], base[6], base[7], vec3(0.1, 0.5, 0.1)));
    shapes.push_back(new Box(boxy[0], boxy[1], boxy[2], boxy[3], boxy[4], boxy[5], boxy[6], boxy[7], vec3(0.8, 0.5, 0.1)));

    shapes.push_back(new TriMesh(10, 0.5, vec3(0.9, 0.0, 0.5)));
}

Shapes3D::TriMesh::TriMesh(int width, double yHeight, vec3 color)
{
    vector<vec3> points;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            if (i % 2 == 0) {
                double xPos = i / width;
                double zPos = j / width;

                points.push_back(vec3(xPos, yHeight, zPos));
            }
            else {
                double xPos = (i + 1) / width;
                double zPos = j / width;

                points.push_back(vec3(xPos, yHeight, zPos));
            }
        }
    }

    cout << points.size();

    for (int i = 0; i < width - 1; i++) {
        for (int j = 0; j < width - 1; j++) {
            if (i % 2 == 0) {
                shapes.push_back(new Triangle(points[i * width + j + 1], points[i * width + j], points[(i + 1) * width + j + 1], color));
            }
            else {
                shapes.push_back(new Triangle(points[i * width + j], points[(i + 1) * width + j], points[(i + 1) * width + j + 1], color));
            };
        }
    }
}