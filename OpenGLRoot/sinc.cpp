//
//  main.cpp
//  sinc
//
//  Created by Abel Gomes on 23/03/2020.
//  Copyright © 2020 Abel Gomes. All rights reserved.
//

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
using namespace std;


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// shaders header file
#include <ogl-master/common/shader.hpp>

// GLM header file
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

// Vertex array object (VAO)
GLuint VertexArrayID;

// Vertex buffer object (VBO)
GLuint vertexbuffer;

// color buffer object (CBO)
GLuint colorbuffer;

// GLSL program from the shaders
GLuint programID;


#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 900

const int steps = 360;



//--------------------------------------------------------------------------------
void transferDataToGPUMemory(void)
{
    GLfloat x = 0.0f; // xmin of the domain
    GLfloat y = 0.0f;
    GLfloat radius = 1.0f;
    GLfloat angle  = 3.1415926f * 2.0f / steps;
    GLfloat prevX = 1.0f;
    GLfloat prevY = 0.0f;

    // VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("C:/shadersCG/Sinc/SimpleVertexShader.vertexshader", "C:/shadersCG/Sinc/SimpleFragmentShader.fragmentshader");

    
    GLfloat g_vertex_buffer_data[steps * 9];
    for (int i = 0; i < steps; i++)
    {
        GLfloat newX = radius + sin(angle * i);
        GLfloat newY = -radius + cos(angle * i);

        g_vertex_buffer_data[(i * 3)] = 0.0f;
        g_vertex_buffer_data[(i * 3) + 1] = 0.0f;
        g_vertex_buffer_data[(i * 3) + 2] = 0.0f;

        g_vertex_buffer_data[(i * 3) + 3] = newX;
        g_vertex_buffer_data[(i * 3) + 4] = newY;  
        g_vertex_buffer_data[(i * 3) + 5] = 0.0f;
        cout << "(" << newX << ", " << newY << ")\n";

        g_vertex_buffer_data[(i * 3) + 6] = prevX;
        g_vertex_buffer_data[(i * 3) + 7] = prevY;
        g_vertex_buffer_data[(i * 3) + 8] = 0.0f;
        //cout << "(" << prevX << ", " << prevY << ")\n";

        prevX = newX;
        prevY = newY;
    }
    

    GLfloat g_color_buffer_data[steps*9];
    for (int i = 0; i < steps; i++)
    {  
        g_color_buffer_data[i * 3] = 1.0f;
        g_color_buffer_data[(i * 3) + 1] = 0.0f;
        g_color_buffer_data[(i * 3) + 2] = 0.0f;

        g_color_buffer_data[(i * 3) + 3] = 1.0f;
        g_color_buffer_data[(i * 3) + 4] = 0.0f;
        g_color_buffer_data[(i * 3) + 5] = 0.0f;

        g_color_buffer_data[(i * 3) + 6] = 1.0f;
        g_color_buffer_data[(i * 3) + 7] = 0.0f;
        g_color_buffer_data[(i * 3) + 8] = 0.0f;
    }

    
    // Move vertex data to video memory; specifically to VBO called vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    // Move color data to video memory; specifically to CBO called colorbuffer
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
    
}


//--------------------------------------------------------------------------------
void cleanupDataFromGPU()
{
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);
}


//--------------------------------------------------------------------------------
void draw (void)
{
    
    // Clear the screen
    glClear( GL_COLOR_BUFFER_BIT );
    
    // Use our shader
    glUseProgram(programID);
    
    // create domain in R^2
    glm::mat4 mvp = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f);
    // retrieve the matrix uniform locations
    unsigned int matrix = glGetUniformLocation(programID, "mvp");
    glUniformMatrix4fv(matrix, 1, GL_FALSE, &mvp[0][0]);


    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
                          0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
                          1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    
    glEnable(GL_PROGRAM_POINT_SIZE);
    //glPointSize(10);
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLE_FAN, 0, steps * 9); // 3 indices starting at 0 -> 1 triangle
    //glDrawArrays(GL_POINTS, 0, 80); // 3 indices starting at 0 -> 1 triangle
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
//--------------------------------------------------------------------------------


int main(void)
{
    GLFWwindow* window;
    
    // Initialize the library
    if (!glfwInit())
    {
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "function sinc(x)", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    glewExperimental = true;
    // Needed for core profile
    if (glewInit() != GLEW_OK){
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    
   
    // transfer my data (vertices, colors, and shaders) to GPU side
    transferDataToGPUMemory();

    
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
 
        draw();
        
        // Swap front and back buffers
        glfwSwapBuffers(window);
        
        // Poll for and process events
        glfwPollEvents();
    }
    
    // Cleanup VAO, VBOs, and shaders from GPU
    cleanupDataFromGPU();
    
    glfwTerminate();
    
    return 0;
}

