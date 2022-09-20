#include "TestTexture2D.h"
#include "../Utils.h"
#include "../ImGui/imgui.h"

test::TestTexture2D::TestTexture2D()
    : m_Translation(0.0f, 0.0f, 0.0f),
      m_View(glm::mat4(1.0f)),
      m_Projection(glm::perspectiveFov(glm::pi<float>() / 2.0f, 400.0f, 300.0f, 200.0f, 300.0f))
{
    float vertices[] = {
        -50.0f, -50.0f, -210.0f, 0.0f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // xyz, rgba, texCoord, materialID
         50.0f, -50.0f, -210.0f, 0.0f,  0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
         50.0f,  50.0f, -210.0f, 0.0f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -50.0f,  50.0f, -210.0f, 0.0f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

        100.0f, -50.0f, -210.0f, 1.0f, 0.93f, 0.24f, 1.0f, 0.0f, 0.0f, 0.0f,
        200.0f, -50.0f, -210.0f, 1.0f, 0.93f, 0.24f, 1.0f, 1.0f, 0.0f, 0.0f,
        200.0f,  50.0f, -210.0f, 1.0f, 0.93f, 0.24f, 1.0f, 1.0f, 1.0f, 0.0f,
        100.0f,  50.0f, -250.0f, 1.0f, 0.93f, 0.24f, 1.0f, 0.0f, 1.0f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6,
        6, 7, 4
    };

    /*glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

    m_VA = std::make_unique<VertexArray>();
    m_VB = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
    VertexBufferLayout vbLayout;
    vbLayout.push<float>(3); // pos
    vbLayout.push<float>(4); // color
    vbLayout.push<float>(2); // texCoord
    vbLayout.push<float>(1); // materialID
    m_VA->addBuffer(*m_VB, vbLayout);

    m_IB = std::make_unique<IndexBuffer>(indices, 12);

    m_Shader = std::make_unique<Shader>("assets/shaders/BasicVertexShader.glsl", "assets/shaders/BasicFragmentShader.glsl");

    m_Texture = std::make_unique<Texture>("assets/textures/PokeRose.png");
    m_Shader->setUniform1i("uTexture", 0);
}

test::TestTexture2D::~TestTexture2D()
{
}

void test::TestTexture2D::onUpdate(float deltaTime)
{
}

void test::TestTexture2D::onRender()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    m_Texture->bind();
    m_Shader->bind();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
    glm::mat4 mvp = m_Projection * m_View * model;
    m_Shader->setUniformMat4f("uMVP", mvp);

    m_VA->bind();
    m_IB->bind();
    GLCall(glDrawElements(GL_TRIANGLES, m_IB->GetCount(), GL_UNSIGNED_INT, nullptr))
}

void test::TestTexture2D::onImGuiRender()
{
    ImGui::SliderFloat2("Translation", &m_Translation.x, -100.0f, 100.0f);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
