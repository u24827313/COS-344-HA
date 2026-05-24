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
        view = glm::rotate(view, -float(speed / 10), glm::vec3(0.0f, 1.0f, 0.0f));
        MVP = makeMVP(model, view, projection);

        drone.rotation = vec3(drone.rotation.x, drone.rotation.y + degrees(speed / 10), drone.rotation.z);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        view = glm::rotate(view, float(speed / 10), glm::vec3(0.0f, 1.0f, 0.0f));
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
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation = glm::rotate(translation, +float(speed / 10), glm::vec3(1.0, 0.0, 0.0));
        view = view * translation;
        MVP = makeMVP(model, view, projection);

        drone.rotation = vec3(drone.rotation.x - degrees(speed / 10), drone.rotation.y, drone.rotation.z);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        mat4 translation = mat4(1.0f);
        translation = glm::rotate(translation, -float(speed / 10), glm::vec3(1.0, 0.0, 0.0));
        view = view * translation;
        MVP = makeMVP(model, view, projection);

        drone.rotation = vec3(drone.rotation.x + degrees(speed / 10), drone.rotation.y, drone.rotation.z);
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
        
    
        // light
        DirectionalLight sun;
        sun.direction = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));
        sun.colour    = glm::vec3(1.0f, 0.95f, 0.85f);
        sun.ambient   = 0.25f;
        //
        GolfCourse course(
            "assets/terrain/grass.bmp",
            "assets/terrain/dirt.bmp",
            "assets/terrain/stone.bmp",
            "assets/terrain/concrete.bmp",
            "assets/terrain/wood.bmp"
        );
        
        /*// object instantiation
        Terrain terrain(
            "assets/terrain/grass.bmp",
            "assets/terrain/sand.bmp",
            "assets/terrain/stone.bmp",
            "assets/terrain/concrete.bmp",
            "assets/terrain/wood.bmp",
            200,
            500.0f
        );
        // design the terrain
        terrain.addBunker(30.0f, 80.0f, 15.0f, 4.0f);
        terrain.addMound(30.0f, 80.0f, 15.0f, 3.0f);
    
        terrain.addBunker(-12.0f, 50.0f, 6.0f, 2.5f);
        terrain.addMound(-12.0f, 50.0f, 6.0f, -1.5f);

        terrain.addBunker(0.0f,0.0f,30.0f, 13.5f);
        terrain.build();*/

        std::vector<std::string> faces = {
            "assets/skybox/Daylight Box_Right.bmp",
            "assets/skybox/Daylight Box_Left.bmp",
            "assets/skybox/Daylight Box_Top.bmp",
            "assets/skybox/Daylight Box_Bottom.bmp",
            "assets/skybox/Daylight Box_Front.bmp",
            "assets/skybox/Daylight Box_Back.bmp"
        };
        SkyBox skybox(faces);

        // holes
        GLuint objectShader = LoadShaders("shaders/base/vertex.glsl", "shaders/base/fragment.glsl");
        GLuint flagTexture  = loadBMPTexture("assets/course/white.bmp");
        GLuint redTexture  = loadBMPTexture("assets/course/green.bmp");
        GLuint poleTexture  = loadBMPTexture("assets/course/wood.bmp");
        GLuint ballTexture  = loadBMPTexture("assets/course/white.bmp");

        // build a flag stick
        glm::vec3 cameraPos   = glm::vec3(0.0f, 2.0f, 10.0f);
        glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
 
        auto* flag = new ObjectBuilder();
        flag->makeFlag(poleTexture, flagTexture, objectShader);

        auto* cone = new ObjectBuilder();
        cone->makeBridge(flagTexture, objectShader);

        auto* cone2 = new ObjectBuilder();
        cone2->clone(cone);

        // Add them to Hole 1
        Hole01* hole1 = new Hole01(1, glm::vec3(0,0,0), glm::vec3(0,0,100));
        cone2->addObject(hole1);
        //flag->addObject(hole1);
        course.addHole(std::unique_ptr<Hole>(hole1));

        auto* ball = new RenderObject(
            RenderObject::createSphere(16, 16),
            ballTexture,
            objectShader
        );
        ball->setPosition(glm::vec3(1.0f, 0.4f, 100.0f));
        ball->setScale(glm::vec3(0.4f)); 

        // add course terrain
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

        while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            skybox.render(&view[0][0], &projection[0][0]);
            course.render(&view[0][0], &projection[0][0], cameraPos, sun);
            drone.render(&view[0][0], &projection[0][0], cameraPos, sun);

            currentTime = glfwGetTime();
            deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            processControls(window, view, model, projection, MVP, cameraAngle, cameraPos, cameraUp, drone);

            if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && currentTime - delay > 0.5)
            {
                delay = currentTime;
            
                switch (state) {
                    case 0 : {
                        programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/sepia.glsl");
                        skybox.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_sepia.glsl"));
                        course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_sepia.glsl"),
                                        LoadShaders("shaders/base/vertex.glsl", "shaders/base/sepia.glsl"));
                        state++;
                        break;
                    }
                    case 1 : {
                        programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/invert.glsl");
                        skybox.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_invert.glsl"));
                        course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_invert.glsl"),
                                        LoadShaders("shaders/base/vertex.glsl", "shaders/base/invert.glsl"));
                        state++;
                        break;
                    }
                    case 2 : {
                        programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/monochrome.glsl");
                        skybox.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_monochrome.glsl"));
                        course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_monochrome.glsl"),
                                        LoadShaders("shaders/base/vertex.glsl", "shaders/base/monochrome.glsl"));
                        state++;
                        break;
                    }
                    case 3 : {
                        programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/shiftdown.glsl");
                        skybox.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_shiftdown.glsl"));
                        course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_shiftdown.glsl"),
                                        LoadShaders("shaders/base/vertex.glsl", "shaders/base/shiftdown.glsl"));
                        state++;
                        break;
                    }
                    case 4 : {
                        programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/shiftup.glsl");
                        skybox.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_shiftup.glsl"));
                        course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_shiftup.glsl"),
                                        LoadShaders("shaders/base/vertex.glsl", "shaders/base/shiftup.glsl"));
                        state++;
                        break;
                    }
                    case 5 : {
                        programId = LoadShaders("shaders/base/vertex.glsl", "shaders/base/fragment.glsl");
                        skybox.setShader(LoadShaders("shaders/skybox/skybox_vertex.glsl", "shaders/skybox/skybox_fragment.glsl"));
                        course.setShader(LoadShaders("shaders/terrain/terrain_vertex.glsl", "shaders/terrain/terrain_fragment.glsl"),
                                        LoadShaders("shaders/base/vertex.glsl", "shaders/base/fragment.glsl"));
                        state = 0;
                        break;
                    }
                }
            }

            // Night View
            if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
            {
                mat4 translation = mat4(1.0f);
                translation = glm::rotate(translation, -0.01f, glm::vec3(1.0, 0.0, 0.0));
                view = view * translation;
                MVP = makeMVP(model, view, projection);
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