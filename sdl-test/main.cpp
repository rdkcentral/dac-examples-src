/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2021 EPAM SYSTEMS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <SDL.h>
#include <SDL_opengles2.h>
#include <GLES2/gl2.h>
#include <chrono>


// Shader sources
const GLchar* vertexSource =
    "#version 100                                \n"
    "precision mediump float;                    \n"
    "attribute vec2 position;                    \n"
    "void main()                                 \n"
    "{                                           \n"
    "   gl_Position = vec4(position, 0.0, 1.0);  \n"
    "}                                           \n";

const GLchar* fragmentSource =
    "#version 100                  \n"
    "precision mediump float;      \n"
    "uniform float uniformColor1;  \n"
    "uniform float uniformColor2;  \n"
    "void main()                   \n"
    "{                             \n"
    "    gl_FragColor = vec4(uniformColor1, 0.0, uniformColor2, 1.0); \n"
    "}                             \n";


const unsigned int DISP_WIDTH = 640;
const unsigned int DISP_HEIGHT = 480;

// Creates the Vertex Buffer Object (VBO) containing
// the given vertices.
GLuint vboCreate(const GLfloat *vertices, GLuint verticesSize) {
    // Create the Vertex Buffer Object
    GLuint vbo;
    int nBuffers = 1;
    glGenBuffers(nBuffers, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Copy the vertex data in, and deactivate
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices,
    GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Check for problems
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        // Failed
        glDeleteBuffers(nBuffers, &vbo);
        SDL_Log("Creating VBO failed, code %u\n", err);
        vbo = 0;
    }
    return vbo;
}

// Loads and compiles a shader.
GLuint shaderLoad(const GLchar* source, GLenum shaderType) {
    // Create the shader
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    
    // Compile the shader
    glCompileShader(shader);
    GLint compileSucceeded = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSucceeded);

    // Check shader compilation result
    if (!compileSucceeded) {
        // Compilation failed. Print error info
        SDL_Log("Compilation of shader failed.\n");

        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        GLchar *errLog = (GLchar*)malloc(logLength);
        if (errLog) {
            glGetShaderInfoLog(shader, logLength, &logLength, errLog);
            SDL_Log("%s\n", errLog);
            free(errLog);
        }
        else {
            SDL_Log("Couldn't get shader log; out of memory\n");
        }
        glDeleteShader(shader);
        shader = 0;
    }
    return shader;
}


int main(int argc, char** argv)
{
    // Store app start time.
    auto timeStart = std::chrono::high_resolution_clock::now();

    // The window
    SDL_Window *window = NULL;
    // The OpenGL context
    SDL_GLContext context = NULL;
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Setup the exit hook
    atexit(SDL_Quit);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);


    window = SDL_CreateWindow("GLES2+SDL2 Window", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, DISP_WIDTH, DISP_HEIGHT,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    
    if (!window) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                "Couldn't create the main window.", NULL);
        return EXIT_FAILURE;
    }

    context = SDL_GL_CreateContext(window);
    if (!context) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                "Couldn't create an OpenGL context.", NULL);
        return EXIT_FAILURE;
    }

    // Triangle params
    const GLfloat vertices[] = {0.0f,  0.5f,
                                0.5f, -0.5f,
                               -0.5f, -0.5f};

    const GLsizei vertSize = sizeof(vertices);

    // Creates the Vertex Buffer Object (VBO)
    GLuint vbo = vboCreate(vertices, vertSize);
    if (!vbo) {
        return EXIT_FAILURE;
    }

    // Create vertex shader
    GLuint vertexShader = shaderLoad(vertexSource, GL_VERTEX_SHADER);
    if (!vertexShader) {
        SDL_Log("Couldn't load vertex shader.\n");
        return EXIT_FAILURE;
    }


    // Create fragment shader
    GLuint fragmentShader = shaderLoad(fragmentSource, GL_FRAGMENT_SHADER);
    if (!fragmentShader) {
        SDL_Log("Couldn't load fragment shader.\n");
        glDeleteShader(vertexShader);
        return EXIT_FAILURE;
    }

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProg = glCreateProgram();

    if (shaderProg) {
        glAttachShader(shaderProg, vertexShader);
        glAttachShader(shaderProg, fragmentShader);
        glLinkProgram(shaderProg);
        GLint linkingSucceeded = GL_FALSE;
        glGetProgramiv(shaderProg, GL_LINK_STATUS, &linkingSucceeded);
        
        if (!linkingSucceeded) {
            SDL_Log("Linking shader failed\n");
            GLint logLength = 0;
            glGetProgramiv(shaderProg, GL_INFO_LOG_LENGTH, &logLength);
            GLchar *errLog = (GLchar*)malloc(logLength);
           
            if (errLog) {
                glGetProgramInfoLog(shaderProg, logLength, &logLength, errLog);
                SDL_Log("%s\n", errLog);
                free(errLog);
            }
            else {
                SDL_Log("Couldn't get shader link log; out of memory\n");
            }
            glDeleteProgram(shaderProg);
            shaderProg = 0;
        }
    }
    else {
        SDL_Log("Couldn't create shader program\n");
    }

    if (!shaderProg) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return EXIT_FAILURE;
    }

    glUseProgram(shaderProg);

    float fragmentColor1 = 0.5f;
    bool running = true;

    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Log("SDL_QUIT event occured\n");
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_UP) {
                    if (fragmentColor1 < 1.0f) {
                        fragmentColor1 += 0.1f;
                    }
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    if (fragmentColor1 > 0.0f) {
                        fragmentColor1 -= 0.1f;
                    }
                }
            }
        }

        // Set the color of the triangle
        auto timeNow = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(timeNow - timeStart).count();

        GLint posAttrib = glGetAttribLocation(shaderProg, "position");
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(posAttrib);

        glUseProgram(shaderProg);

        // Get the location of the color uniform
        GLshort uniColor1 = glGetUniformLocation(shaderProg, "uniformColor1");
        GLshort uniColor2 = glGetUniformLocation(shaderProg, "uniformColor2");

        // Color set by keyboard input
        glUniform1f(uniColor1, fragmentColor1);

        // Color which changes with elapsed time
        glUniform1f(uniColor2, sin(time));

        // Draw
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    };

    // Cleanup
    glDeleteBuffers(1, &vbo);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(shaderProg);

    return 0;
}
