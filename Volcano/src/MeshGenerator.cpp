#include "MeshGenerator.hpp"
#include <glad/glad.h>
#include <glm/gtc/constants.hpp>
#include <cmath>

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void MeshGenerator::addVertex(std::vector<float>& vertices, const glm::vec3& position, const glm::vec3& normal) {
    vertices.push_back(position.x);
    vertices.push_back(position.y);
    vertices.push_back(position.z);
    vertices.push_back(normal.x);
    vertices.push_back(normal.y);
    vertices.push_back(normal.z);
}

Mesh MeshGenerator::createSimpleGround(float size) {
    Mesh mesh;
    
    float halfSize = size * 0.5f;
    
    glm::vec3 positions[4] = {
        glm::vec3(-halfSize, 0.0f, -halfSize),
        glm::vec3(halfSize, 0.0f, -halfSize),
        glm::vec3(halfSize, 0.0f, halfSize),
        glm::vec3(-halfSize, 0.0f, halfSize)
    };
    
    glm::vec3 normal(0.0f, 1.0f, 0.0f);
    
    addVertex(mesh.vertices, positions[0], normal);
    addVertex(mesh.vertices, positions[1], normal);
    addVertex(mesh.vertices, positions[2], normal);
    
    addVertex(mesh.vertices, positions[0], normal);
    addVertex(mesh.vertices, positions[2], normal);
    addVertex(mesh.vertices, positions[3], normal);
    
    for (unsigned int i = 0; i < 6; i++) {
        mesh.indices.push_back(i);
    }
    
    return mesh;
}

Mesh MeshGenerator::createSphere(int latitudeBands, int longitudeBands) {
    Mesh mesh;
    
    for (int lat = 0; lat <= latitudeBands; lat++) {
        float theta = lat * glm::pi<float>() / latitudeBands;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= longitudeBands; lon++) {
            float phi = lon * 2 * glm::pi<float>() / longitudeBands;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            glm::vec3 position;
            position.x = cosPhi * sinTheta;
            position.y = cosTheta;
            position.z = sinPhi * sinTheta;

            glm::vec3 normal = glm::normalize(position);

            addVertex(mesh.vertices, position, normal);
        }
    }

    for (int lat = 0; lat < latitudeBands; lat++) {
        for (int lon = 0; lon < longitudeBands; lon++) {
            int first = (lat * (longitudeBands + 1)) + lon;
            int second = first + longitudeBands + 1;

            mesh.indices.push_back(first);
            mesh.indices.push_back(second);
            mesh.indices.push_back(first + 1);

            mesh.indices.push_back(second);
            mesh.indices.push_back(second + 1);
            mesh.indices.push_back(first + 1);
        }
    }

    return mesh;
}

Mesh MeshGenerator::createHelicopterBlade() {
    Mesh mesh;
    
    float halfWidth = 0.5f;
    float halfLength = 2.0f; 
    float halfHeight = 0.05f;
    
    glm::vec3 vertices[8] = {
        glm::vec3(-halfWidth, -halfHeight, -halfLength),
        glm::vec3( halfWidth, -halfHeight, -halfLength),
        glm::vec3( halfWidth, -halfHeight,  halfLength),
        glm::vec3(-halfWidth, -halfHeight,  halfLength),
        
        glm::vec3(-halfWidth,  halfHeight, -halfLength),
        glm::vec3( halfWidth,  halfHeight, -halfLength),
        glm::vec3( halfWidth,  halfHeight,  halfLength),
        glm::vec3(-halfWidth,  halfHeight,  halfLength)
    };
    
    glm::vec3 normals[6] = {
        glm::vec3(0.0f, -1.0f, 0.0f), 
        glm::vec3(0.0f, 1.0f, 0.0f),  
        glm::vec3(0.0f, 0.0f, -1.0f), 
        glm::vec3(0.0f, 0.0f, 1.0f),  
        glm::vec3(-1.0f, 0.0f, 0.0f), 
        glm::vec3(1.0f, 0.0f, 0.0f)   
    };
    
    int faces[6][4] = {
        {0, 1, 2, 3}, 
        {4, 5, 6, 7}, 
        {0, 1, 5, 4}, 
        {2, 3, 7, 6}, 
        {0, 3, 7, 4}, 
        {1, 2, 6, 5}  
    };
    
    for (int face = 0; face < 6; face++) {
        int v0 = faces[face][0];
        int v1 = faces[face][1];
        int v2 = faces[face][2];
        int v3 = faces[face][3];
        
        addVertex(mesh.vertices, vertices[v0], normals[face]);
        addVertex(mesh.vertices, vertices[v1], normals[face]);
        addVertex(mesh.vertices, vertices[v2], normals[face]);
        
        addVertex(mesh.vertices, vertices[v0], normals[face]);
        addVertex(mesh.vertices, vertices[v2], normals[face]);
        addVertex(mesh.vertices, vertices[v3], normals[face]);
    }
    
    for (unsigned int i = 0; i < 36; i++) {
        mesh.indices.push_back(i);
    }
    
    return mesh;
}

Mesh MeshGenerator::createFullscreenQuad() {
    Mesh mesh;
    
    float quadVertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,
        
        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };
    
    mesh.vertices.assign(quadVertices, quadVertices + 12);
    
    for (unsigned int i = 0; i < 6; i++) {
        mesh.indices.push_back(i);
    }
    
    return mesh;
}

Mesh MeshGenerator::createArrowTriangle() {
    Mesh mesh;
    
    float size = 1.0f;
    
    glm::vec3 vertices[3] = {
        glm::vec3(0.0f, 0.0f, size),      
        glm::vec3(-size * 0.5f, 0.0f, 0.0f), 
        glm::vec3(size * 0.5f, 0.0f, 0.0f)   
    };
    
    glm::vec3 normal(0.0f, 1.0f, 0.0f);
    
    for (int i = 0; i < 3; i++) {
        addVertex(mesh.vertices, vertices[i], normal);
    }
    
    for (unsigned int i = 0; i < 3; i++) {
        mesh.indices.push_back(i);
    }
    
    return mesh;
}

Mesh MeshGenerator::createBoundaryWalls(float areaSize, float wallHeight) {
    Mesh mesh;
    
    float halfSize = areaSize * 0.5f;
    
    
    glm::vec3 wallPositions[4] = {
        glm::vec3(-halfSize, 0.0f, 0.0f), 
        glm::vec3(halfSize, 0.0f, 0.0f),   
        glm::vec3(0.0f, 0.0f, -halfSize),  
        glm::vec3(0.0f, 0.0f, halfSize)    
    };
    
    glm::vec3 wallNormals[4] = {
        glm::vec3(1.0f, 0.0f, 0.0f),   
        glm::vec3(-1.0f, 0.0f, 0.0f),  
        glm::vec3(0.0f, 0.0f, 1.0f),   
        glm::vec3(0.0f, 0.0f, -1.0f)  
    };
    
    float wallThickness = 1.0f;
    float wallLength = areaSize;
    
    for (int wall = 0; wall < 4; wall++) {
        glm::vec3 corners[4];
        glm::vec3 normal;
        
        switch (wall) {
            case 0: 
                corners[0] = glm::vec3(-halfSize, 0.0f, -halfSize);
                corners[1] = glm::vec3(-halfSize, 0.0f, halfSize);
                corners[2] = glm::vec3(-halfSize, wallHeight, halfSize);
                corners[3] = glm::vec3(-halfSize, wallHeight, -halfSize);
                normal = glm::vec3(1.0f, 0.0f, 0.0f); 
                break;
                
            case 1: 
                corners[0] = glm::vec3(halfSize, 0.0f, -halfSize);
                corners[1] = glm::vec3(halfSize, 0.0f, halfSize);
                corners[2] = glm::vec3(halfSize, wallHeight, halfSize);
                corners[3] = glm::vec3(halfSize, wallHeight, -halfSize);
                normal = glm::vec3(-1.0f, 0.0f, 0.0f); 
                break;
                
            case 2: 
                corners[0] = glm::vec3(-halfSize, 0.0f, -halfSize);
                corners[1] = glm::vec3(halfSize, 0.0f, -halfSize);
                corners[2] = glm::vec3(halfSize, wallHeight, -halfSize);
                corners[3] = glm::vec3(-halfSize, wallHeight, -halfSize);
                normal = glm::vec3(0.0f, 0.0f, 1.0f); 
                break;
                
            case 3:
                corners[0] = glm::vec3(-halfSize, 0.0f, halfSize);
                corners[1] = glm::vec3(halfSize, 0.0f, halfSize);
                corners[2] = glm::vec3(halfSize, wallHeight, halfSize);
                corners[3] = glm::vec3(-halfSize, wallHeight, halfSize);
                normal = glm::vec3(0.0f, 0.0f, -1.0f); 
                break;
        }
        
        addVertex(mesh.vertices, corners[0], normal);
        addVertex(mesh.vertices, corners[1], normal);
        addVertex(mesh.vertices, corners[2], normal);
        
        addVertex(mesh.vertices, corners[0], normal);
        addVertex(mesh.vertices, corners[2], normal);
        addVertex(mesh.vertices, corners[3], normal);
    }
    
    for (unsigned int i = 0; i < 4 * 6; i++) {
        mesh.indices.push_back(i);
    }
    
    return mesh;
}

Mesh MeshGenerator::createVolcano(float baseRadius, float craterRadius, float height, int segments, int rings) {
    Mesh mesh;
    
    for (int ring = 0; ring <= rings; ring++) {
        float y = (float)ring / rings * height;
        
        float t = y / height;
        float radius = baseRadius * (1.0f - t) + craterRadius * t;
        
        radius += sin(y * 0.01f) * 50.0f;
        
        for (int segment = 0; segment <= segments; segment++) {
            float angle = (float)segment / segments * 2.0f * glm::pi<float>();
            
            glm::vec3 position;
            position.x = radius * cos(angle);
            position.y = y;
            position.z = radius * sin(angle);
            
            glm::vec3 normal = glm::normalize(glm::vec3(position.x, 0.0f, position.z));
            
            addVertex(mesh.vertices, position, normal);
        }
    }
    
    for (int ring = 0; ring < rings; ring++) {
        for (int segment = 0; segment < segments; segment++) {
            int current = ring * (segments + 1) + segment;
            int next = current + segments + 1;
            
            mesh.indices.push_back(current);
            mesh.indices.push_back(next);
            mesh.indices.push_back(current + 1);
            
            mesh.indices.push_back(current + 1);
            mesh.indices.push_back(next);
            mesh.indices.push_back(next + 1);
        }
    }
    
    return mesh;
}

Mesh MeshGenerator::createGroundPlane(float size, int subdivisions) {
    Mesh mesh;
    
    float step = size / subdivisions;
    float halfSize = size * 0.5f;
    
    for (int z = 0; z <= subdivisions; z++) {
        for (int x = 0; x <= subdivisions; x++) {
            glm::vec3 position;
            position.x = -halfSize + x * step;
            position.y = 0.0f; 
            position.z = -halfSize + z * step;
            
            glm::vec3 normal(0.0f, 1.0f, 0.0f); 
            
            addVertex(mesh.vertices, position, normal);
        }
    }
    
    for (int z = 0; z < subdivisions; z++) {
        for (int x = 0; x < subdivisions; x++) {
            int topLeft = z * (subdivisions + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (subdivisions + 1) + x;
            int bottomRight = bottomLeft + 1;
            
            mesh.indices.push_back(topLeft);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(topRight);
            
            mesh.indices.push_back(topRight);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(bottomRight);
        }
    }
    
    return mesh;
}

Mesh MeshGenerator::createQuad() {
    Mesh mesh;
    
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    mesh.vertices.assign(quadVertices, quadVertices + 24);
    
    return mesh;
}