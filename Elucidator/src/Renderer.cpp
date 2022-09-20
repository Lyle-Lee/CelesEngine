#include "Renderer.h"
#include "Utils.h"

Renderer::Renderer(std::vector<MeshObject*>& objs, DirectionalLight* light, const int numAttachments)
    : m_Objects(objs), m_Light(light)
{
    if (numAttachments > 0)
    {
        m_GBufferTextures.reserve(numAttachments);
        m_Attachments.reserve(numAttachments);

        m_GBuffer = new FrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
        m_GBufferTextures.push_back(std::make_unique<Texture>(WINDOW_WIDTH, WINDOW_HEIGHT)); // Kd
        m_GBuffer->bindTexture(*m_GBufferTextures[0]);
        m_Attachments.push_back(GL_COLOR_ATTACHMENT0);
        for (unsigned int i = 1; i < numAttachments; ++i)
        {   // Geometry infos
            m_GBufferTextures.push_back(std::make_unique<Texture>(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA16F, GL_FLOAT));
            m_GBuffer->bindTexture(*m_GBufferTextures[i], i);
            m_Attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        glDrawBuffers(numAttachments, &m_Attachments[0]);
        m_GBuffer->bindRenderBuffer();
        m_GBuffer->checkStatus();
        m_GBuffer->unbind();

        m_GBufferMtl = new Material("assets/shaders/GBufferVertexShader.glsl", "assets/shaders/GBufferFragmentShader.glsl");
    }
}

Renderer::~Renderer()
{
    delete m_GBuffer;
    delete m_GBufferMtl;
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
    // Light pass
    glm::mat4 lightVP;
    if (m_Light->material->hasShadowMap)
    {
        m_Light->fbo->bind();
        glViewport(0, 0, RESOLUTION, RESOLUTION);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        clear();
        glEnable(GL_DEPTH_TEST);

        lightVP = m_Light->calcLightVP();
        m_Light->material->shader->bind();
        m_Light->material->shader->setUniformMat4f("uLightVP", lightVP);
        for (int i = 0; i < m_Objects.size(); ++i)
        {
            m_Light->material->shader->setUniformMat4f("uModel", m_Objects[i]->translation);
            draw(m_VAs[i], m_IBs[i], m_Light->material->shader);
        }
        m_Light->fbo->unbind();
    }

    // G-buffer pass
    if (m_Attachments.size() > 0)
    {
        m_GBuffer->bind();
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        clear();
        glEnable(GL_DEPTH_TEST);

        m_GBufferMtl->shader->bind();
        // TODO: camera VP, camera pos

        if (m_Light->material->hasShadowMap)
        {
            m_GBufferMtl->shader->setUniformMat4f("uLightVP", lightVP);
            m_Light->material->shadowMap->get()->bind();
            m_GBufferMtl->shader->setUniform1i("uShadowMap", 0);
        }

        for (int i = 0; i < m_Objects.size(); ++i)
        {
            m_GBufferMtl->shader->setUniform1i("materialID", i);
            m_GBufferMtl->shader->setUniformMat4f("uModel", m_Objects[i]->translation);
            // TODO: bind Texture Unit to G-buffer shader for each obj

            draw(m_VAs[i], m_IBs[i], m_GBufferMtl->shader);
        }
        m_GBuffer->unbind();
    }

    // Camera pass
    for (int i = 0; i < m_Objects.size(); ++i)
    {
        m_Objects[i]->material->shader->bind();
        m_Objects[i]->material->shader->setUniformMat4f("uModel", m_Objects[i]->translation);
        // TODO: camera VP, camera pos
        m_Objects[i]->material->shader->setUniform3fv("uLightPos", 1, &m_Light->lightPos.x);

        if (m_Light->material->hasShadowMap && m_Attachments.size() == 0)
        {
            m_Objects[i]->material->shader->setUniformMat4f("uLightVP", lightVP);
            m_Light->material->shadowMap->get()->bind();
            m_Objects[i]->material->shader->setUniform1i("uShadowMap", 0);
        }

        draw(m_VAs[i], m_IBs[i], m_Objects[i]->material->shader);
    }
}

//void Renderer::updateLightInfo(const glm::vec3 lightPos, const glm::vec3 lightDir)
//{
//    m_Light->lightPos = lightPos;
//    m_Light->lightDir = lightDir;
//}
