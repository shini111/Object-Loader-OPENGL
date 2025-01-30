#include "PrepareModel.h"



void prep_obj(const char* filename, ObjectData& objectData,  glm::vec3& position,  glm::vec3& size, const char* texturePath, bool isStatic) {
    load_obj(filename, objectData.vertices, objectData.normals, objectData.elements, objectData.texcoord);
    objectData.helper = CollisionHelper(&position, &objectData.vertices);

    objectData.isStaticValue = isStatic;

    // Generate and bind VAO, VBO, EBO
    glGenVertexArrays(1, &objectData.vao);
    glGenBuffers(1, &objectData.vbo);
    glGenBuffers(1, &objectData.ebo);

    glBindVertexArray(objectData.vao);

    // Bind and buffer vertex data
    glBindBuffer(GL_ARRAY_BUFFER, objectData.vbo);
    glBufferData(GL_ARRAY_BUFFER, objectData.vertices.size() * sizeof(glm::vec4), objectData.vertices.data(), GL_STATIC_DRAW);

    // Set the vertex attribute pointers for positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(0);

    // Bind and buffer element data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objectData.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, objectData.elements.size() * sizeof(GLushort), objectData.elements.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(1);



    // Unbind VAO
    glBindVertexArray(0);

    // Load texture
    glGenTextures(1, &objectData.texture);
    glBindTexture(GL_TEXTURE_2D, objectData.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else {
            std::cout << "Unsupported channel count: " << nrChannels << std::endl;
            stbi_image_free(data);
            return;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0); // Optional: unbind the texture
    }
    else {
        std::cout << "Failed to load texture: " << texturePath << std::endl;
    }

    stbi_image_free(data);

    // Calculate the model matrix
    objectData.model = glm::mat4(1.0f); // Identity matrix
    objectData.model = glm::translate(objectData.model, position);
    // Add rotation if needed: objectData.model = glm::rotate(objectData.model, glm::radians(rotationAngle), rotationAxis);
    objectData.model = glm::scale(objectData.model, size);
    glDrawElements(GL_TRIANGLES, objectData.elements.size(), GL_UNSIGNED_SHORT, 0);

}
