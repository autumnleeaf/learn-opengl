#include "../headers/helpers.hpp"
#include "../headers/model.hpp"
#include "../headers/shader.hpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main() {
    GLFWwindow *window = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Framebuffer Demo");
    glEnable(GL_DEPTH_TEST);

    auto skyboxShader = Shader("../shaders/skybox.vert", "../shaders/skybox.frag");
    auto shader = Shader("../shaders/depth_testing.vert", "../shaders/blending.frag");
    auto screenShader = Shader("../shaders/screen_shader.vert", "../shaders/screen_shader.frag");
    auto reflectionShader = Shader("../shaders/reflection.vert", "../shaders/reflection.frag");
    auto refractionShader = Shader("../shaders/reflection.vert", "../shaders/refraction.frag");

    unsigned int skyboxVAO, skyboxVBO;
    createBuffers(&skyboxVAO, &skyboxVBO, skyboxVertices, sizeof(skyboxVertices), false, 3);

    unsigned int cubeVAO, cubeVBO;
    createBuffers(&cubeVAO, &cubeVBO, cubeVertices, sizeof(cubeVertices));

    unsigned int reflectVAO, reflectVBO;
    createBuffers(&reflectVAO, &reflectVBO, cubeWithNormals, sizeof(cubeWithNormals), true, 6);

    unsigned int planeVAO, planeVBO;
    createBuffers(&planeVAO, &planeVBO, planeVertices, sizeof(planeVertices));

    // Create a buffer for our quad, note that this is the same as a plane except 2D so we eliminate one dimension from the coordinates
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    // Tell the buffer what data we will be using to render our shapes along with the size
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Tell the array object that the first 3 values go to index 0 and the next 2 go to index 1 when reading data
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));

    const std::vector<std::string> faces = {
        "../images/skybox/right.jpg",
        "../images/skybox/left.jpg",
        "../images/skybox/top.jpg",
        "../images/skybox/bottom.jpg",
        "../images/skybox/front.jpg",
        "../images/skybox/back.jpg"
    };
    const unsigned int skyboxTexture = loadCubeMap(faces);
    const unsigned int cubeTexture = Model::textureFromFile("container.jpg", "../images");
    const unsigned int floorTexture = Model::textureFromFile("marble.png", "../images");

    shader.use();
    shader.setInt("texture1", 0);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    // Create and bind our framebuffer
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create a new texture with no data (since nothing is rendered yet) and apply linear filtering
    unsigned int textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB,
        GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Attach it to the framebuffer object we just made (this is a color attachment to the draw and read buffers)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        textureColorBuffer, 0);

    /*
     * We also want to add depth and stencil attachments, but since we will not be
     * sampling those buffers we can use a renderbuffer object. We will also unbind
     * the buffer once we've allocated enough memory for the buffer
     */
    unsigned int renderbuffer;
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
        SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach the renderbuffer to the depth and stencil attachments of the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, renderbuffer);

    // Ensure that the framebuffer is complete before continuing and unbind
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        exit(1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Unbind the array to prevent accidental overwrites
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        updateDeltaTime(static_cast<float>(glfwGetTime()));
        processInput(window);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set uniforms on both shaders
        const glm::mat4 view = camera.GetViewMatrix();
        const glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // Set face culling for plane
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        // Draw the plane
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        drawPlane(shader, glm::vec3(0.0f));

        // Cull back faces of cubes since we aren't meant to see them
        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);

        // Draw cubes normally
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);

        reflectionShader.use();
        reflectionShader.setVec3("cameraPos", camera.Position);
        reflectionShader.setMat4("view", view);
        reflectionShader.setMat4("projection", projection);
        glBindVertexArray(reflectVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        drawCube(reflectionShader, glm::vec3(2.0f, 0.0f, 0.0f));
        refractionShader.use();
        refractionShader.setVec3("cameraPos", camera.Position);
        refractionShader.setMat4("view", view);
        refractionShader.setMat4("projection", projection);
        drawCube(refractionShader, glm::vec3(-1.0f, 0.0f, -1.0f));

        // Draw Skybox
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        // We convert the view matrics to a 3d matrix and back to remove the translation part
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
        skyboxShader.setMat4("projection", projection);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteRenderbuffers(1, &renderbuffer);
    glDeleteFramebuffers(1, &framebuffer);

    glfwTerminate();
    return 0;
}