#ifndef MESHGENERATOR_HPP
#define MESHGENERATOR_HPP

#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

class MeshGenerator {
public:
    static Mesh generateOctahedron();
    static Mesh generateSphere(int latitudeBands = 16, int longitudeBands = 16);
    static Mesh generateCube();
    static Mesh generateWireframeCube();
    
private:
    static void addTriangle(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                            const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
                            const glm::vec2& texA, const glm::vec2& texB, const glm::vec2& texC,
                                           const glm::vec3& normal);
    static void addLine(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                       const glm::vec3& a, const glm::vec3& b);
};

#endif