#pragma once
#include "Engine.h"
#include "Window/Window.h"

#include "Entities/GameObject.h"
#include "Components/MeshComponent.h"
#include "Components/PositionComponent.h"
#include "Components/MoveComponent.h"
#include "Systems/RenderSystem/RenderSystem.h"
#include "Systems/InterfaceSystem/InterfaceSystem.h"
#include "Systems/MoveSystem.h"
#include <random>
#include "DirectXMath.h"
#include "d3d11.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

using namespace DirectX;

class Application
{
private:
	static constexpr float DELTA_TIME_STEP{ 1.0f / 60.0f };

	Window					m_Window;
	HWND*					m_HWND;
	Engine					engine;
	RenderSystem*           m_RenderSystem;
	MoveSystem*				m_MoveSystem;
	InterfaceSystem*		m_InterfaceSystem;
	std::vector<GameObject*> gameObjects;
private:
	void InitScene()
	{
		std::vector<SimpleVertex> vertices;
		vertices.push_back({ XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) });
		vertices.push_back({ XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) });
		vertices.push_back({ XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) });
		vertices.push_back({ XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) });

		vertices.push_back({ XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) });
		vertices.push_back({ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) });
		vertices.push_back({ XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) });
		vertices.push_back({ XMFLOAT4(1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) });

		std::vector<unsigned int> vindices;
		unsigned int * indices = new unsigned int[36]{
				2, 1, 0,
				2, 0, 3,
				2, 5, 1,
				5, 7, 1,
				4, 2, 3,
				4, 5, 2,
				4, 3, 0,
				4, 0, 6,
				1, 6, 0,
				1, 7, 6,
				5, 6, 7,
				5, 4, 6
		};
		for (int i = 0; i < 36; i++)
			vindices.push_back(indices[i]);

		std::default_random_engine generator;
		std::uniform_int_distribution<int> distribution(-10, 10);

		for (int i = 0; i < 10; i++) {
			GameObject * object = engine.m_EntityManager->CreateEntity<GameObject>();
			object->AddComponent<MeshComponent, ID3D11Device*, std::vector<SimpleVertex>*, std::vector<unsigned int>*>(this->m_RenderSystem->GetDevice(), &vertices, &vindices);
			object->AddComponent<PositionComponent, float, float, float, float>((float)distribution(generator), (float)distribution(generator), (float)distribution(generator), .0f);
			object->AddComponent<MoveComponent>();
			gameObjects.push_back(object);
		}
	}
public:
	Application(HINSTANCE hInst, LPWSTR pArgs) :
		m_Window(hInst, SCREEN_WIDTH, SCREEN_HEIGHT)
	{
		this->m_HWND = this->m_Window.getHWND();
		this->m_RenderSystem = new RenderSystem(this->m_HWND, SCREEN_WIDTH, SCREEN_HEIGHT);
		this->m_InterfaceSystem = new InterfaceSystem(this->m_HWND, this->m_RenderSystem->GetDevice(), this->m_RenderSystem->GetDeviceContext(), engine.m_EntityManager, engine.m_ComponentManager);
		this->m_MoveSystem = new MoveSystem();

		this->InitScene();
	}

	void Run() {
		while (m_Window.ProcessMessage())
		{
			engine.m_Timer->Tick(DELTA_TIME_STEP);
			this->m_RenderSystem->PreUpdate(DELTA_TIME_STEP);
			this->m_MoveSystem->Update(DELTA_TIME_STEP, gameObjects);
			this->m_RenderSystem->Update(DELTA_TIME_STEP, gameObjects);
			this->m_InterfaceSystem->Update(DELTA_TIME_STEP);
			this->m_RenderSystem->PostUpdate(DELTA_TIME_STEP);

			engine.m_EventHandler->DispatchEvents();
		}
	}

};