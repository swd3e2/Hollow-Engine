#pragma once
#define OPENGL

#include "Platform.h"
#include "ECS/ComponentManager.h"
#include "ECS/EntityManager.h"
#include "Events/EventSystem.h"
#include "Common/Timer.h"
#include "Input/InputManager.h"
#include "Common/Console.h"
#include "ECS/SystemManager.h"
#include <thread>
#include "Layers/LayerStack.h"
#include "Graphics/Camera.h"
#include "Graphics/ForwardRenderSystem.h"
#include "Graphics/Camera.h"
#include "Graphics/WindowManager.h"
#include "Core/TaskManager.h"
#include "Graphics/RenderApiManager.h"
#include "Graphics/Renderer/OpenGL/OGLRenderApi.h"
#include "Common/Log.h"
#include "Graphics/SkyMap.h"
#include "AnimationSystem.h"
#include "Importer/FreeImgImporter.h"
#include "Graphics/GUISystem.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

class Application
{
public:
	HWND*							m_HWND;
	EntityManager					entityManager;
	ComponentManager				componentManager;
	Hollow::SystemManager           systemManager;
	Timer							m_Timer;
	EventSystem						eventSystem;
	LayerStack						m_LayerStack;
	Camera*							camera;
	InputManager					inputManager;
	ForwardRenderSystem*			renderPass;
	Hollow::AnimationSystem*		animationSystem;
	TaskManager						taskManager;
	RenderApiManager				renderApiManager;
	Window*							window;
	RenderApi*						m_Renderer;
	FreeImgImporter*				imgImporter;
	GUISystem*						gui;
	double							dt;
public:
	Application();
	~Application();

	void Run();
};