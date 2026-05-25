#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "src/shader.hpp"

#include "src/TextureLoader.h"
#include "src/Terrain.h"
#include "src/SkyBox.h"
#include "src/Hole.h"
#include "src/Hole01.h"
#include "src/Hole02.h"
#include "src/Hole03.h"
#include "src/Hole04.h"
#include "src/Hole07.h"
#include "src/Hole08.h"
#include "src/Hole09.h"
#include "src/Hole10.h"
#include "src/Hole11.h"
#include "src/Hole12.h"
#include "src/Hole13.h"
#include "src/Hole14.h"
#include "src/Hole15.h"
#include "src/Hole16.h"
#include "src/GolfCourse.h"
#include "src/RenderObject.h"
#include "src/ObjectBuilder.h"
#include "src/MathUtils.h"
#include "src/drone.h"

using namespace glm;
using namespace std;

// camer orientation variables
static float camYaw = -90.0f;
static float camPitch = 0.0f;
static float lastX = 750.0f;  
static float lastY = 500.0f;  
static bool  firstMouse = true;
static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
static bool nightView = false, droneView = false;

static double lastTime, currentTime, deltaTime;
int state = 0;
double delay = 0.0;

mat4 makeMVP(mat4 M, mat4 V, mat4 P) {
    return (P * (V * M));
}

static void processControls(GLFWwindow* window, mat4 &view, mat4 &model, mat4 &projection, mat4 &MVP, float &cameraAngle, vec3 &cameraPos, vec3 &cameraUp, Drone& drone) {
    double speed = 5 * deltaTime;

    // Left-Front-Right-Back Movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation[3] = vec4(0, 0, speed, 1);
        view = translation * view;
        MVP = makeMVP(model, view, projection);

        drone.position = vec3(drone.position.x, drone.position.y, drone.position.z - speed);
        cameraPos = vec3(cameraPos.x, cameraPos.y, cameraPos.z + speed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation[3] = vec4(0, 0, -speed, 1);
        view = translation * view;
        MVP = makeMVP(model, view, projection);

        drone.position = vec3(drone.position.x, drone.position.y, drone.position.z + speed);
        cameraPos = vec3(cameraPos.x, cameraPos.y, cameraPos.z - speed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation[3] = vec4(-speed, 0, 0, 1);
        view = translation * view;
        MVP = makeMVP(model, view, projection);

        drone.position = vec3(drone.position.x + speed, drone.position.y, drone.position.z);
        cameraPos = vec3(cameraPos.x - speed, cameraPos.y, cameraPos.z);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation[3] = vec4(speed, 0, 0, 1);
        view = translation * view;
        MVP = makeMVP(model, view, projection);

        drone.position = vec3(drone.position.x - speed, drone.position.y, drone.position.z);
        cameraPos = vec3(cameraPos.x + speed, cameraPos.y, cameraPos.z);
    }

    // View Center Rotation
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        view = glm::rotate(view, float(speed / 10), glm::vec3(0.0f, 1.0f, 0.0f));
        MVP = makeMVP(model, view, projection);

        drone.rotation = vec3(drone.rotation.x, drone.rotation.y + degrees(speed / 10), drone.rotation.z);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        view = glm::rotate(view, -float(speed / 10), glm::vec3(0.0f, 1.0f, 0.0f));
        MVP = makeMVP(model, view, projection);
        cameraAngle -= 0.0001f;

        drone.rotation = vec3(drone.rotation.x, drone.rotation.y - degrees(speed / 10), drone.rotation.z);
    }

    // Up-Down Movement
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation[3] = vec4(0, -speed, 0, 1);
        view = translation * view;
        MVP = makeMVP(model, view, projection);

        drone.position = vec3(drone.position.x, drone.position.y + speed, drone.position.z);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation[3] = vec4(0, speed, 0, 1);
        view = translation * view;
        MVP = makeMVP(model, view, projection);

        drone.position = vec3(drone.position.x, drone.position.y - speed, drone.position.z);
    }

    // Scene Scaling
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation[0] = vec4((1 - speed / 10), 0, 0, 0);
        translation[1] = vec4(0, (1 - speed / 10), 0, 0);
        translation[2] = vec4(0, 0, (1 - speed / 10), 0);
        view = view * translation;
        MVP = makeMVP(model, view, projection);
    }
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation[0] = vec4((1 + speed / 10), 0, 0, 0);
        translation[1] = vec4(0, (1 + speed / 10), 0, 0);
        translation[2] = vec4(0, 0, (1 + speed / 10), 0);
        view = view * translation;
        MVP = makeMVP(model, view, projection);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        float aspect = (float)fbWidth / (float)fbHeight;

        model = mat4(1.0f);
        cameraAngle = 0.0f;
        cameraPos = glm::vec3(0.0f, 2.0f, 10.0f);
        cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
        view = lookAt(
            cameraPos,               // Camera position
            cameraPos + cameraFront, // Look at point
            cameraUp                 // Up vector
        );
        projection = perspective(glm::radians(45.0f), aspect, 1.0f, -500.0f);

        MVP = makeMVP(model, view, projection);

        drone.position = vec3(0.0f, 2.0f, 10.0f);
        drone.rotation = vec3(0.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation = glm::rotate(translation, +float(speed / 10), glm::vec3(1.0f, 0.0f, 0.0f));
        view = view * translation;
        MVP = makeMVP(model, view, projection);

        drone.rotation = vec3(drone.rotation.x - degrees(speed / 10), drone.rotation.y, drone.rotation.z);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation = glm::rotate(translation, -float(speed / 10), glm::vec3(1.0f, 0.0f, 0.0f));
        view = view * translation;
        MVP = makeMVP(model, view, projection);

        drone.rotation = vec3(drone.rotation.x + degrees(speed / 10), drone.rotation.y, drone.rotation.z);
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation = glm::rotate(translation, -float(speed / 10), glm::vec3(0.0f, 0.0f, 1.0f));
        view = view * translation;
        MVP = makeMVP(model, view, projection);

        drone.rotation = vec3(drone.rotation.x - degrees(speed / 10), drone.rotation.y, drone.rotation.z);
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation = glm::rotate(translation, +float(speed / 10), glm::vec3(0.0f, 0.0f, 1.0f));
        view = view * translation;
        MVP = makeMVP(model, view, projection);

        drone.rotation = vec3(drone.rotation.x + degrees(speed / 10), drone.rotation.y, drone.rotation.z);
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && currentTime - delay > 0.2)
    {
        delay = currentTime;
        droneView = !droneView;

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        float aspect = (float)fbWidth / (float)fbHeight;

        model = mat4(1.0f);
        cameraAngle = 0.0f;
        cameraPos = glm::vec3(0.0f, 2.0f, 10.0f);
        cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
        view = lookAt(
            cameraPos,               // Camera position
            cameraPos + cameraFront, // Look at point
            cameraUp                 // Up vector
        );
        projection = perspective(glm::radians(45.0f), aspect, 1.0f, -500.0f);

        MVP = makeMVP(model, view, projection);

        drone.position = vec3(0.0f, 2.0f, 10.0f);

        if (droneView) {
            mat4 translation = mat4(1.0f);
            translation[3] = vec4(0, 0, -10.0, 1);
            view = translation * view;
            MVP = makeMVP(model, view, projection);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && currentTime - delay > 0.2)
    {
        delay = currentTime;
        nightView = !nightView;
    }

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        float aspect = (float)fbWidth / (float)fbHeight;

        model = mat4(1.0f);
        cameraAngle = 0.0f;
        cameraPos = glm::vec3(0.0f, 2.0f, 10.0f);
        cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
        view = lookAt(
            cameraPos,               // Camera position
            cameraPos + cameraFront, // Look at point
            cameraUp                 // Up vector
        );
        projection = perspective(glm::radians(45.0f), aspect, 1.0f, -500.0f);

        mat4 translation = mat4(1.0f);
        translation = glm::rotate(translation, radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        view = view * translation;

        translation = mat4(1.0f);
        translation[3] = vec4(0, 0, -100, 1);
        view = translation * view;
        MVP = makeMVP(model, view, projection);
    }
}

static void processMouseMovement(GLFWwindow* w, double xpos, double ypos){
    if (firstMouse){
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }
    float xoffset = (float) xpos - lastX;
    float yoffset = lastY - (float)ypos; 
    lastX = (float)xpos;
    lastY = (float)ypos;

    const float sensitivity = 0.07f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camYaw   += xoffset;
    camPitch += yoffset;

    // clamp to avoid camera flip
    if (camPitch >  89.0f) 
        camPitch =  89.0f;
    if (camPitch < -89.0f) 
        camPitch = -89.0f;

    glm::vec3 dir;
    dir.x = cos(glm::radians(camYaw)) * cos(glm::radians(camPitch));
    dir.y = sin(glm::radians(camPitch));
    dir.z = sin(glm::radians(camYaw)) * cos(glm::radians(camPitch));
    cameraFront = glm::normalize(dir);
}

const char *getError()
{
    const char *errorDescription;
    glfwGetError(&errorDescription);
    return errorDescription;
}

inline void startUpGLFW()
{
    std::cout << "startuoGLFW() was called by setUp()" <<std::endl;
    //glewExperimental = true; // Needed for core profile
    
    if (!glfwInit())
    {
        throw getError();
    }
    std::cout << "glfwInit() did not lead to null" <<std::endl;
}

inline void startUpGLEW()
{
    std::cout << "startupGLEW() has been called." << std::endl;
    glewExperimental = true;
    
    GLenum m = glewInit();
    if (m != GLEW_OK && m != 4)  // Ignore this known core profile bug
    {
        std::cerr << "GLEW Error Code: " << m << std::endl;
        std::cerr << "GLEW Error String: " << glewGetErrorString(m) << std::endl;
        throw std::runtime_error("GLEW initialisation failed.");
    }

    glGetError(); // Clear any GL error GLEW may have caused

    std::cout << "glewInit() successful" << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
}

inline GLFWwindow *setUp(){
    

    std::cout << "starting up GLFW and configuring" <<std::endl;
    startUpGLFW();
    glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);           // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);  // Make sure window is visible

    std::cout << "Done setting up GLFW" <<std::endl;
    std::cout << "Creating GLFWwindow in setUp() (so we can return it to main)" <<std::endl;
    GLFWwindow *window;                                            // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow(1500, 1000, "View", NULL, NULL);
    if (window == NULL)
    {
        cout << getError() << endl;
        glfwTerminate();
        throw "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n";
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    std::cout << "checking if GLFWwindow is null" <<std::endl;
    
    std::cout << "GLFWwindow was not null\nNow calling glfwMakeContextCurrent(window)" <<std::endl;
    glfwMakeContextCurrent(window); // Initialize GLEW
    std::cout << "calling startupGLEW()" <<std::endl;
    
    try{
        startUpGLEW();
    }catch(const std::exception& e){
        std::cerr << "Setup failed: " << e.what() << std::endl;
        return nullptr;
    }
    return window;
}

int main()
{
    GLFWwindow *window;
    std::cout << "Creating GLFWwindow" <<std::endl;
    try
    {
        std::cout << "Calling setUp()" <<std::endl;
        window = setUp();
        std::cout << "setUp() complete" <<std::endl;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, processMouseMovement);
    }
    catch (const char *e)
    {
        cout << e << endl;
        throw;
    }catch(const std::exception& e){
        cout << "Error: " << e.what() << endl;
        return 1;
    }catch(...){
        cerr << "Unknown error" << endl;
        return 1;
    }

    //Add code here
    
    try {
        
        GLuint programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/fragment.glsl");

        
        // get uniform locations
        GLint modelLoc      = glGetUniformLocation(programId, "model");
        GLint viewLoc       = glGetUniformLocation(programId, "view");
        GLint projectionLoc = glGetUniformLocation(programId, "projection");
        GLint colourLoc     = glGetUniformLocation(programId, "shapeColour");
        
        
        std::cout << "modelLoc: " << modelLoc << std::endl;
        std::cout << "viewLoc: " << viewLoc << std::endl;
        std::cout << "projectionLoc: " << projectionLoc << std::endl;
        std::cout << "colourLoc: " << colourLoc << std::endl;
        
        // Light

        DirectionalLight sun;
        sun.direction = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));
        sun.colour    = glm::vec3(1.0f, 0.95f, 0.85f);
        sun.ambient   = 0.25f;

        // Base

        GolfCourse course(
            "assets/terrain/grass.bmp",
            "assets/terrain/dirt.bmp",
            "assets/terrain/stone.bmp",
            "assets/terrain/concrete.bmp",
            "assets/terrain/wood.bmp"
        );

        std::vector<std::string> facesDay = {
            "assets/skybox/Daylight Box_Right.bmp",
            "assets/skybox/Daylight Box_Left.bmp",
            "assets/skybox/Daylight Box_Top.bmp",
            "assets/skybox/Daylight Box_Bottom.bmp",
            "assets/skybox/Daylight Box_Front.bmp",
            "assets/skybox/Daylight Box_Back.bmp"
        };

        std::vector<std::string> facesNight = {
            "assets/skybox/Nightlight Box_Side.bmp",
            "assets/skybox/Nightlight Box_Side.bmp",
            "assets/skybox/Nightlight Box_Top.bmp",
            "assets/skybox/Nightlight Box_Bottom.bmp",
            "assets/skybox/Nightlight Box_Side.bmp",
            "assets/skybox/Nightlight Box_Side.bmp"
        };

        SkyBox skyboxDay(facesDay);
        SkyBox skyboxNight(facesNight);

        // Textures
        GLuint objectShader = LoadShaders("shaders/base/vertex.glsl", "shaders/base/fragment.glsl");
        GLuint flagTexture  = loadBMPTexture("assets/course/white.bmp");
        GLuint redTexture  = loadBMPTexture("assets/course/green.bmp");
        GLuint poleTexture  = loadBMPTexture("assets/course/wood.bmp");
        GLuint ballTexture  = loadBMPTexture("assets/course/white.bmp");

        glm::vec3 cameraPos   = glm::vec3(0.0f, 2.0f, 10.0f);
        glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

        // Build Objects
 
        auto* flag01 = new ObjectBuilder();
        flag01->makeFlag(poleTexture, flagTexture, objectShader);

        auto* wallTower01 = new ObjectBuilder();
        wallTower01->makeWallTower(flagTexture, objectShader);

        auto* loop01 = new ObjectBuilder();
        loop01->makeLoop(flagTexture, objectShader);

        auto* bridge01 = new ObjectBuilder();
        bridge01->makeBridge(flagTexture, objectShader);

        auto* volcano01 = new ObjectBuilder();
        volcano01->makeVolcano(flagTexture, objectShader);

        auto* marker01 = new ObjectBuilder();
        marker01->makeMarker(flagTexture, objectShader);

        auto* podium01 = new ObjectBuilder();
        podium01->makePodium(flagTexture, objectShader);

        auto* shade01 = new ObjectBuilder();
        shade01->makeShade(flagTexture, objectShader);

        auto* lamp01 = new ObjectBuilder();
        lamp01->makeLamp(flagTexture, objectShader);

        auto* signpost01 = new ObjectBuilder();
        signpost01->makeSignpost(flagTexture, objectShader);

        auto* flag02 = new ObjectBuilder();
        flag02->makeFlag(poleTexture, flagTexture, objectShader);

        auto* wallTower02 = new ObjectBuilder();
        wallTower02->makeWallTower(flagTexture, objectShader);

        auto* loop02 = new ObjectBuilder();
        loop02->makeLoop(flagTexture, objectShader);

        auto* bridge02 = new ObjectBuilder();
        bridge02->makeBridge(flagTexture, objectShader);

        auto* volcano02 = new ObjectBuilder();
        volcano02->makeVolcano(flagTexture, objectShader);

        auto* marker02 = new ObjectBuilder();
        marker02->makeMarker(flagTexture, objectShader);

        auto* podium02 = new ObjectBuilder();
        podium02->makePodium(flagTexture, objectShader);

        auto* shade02 = new ObjectBuilder();
        shade02->makeShade(flagTexture, objectShader);

        auto* lamp02 = new ObjectBuilder();
        lamp02->makeLamp(flagTexture, objectShader);

        auto* signpost02 = new ObjectBuilder();
        signpost02->makeSignpost(flagTexture, objectShader);

        auto* flag03 = new ObjectBuilder();
        flag03->makeFlag(poleTexture, flagTexture, objectShader);

        auto* wallTower03 = new ObjectBuilder();
        wallTower03->makeWallTower(flagTexture, objectShader);

        auto* loop03 = new ObjectBuilder();
        loop03->makeLoop(flagTexture, objectShader);

        auto* bridge03 = new ObjectBuilder();
        bridge03->makeBridge(flagTexture, objectShader);

        auto* volcano03 = new ObjectBuilder();
        volcano03->makeVolcano(flagTexture, objectShader);

        auto* marker03 = new ObjectBuilder();
        marker03->makeMarker(flagTexture, objectShader);

        auto* podium03 = new ObjectBuilder();
        podium03->makePodium(flagTexture, objectShader);

        auto* shade03 = new ObjectBuilder();
        shade03->makeShade(flagTexture, objectShader);

        auto* lamp03 = new ObjectBuilder();
        lamp03->makeLamp(flagTexture, objectShader);

        auto* signpost03 = new ObjectBuilder();
        signpost03->makeSignpost(flagTexture, objectShader);

        // Build Holes        

        Hole01* hole1 = new Hole01(1, glm::vec3(0,0,0), glm::vec3(0,0,100));
        Hole02* hole2 = new Hole02(2, glm::vec3(0,0,0), glm::vec3(0,0,100));
        Hole03* hole3 = new Hole03(3, glm::vec3(0,0,0), glm::vec3(0,0,100));
        Hole04* hole4 = new Hole04(4, glm::vec3(0,0,0), glm::vec3(0,0,100));
        //Hole05* hole05 = new Hole05(5, glm::vec3(15,0,-8), glm::vec3(0,0,100));
        //Hole06* hole06 = new Hole06(6, glm::vec3(15,0,-8), glm::vec3(0,0,100));
        Hole07* hole7 = new Hole07(3, glm::vec3(25,0,-35), glm::vec3(100,0,-65));
        Hole08* hole8 = new Hole08(3, glm::vec3(35,0,-25), glm::vec3(200,0,-125));
        Hole09* hole9 = new Hole09(3, glm::vec3(45,0,-10), glm::vec3(0,0,0));
        Hole10* hole10 = new Hole10(10, glm::vec3(15,0,-8), glm::vec3(0,0,100));
        Hole11* hole11 = new Hole11(11, glm::vec3(15,0,-8), glm::vec3(0,0,100));
        Hole12* hole12 = new Hole12(12, glm::vec3(0,0,0), glm::vec3(0,0,100));
        Hole13* hole13 = new Hole13(13, glm::vec3(0,0,0), glm::vec3(0,0,100));
        Hole14* hole14 = new Hole14(14, glm::vec3(30,0,0), glm::vec3(30,0,0));
        Hole15* hole15 = new Hole15(15,glm::vec3(-30, 0, -10),glm::vec3(-30, 0, -10));
        Hole16* hole16 = new Hole16(16, glm::vec3(-30, 0, -30),glm::vec3(32, 30, 14.5f));
        //Hole17* hole17 = new Hole17(17, glm::vec3(15,0,-8), glm::vec3(0,0,100));
        //Hole18* hole18 = new Hole18(18, glm::vec3(15,0,-8), glm::vec3(0,0,100));

        // Build Course
        course.addHole(std::unique_ptr<Hole>(hole1));
        course.addHole(std::unique_ptr<Hole>(hole2));
        course.addHole(std::unique_ptr<Hole>(hole3));
        //course.addHole(std::unique_ptr<Hole>(hole4));
        //course.addHole(std::unique_ptr<Hole>(hole5));
        //course.addHole(std::unique_ptr<Hole>(hole6));
        course.addHole(std::unique_ptr<Hole>(hole7));
        course.addHole(std::unique_ptr<Hole>(hole8));
        course.addHole(std::unique_ptr<Hole>(hole9));
        course.addHole(std::unique_ptr<Hole>(hole10));
        course.addHole(std::unique_ptr<Hole>(hole11));
        course.addHole(std::unique_ptr<Hole>(hole12));
        course.addHole(std::unique_ptr<Hole>(hole13));
        course.addHole(std::unique_ptr<Hole>(hole14));
        course.addHole(std::unique_ptr<Hole>(hole15));
        course.addHole(std::unique_ptr<Hole>(hole16));
        //course.addHole(std::unique_ptr<Hole>(hole17));
        //course.addHole(std::unique_ptr<Hole>(hole18));

        // Add clones, move them

        podium01->addObject(hole1);
        podium01->setPosition(vec3(0.0f, 0.0f, 5.0f));

        shade01->addObject(hole1);
        shade01->setPosition(vec3(-7.0f, 0.0f, -15.0f));

        lamp01->addObject(hole1);
        lamp01->setPosition(vec3(-7.0f, 0.0f, -5.0f));

        signpost01->addObject(hole1);
        signpost01->setPosition(vec3(7.0f, 0.0f, 0.0f));

        wallTower01->addObject(hole1);
        wallTower01->setPosition(vec3(-0.1f, -0.2f, -16.0f));
        wallTower01->setScale(vec3(0.7f, 0.7f, 0.7f));
        wallTower01->setRotation(vec3(0.0f, 45.0f, 0.0f));

        loop01->addObject(hole2);
        loop01->setPosition(vec3(20.0f, 0.0f, -14.0f));

        bridge01->addObject(hole2);
        bridge01->setPosition(vec3(20.0f, 0.0f, -24.0f));

        volcano01->addObject(hole2);
        volcano01->setPosition(vec3(30.0f, -0.5f, 5.0f));

        // Place All Of These

        // *
        shade02->addObject(hole1);
        shade02->setPosition(vec3(-7.0f, 0.0f, -15.0f));

        // *
        lamp02->addObject(hole1);
        lamp02->setPosition(vec3(-7.0f, 0.0f, -5.0f));

        // *
        signpost02->addObject(hole1);
        signpost02->setPosition(vec3(7.0f, 0.0f, 0.0f));

        // *
        wallTower02->addObject(hole1);
        wallTower02->setPosition(vec3(-0.1f, -0.2f, -16.0f));
        wallTower02->setScale(vec3(0.7f, 0.7f, 0.7f));

        // *
        loop02->addObject(hole2);
        loop02->setPosition(vec3(20.0f, 0.0f, -14.0f));

        // *
        bridge02->addObject(hole2);
        bridge02->setPosition(vec3(20.0f, 0.0f, -24.0f));

        volcano02->addObject(hole2);
        volcano02->setPosition(vec3(40.0f, -0.5f, 0.0f));

        // *
        shade03->addObject(hole1);
        shade03->setPosition(vec3(-7.0f, 0.0f, -15.0f));

        // *
        lamp03->addObject(hole1);
        lamp03->setPosition(vec3(-7.0f, 0.0f, -5.0f));

        // *
        signpost03->addObject(hole1);
        signpost03->setPosition(vec3(7.0f, 0.0f, 0.0f));

        // *
        bridge03->addObject(hole2);
        bridge03->setPosition(vec3(20.0f, 0.0f, -24.0f));

        volcano03->addObject(hole2);
        volcano03->setPosition(vec3(-30.0f, -0.5f, -30.0f));

        course.build();
        course.setShader(programId, objectShader);

        Drone drone;

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        float aspect = (float)fbWidth / (float)fbHeight;
 
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        float cameraAngle = 0.0f;
        glm::mat4 model(1.0f);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat4 view       = makeLookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = makePerspective(glm::radians(45.0f), aspect, 0.1f, 500.0f);
        mat4 MVP = makeMVP(model, view, projection);

        bool nightShader = false;

        while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (nightView) {
                skyboxNight.render(&view[0][0], &projection[0][0]);
            } else {
                skyboxDay.render(&view[0][0], &projection[0][0]);
            }

            
            course.render(&view[0][0], &projection[0][0], cameraPos, sun);

            if (droneView) {
                drone.render(&view[0][0], &projection[0][0], cameraPos, sun);
            }
            
            currentTime = glfwGetTime();
            deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            processControls(window, view, model, projection, MVP, cameraAngle, cameraPos, cameraUp, drone);

            if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && currentTime - delay > 0.2 && !nightShader)
            {
                delay = currentTime;
            
                switch (state) {
                    case 0 : {
                        programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/sepia.glsl");
                        skyboxDay.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_sepia.glsl"));
                        skyboxNight.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_sepia.glsl"));
                        course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_sepia.glsl"),
                                        LoadShaders("shaders/base/vertex.glsl", "shaders/base/sepia.glsl"));
                        state++;
                        break;
                    }
                    case 1 : {
                        programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/invert.glsl");
                        skyboxDay.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_invert.glsl"));
                        skyboxNight.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_invert.glsl"));
                        course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_invert.glsl"),
                                        LoadShaders("shaders/base/vertex.glsl", "shaders/base/invert.glsl"));
                        state++;
                        break;
                    }
                    case 2 : {
                        programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/monochrome.glsl");
                        skyboxDay.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_monochrome.glsl"));
                        skyboxNight.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_monochrome.glsl"));
                        course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_monochrome.glsl"),
                                        LoadShaders("shaders/base/vertex.glsl", "shaders/base/monochrome.glsl"));
                        state++;
                        break;
                    }
                    case 3 : {
                        programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/shiftdown.glsl");
                        skyboxDay.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_shiftdown.glsl"));
                        skyboxNight.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_shiftdown.glsl"));
                        course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_shiftdown.glsl"),
                                        LoadShaders("shaders/base/vertex.glsl", "shaders/base/shiftdown.glsl"));
                        state++;
                        break;
                    }
                    case 4 : {
                        programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/shiftup.glsl");
                        skyboxDay.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_shiftup.glsl"));
                        skyboxNight.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_shiftup.glsl"));
                        course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_shiftup.glsl"),
                                        LoadShaders("shaders/base/vertex.glsl", "shaders/base/shiftup.glsl"));
                        state++;
                        break;
                    }
                    case 5 : {
                        programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/fragment.glsl");
                        skyboxDay.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_fragment.glsl"));
                        skyboxNight.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_fragment.glsl"));
                        course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_fragment.glsl"),
                                        LoadShaders("shaders/base/vertex.glsl", "shaders/base/fragment.glsl"));
                        state = 0;
                        break;
                    }
                }
            }

            if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && currentTime - delay > 0.2)
            {
                delay = currentTime;
                nightShader = !nightShader;
                drone.isNightVisionActive = nightShader;
            
                if (nightShader) {
                    programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/nightview.glsl");
                    skyboxDay.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_nightview.glsl"));
                    skyboxNight.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_nightview.glsl"));
                    course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_nightview.glsl"),
                                    LoadShaders("shaders/base/vertex.glsl", "shaders/base/nightview.glsl"));    
                } else {
                    programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/fragment.glsl");
                    skyboxDay.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_fragment.glsl"));
                    skyboxNight.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_fragment.glsl"));
                    course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_fragment.glsl"),
                                    LoadShaders("shaders/base/vertex.glsl", "shaders/base/fragment.glsl"));
                }
                
            }

            glfwSwapBuffers(window);
            glfwPollEvents();

            //cout << "FPS: " << 1 / deltaTime << '\n';
        }
    } catch (const char* e) {
        cout << e << endl;
        glfwTerminate();
        return -1;
    }

    glfwTerminate();
    return 0;
}