#include "Application.h"

Application::Application()
{
	Hollow::Console::RedirectIOToConsole();
	Hollow::Log::Init();

	meshImporter = new MeshImporter();
	imgImporter = new FreeImgImporter();

	camera = new Camera(true);
	camera->SetProjectionValues(90.0f, static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 10000.0f);

	eventSystem.startUp();
	inputManager.startUp();
	componentManager.startUp();
	entityManager.startUp();
	systemManager.startUp();

	m_Renderer = renderApiManager.initialize(SCREEN_WIDTH, SCREEN_HEIGHT);
	window = WindowManager::instance()->Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);

	m_Renderer->startUp();
#ifdef OPENGL
	oglRenderSystem = new OGLRenderSystem(static_cast<OGLRenderApi*>(m_Renderer), camera);
	systemManager.AddSystem(oglRenderSystem);
#endif
#ifdef D3D11
	renderPass = new ForwardRenderSystem(static_cast<D3D11RenderApi*>(m_Renderer));
	renderPass->m_Camera = camera;
	systemManager.AddSystem(renderPass);

	renderPass->skyMap = new SkyMap(10, 10);
	renderPass->water = new Water();
#endif

	animationSystem = new Hollow::AnimationSystem();
	systemManager.AddSystem(animationSystem);
}

Application::~Application()
{
	systemManager.shutdown();
	entityManager.shutdown();
	componentManager.shutdown();
	inputManager.shutdown();
	eventSystem.shutdown();
}

void Application::Run()
{
	m_Timer.Start();

	while (!window->isClosed())
	{
		dt = m_Timer.GetMilisecondsElapsed();
		m_Timer.Restart();
		window->ProcessMessage();

#ifdef D3D11
#endif

		camera->Update(dt);

		m_LayerStack.PreUpdate(dt);
		systemManager.PreUpdateSystems(dt);

		systemManager.UpdateSystems(dt);
		m_LayerStack.Update(dt);

		m_LayerStack.PostUpdate(dt);
		systemManager.PostUpdateSystems(dt);

		eventSystem.dispatch();
		inputManager.Clear();

		m_Timer.Stop();
	}
}

