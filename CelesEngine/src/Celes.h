#pragma once

// For client side
#include "Celes/Core/Application.h"
#include "Celes/Core/Log.h"
#include "Celes/Core/Timestep.h"

#include "Celes/Core/Layer.h"
#include "Celes/GUI/GUILayer.h"

#include "Celes/Core/Input.h"
#include "Celes/Core/KeyCodes.h"
#include "Celes/Core/MouseButtonCodes.h"
#include "Celes/Renderer/CameraController.h"

#include "Celes/Renderer/Renderer.h"
#include "Celes/Renderer/Renderer2D.h"
#include "Celes/Renderer/Buffer.h"
#include "Celes/Renderer/VertexArray.h"
#include "Celes/Renderer/Shader.h"
#include "Celes/Renderer/Texture.h"
#include "Celes/Renderer/Camera.h"
#include "Celes/Renderer/Light.h"

#include "Celes/Scene/Scene.h"
#include "Celes/Scene/Entity.h"
#include "Celes/Scene/EntityComponent.h"
#include "Celes/Scene/SceneSerializer.h"

#include "Celes/Utils/PlatformUtils.h"
#include "Celes/Utils/Timer.h"

#include "Celes/Math/Math.h"
