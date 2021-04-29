//
//  main.cpp
//  survivalGame
//
//  Created by NewTest on 2021-04-27.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glm.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow* window;

GLenum RenderingType = GL_LINES;
bool DebugMode = false;

#include "noise.h"
#include "shader.h"
#include "cube.h"
#include "terrain.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

#define CAMERA_SPEED .1

int windowWidth = SCREEN_WIDTH, windowHeight = SCREEN_HEIGHT;
float xrot = 0, yrot = 0;
float zoom = 45;

vec2 last = vec2(0,0);
vec3 position;
vec3 direction;

void window_size_callback(GLFWwindow* window, int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

void scroll(GLFWwindow* window,double x,double y) {
    zoom -= y;
    cout << x << endl;
}

void keyboard() {
    
    vec3 right = normalize(cross(direction, vec3(0,1,0)));
        
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) position -= vec3(direction.x * CAMERA_SPEED, direction.y * CAMERA_SPEED, direction.z * CAMERA_SPEED);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) position += vec3(direction.x * CAMERA_SPEED, direction.y * CAMERA_SPEED, direction.z * CAMERA_SPEED);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) position += vec3(right.x * CAMERA_SPEED, right.y * CAMERA_SPEED, right.z * CAMERA_SPEED);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) position -= vec3(right.x * CAMERA_SPEED, right.y * CAMERA_SPEED, right.z * CAMERA_SPEED);
}

void mouse_cursor_callback( GLFWwindow * window, double xpos, double ypos) {
    
    double x, y;
    int width, height;
    glfwGetCursorPos(window, &x, &y);
    glfwGetWindowSize(window, &width, &height);
    
    x = (x/width-.5f)*2;
    y = (y/height-.5f)*(-2);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        
        float xDir = last.x - x;
        float yDir = last.y - y;
        
        yrot += yDir*3;
        xrot += xDir*3;
        
        cout << x << " " << y << endl;
    }
    last = vec2(x, y);
}


int main(int argc, const char * argv[]) {
    
    if (!glfwInit()) glfwTerminate();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shadows", NULL, NULL);
    
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_callback);
    glfwSetScrollCallback(window,scroll);
    
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    
    Shader shader = Shader("vertexShader.glsl", "fragmentShader.glsl");
    
    glEnable(GL_DEPTH_TEST);
    initializeCube();
    Terrain terrain = Terrain();
    
    while (!glfwWindowShouldClose(window)) {
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        keyboard();
        
        float height = terrain.getHeights(position.x, position.z);
        position = vec3(position.x, height, position.z);
        
        if (yrot > 1.93f) yrot = 1.93f;
        if (yrot < -1.93f) yrot = -1.93f;
        
        if (zoom < 2) zoom = 2;
        direction = normalize(vec3(sin(xrot) * zoom, 0, cos(xrot) * zoom));
        
        mat4 model = mat4(1.0);
        mat4 view = lookAt(vec3(sin(xrot) * zoom, sin(yrot) * zoom, cos(xrot) * zoom)+position, position, vec3(0,1,0));
        mat4 projection = perspective(90.f, 1.0f, .1f, 2000.f);
        
#ifdef __APPLE__
        
        if (windowWidth > windowHeight) {
            glViewport(0, -(windowWidth-windowHeight), windowWidth*2, windowWidth*2);
        }
        else {
            glViewport(0, -(windowHeight-windowWidth), windowHeight*2, windowHeight*2);
        }
#else
        
        
        
#endif
        
        shader.use();
        shader.set_mat4(glGetUniformLocation(shader.program, "projection"), projection);
        shader.set_mat4(glGetUniformLocation(shader.program, "view"), view);
        shader.set_mat4(glGetUniformLocation(shader.program, "model"), model);
        
        vec3 color = vec3(1.0, 0.0, 0.0);
        shader.set_vec3(glGetUniformLocation(shader.program, "color"), color);
        terrain.render_terrain(shader);
        renderCube(shader, position);
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
}
