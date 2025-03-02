#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/random.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"

using std::vector;

const float toRadians = 3.14159265f / 180.0f;
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader> shaderList;
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20);

void CrearCubo()
{
    unsigned int cubo_indices[] = {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
    };

    GLfloat cubo_vertices[] = {
        // front
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        // back
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f
    };

    Mesh* cubo = new Mesh();
    cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
    meshList.push_back(cubo);
}

void CrearPiramideTriangular()
{
    unsigned int indices_piramide_triangular[] = {
        0, 1, 2,
        1, 3, 2,
        3, 0, 2,
        1, 0, 3
    };

    GLfloat vertices_piramide_triangular[] = {
        -1.0f, 0.0f,  0.0f,    
         1.0f, 0.0f,  0.0f,    
         0.0f, 1.73f, 0.0f,    
         0.0f, 0.58f,-1.73f    
    };

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
    meshList.push_back(obj1);
}


void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);

    Shader* shader2 = new Shader();
    shader2->CreateFromFiles(vShaderColor, fShader);
    shaderList.push_back(*shader2);
}

int main()
{
    mainWindow = Window(800, 600);
    mainWindow.Initialise();
    CrearCubo();                     // índice 0 
    CrearPiramideTriangular();         // índice 1 
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),-60.0f, 0.0f, 0.3f, 0.3f);

    GLuint uniformProjection = 0;
    GLuint uniformModel = 0;
    GLuint uniformView = 0;
    GLuint uniformColor = 0;
    glm::mat4 projection = glm::perspective(glm::radians(60.0f),mainWindow.getBufferWidth() / mainWindow.getBufferHeight(),0.1f, 100.0f);

    sp.init();
    sp.load();
    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        deltaTime += (now - lastTime) / limitFPS;
        lastTime = now;
        glfwPollEvents();
        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderList[0].useShader();
        uniformModel = shaderList[0].getModelLocation();
        uniformProjection = shaderList[0].getProjectLocation();
        uniformView = shaderList[0].getViewLocation();
        uniformColor = shaderList[0].getColorLocation();

        // 1. Matriz global "base", CREACIÓN DE LA PIRAMIDE BASE NEGRA
        glm::mat4 base = glm::mat4(1.0f);
        base = glm::translate(base, glm::vec3(0.0f, 0.0f, -4.0f));
        base = glm::rotate(base, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
        base = glm::rotate(base, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
        base = glm::rotate(base, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        meshList[1]->RenderMesh();

        // CARA 1:  9 pirámides
        color = glm::vec3(1.0f, 0.4f, 0.7f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
   
        // Pirámide 1
        glm::mat4 model1 = base;
        model1 = glm::translate(model1, glm::vec3(-0.5f, 0.1f, 0.02f));
        model1 = glm::scale(model1, glm::vec3(0.22f, 0.22f, 0.22f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model1));
        meshList[1]->RenderMesh();

        // Pirámide 2
        glm::mat4 model2 = base;
        model2 = glm::translate(model2, glm::vec3(0.0f, 0.1f, 0.02f));
        model2 = glm::scale(model2, glm::vec3(0.22f, 0.22f, 0.22f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model2));
        meshList[1]->RenderMesh();

        // Pirámide 3
        glm::mat4 model3 = base;
        model3 = glm::translate(model3, glm::vec3(0.5f, 0.1f, 0.02f));
        model3 = glm::scale(model3, glm::vec3(0.22f, 0.22f, 0.22f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model3));
        meshList[1]->RenderMesh();

        // Pirámide 4
        glm::mat4 model4 = base;
        model4 = glm::translate(model4, glm::vec3(0.25f, 0.55f, 0.02f));
        model4 = glm::scale(model4, glm::vec3(0.22f, 0.22f, 0.22f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model4));
        meshList[1]->RenderMesh();

        // Pirámide 5
        glm::mat4 model5 = base;
        model5 = glm::translate(model5, glm::vec3(0.25f, 0.55f, 0.02f));
        model5 = glm::scale(model5, glm::vec3(0.22f, 0.22f, 0.22f));
        model5 = glm::rotate(model5, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model5 = glm::translate(model5, glm::vec3(0.0f, 0.08f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model5));
        meshList[1]->RenderMesh();

        // Pirámide 6
        glm::mat4 model6 = base;
        model6 = glm::translate(model6, glm::vec3(-0.25f, 0.55f, 0.02f));
        model6 = glm::scale(model6, glm::vec3(0.22f, 0.22f, 0.22f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model6));
        meshList[1]->RenderMesh();

        // Pirámide 7
        glm::mat4 model7 = base;
        model7 = glm::translate(model7, glm::vec3(-0.25f, 0.55f, 0.02f));
        model7 = glm::scale(model7, glm::vec3(0.22f, 0.22f, 0.22f));
        model7 = glm::rotate(model7, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model7 = glm::translate(model7, glm::vec3(0.0f, 0.08f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model7));
        meshList[1]->RenderMesh();

        // Pirámide 8
        glm::mat4 model8 = base;
        model8 = glm::translate(model8, glm::vec3(0.0f, 1.0f, 0.02f));
        model8 = glm::scale(model8, glm::vec3(0.22f, 0.22f, 0.22f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model8));
        meshList[1]->RenderMesh();

        // Pirámide 9 
        glm::mat4 model9 = base;
        model9 = glm::translate(model9, glm::vec3(0.0f, 1.0f, 0.02f));
        model9 = glm::scale(model9, glm::vec3(0.22f, 0.22f, 0.22f));
        model9 = glm::rotate(model9, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model9 = glm::translate(model9, glm::vec3(0.0f, 0.08f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model9));
        meshList[1]->RenderMesh();

        glUseProgram(0);
        mainWindow.swapBuffers();
    }
    return 0;
}
