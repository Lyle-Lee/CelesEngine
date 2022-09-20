#include "TestMeshObject.h"
#include "../Utils.h"
#include "../ImGui/imgui.h"

test::TestMeshObject::TestMeshObject()
    : m_Translation(0.0f, 0.0f, 0.0f), m_LightPos(-5.0f, 10.0f, 20.0f), m_CameraPos(0.0f, 2.0f, 5.0f),
    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, -5.0f))),
    m_Projection(glm::perspectiveFov(glm::pi<float>() / 2.0f, 4.0f, 3.0f, 1.5f, 100.0f))
{
    m_Material = new Material("assets/shaders/PhongVertexShader.glsl", "assets/shaders/PhongFragmentShader.glsl");

    m_Obj = std::make_unique<MeshObject>("assets/models/TestObj.obj", m_Material);
    unsigned int vertexCount = m_Obj->vertices.size();
    unsigned int indexCount = m_Obj->indices.size();

    m_VA = std::make_unique<VertexArray>();
    m_VB = std::make_unique<VertexBuffer>(&(m_Obj->vertices[0]), vertexCount * sizeof(Vertex));
    VertexBufferLayout vbLayout;
    vbLayout.push<float>(3); // pos
    vbLayout.push<float>(3); // normal
    vbLayout.push<float>(2); // texCoord
    m_VA->addBuffer(*m_VB, vbLayout);

    m_IB = std::make_unique<IndexBuffer>(&(m_Obj->indices[0]), indexCount);

    m_Obj->material->shader->setUniform3f("uKa", 1.0f, 1.0f, 1.0f);
    m_Obj->material->shader->setUniform3f("uKd", 0.796f, 0.52f, 0.482f);
    m_Obj->material->shader->setUniform3f("uKs", 0.5f, 0.5f, 0.5f);
    m_Obj->material->shader->setUniform3f("uLightIntensity", 225.0f, 225.0f, 225.0f);

    glEnable(GL_DEPTH_TEST);
}

test::TestMeshObject::~TestMeshObject()
{
    delete m_Material;
}

void test::TestMeshObject::onUpdate(float deltaTime)
{
}

void test::TestMeshObject::onRender()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Obj->material->shader->bind();

    m_Obj->material->shader->setUniform3fv("uLightPos", 1, &m_LightPos.x);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
    m_Obj->material->shader->setUniformMat4f("uModel", model);
    m_View = glm::lookAtRH(m_CameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 vp = m_Projection * m_View;
    m_Obj->material->shader->setUniformMat4f("uViewProj", vp);

    m_Obj->material->shader->setUniform3fv("uCameraPos", 1, &m_CameraPos.x);

    m_VA->bind();
    m_IB->bind();
    GLCall(glDrawElements(GL_TRIANGLES, m_IB->GetCount(), GL_UNSIGNED_INT, nullptr))
}

void test::TestMeshObject::onImGuiRender()
{
    ImGui::SliderFloat3("Light Position", &m_LightPos.x, -50.0f, 50.0f);
    ImGui::SliderFloat3("Translation", &m_Translation.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Camera Position", &m_CameraPos.x, -5.0f, 5.0f);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
