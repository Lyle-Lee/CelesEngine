#include "TestSSGI.h"
#include "../Utils.h"
#include "../ImGui/imgui.h"

test::TestSSGI::TestSSGI()
    : m_Translation(0.0f, 0.0f, 0.0f), m_LightPos(-5.0f, 10.0f, 2.5f), m_CameraPos(5.0f, 5.0f, 5.0f),
    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, -5.0f))),
    m_Projection(glm::perspectiveFov(glm::pi<float>() / 2.0f, 4.0f, 3.0f, 1.5f, 100.0f))
{
    m_Material = new Material("assets/shaders/SSGIVertexShader.glsl", "assets/shaders/SSGIFragmentShader.glsl");
    m_LightMtl = new EmissiveMaterial("assets/shaders/ShadowVertexShader.glsl", "assets/shaders/ShadowFragmentShader.glsl", glm::vec3(1.0f));
    m_GBufferMtl = new Material("assets/shaders/GBufferVertexShader.glsl", "assets/shaders/GBufferFragmentShader.glsl");

    m_Textures.push_back(std::make_unique<Texture>("assets/models/floor/siEoZ_2K_Albedo.jpg"));
    m_Textures.push_back(std::make_unique<Texture>("assets/models/floor/siEoZ_2K_Normal_LOD0.jpg"));

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

    m_Light = std::make_unique<DirectionalLight>(m_LightPos, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), m_LightMtl);

    m_Material->shader->bind();
    m_Material->shader->setUniform3fv("uLightRadiance", 1, &m_Light->radiance.x);
    m_GBufferMtl->shader->bind();
    m_GBufferMtl->shader->setUniform3f("uKd_vec", 0.796f, 0.52f, 0.482f);

    m_LightFB = std::make_unique<FrameBuffer>(RESOLUTION, RESOLUTION);
    m_LightMtl->shadowMap->get()->bind();
    m_LightFB->bindTexture(*m_LightMtl->shadowMap->get());
    m_LightFB->bindRenderBuffer();
    m_LightFB->checkStatus();
    m_LightFB->unbind();

    m_GBuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_GBufferTextures.push_back(std::make_unique<Texture>(WINDOW_WIDTH, WINDOW_HEIGHT)); // Kd
    m_GBuffer->bindTexture(*m_GBufferTextures[0]);
    m_Attachments.push_back(GL_COLOR_ATTACHMENT0);
    for (unsigned int i = 1; i < 4; ++i)
    {   // Geometry infos
        m_GBufferTextures.push_back(std::make_unique<Texture>(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA16F, GL_FLOAT));
        m_GBuffer->bindTexture(*m_GBufferTextures[i], i);
        m_Attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    }
    glDrawBuffers(m_Attachments.size(), &m_Attachments[0]);
    m_GBuffer->bindRenderBuffer();
    m_GBuffer->checkStatus();
    m_GBuffer->unbind();

    glEnable(GL_DEPTH_TEST);
}

test::TestSSGI::~TestSSGI()
{
    delete m_Material;
    delete m_LightMtl;
    delete m_GBufferMtl;
}

void test::TestSSGI::onUpdate(float deltaTime)
{
}

void test::TestSSGI::onRender()
{
    // Light pass
    m_LightFB->bind();
    glViewport(0, 0, RESOLUTION, RESOLUTION);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
    m_Light->lightPos = m_LightPos;
    m_Light->lightDir = glm::normalize(-m_LightPos);
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
    m_LightFB->unbind();

    // G-buffer pass
    m_GBuffer->bind();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_GBufferMtl->shader->bind();
    m_GBufferMtl->shader->setUniformMat4f("uModel", model);
    m_View = glm::lookAtRH(m_CameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 vp = m_Projection * m_View;
    m_GBufferMtl->shader->setUniformMat4f("uViewProj", vp);
    m_GBufferMtl->shader->setUniformMat4f("uLightVP", lightVP);

    m_Textures[0]->bind();
    m_Textures[1]->bind(1);
    m_GBufferMtl->shader->setUniform1i("uKd", 0);
    m_GBufferMtl->shader->setUniform1i("uNt", 1);
    m_LightMtl->shadowMap->get()->bind(2);
    m_GBufferMtl->shader->setUniform1i("uShadowMap", 2);

    for (int i = 0; i < m_Objs.size(); ++i)
    {
        m_GBufferMtl->shader->setUniform1i("materialID", i);

        m_VAs[i]->bind();
        m_IBs[i]->bind();
        GLCall(glDrawElements(GL_TRIANGLES, m_IBs[i]->GetCount(), GL_UNSIGNED_INT, nullptr))
    }
    m_GBuffer->unbind();

    // Camera pass
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Material->shader->bind();
    m_Material->shader->setUniformMat4f("uModel", model);
    m_Material->shader->setUniformMat4f("uViewProj", vp);
    //m_Material->shader->setUniformMat4f("uLightVP", lightVP);
    m_Light->lightDir = -m_Light->lightDir;
    m_Material->shader->setUniform3fv("uLightDir", 1, &m_Light->lightDir.x);
    m_Material->shader->setUniform3fv("uCameraPos", 1, &m_CameraPos.x);

    for (int i = 0; i < m_GBufferTextures.size(); ++i)
    {
        m_GBufferTextures[i]->bind(i);
    }
    m_Material->shader->setUniform1i("uGDiffuse", 0);
    m_Material->shader->setUniform1i("uGDepth", 1);
    m_Material->shader->setUniform1i("uGNormalWorld", 2);
    m_Material->shader->setUniform1i("uGShadow", 3);
    //m_Material->shader->setUniform1i("uGPosWorld", 4);

    for (int i = 0; i < m_Objs.size(); ++i)
    {
        m_VAs[i]->bind();
        m_IBs[i]->bind();
        GLCall(glDrawElements(GL_TRIANGLES, m_IBs[i]->GetCount(), GL_UNSIGNED_INT, nullptr))
    }
}

void test::TestSSGI::onImGuiRender()
{
    ImGui::SliderFloat3("Light Position", &m_LightPos.x, -50.0f, 50.0f);
    //ImGui::SliderFloat3("Translation", &m_Translation.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Camera Position", &m_CameraPos.x, -5.0f, 5.0f);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
