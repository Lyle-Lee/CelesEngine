#include <gtc/matrix_transform.hpp>
#include "OBJLoader.h"

loader::OBJLoader::OBJLoader(std::vector<Vertex>& meshVertices, std::vector<unsigned int>& meshIndices)
    : m_LoadedVertices(meshVertices), m_LoadedIndices(meshIndices)
{
}

loader::OBJLoader::~OBJLoader()
{
    //LoadedMeshes.clear();
}

bool loader::OBJLoader::loadFile(const std::string& filepath)
{
    // If the file is not an .obj file return false
    if (filepath.substr(filepath.size() - 4, 4) != ".obj")
        return false;


    std::ifstream file(filepath);

    if (!file.is_open())
        return false;

    //LoadedMeshes.clear();
    m_LoadedVertices.clear();
    m_LoadedIndices.clear();

    std::vector<glm::vec3> Positions;
    std::vector<glm::vec2> TCoords;
    std::vector<glm::vec3> Normals;

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    std::vector<std::string> MeshMatNames;

    bool listening = false;
    std::string meshname;

    //Mesh tempMesh;

#ifdef LOADER_CONSOLE_OUTPUT
    const unsigned int outputEveryNth = 1000;
    unsigned int outputIndicator = outputEveryNth;
#endif

    std::string curline;
    while (std::getline(file, curline))
    {
#ifdef LOADER_CONSOLE_OUTPUT
        if ((outputIndicator = ((outputIndicator + 1) % outputEveryNth)) == 1)
        {
            if (!meshname.empty())
            {
                std::cout
                    << "\r- " << meshname
                    << "\t| vertices > " << Positions.size()
                    << "\t| texcoords > " << TCoords.size()
                    << "\t| normals > " << Normals.size()
                    << "\t| triangles > " << (Vertices.size() / 3)
                    << (!MeshMatNames.empty() ? "\t| material: " + MeshMatNames.back() : "");
            }
        }
#endif

        // Generate a Mesh Object or Prepare for an object to be created
        if (firstToken(curline) == "o" || firstToken(curline) == "g" || curline[0] == 'g')
        {
            if (!listening)
            {
                listening = true;

                if (firstToken(curline) == "o" || firstToken(curline) == "g")
                {
                    meshname = tail(curline);
                }
                else
                {
                    meshname = "unnamed";
                }
            }
            else
            {
                // Generate the mesh to put into the array

                if (!Indices.empty() && !Vertices.empty())
                {
                    // Create Mesh
                    //tempMesh = Mesh(Vertices, Indices);
                    //tempMesh.MeshName = meshname;

                    // Insert Mesh
                    //LoadedMeshes.push_back(tempMesh);

                    // Cleanup
                    Vertices.clear();
                    Indices.clear();
                    meshname.clear();

                    meshname = tail(curline);
                }
                else
                {
                    if (firstToken(curline) == "o" || firstToken(curline) == "g")
                    {
                        meshname = tail(curline);
                    }
                    else
                    {
                        meshname = "unnamed";
                    }
                }
            }
#ifdef LOADER_CONSOLE_OUTPUT
            std::cout << std::endl;
            outputIndicator = 0;
#endif
        }
        // Generate a Vertex Position
        if (firstToken(curline) == "v")
        {
            std::vector<std::string> spos;
            glm::vec3 vpos;
            split(tail(curline), spos, " ");

            vpos.x = std::stof(spos[0]);
            vpos.y = std::stof(spos[1]);
            vpos.z = std::stof(spos[2]);

            Positions.push_back(vpos);
        }
        // Generate a Vertex Texture Coordinate
        if (firstToken(curline) == "vt")
        {
            std::vector<std::string> stex;
            glm::vec2 vtex;
            split(tail(curline), stex, " ");

            vtex.x = std::stof(stex[0]);
            vtex.y = std::stof(stex[1]);

            TCoords.push_back(vtex);
        }
        // Generate a Vertex Normal;
        if (firstToken(curline) == "vn")
        {
            std::vector<std::string> snor;
            glm::vec3 vnor;
            split(tail(curline), snor, " ");

            vnor.x = std::stof(snor[0]);
            vnor.y = std::stof(snor[1]);
            vnor.z = std::stof(snor[2]);

            Normals.push_back(vnor);
        }
        // Generate a Face (vertices & indices)
        if (firstToken(curline) == "f")
        {
            // Generate the vertices
            std::vector<Vertex> vVerts;
            genVerticesFromRawOBJ(vVerts, Positions, TCoords, Normals, curline);

            // Add Vertices
            for (int i = 0; i < vVerts.size(); ++i)
            {
                Vertices.push_back(vVerts[i]);

                m_LoadedVertices.push_back(vVerts[i]);
            }

            std::vector<unsigned int> iIndices;

            vertexTriangluation(iIndices, vVerts);

            // Add Indices
            for (int i = 0; i < iIndices.size(); ++i)
            {
                unsigned int indnum = (unsigned int)((Vertices.size()) - vVerts.size()) + iIndices[i];
                Indices.push_back(indnum);

                indnum = (unsigned int)((m_LoadedVertices.size()) - vVerts.size()) + iIndices[i];
                m_LoadedIndices.push_back(indnum);

            }
        }
        // Get Mesh Material Name
        if (firstToken(curline) == "usemtl")
        {
            MeshMatNames.push_back(tail(curline));

            // Create new Mesh, if Material changes within a group
            if (!Indices.empty() && !Vertices.empty())
            {
                // Create Mesh
                /*tempMesh = Mesh(Vertices, Indices);
                tempMesh.MeshName = meshname;
                int i = 2;
                while (1) {
                    tempMesh.MeshName = meshname + "_" + std::to_string(i);

                    for (auto& m : LoadedMeshes)
                        if (m.MeshName == tempMesh.MeshName)
                            continue;
                    break;
                }*/

                // Insert Mesh
                //LoadedMeshes.push_back(tempMesh);

                // Cleanup
                Vertices.clear();
                Indices.clear();
            }

#ifdef LOADER_CONSOLE_OUTPUT
            outputIndicator = 0;
#endif
        }
        // Load Materials
        if (firstToken(curline) == "mtllib")
        {
            // Generate LoadedMaterial

            // Generate a path to the material file
            std::vector<std::string> temp;
            split(filepath, temp, "/");

            std::string pathtomat = "";

            if (temp.size() != 1)
            {
                for (int i = 0; i < temp.size() - 1; ++i)
                {
                    pathtomat += temp[i] + "/";
                }
            }


            pathtomat += tail(curline);

#ifdef LOADER_CONSOLE_OUTPUT
            std::cout << std::endl << "- find materials in: " << pathtomat << std::endl;
#endif

            // Load Materials
            loadMaterials(pathtomat);
        }
    }

#ifdef LOADER_CONSOLE_OUTPUT
    std::cout << std::endl;
#endif

    // Deal with last mesh

    if (!Indices.empty() && !Vertices.empty())
    {
        // Create Mesh
        //tempMesh = Mesh(Vertices, Indices);
        //tempMesh.MeshName = meshname;

        // Insert Mesh
        //LoadedMeshes.push_back(tempMesh);
    }

    file.close();

    // Set Materials for each Mesh
    for (int i = 0; i < MeshMatNames.size(); ++i)
    {
        std::string matname = MeshMatNames[i];

        // Find corresponding material name in loaded materials
        // when found copy material variables into mesh material
        for (int j = 0; j < LoadedMaterials.size(); ++j)
        {
            if (LoadedMaterials[j].name == matname)
            {
                //LoadedMeshes[i].MeshMaterial = LoadedMaterials[j];
                break;
            }
        }
    }

    //if (LoadedMeshes.empty() && m_LoadedVertices.empty() && m_LoadedIndices.empty())
    if (m_LoadedVertices.empty() && m_LoadedIndices.empty())
    {
        return false;
    }
    return true;
}

void loader::OBJLoader::genVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
    const std::vector<glm::vec3>& iPositions,
    const std::vector<glm::vec2>& iTCoords,
    const std::vector<glm::vec3>& iNormals,
    std::string icurline)
{
    std::vector<std::string> sface, svert;
    Vertex vVert;
    split(tail(icurline), sface, " ");
    bool noNormal = false;

    // For every given vertex do this
    for (std::string& face : sface)
    {
        // See What type the vertex is.
        int vtype = 0;

        split(face, svert, "/");

        // Check for just position - v1
        if (svert.size() == 1)
        {
            // Only position
            vtype = 1;
        }

        // Check for position & texture - v1/vt1
        if (svert.size() == 2)
        {
            // Position & Texture
            vtype = 2;
        }

        // Check for Position, Texture and Normal - v1/vt1/vn1
        // or if Position and Normal - v1//vn1
        if (svert.size() == 3)
        {
            if (svert[1].size())
            {
                // Position, Texture, and Normal
                vtype = 4;
            }
            else
            {
                // Position & Normal
                vtype = 3;
            }
        }

        // Calculate and store the vertex
        switch (vtype)
        {
        case 1: // P
            vVert.position = getElement(iPositions, svert[0]);
            vVert.texCoord = glm::vec2(0.0f);
            noNormal = true;
            oVerts.push_back(vVert);
            break;
        case 2: // P/T
            vVert.position = getElement(iPositions, svert[0]);
            vVert.texCoord = getElement(iTCoords, svert[1]);
            noNormal = true;
            oVerts.push_back(vVert);
            break;
        case 3: // P//N
            vVert.position = getElement(iPositions, svert[0]);
            vVert.texCoord = glm::vec2(0.0f);
            vVert.normal = getElement(iNormals, svert[2]);
            oVerts.push_back(vVert);
            break;
        case 4: // P/T/N
            vVert.position = getElement(iPositions, svert[0]);
            vVert.texCoord = getElement(iTCoords, svert[1]);
            vVert.normal = getElement(iNormals, svert[2]);
            oVerts.push_back(vVert);
            break;
        default:
            break;
        }
    }

    // take care of missing normals
    // these may not be truly acurate but it is the
    // best they get for not compiling a mesh with normals
    if (noNormal)
    {
        glm::vec3 A = oVerts[0].position - oVerts[1].position;
        glm::vec3 B = oVerts[2].position - oVerts[1].position;

        glm::vec3 normal = glm::cross(A, B);

        for (int i = 0; i < oVerts.size(); ++i)
        {
            oVerts[i].normal = normal;
        }
    }
}

void loader::OBJLoader::vertexTriangluation(std::vector<unsigned int>& oIndices, const std::vector<Vertex>& iVerts)
{
    // If there are 2 or less verts,
    // no triangle can be created,
    // so exit
    if (iVerts.size() < 3)
    {
        return;
    }
    // If it is a triangle no need to calculate it
    if (iVerts.size() == 3)
    {
        oIndices.push_back(0);
        oIndices.push_back(1);
        oIndices.push_back(2);
        return;
    }

    // Create a list of vertices
    std::vector<Vertex> tVerts = iVerts;

    while (true)
    {
        // For every vertex
        for (int i = 0; i < tVerts.size(); ++i)
        {
            // pPrev = the previous vertex in the list
            Vertex pPrev = tVerts[(i - 1 + tVerts.size()) % tVerts.size()];

            // pCur = the current vertex;
            Vertex pCur = tVerts[i];

            // pNext = the next vertex in the list
            Vertex pNext = tVerts[(i + 1) % tVerts.size()];

            // Check to see if there are only 3 verts left
            // if so this is the last triangle
            if (tVerts.size() == 3)
            {
                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < int(tVerts.size()); j++)
                {
                    if (iVerts[j].position == pCur.position)
                        oIndices.push_back(j);
                    if (iVerts[j].position == pPrev.position)
                        oIndices.push_back(j);
                    if (iVerts[j].position == pNext.position)
                        oIndices.push_back(j);
                }

                tVerts.clear();
                break;
            }
            if (tVerts.size() == 4)
            {
                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < iVerts.size(); ++j)
                {
                    if (iVerts[j].position == pCur.position)
                        oIndices.push_back(j);
                    if (iVerts[j].position == pPrev.position)
                        oIndices.push_back(j);
                    if (iVerts[j].position == pNext.position)
                        oIndices.push_back(j);
                }

                glm::vec3 tempVec;
                for (int j = 0; j < tVerts.size(); ++j)
                {
                    if (tVerts[j].position != pCur.position
                        && tVerts[j].position != pPrev.position
                        && tVerts[j].position != pNext.position)
                    {
                        tempVec = tVerts[j].position;
                        break;
                    }
                }

                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < iVerts.size(); ++j)
                {
                    if (iVerts[j].position == pPrev.position)
                        oIndices.push_back(j);
                    if (iVerts[j].position == pNext.position)
                        oIndices.push_back(j);
                    if (iVerts[j].position == tempVec)
                        oIndices.push_back(j);
                }

                tVerts.clear();
                break;
            }

            // If Vertex is not an interior vertex
            float angle = acosf(glm::dot(glm::normalize(pPrev.position - pCur.position), glm::normalize(pNext.position - pCur.position))) * (180.0f / glm::pi<float>());
            if (angle <= 0 && angle >= 180)
                continue;

            // If any vertices are within this triangle
            bool inTri = false;
            for (int j = 0; j < iVerts.size(); ++j)
            {
                if (inTriangle(iVerts[j].position, pPrev.position, pCur.position, pNext.position)
                    && iVerts[j].position != pPrev.position
                    && iVerts[j].position != pCur.position
                    && iVerts[j].position != pNext.position)
                {
                    inTri = true;
                    break;
                }
            }
            if (inTri)
                continue;

            // Create a triangle from pCur, pPrev, pNext
            for (int j = 0; j < iVerts.size(); ++j)
            {
                if (iVerts[j].position == pCur.position)
                    oIndices.push_back(j);
                if (iVerts[j].position == pPrev.position)
                    oIndices.push_back(j);
                if (iVerts[j].position == pNext.position)
                    oIndices.push_back(j);
            }

            // Delete pCur from the list
            for (int j = 0; j < tVerts.size(); ++j)
            {
                if (tVerts[j].position == pCur.position)
                {
                    tVerts.erase(tVerts.begin() + j);
                    break;
                }
            }

            // reset i to the start
            // -1 since loop will add 1 to it
            i = -1;
        }

        // if no triangles were created
        if (oIndices.size() == 0)
            break;

        // if no more vertices
        if (tVerts.size() == 0)
            break;
    }
}

bool loader::OBJLoader::loadMaterials(std::string& path)
{
    // If the file is not a material file return false
    if (path.substr(path.size() - 4, path.size()) != ".mtl")
        return false;

    std::ifstream file(path);

    // If the file is not found return false
    if (!file.is_open())
        return false;

    Material tempMaterial;

    bool listening = false;

    // Go through each line looking for material variables
    std::string curline;
    while (std::getline(file, curline))
    {
        // new material and material name
        if (firstToken(curline) == "newmtl")
        {
            if (!listening)
            {
                listening = true;

                if (curline.size() > 7)
                {
                    tempMaterial.name = tail(curline);
                }
                else
                {
                    tempMaterial.name = "none";
                }
            }
            else
            {
                // Generate the material

                // Push Back loaded Material
                LoadedMaterials.push_back(tempMaterial);

                // Clear Loaded Material
                tempMaterial = Material();

                if (curline.size() > 7)
                {
                    tempMaterial.name = tail(curline);
                }
                else
                {
                    tempMaterial.name = "none";
                }
            }
        }
        // Ambient Color
        if (firstToken(curline) == "Ka")
        {
            std::vector<std::string> temp;
            split(tail(curline), temp, " ");

            if (temp.size() != 3)
                continue;

            tempMaterial.Ka.x = std::stof(temp[0]);
            tempMaterial.Ka.y = std::stof(temp[1]);
            tempMaterial.Ka.z = std::stof(temp[2]);
        }
        // Diffuse Color
        if (firstToken(curline) == "Kd")
        {
            std::vector<std::string> temp;
            split(tail(curline), temp, " ");

            if (temp.size() != 3)
                continue;

            tempMaterial.Kd.x = std::stof(temp[0]);
            tempMaterial.Kd.y = std::stof(temp[1]);
            tempMaterial.Kd.z = std::stof(temp[2]);
        }
        // Specular Color
        if (firstToken(curline) == "Ks")
        {
            std::vector<std::string> temp;
            split(tail(curline), temp, " ");

            if (temp.size() != 3)
                continue;

            tempMaterial.Ks.x = std::stof(temp[0]);
            tempMaterial.Ks.y = std::stof(temp[1]);
            tempMaterial.Ks.z = std::stof(temp[2]);
        }
        // Specular Exponent
        if (firstToken(curline) == "Ns")
        {
            tempMaterial.Ns = std::stof(tail(curline));
        }
        // Optical Density
        if (firstToken(curline) == "Ni")
        {
            tempMaterial.Ni = std::stof(tail(curline));
        }
        // Dissolve
        if (firstToken(curline) == "d")
        {
            tempMaterial.d = std::stof(tail(curline));
        }
        // Illumination
        if (firstToken(curline) == "illum")
        {
            tempMaterial.illum = std::stoi(tail(curline));
        }
        // Ambient Texture Map
        if (firstToken(curline) == "map_Ka")
        {
            tempMaterial.map_Ka = tail(curline);
        }
        // Diffuse Texture Map
        if (firstToken(curline) == "map_Kd")
        {
            tempMaterial.map_Kd = tail(curline);
        }
        // Specular Texture Map
        if (firstToken(curline) == "map_Ks")
        {
            tempMaterial.map_Ks = tail(curline);
        }
        // Specular Hightlight Map
        if (firstToken(curline) == "map_Ns")
        {
            tempMaterial.map_Ns = tail(curline);
        }
        // Alpha Texture Map
        if (firstToken(curline) == "map_d")
        {
            tempMaterial.map_d = tail(curline);
        }
        // Bump Map
        if (firstToken(curline) == "map_Bump" || firstToken(curline) == "map_bump" || firstToken(curline) == "bump")
        {
            tempMaterial.map_bump = tail(curline);
        }
    }

    // Deal with last material

    // Push Back loaded Material
    LoadedMaterials.push_back(tempMaterial);

    // Test to see if anything was loaded
    // If not return false
    if (LoadedMaterials.empty())
        return false;
    // If so return true
    else
        return true;
}
