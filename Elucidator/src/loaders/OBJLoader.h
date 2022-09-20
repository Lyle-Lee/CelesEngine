#pragma once

#include "Loader.h"
#include "../MeshObject.h"

namespace loader
{
    struct Material
    {
        Material()
        {
            name = "";
            Ns = 0.0f;
            Ni = 0.0f;
            d = 0.0f;
            illum = 0;
        }

        // Material Name
        std::string name;
        // Ambient Color
        glm::vec3 Ka;
        // Diffuse Color
        glm::vec3 Kd;
        // Specular Color
        glm::vec3 Ks;
        // Specular Exponent
        float Ns;
        // Optical Density
        float Ni;
        // Dissolve
        float d;
        // Illumination
        int illum;
        // Ambient Texture Map
        std::string map_Ka;
        // Diffuse Texture Map
        std::string map_Kd;
        // Specular Texture Map
        std::string map_Ks;
        // Specular Hightlight Map
        std::string map_Ns;
        // Alpha Texture Map
        std::string map_d;
        // Bump Map
        std::string map_bump;
    };

    // Structure: Mesh
    //
    // Description: A Simple Mesh Object that holds
    //	a name, a vertex list, and an index list
    //struct Mesh
    //{
    //    // Default Constructor
    //    Mesh()
    //    {

    //    }
    //    // Variable Set Constructor
    //    Mesh(std::vector<Vertex>& _Vertices, std::vector<unsigned int>& _Indices)
    //    {
    //        Vertices = _Vertices;
    //        Indices = _Indices;
    //        MeshMaterial = std::nullopt;
    //    }
    //    // Mesh Name
    //    std::string MeshName;
    //    // Vertex List
    //    std::vector<Vertex> Vertices;
    //    // Index List
    //    std::vector<unsigned int> Indices;

    //    // Material
    //    std::optional<Material> MeshMaterial;
    //};

	// Description: The OBJ Model Loader
	class OBJLoader
	{
    public:
        OBJLoader(std::vector<Vertex>& meshVertices, std::vector<unsigned int>& meshIndices);
        ~OBJLoader();

        // Load a file into the loader
        //
        // If file is loaded return true
        //
        // If the file is unable to be found
        // or unable to be loaded return false
        bool loadFile(const std::string& filepath);

        // Loaded Mesh Objects
        //std::vector<Mesh> LoadedMeshes;
        // Loaded Material Objects
        std::vector<Material> LoadedMaterials;

    private:
        // Generate vertices from a list of positions,
        //	tcoords, normals and a face line
        void genVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
            const std::vector<glm::vec3>& iPositions,
            const std::vector<glm::vec2>& iTCoords,
            const std::vector<glm::vec3>& iNormals,
            std::string icurline);

        // Triangulate a list of vertices into a face by printing
        //	indicies corresponding with triangles within it
        void vertexTriangluation(std::vector<unsigned int>& oIndices, const std::vector<Vertex>& iVerts);

        // Load Materials from .mtl file
        bool loadMaterials(std::string& path);

        // Loaded Vertex Objects
        std::vector<Vertex>& m_LoadedVertices;
        // Loaded Index Positions
        std::vector<unsigned int>& m_LoadedIndices;
	};
}