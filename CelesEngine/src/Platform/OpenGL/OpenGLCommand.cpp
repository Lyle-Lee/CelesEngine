#include "PCH.h"
#include "OpenGLCommand.h"
#include <GL/glew.h>

namespace Celes {

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         CE_CORE_FATAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       CE_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          CE_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: CE_CORE_TRACE(message); return;
		default:							 CE_CORE_ASSERT(false, "Unknown severity level!"); return;
		}
	}

	void OpenGLCommand::Init()
	{
#ifdef CE_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		if (GLEW_VERSION_4_3)
		{
			glDebugMessageCallback(OpenGLMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		}
		else if (GLEW_ARB_debug_output)
		{
			glDebugMessageCallbackARB(OpenGLMessageCallback, nullptr);
			glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		}
#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLCommand::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLCommand::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLCommand::SetViewport(int x, int y, int width, int height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLCommand::SetLineWidth(float width)
	{
		glLineWidth(width);
	}

	void OpenGLCommand::DrawIndexed(const Ref<VertexArray>& va, uint32_t indexCnt)
	{
		va->Bind();
		uint32_t count = indexCnt ? indexCnt : va->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLCommand::DrawLines(const Ref<VertexArray>& va, uint32_t vertexCnt)
	{
		va->Bind();
		glDrawArrays(GL_LINES, 0, vertexCnt);
	}

}
