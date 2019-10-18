#pragma once
#include <Hollow/Core.h>
#include "Sandbox/Components/TransformComponent.h"
#include "Sandbox/Entities/GameObject.h"
#include "Sandbox/Systems/RenderSystem.h"
#include "Sandbox/GUI/GUISystem.h"
#include "Sandbox/Systems/MoveSystem.h"
#include "Sandbox/Components/MoveComponent.h"
#include "Sandbox/Components/SelectComponent.h"
#include "Sandbox/Components/RenderableComponent.h"
#include "Sandbox/ProjectSettings.h"
#include "Hollow/ECS/EntityManager.h"
#include "Hollow/ECS/ComponentManager.h"
#include "Sandbox/Components/TransformComponent.h"
#include "Sandbox/Entities/Terrain.h"
#include "Sandbox/Components/TerrainData.h"
#include "Hollow/Graphics/InputLayout.h"
#include "Sandbox/Entities/Light.h"
#include "Sandbox/Components/LightComponent.h"
#include "Sandbox/Systems/PhysicsSystem.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

// App entrypoint
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hInst2, LPWSTR pArgs, INT)
{
	Hollow::Core core;

	Hollow::Window* window = WindowManager::create(Hollow::RendererType::OpenGL, SCREEN_WIDTH, SCREEN_HEIGHT, Hollow::WindowType::Bordered);
	Hollow::RenderApi* renderer = RenderApiManager::create(Hollow::RendererType::OpenGL, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	ProjectSettings::startUp<ProjectSettings>(Hollow::RendererType::OpenGL);

	Hollow::Camera* camera = new Hollow::Camera(true);
	camera->setProjectionValues(80.0f, static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100000.0f);

	RenderSystem renderPass(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	renderPass.skyMap = new SkyMap();
	renderPass.m_Camera = camera;

	MoveSystem moveSystem(camera);

	SystemManager::instance()->addSystem(&renderPass);
	SystemManager::instance()->addSystem(&moveSystem);

	GUISystem* gui = new GUISystem(window, renderer);
	gui->rendererTab.renderSystem = &renderPass;

	PhysicsSystem::startUp();

	//ProjectSettings::instance()->load("C:\\dev\\Hollow Engine\\Project1\\Project1.json");
	DelayedTaskManager::instance()->update();

	Light* light = Hollow::EntityManager::instance()->create<Light>();
	light->addComponent<LightComponent>();

	SystemManager::instance()->addSystem(PhysicsSystem::instance());

	{
		GameObject* entity = Hollow::EntityManager::instance()->create<GameObject>();
		Hollow::s_ptr<Hollow::Import::Model> mesh = Hollow::MeshManager::instance()->import("C:/dev/Hollow Engine/Sandbox/Sandbox/Resources/Meshes/DefenderLingerie00.fbx");
		RenderableComponent* renderable = entity->addComponent<RenderableComponent>();
		renderable->load(mesh);
		entity->addComponent<TransformComponent>();
	}
	{
		GameObject* entity = Hollow::EntityManager::instance()->create<GameObject>();
		Hollow::s_ptr<Hollow::Import::Model> mesh = Hollow::MeshManager::instance()->import("C:/dev/Hollow Engine/Sandbox/Sandbox/Resources/Meshes/AnimatedCube.gltf");
		RenderableComponent* renderable = entity->addComponent<RenderableComponent>();
		renderable->load(mesh);
		entity->addComponent<TransformComponent>();
	}

	/*{
		GameObject* obj = EntityManager::instance()->create<GameObject>();
		obj->addComponent<TransformComponent>();
		RenderableComponent* objRenderable = obj->addComponent<RenderableComponent>();
		PhysicsComponent* objPhysics = obj->addComponent<PhysicsComponent>();
		
		Hollow::Import::Model* model = MeshManager::instance()->import("C:\\dev\\DirectXApp\\DirectXApp\\Data\\Models\\plane.obj");
		objRenderable->load(model);
		objPhysics->load(model);
		delete model;
	}*/

	while (!window->isClosed()) {
		core.preUpdate();

		window->processMessage();
		camera->update(core.dt);
		core.update();

		gui->update(core.dt);

		core.postUpdate();
	}
	
	return 0;
}