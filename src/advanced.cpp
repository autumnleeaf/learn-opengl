#include <glm/glm.hpp>
#include <vector>
#include <map>

#include "../headers/helpers.hpp"
#include "../headers/model.hpp"
#include "../headers/shader.hpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main() {
    GLFWwindow *window = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Learn OpenGL");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto shader = Shader("../shaders/depth_testing.vert", "../shaders/blending.frag");
    auto shaderSingleColor = Shader("../shaders/depth_testing.vert", "../shaders/single_color.frag");

    unsigned int cubeVAO, cubeVBO;
    createBuffers(&cubeVAO, &cubeVBO, cubeVertices, sizeof(cubeVertices));

    unsigned int planeVAO, planeVBO;
    createBuffers(&planeVAO, &planeVBO, planeVertices, sizeof(planeVertices));

    unsigned int glassVAO, glassVBO;
    createBuffers(&glassVAO, &glassVBO, glassVertices, sizeof(glassVertices));

    const unsigned int cubeTexture = Model::textureFromFile("metal.png", "../images");
    const unsigned int floorTexture = Model::textureFromFile("marble.png", "../images");
    const unsigned int glassTexture = Model::textureFromFile("blending_transparent_window.png", "../images", GL_RGBA);

    shader.use();
    shader.setInt("texture1", 0);

    std::vector<glm::vec3> glass;
    glass.emplace_back(-1.5f, 0.0f, -0.48f);
    glass.emplace_back(1.5f, 0.0f, 0.51f);
    glass.emplace_back(0.0f, 0.0f, 0.7f);
    glass.emplace_back(-0.3f, 0.0f, -2.3f);
    glass.emplace_back(0.5f, 0.0f, -0.6f);

    while(!glfwWindowShouldClose(window)) {
        updateDeltaTime(static_cast<float>(glfwGetTime()));
        processInput(window);

        // Clear data after loop
        glEnable(GL_DEPTH_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Set face culling for plane
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        // Set uniforms on both shaders
        const glm::mat4 view = camera.GetViewMatrix();
        const glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);
        shaderSingleColor.use();
        shaderSingleColor.setMat4("view", view);
        shaderSingleColor.setMat4("projection", projection);
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // Draw the plane
        glStencilMask(0x00);
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        drawPlane(shader, glm::vec3(0.0f));

        // Stencil testing to draw everything
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        // Cull back faces of cubes since we aren't meant to see them
        glCullFace(GL_BACK);

        // Draw cubes normally
        shader.use();
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        drawCube(shader, glm::vec3(-1.0f, 0.0f, -1.0f));
        drawCube(shader, glm::vec3(2.0f, 0.0f, 0.0f));

        // Render only data that isn't filled with 1's already and disable depth testing
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        // Draw cubes slightly bigger and with only one color
        shaderSingleColor.use();
        drawCube(shaderSingleColor, glm::vec3(-1.0f, 0.0f, -1.0f), 1.1f);
        drawCube(shaderSingleColor, glm::vec3(2.0f, 0.0f, 0.0f), 1.1f);

        // Reset stencil values
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);

        // Disable face culling for windows so they can be seen from both sides
        glDisable(GL_CULL_FACE);

        // Sort all transparent objects so that we can see through them properly
        std::map<float, glm::vec3> sorted;
        for (auto i : glass) {
            float distance = glm::length(camera.Position - i);
            sorted[distance] = i;
        }

        shader.use();
        glBindVertexArray(glassVAO);
        glBindTexture(GL_TEXTURE_2D, glassTexture);
        for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
            drawPlane(shader, it->second);
        }

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &glassVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &glassVBO);

    glfwTerminate();
    return 0;
}