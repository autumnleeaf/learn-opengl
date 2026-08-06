#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "stb_implementation.hpp"
#include "shader.hpp"
#include "helpers.hpp"
#include "model.hpp"

int main(void) {
    GLFWwindow *window = createWindow(800, 600, "LearnOpenGL");

    stbi_set_flip_vertically_on_load(true);

    Shader shader = Shader("../shaders/lighting.vert", "../shaders/model_loading.frag");

    Model model = Model("../models/backpack/backpack.obj");

    while (!glfwWindowShouldClose(window)) {
        updateDeltaTime(glfwGetTime());
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)800 / (float)600, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // render the loaded model
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        shader.setMat4("model", modelMatrix);
        model.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}