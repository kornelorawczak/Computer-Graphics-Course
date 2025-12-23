#include "MeshGenerator.hpp"
#include <cmath>

Mesh MeshGenerator::generateOctahedron() {
    Mesh mesh;
    
    std::vector<glm::vec3> positions = {
        glm::vec3(0.0f, 1.0f, 0.0f),   // top
        glm::vec3(0.0f, -1.0f, 0.0f),  // bottom
        glm::vec3(0.0f, 0.0f, 1.0f),   // front
        glm::vec3(0.0f, 0.0f, -1.0f),  // back
        glm::vec3(-1.0f, 0.0f, 0.0f),  // left
        glm::vec3(1.0f, 0.0f, 0.0f)    // right
    };
    
    // Create triangles (8 faces)
    auto addPlainTriangle = [&](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
        glm::vec2 dummyTex(0.0f, 0.0f);
        addTriangle(mesh.vertices, mesh.indices, a, b, c, dummyTex, dummyTex, dummyTex, normal);
    };
    
    addPlainTriangle(positions[0], positions[2], positions[5]); // Top front-right
    addPlainTriangle(positions[0], positions[5], positions[3]); // Top right-back
    addPlainTriangle(positions[0], positions[3], positions[4]); // Top back-left
    addPlainTriangle(positions[0], positions[4], positions[2]); // Top left-front
    
    addPlainTriangle(positions[1], positions[5], positions[2]); // Bottom front-right
    addPlainTriangle(positions[1], positions[3], positions[5]); // Bottom right-back
    addPlainTriangle(positions[1], positions[4], positions[3]); // Bottom back-left
    addPlainTriangle(positions[1], positions[2], positions[4]); // Bottom left-front
    
    return mesh;
}

Mesh MeshGenerator::generateSphere(int latitudeBands, int longitudeBands) {
    Mesh mesh;
    
    for (int lat = 0; lat <= latitudeBands; lat++) {
        float theta = lat * 3.14159265358979323846f / latitudeBands;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        
        for (int lon = 0; lon <= longitudeBands; lon++) {
            float phi = lon * 2 * 3.14159265358979323846f / longitudeBands;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            
            Vertex vertex;
            vertex.normal.x = cosPhi * sinTheta;
            vertex.normal.y = cosTheta;
            vertex.normal.z = sinPhi * sinTheta;
            vertex.position = vertex.normal; 
            
            mesh.vertices.push_back(vertex);
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

Mesh MeshGenerator::generateCube() {
    Mesh mesh;
    
    float offset = 0.001f;
    std::vector<glm::vec3> positions = {
        glm::vec3(0.0f + offset, 0.0f + offset, 0.0f + offset), // 0
        glm::vec3(1.0f - offset, 0.0f + offset, 0.0f + offset), // 1
        glm::vec3(1.0f - offset, 1.0f - offset, 0.0f + offset), // 2
        glm::vec3(0.0f + offset, 1.0f - offset, 0.0f + offset), // 3
        glm::vec3(0.0f + offset, 0.0f + offset, 1.0f - offset), // 4
        glm::vec3(1.0f - offset, 0.0f + offset, 1.0f - offset), // 5
        glm::vec3(1.0f - offset, 1.0f - offset, 1.0f - offset), // 6
        glm::vec3(0.0f + offset, 1.0f - offset, 1.0f - offset)  // 7
    };
    
    // Define the 6 faces (2 triangles each)
    std::vector<std::vector<int>> faces = {
        // Front face
        {0, 1, 2, 3},
        // Back face
        {4, 7, 6, 5},
        // Left face
        {0, 3, 7, 4},
        // Right face
        {1, 5, 6, 2},
        // Bottom face
        {0, 4, 5, 1},
        // Top face
        {3, 2, 6, 7}
    };
    
    std::vector<glm::vec3> normals = {
        glm::vec3(0.0f, 0.0f, 1.0f),  // Front
        glm::vec3(0.0f, 0.0f, -1.0f), // Back
        glm::vec3(-1.0f, 0.0f, 0.0f), // Left
        glm::vec3(1.0f, 0.0f, 0.0f),  // Right
        glm::vec3(0.0f, -1.0f, 0.0f), // Bottom
        glm::vec3(0.0f, 1.0f, 0.0f)   // Top
    };
    
    for (int face = 0; face < 6; face++) {
        const auto& faceIndices = faces[face];
        const auto& normal = normals[face];
        std::vector<glm::vec2> faceTexCoords = {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f), 
            glm::vec2(1.0f, 1.0f),
            glm::vec2(0.0f, 1.0f)
        };
        // First triangle 
        addTriangle(mesh.vertices, mesh.indices, 
                            positions[faceIndices[0]], positions[faceIndices[1]], positions[faceIndices[2]],
                            faceTexCoords[0], faceTexCoords[1], faceTexCoords[2], normal);
        // Second triangle   
        addTriangle(mesh.vertices, mesh.indices,
                            positions[faceIndices[0]], positions[faceIndices[2]], positions[faceIndices[3]],
                            faceTexCoords[0], faceTexCoords[2], faceTexCoords[3], normal);
    }
    
    return mesh;
}

void MeshGenerator::addTriangle(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                               const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
                               const glm::vec2& texA, const glm::vec2& texB, const glm::vec2& texC,
                                           const glm::vec3& normal) {
    unsigned int baseIndex = vertices.size();
    
    Vertex v1, v2, v3;
    v1.position = a;
    v2.position = b; 
    v3.position = c;
    
    v1.normal = normal;
    v2.normal = normal;
    v3.normal = normal;
    
    v1.texCoords = texA;
    v2.texCoords = texB;
    v3.texCoords = texC;
    
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
}

Mesh MeshGenerator::generateWireframeCube() {
    Mesh mesh;
    
    float offset = 0.001f; 
    std::vector<glm::vec3> positions = {
        glm::vec3(0.0f + offset, 0.0f + offset, 0.0f + offset), 
        glm::vec3(1.0f - offset, 0.0f + offset, 0.0f + offset), 
        glm::vec3(1.0f - offset, 1.0f - offset, 0.0f + offset), 
        glm::vec3(0.0f + offset, 1.0f - offset, 0.0f + offset), 
        glm::vec3(0.0f + offset, 0.0f + offset, 1.0f - offset), 
        glm::vec3(1.0f - offset, 0.0f + offset, 1.0f - offset), 
        glm::vec3(1.0f - offset, 1.0f - offset, 1.0f - offset), 
        glm::vec3(0.0f + offset, 1.0f - offset, 1.0f - offset)  
    };
    
    // Define the 12 edges of the cube
    std::vector<std::pair<int, int>> edges = {
        // Bottom face
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        // Top face  
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        // Vertical edges
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    
    // Create line segments for each edge
    for (const auto& edge : edges) {
        addLine(mesh.vertices, mesh.indices, positions[edge.first], positions[edge.second]);
    }
    
    return mesh;
}

void MeshGenerator::addLine(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                           const glm::vec3& a, const glm::vec3& b) {
    unsigned int baseIndex = vertices.size();
    
    Vertex v1, v2;
    v1.position = a;
    v2.position = b;
    v1.normal = glm::vec3(0.0f, 1.0f, 0.0f); 
    v2.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    
    vertices.push_back(v1);
    vertices.push_back(v2);
    
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
}