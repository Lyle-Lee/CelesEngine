#include "TestShadowMap.h"
#include "../Utils.h"
#include "../ImGui/imgui.h"

test::TestShadowMap::TestShadowMap()
    : m_Translation(0.0f, 0.0f, 0.0f), m_LightPos(-5.0f, 10.0f, 20.0f), m_CameraPos(5.0f, 5.0f, 5.0f),
    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, -5.0f))),
    m_Projection(glm::perspectiveFov(glm::pi<float>() / 2.0f, 4.0f, 3.0f, 1.5f, 100.0f))
{
    m_Material = new PhongMaterial("assets/shaders/PhongVertexShader.glsl", "assets/shaders/PhongFragmentShader.glsl");
    m_LightMtl = new EmissiveMaterial("assets/shaders/ShadowVertexShader.glsl", "assets/shaders/ShadowFragmentShader.glsl", glm::vec3(1.0f));

    m_Objs.push_back(std::make_unique<MeshObject>("assets/models/TestObj.obj", m_Material));
    m_Objs.push_back(std::make_unique<MeshObject>("assets/models/floor/floor.obj", m_Material));
    for (int i = 0; i < m_Objs.size(); ++i) {
        unsigned int vertexCount = m_Objs[i]->vertices.size();
        unsigned int indexCount = m_Objs[i]->indices.size();

        m_VAs.push_back(std::make_unique<VertexArray>());
        m_VBs.push_back(std::make_unique<VertexBuffer>(&(m_Objs[i]->vertices[0]), vertexCount * sizeof(Vertex)));
        VertexBufferLayout vbLayout;
        vbLayout.push<float>(3); // pos
        vbLayout.push<float>(3); // normal
        vbLayout.push<float>(2); // texCoord
        m_VAs[i]->addBuffer(*m_VBs[i], vbLayout);
        
        m_IBs.push_back(std::make_unique<IndexBuffer>(&(m_Objs[i]->indices[0]), indexCount));
    }

    m_Material->shader->bind();
    m_Material->shader->setUniform3f("uKa", 0.0f, 0.5f, 1.0f);
    m_Material->shader->setUniform3f("uKd", 0.796f, 0.52f, 0.482f);
    m_Material->shader->setUniform3f("uKs", 0.5f, 0.5f, 0.5f);
    m_Material->shader->setUniform3f("uLightIntensity", 225.0f, 225.0f, 225.0f);

    m_Light = std::make_unique<DirectionalLight>(m_LightPos, -m_LightPos, glm::vec3(0.0f, 1.0f, 0.0f), m_LightMtl);

    m_FB = std::make_unique<FrameBuffer>(RESOLUTION, RESOLUTION);
    m_LightMtl->shadowMap->get()->bind();
    m_FB->bindTexture(*m_LightMtl->shadowMap->get());
    m_FB->bindRenderBuffer();
    m_FB->checkStatus();
    m_FB->unbind();

    glEnable(GL_DEPTH_TEST);
}

test::TestShadowMap::~TestShadowMap()
{
    delete m_Material;
    delete m_LightMtl;
}

void test::TestShadowMap::onUpdate(float deltaTime)
{
}

void test::TestShadowMap::onRender()
{
    m_FB->bind();
    glViewport(0, 0, RESOLUTION, RESOLUTION);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
    m_Light->lightPos = m_LightPos;
    m_Light->lightDir = -m_LightPos;
    glm::mat4 lightVP = m_Light->calcLightVP();

    m_LightMtl->shader->bind();
    m_LightMtl->shader->setUniformMat4f("uModel", model);
    m_LightMtl->shader->setUniformMat4f("uLightVP", lightVP);

    for (int i = 0; i < m_Objs.size(); ++i)
    {
        m_VAs[i]->bind();
        m_IBs[i]->bind();
        GLCall(glDrawElements(GL_TRIANGLES, m_IBs[i]->GetCount(), GL_UNSIGNED_INT, nullptr))
    }

    m_FB->unbind();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Material->shader->bind();
    m_Material->shader->setUniform3fv("uLightPos", 1, &m_LightPos.x);
    m_Material->shader->setUniformMat4f("uModel", model);
    m_View = glm::lookAtRH(m_CameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 vp = m_Projection * m_View;
    m_Material->shader->setUniformMat4f("uViewProj", vp);
    m_Material->shader->setUniform3fv("uCameraPos", 1, &m_CameraPos.x);

    m_Material->shader->setUniformMat4f("uLightVP", lightVP);
    m_LightMtl->shadowMap->get()->bind();
    m_Material->shader->setUniform1i("uShadowMap", 0);

    for (int i = 0; i < m_Objs.size(); ++i)
    {
        m_VAs[i]->bind();
        m_IBs[i]->bind();
        GLCall(glDrawElements(GL_TRIANGLES, m_IBs[i]->GetCount(), GL_UNSIGNED_INT, nullptr))
    }
}

void test::TestShadowMap::onImGuiRender()
{
    ImGui::SliderFloat3("Light Position", &m_LightPos.x, -50.0f, 50.0f);
    ImGui::SliderFloat3("Translation", &m_Translation.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Camera Position", &m_CameraPos.x, -5.0f, 5.0f);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
