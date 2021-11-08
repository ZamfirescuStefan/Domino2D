#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>

std::string ParseShader(const std::string &path) {
    std::string shaderCode;
    std::ifstream shaderStream(path, std::ios::in);
    if (shaderStream.is_open()) {
        std::string Line = "";
        while (getline(shaderStream, Line))
            shaderCode += "\n" + Line;
        shaderStream.close();
    }
    return shaderCode;
}

GLuint CompileShader(GLuint type, const std::string &iSource) {
    const char* source = iSource.c_str();

    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &source , NULL);
    glCompileShader(id);

    GLint result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    
    if (result == GL_FALSE) {
        int length = 1024;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "veterx" : "fragment") << " shader\n";
        std::cout << message << '\n';
        glDeleteShader(id);
        return 1;
    }

    return id;
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {
 
    std::string vertexShaderCode = ParseShader(vertex_file_path);
    GLuint vertexShaderID = CompileShader(GL_VERTEX_SHADER, vertexShaderCode);
    
    std::string fragmentShaderCode = ParseShader(fragment_file_path);
    GLuint fragmentShaderID = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, vertexShaderID);
    glAttachShader(ProgramID, fragmentShaderID);
    glLinkProgram(ProgramID);

    int result;
    int infoLogLength;
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> ProgramErrorMessage(std::max(infoLogLength, int(1)));
        glGetProgramInfoLog(ProgramID, infoLogLength, NULL, &ProgramErrorMessage[0]);
        fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
        return 1;
    }

	glUseProgram(0);

    glDetachShader(ProgramID, vertexShaderID);
    glDetachShader(ProgramID, fragmentShaderID);

    glDeleteShader(fragmentShaderID);
    glDeleteShader(vertexShaderID);

    return ProgramID;
}