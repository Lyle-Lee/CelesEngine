#include "Renderer.h"
#include "Utils.h"

Renderer::Renderer(std::vector<MeshObject*>& objs, DirectionalLight* light)
    : m_Objects(objs), m_Light(light)
{
}

Renderer::~Renderer()
{
}

void Renderer::init()
{
    for (int i = 0; i < m_Objects.size(); ++i)
    {
        unsigned int vertexCount = m_Objects[i]->vertices.size();
        unsigned int indexCount = m_Objects[i]->indices.size();
        m_VAs[i] = VertexArray();
        m_VBs[i] = VertexBuffer(&(m_Objects[i]->vertices[0]), vertexCount * sizeof(Vertex));

        VertexBufferLayout vbLayout;
        vbLayout.push<float>(3); // pos
        vbLayout.push<float>(3); // normal
        vbLayout.push<float>(2); // texCoord
        m_VAs[i].addBuffer(m_VBs[i], vbLayout);

        m_IBs[i] = IndexBuffer(&(m_Objects[i]->indices[0]), indexCount);

        m_Objects[i]->material->bindAttribute();
    }

    m_FB = std::make_unique<FrameBuffer>(RESOLUTION, RESOLUTION);
    if (m_Light->material->hasShadowMap)
    {
        m_Light->material->shadowMap->get()->bind();
        m_FB->bindTexture(*m_Light->material->shadowMap->get());
        m_FB->bindRenderBuffer();
        m_FB->checkStatus();
    }
    m_FB->unbind();

    glEnable(GL_DEPTH_TEST);
}

void Renderer::shutdown()
{
}

void Renderer::draw(VertexArray& va, IndexBuffer& ib, std::unique_ptr<Shader>& shader) const
{
    shader->bind();
    va.bind();
    ib.bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr))
}

void Renderer::clear() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render()
{
    m_FB->bind();
    glViewport(0, 0, RESOLUTION, RESOLUTION);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    clear();
    glEnable(GL_DEPTH_TEST);

    // Light pass
    glm::mat4 lightVP;
    if (m_Light->material->hasShadowMap)
    {
        lightVP = m_Light->calcLightVP();
        m_Light->material->shader->bind();
        m_Light->material->shader->setUniformMat4f("uLightVP", lightVP);
        for (int i = 0; i < m_Objects.size(); ++i)
        {
            m_Light->material->shader->setUniformMat4f("uModel", m_Objects[i]->translation);
            draw(m_VAs[i], m_IBs[i], m_Light->material->shader);
        }
    }

    m_FB->unbind();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Camera pass
    for (int i = 0; i < m_Objects.size(); ++i)
    {
        m_Objects[i]->material->shader->bind();
        m_Objects[i]->material->shader->setUniformMat4f("uModel", m_Objects[i]->translation);
        // TODO: camera VP, camera pos
        m_Objects[i]->material->shader->setUniform3fv("uLightPos", 1, &m_Light->lightPos.x);

        if (m_Light->material->hasShadowMap)
        {
            m_Objects[i]->material->shader->setUniformMat4f("uLightVP", lightVP);
            m_Light->material->shadowMap->get()->bind();
            m_Objects[i]->material->shader->setUniform1i("uShadowMap", 0); // TODO: G-buffer
        }

        draw(m_VAs[i], m_IBs[i], m_Objects[i]->material->shader);
    }
}

//void Renderer::updateLightInfo(const glm::vec3 lightPos, const glm::vec3 lightDir)
//{
//    m_Light->lightPos = lightPos;
//    m_Light->lightDir = lightDir;
//}
