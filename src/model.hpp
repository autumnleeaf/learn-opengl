#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "assimp/material.h"
#include "shader.hpp"
#include "mesh.hpp"
#include "stb_implementation.hpp"

class Model {
    public:
        Model(const char *path) {
            loadModel(path);
        }

        void Draw(Shader &shader) {
            // Loop over and draw all the meshes
            for (unsigned int i = 0; i < meshes.size(); i++) {
                meshes[i].Draw(shader);
            }
        }

        static unsigned int textureFromFile(const char *source, const std::string &directory) {
            unsigned int ID;
            glGenTextures(1, &ID);
            int width, height, nrChannels;
            unsigned char* data = stbi_load((directory + '/' + source).c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                // Settings
                glBindTexture(GL_TEXTURE_2D, ID);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                // Get the color format based on the nrChannels variable
                GLenum format;
                switch (nrChannels) {
                case 1:
                    format = GL_RED;
                    break;
                case 3:
                    format = GL_RGB;
                    break;
                case 4:
                    format = GL_RGBA;
                    break;
                default:
                    format = GL_RGB;
                    break;
                }

                // Set texture image
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(data);
            } else {
                std::cout << "Failed to load texture" << std::endl;
                stbi_image_free(data);
            }

            return ID;
        }

    private:
        std::vector<Texture> textures_loaded;
        std::vector<Mesh> meshes;
        std::string directory;

        void loadModel(std::string path) {
            // Read assimp scene from file
            Assimp::Importer importer;
            const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

            // Handle failure to load scene
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
                return;
            }

            // Store the directory
            directory = path.substr(0, path.find_last_of('/'));

            // Process the root node
            processNode(scene->mRootNode, scene);
        }

        void processNode(aiNode *node, const aiScene *scene) {
            // Loop through mesh addresses in the node and process them from the scene object
            for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(processMesh(mesh, scene));
            }

            // Recursively process child nodes
            for (unsigned int i = 0; i < node->mNumChildren; i++) {
                processNode(node->mChildren[i], scene);
            }
        }

        Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;

            // Process vertex data first
            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                Vertex vertex;

                // Get position data
                glm::vec3 position;
                position.x = mesh->mVertices[i].x;
                position.y = mesh->mVertices[i].y;
                position.z = mesh->mVertices[i].z;
                vertex.Position = position;

                // Get normal data
                if (mesh->HasNormals()) {
                    glm::vec3 normal;
                    normal.x = mesh->mNormals[i].x;
                    normal.y = mesh->mNormals[i].y;
                    normal.z = mesh->mNormals[i].z;
                    vertex.Normal = normal;
                }

                // Get texture coordinates
                if (mesh->mTextureCoords[0]) {
                    glm::vec2 coords;
                    coords.x = mesh->mTextureCoords[0][i].x;
                    coords.y = mesh->mTextureCoords[0][i].y;
                    vertex.TexCoords = coords;
                } else {
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                }

                // Add to vertex array
                vertices.push_back(vertex);
            }

            // Process indices
            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]);
                }
            }

            // Add materials
            if (mesh->mMaterialIndex >= 0) {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
                std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
                std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            }

            return Mesh(vertices, indices, textures);
        }

        std::vector<Texture> loadMaterialTextures(aiMaterial *material, aiTextureType type, std::string typeName) {
            std::vector<Texture> textures;
            for (unsigned int i  = 0; i < material->GetTextureCount(type); i++) {
                // Get texture data
                aiString str;
                material->GetTexture(type, i, &str);

                // If we've loaded this texture before, just added it to the list again
                bool skip = false;
                for (unsigned int j = 0; j < textures_loaded.size(); j++) {
                    if (std::strcmp(textures_loaded[i].path.data(), str.C_Str()) == 0) {
                        textures.push_back(textures_loaded[j]);
                        skip = true;
                        break;
                    }
                }

                // Load the texture if it has not been loaded
                if (!skip) {
                    Texture texture;
                    texture.id = textureFromFile(str.C_Str(), directory);
                    texture.type = typeName;
                    texture.path = str.C_Str();
                    textures.push_back(texture);
                }
            }

            // Return
            return textures;
        }
};

#endif