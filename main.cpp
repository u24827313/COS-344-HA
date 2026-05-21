#include <GL/glew.h>
#include <iostream>
#include <thread>
#include <random>
#include <chrono> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

#include "Terrain.h"
#include "SkyBox.h"

using namespace glm;
using namespace std;


// camer orientation variables
static float camYaw = -90.0f;
static float camPitch = 0.0f;
static float lastX = 750.0f;  
static float lastY = 500.0f;  
static bool  firstMouse = true;
static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);


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
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

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
        
        GLuint programId = LoadShaders("vertex.glsl", "fragment.glsl");

        
        // get uniform locations
        GLint modelLoc      = glGetUniformLocation(programId, "model");
        GLint viewLoc       = glGetUniformLocation(programId, "view");
        GLint projectionLoc = glGetUniformLocation(programId, "projection");
        GLint colourLoc     = glGetUniformLocation(programId, "shapeColour");
        
        
        std::cout << "modelLoc: " << modelLoc << std::endl;
        std::cout << "viewLoc: " << viewLoc << std::endl;
        std::cout << "projectionLoc: " << projectionLoc << std::endl;
        std::cout << "colourLoc: " << colourLoc << std::endl;
        
    
        // light
        DirectionalLight sun;
        sun.direction = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));
        sun.colour    = glm::vec3(1.0f, 0.95f, 0.85f);
        sun.ambient   = 0.25f;
        // object instantiation
        Terrain terrain(
            "assets/terrain/grass.bmp",
            "assets/terrain/dirt.bmp",
            "assets/terrain/stone.bmp",
            "assets/terrain/concrete.bmp",
            "assets/terrain/wood.bmp",
            200,
            500.0f
        );
        // design the terrain
        terrain.addMound(30.0f, 80.0f, 15.0f, 3.0f);
        
        terrain.addBunker(-12.0f, 50.0f, 6.0f, 2.5f);
        terrain.build();
        std::vector<std::string> faces = {
            "assets/skybox/Daylight Box_Right.bmp",
            "assets/skybox/Daylight Box_Left.bmp",
            "assets/skybox/Daylight Box_Top.bmp",
            "assets/skybox/Daylight Box_Bottom.bmp",
            "assets/skybox/Daylight Box_Front.bmp",
            "assets/skybox/Daylight Box_Back.bmp"
        };
        SkyBox skybox(faces);

        glm::vec3 cameraPos   = glm::vec3(0.0f, 2.0f, 10.0f);
        glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
 
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        float aspect = (float)fbWidth / (float)fbHeight;
 
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);


        float cameraSpeed = 0.3f;  // tune to taste
        glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 view       = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 500.0f);

            skybox.render(glm::value_ptr(view), glm::value_ptr(projection));
            terrain.render(glm::value_ptr(view), glm::value_ptr(projection), cameraPos, sun);

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                firstMouse = true;
            }
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= cameraSpeed * right;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += cameraSpeed * right;
            if (glfwGetKey(window, GLFW_KEY_SPACE)        == GLFW_PRESS) cameraPos += cameraSpeed * cameraUp;
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)   == GLFW_PRESS) cameraPos -= cameraSpeed * cameraUp;

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    } catch (const char* e) {
        cout << e << endl;
        glfwTerminate();
        return -1;
    }

    glfwTerminate();
    return 0;
}