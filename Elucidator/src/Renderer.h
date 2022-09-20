#pragma once

#include <memory>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "FrameBuffer.h"
#include "Shader.h"
#include "MeshObject.h"
#include "Material.h"
#include "Light.h"

class Renderer
{
public:
    Renderer(std::vector<MeshObject*>& objs, DirectionalLight* light, const int numAttachments = 0);
    ~Renderer();

    void init();
    void shutdown();
    void draw(VertexArray& va, IndexBuffer& ib, std::unique_ptr<Shader>& shader) const;
    void clear() const;
    void render();
    //void updateLightInfo(const glm::vec3 lightPos, const glm::vec3 lightDir);
private:
    std::vector<MeshObject*> m_Objects;
    DirectionalLight* m_Light;

    std::vector<VertexBuffer> m_VBs;
    std::vector<VertexArray> m_VAs;
    std::vector<IndexBuffer> m_IBs;

    FrameBuffer* m_GBuffer = nullptr;
    std::vector<std::unique_ptr<Texture>> m_GBufferTextures;
    std::vector<unsigned int> m_Attachments;
    Material* m_GBufferMtl = nullptr;
};