#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // ID of the program
    unsigned int ID;

    // Constructor that takes in paths to the vertex and fragment shader files
    Shader(const char* vertexPath, const char* fragmentPath) {
        /* Get the source code from the files
        */
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        // Enable exceptions
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try {
            // Open the files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);

            // Read buffer into streams
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            // Close the files since we can use the streams
            vShaderFile.close();
            fShaderFile.close();

            // Convert to string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (std::ifstream::failure &e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        /*========================================================================*/
        /* Compile our shaders
        */
        unsigned int vertex, fragment;
        int success;
        char infoLog[1024];

        // Vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);

        // Validate vertex shader
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);

        // Validate fragment shader
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Create the shader program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);

        // Validate shader program
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // Delete the linked shaders
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // Activates the shader
    void use() {
        glUseProgram(ID);
    }

    // Setters
    void setBool(const std::string &name, const bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
    }
    
    void setInt(const std::string &name, const int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const std::string &name, const float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setMat4(const std::string &name, glm::mat4 value) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void setVec3(const std::string &name, const float x, const float y, const float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void setVec3(const std::string &name, const glm::vec3 value) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
    }

    void setMaterial(const std::string &name, int diffuse, int specular, float shininess) const {
        setInt(name + ".diffuse", diffuse);
        setInt(name + ".specular", specular);
        setFloat(name + ".shininess", shininess);
    }

    void initializeDirectionalLight(const std::string &name, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) const {
        setVec3(name + ".direction", direction);
        setVec3(name + ".ambient", ambient);
        setVec3(name + ".diffuse", diffuse);
        setVec3(name + ".specular", specular);
    }

    void initializePointLight(const std::string &name, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic) const {
        setVec3(name + ".position", position);
        setVec3(name + ".ambient", ambient);
        setVec3(name + ".diffuse", diffuse);
        setVec3(name + ".specular", specular);
        setFloat(name + ".constant", constant);
        setFloat(name + ".linear", linear);
        setFloat(name + ".quadratic", quadratic);
    }

    void initializeSpotLight(const std::string &name, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic, float cutOff, float outerCutOff) const {
        setVec3(name + ".position", position);
        setVec3(name + ".direction", direction);
        setVec3(name + ".ambient", ambient);
        setVec3(name + ".diffuse", diffuse);
        setVec3(name + ".specular", specular);
        setFloat(name + ".constant", constant);
        setFloat(name + ".linear", linear);
        setFloat(name + ".quadratic", quadratic);
        setFloat(name + ".cutOff", cutOff);
        setFloat(name + ".outerCutOff", outerCutOff);
    }

    void updateSpotLightPosition(const std::string &name, glm::vec3 position, glm::vec3 direction) const {
        setVec3(name + ".position", position);
        setVec3(name + ".direction", direction);
    }
};

#endif