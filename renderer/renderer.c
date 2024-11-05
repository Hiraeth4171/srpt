#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "utils/io.h"
#include <stdio.h>
#include <stdlib.h>
#include "renderer.h"

int indicies[] = {
    0, 1, 3,
    1, 2, 3
};

// ortho projection matrix


Settings* g_settings = NULL;
int g_res_loc, g_proj_loc;

void process_input(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    g_settings->resolution = (vec2){width, height};
    glUniform2f(g_res_loc, (float)g_settings->resolution.x, (float)g_settings->resolution.y);
    //glMatrixMode(GL_PROJECTION_MATRIX);
    float proj[16] = {
        (2.0f/(float)width) , 0.0f                  , 0.0f  , 0.0f,
        0.0f                , -(2.0f/(float)height) , 0.0f  , 0.0f,
        0.0f                , 0.0f                  , -0.5f , 0.0f,
        -1.0f               , 1.0f                  , 0.0f  , 1.0f
    };
    glUniformMatrix4fv(g_proj_loc, 1, GL_FALSE, proj);
}

unsigned int load_shaders() {
    const char* _vertex_shader_source = read_file_io("./renderer/shaders/basic_shader.shader");
    const char* _frag_shader_source = read_file_io("./renderer/shaders/basic_frag_shader.shader");
    unsigned int _vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(_vertex_shader, 1, &_vertex_shader_source, NULL);
    glCompileShader(_vertex_shader);

    GLint _success;
    char _log[512];
    glGetShaderiv(_vertex_shader, GL_COMPILE_STATUS, &_success);
    if (!_success) {
        glGetShaderInfoLog(_vertex_shader, 512, NULL, _log);
        printf("VERT SHADER ERROR: %s\n\n", _log);
    }

    unsigned int _frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(_frag_shader, 1, &_frag_shader_source, NULL);
    glCompileShader(_frag_shader);


    glGetShaderiv(_frag_shader, GL_COMPILE_STATUS, &_success);
    if (!_success) {
        glGetShaderInfoLog(_frag_shader, 512, NULL, _log);
        printf("FRAG SHADER ERROR: %s\n\n", _log);
    }

    unsigned int _shader_program = glCreateProgram();
    glAttachShader(_shader_program, _vertex_shader);
    glAttachShader(_shader_program, _frag_shader);
    glLinkProgram(_shader_program);

    glGetProgramiv(_shader_program, GL_LINK_STATUS, &_success);
    if (!_success) {
        glGetProgramInfoLog(_shader_program, 512, NULL, _log);
        printf("PROGRAM SHADER ERROR: %s\n\n", _log);
    }

    glUseProgram(_shader_program);
    glDeleteShader(_frag_shader);
    glDeleteShader(_vertex_shader);
    return _shader_program;
}


void draw_element(Element* elem) {
    float verts[] = {
        (float)elem->dim.pos.x, (float)(elem->dim.pos.y+elem->dim.size.y), -1.0f, (float)elem->color.r, (float)elem->color.g, (float)elem->color.b, (float)elem->color.a,
        (float)(elem->dim.pos.x+elem->dim.size.x), (float)(elem->dim.pos.y+elem->dim.size.y), -1.0f, (float)elem->color.r, (float)elem->color.g, (float)elem->color.b, (float)elem->color.a,
        (float)(elem->dim.pos.x+elem->dim.size.x), (float)elem->dim.pos.y, -1.0f, (float)elem->color.r, (float)elem->color.g, (float)elem->color.b, (float)elem->color.a,
        (float)elem->dim.pos.x, (float)elem->dim.pos.y, -1.0f, (float)elem->color.r, (float)elem->color.g, (float)elem->color.b, (float)elem->color.a
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    for (size_t i = 0; i < elem->children_length; ++i) {
        draw_element(elem->children[i]);
    }
}


int renderer_run(Element* main, Settings* settings) {
    // glfw initialization
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // window initialization
    print_settings(settings);
    GLFWwindow* window = glfwCreateWindow(settings->resolution.x, settings->resolution.y, settings->title.data, NULL, NULL);
    if (window == NULL) {
        printf("failed.\n");
        glfwTerminate();
        return -1;
    }
    // globals init
    g_settings = settings;

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // get glad to register function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("failed to init glad.\n");
        return -1;
    }

    // viewport
    glViewport(0, 0, settings->resolution.x, settings->resolution.y);

    // Vertex Array Object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // EBO 
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // vertex buffer object
    unsigned int VBO_triangle;
    // generate vertex buffer object
    glGenBuffers(1, &VBO_triangle);

    glBindVertexArray(VAO);

    // bind vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, VBO_triangle);
    // insert data into vertex buffer object
    //
    // DO SOME BATCH RENDERING.

    /*float verts[] = {
        (float)main->dim.pos.x, (float)(main->dim.pos.y+main->dim.size.y), -1.0f,
        (float)(main->dim.pos.x+main->dim.size.x), (float)(main->dim.pos.y+main->dim.size.y), -1.0f,
        (float)(main->dim.pos.x+main->dim.size.x), (float)main->dim.pos.y, -1.0f,
        (float)main->dim.pos.x, (float)main->dim.pos.y, -1.0f,
    };*/
    /*float verts[] = {
        -1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
    };*/

    // EBO bind and data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);


    // define interpretation of vertex array 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (const void*)12);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shaders
    unsigned int shader_program = load_shaders();
    // wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // draw loop
    g_res_loc = glGetUniformLocation(shader_program, "resolution");
    g_proj_loc = glGetUniformLocation(shader_program, "proj");
    int view_loc = glGetUniformLocation(shader_program, "view");
    float view[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, view);
    glUseProgram(shader_program);
    while(!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // RENDER
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_triangle);
        draw_element(main);


        glfwPollEvents();
        glfwSwapBuffers(window); 
    }
    
    // free glfw resources
    glfwTerminate();
    return 0;
}
