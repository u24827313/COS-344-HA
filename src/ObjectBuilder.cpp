#include "ObjectBuilder.h"

ObjectBuilder::ObjectBuilder()
{

}

ObjectBuilder::~ObjectBuilder()
{

}

void ObjectBuilder::clone(ObjectBuilder *old)
{
    this->setObjects(old->getObjects());
}

std::vector<RenderObject *> ObjectBuilder::getObjects()
{
    return this->objects;
}

void ObjectBuilder::setObjects(std::vector<RenderObject *> objs)
{
    this->objects = objs;
}

void ObjectBuilder::setPosition(const glm::vec3 &p)
{
    for (auto* obj : objects) {
        obj->setPosition(p);  
    }
}

void ObjectBuilder::setRotation(const glm::vec3 &rDegrees)
{
    for (auto* obj : objects) {
        obj->setRotation(rDegrees);  
    }
}

void ObjectBuilder::setScale(const glm::vec3 &s)
{
    for (auto* obj : objects) {
        obj->setScale(s);  
    }
}

void ObjectBuilder::setScale(float uniform)
{
    for (auto* obj : objects) {
        obj->setScale(uniform);  
    }
}

void ObjectBuilder::setTexture(GLuint t)
{
    for (auto* obj : objects) {
        obj->setTexture(t);  
    }
}

void ObjectBuilder::setShaderProgram(GLuint s)
{
    for (auto* obj : objects) {
        obj->setShaderProgram(s);  
    }
}

void ObjectBuilder::render(const float *view, const float *projection, const glm::vec3 &cameraPos, const DirectionalLight &light)
{
    for (auto* obj : objects) {
        obj->render(view, projection, cameraPos, light );  
    }
    (void)view; (void)projection; (void)cameraPos; (void)light;
}

void ObjectBuilder::addObject(Hole *h)
{
    for (auto* obj : objects) {
        h->addObject(obj);  
    }
}

void ObjectBuilder::makeBarn(GLuint texture_, GLuint shaderProgram_)
{
    
}

void ObjectBuilder::makeWallTower(GLuint texture_, GLuint shaderProgram_)
{
    auto* cone1 = new RenderObject(
        RenderObject::createCone(12),
            texture_,
            shaderProgram_
        );
    cone1->setPosition(glm::vec3(1.2f, 1.0f, 0.0f));
    cone1->setScale(glm::vec3(1.0f, 0.75f, 1.0f)); 

    auto* cone2 = new RenderObject(
        RenderObject::createCone(12),
            texture_,
            shaderProgram_
        );
    cone2->setPosition(glm::vec3(-1.2f, 1.0f, 0.0f));
    cone2->setScale(glm::vec3(1.0f, 0.75f, 1.0f)); 

    auto* pillar1 = new RenderObject(
        RenderObject::createCylinder(12),
            texture_,
            shaderProgram_
        );
    pillar1->setPosition(glm::vec3(1.2f, 0.3f, 0.0f));
    pillar1->setScale(glm::vec3(0.4f, 1.0f, 0.4f)); 

    auto* pillar2 = new RenderObject(
        RenderObject::createCylinder(12),
            texture_,
            shaderProgram_
        );
    pillar2->setPosition(glm::vec3(-1.2f, 0.3f, 0.0f));
    pillar2->setScale(glm::vec3(0.4f, 1.0f, 0.4f));

    auto* wall1 = new RenderObject(
            RenderObject::createQuad(),
            texture_,
            shaderProgram_
        );
    wall1->setPosition(glm::vec3(2.0f, 0.5f, 0.0f));
    wall1->setRotation(glm::vec3(90.0f, 0.0f, 0.0f));
    wall1->setScale(glm::vec3(2.0f, 1.0f, 0.5f));

    auto* wall2 = new RenderObject(
            RenderObject::createQuad(),
            texture_,
            shaderProgram_
        );
    wall2->setPosition(glm::vec3(-2.0f, 0.5f, 0.0f));
    wall2->setRotation(glm::vec3(90.0f, 0.0f, 0.0f));
    wall2->setScale(glm::vec3(2.0f, 1.0f, 0.5f));

    objects.push_back(cone1);
    objects.push_back(cone2);
    objects.push_back(pillar1);
    objects.push_back(pillar2);
    objects.push_back(wall1);
    objects.push_back(wall2);
}

void ObjectBuilder::makeLoop(GLuint texture_, GLuint shaderProgram_)
{
    auto* slope1 = new RenderObject(
        RenderObject::createBox(0.4f, 0.05f, 1.0f),
            texture_,
            shaderProgram_
        );
    slope1->setPosition(glm::vec3(0.2f, 0.5f, -0.35f));
    slope1->setRotation(glm::vec3(45.0f, 0.0f, 0.0f));

    auto* slope2 = new RenderObject(
        RenderObject::createBox(0.8f, 0.05f, 1.0f),
            texture_,
            shaderProgram_
        );
    slope2->setPosition(glm::vec3(0.0f, 1.2f, -0.35f));
    slope2->setRotation(glm::vec3(-45.0f, 0.0f, 0.0f));

    auto* slope3 = new RenderObject(
        RenderObject::createBox(0.8f, 0.05f, 1.0f),
            texture_,
            shaderProgram_
        );
    slope3->setPosition(glm::vec3(0.0f, 1.2f, 0.35f));
    slope3->setRotation(glm::vec3(45.0f, 0.0f, 0.0f));

    auto* slope4 = new RenderObject(
        RenderObject::createBox(0.4f, 0.05f, 1.0f),
            texture_,
            shaderProgram_
        );
    slope4->setPosition(glm::vec3(-0.2f, 0.5f, 0.35f));
    slope4->setRotation(glm::vec3(-45.0f, 0.0f, 0.0f));

    auto* slope5 = new RenderObject(
        RenderObject::createBox(0.4f, 0.05f, 1.0f),
            texture_,
            shaderProgram_
        );
    slope5->setPosition(glm::vec3(-0.2f, 0.2f, -0.6f));

    auto* slope6 = new RenderObject(
        RenderObject::createBox(0.4f, 0.05f, 1.0f),
            texture_,
            shaderProgram_
        );
    slope6->setPosition(glm::vec3(0.2f, 0.2f, 0.6f));

    objects.push_back(slope1);
    objects.push_back(slope2);
    objects.push_back(slope3);
    objects.push_back(slope4);
    objects.push_back(slope5);
    objects.push_back(slope6);
}

void ObjectBuilder::makeBridge(GLuint texture_, GLuint shaderProgram_)
{
    
}

void ObjectBuilder::makeVolcano(GLuint texture_, GLuint shaderProgram_)
{
    auto* cone1 = new RenderObject(
        RenderObject::createCone(12),
            texture_,
            shaderProgram_
        );
    cone1->setPosition(glm::vec3(0.0f, 0.5f, 0.0f));
    cone1->setScale(glm::vec3(1.0f, 1.0f, 1.0f)); 

    auto* ridge = new RenderObject(
        RenderObject::createCylinder(12),
            texture_,
            shaderProgram_
        );
    ridge->setPosition(glm::vec3(0.0f, 1.2f, 0.0f));
    ridge->setScale(glm::vec3(0.3f, 0.3f, 0.3f));

    auto* slope = new RenderObject(
        RenderObject::createBox(0.4f, 0.1f, 1.0f),
            texture_,
            shaderProgram_
        );
    slope->setPosition(glm::vec3(0.0f, 0.9f, 0.6f));
    slope->setRotation(glm::vec3(45.0f, 0.0f, 0.0f));

    objects.push_back(cone1);
    objects.push_back(ridge);
    objects.push_back(slope);
}

void ObjectBuilder::makeFlag(GLuint textureA_, GLuint textureB_, GLuint shaderProgram_)
{
    auto* flagstick = new RenderObject(
        RenderObject::createCylinder(12),
            textureA_,
            shaderProgram_
        );
    flagstick->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));   // at the pin
    flagstick->setScale(glm::vec3(0.1f, 3.0f, 0.1f));        // thin and tall

    auto* flag = new RenderObject(
            RenderObject::createQuad(),
            textureB_,
            shaderProgram_
        );
    flag->setPosition(glm::vec3(0.5f, 2.7f, 0.0f));
    flag->setRotation(glm::vec3(90.0f, 0.0f, 0.0f));         // stand it up vertically
    flag->setScale(glm::vec3(1.0f, 0.6f, 1.0f));

    objects.push_back(flag);
    objects.push_back(flagstick);
}