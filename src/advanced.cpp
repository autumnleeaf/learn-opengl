#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <map>

#include "../headers/helpers.hpp"
#include "../headers/model.hpp"
#include "../headers/shader.hpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

static float cubeVertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Left face
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    // Right face
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left
};

static float planeVertices[] = {
    // positions         // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
    5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

    5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
    5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
};

static float glassVertices[] = {
    0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
    0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
    1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

    0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
    1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
    1.0f,  0.5f,  0.0f,  1.0f,  0.0f
};

static void drawCube(Shader shader, glm::vec3 position, float scale);
static void drawCube(Shader shader, glm::vec3 position);
static void drawPlane(Shader shader, glm::vec3 position);
static void drawShape(Shader shader, glm::vec3 position, float scale, GLsizei numVertices);
static void createBuffers(unsigned int *vao, unsigned int *vbo, const float *vertices, unsigned int numVertices);

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

void drawCube(const Shader shader, const glm::vec3 position) {
    drawShape(shader, position, 1.0f, 36);
}

void drawCube(const Shader shader, const glm::vec3 position, const float scale) {
    drawShape(shader, position, scale, 36);
}

void drawPlane(const Shader shader, const glm::vec3 position) {
    drawShape(shader, position, 1.0f, 6);
}

void drawShape(Shader shader, const glm::vec3 position, const float scale, const GLsizei numVertices) {
    /*
     * Every render operation can be boiled down to setting the position in the model matrix, adjusting
     * the size if needed, updating the shader being used, and calling glDrawArrays. Note that the proper
     * array object and shader need to be bound in order for this to work, but we don't need to call these
     * every time the function is called so we can leave those operations outside the function.
     */
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    if (scale != 1.0f) {
        model = glm::scale(model, glm::vec3(scale));
    }
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void createBuffers(unsigned int *vao, unsigned int *vbo, const float *vertices, const unsigned int numVertices) {
    /*
     * To create our vertex array and vertex buffer objects we need to tell the program
     * how the data is stored in the vertex arrays passed in. Luckily for us all of the planes
     * and cubes we will be rendering all use the same array format so we can abstract this into a function
     * to make life easier.
     */
    // Initialize the array and buffer and bind them to be the active array and buffer
    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    // Tell the buffer what data we will be using to render our shapes along with the size
    glBufferData(GL_ARRAY_BUFFER, numVertices, vertices, GL_STATIC_DRAW);

    // Tell the array object that the first 3 values go to index 0 and the next 2 go to index 1 when reading data
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));

    // Unbind the array to prevent accidental overwrites
    glBindVertexArray(0);
}