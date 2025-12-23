#ifndef MESHGENERATOR_HPP
#define MESHGENERATOR_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Mesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    unsigned int VAO, VBO, EBO;
    
    Mesh() : VAO(0), VBO(0), EBO(0) {}
    
    Mesh(Mesh&& other) noexcept 
        : vertices(std::move(other.vertices))
        , indices(std::move(other.indices))
        , VAO(other.VAO)
        , VBO(other.VBO)
        , EBO(other.EBO) {
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }
    
    Mesh& operator=(Mesh&& other) noexcept {
        if (this != &other) {
            cleanup();
            vertices = std::move(other.vertices);
            indices = std::move(other.indices);
            VAO = other.VAO;
            VBO = other.VBO;
            EBO = other.EBO;
            other.VAO = 0;
            other.VBO = 0;
            other.EBO = 0;
        }
        return *this;
    }
    
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    
    void setupMesh();
    void draw() const;
    void cleanup();
};

class MeshGenerator {
public:
    static Mesh createSphere(int latitudeBands = 16, int longitudeBands = 16);
    static Mesh createVolcano(float baseRadius = 2000.0f, float craterRadius = 500.0f, 
                             float height = 3000.0f, int segments = 64, int rings = 32);
    static Mesh createGroundPlane(float size, int subdivisions);
    static Mesh createBoundaryWalls(float areaSize, float wallHeight);
    static Mesh createQuad();
    static Mesh createHelicopterBlade();
    static Mesh MeshGenerator::createSimpleGround(float size);
    static Mesh createHelicopterBlades(float length = 8.0f, float width = 0.5f, float thickness = 0.1f);
    static Mesh createFullscreenQuad();
    static Mesh createArrowTriangle();

    
private:
    static void addVertex(std::vector<float>& vertices, const glm::vec3& position, const glm::vec3& normal);
};

#endif