#pragma once
#include "ECS/Systems/System.h"
#include "Common/Log.h"
#include "Sandbox/Events/MoveEvent.h"
#include "Sandbox/Entities/GameObject.h"
#include "Sandbox/Components/PositionComponent.h"
#include "Sandbox/Components/MoveComponent.h"
#include "Events/IEventListener.h"
#include "ECS/EventHandler.h"
#include <vector>

class MoveSystem : public Hollow::System<MoveSystem>, Hollow::IEventListener
{
public:
	MoveSystem() 
	{
		RegisterEventCallback<MoveEvent, MoveSystem>(&MoveSystem::Kek);
	}

	void Kek(const MoveEvent* const e)
	{
		Hollow::Log::GetCoreLogger()->critical("Bounce");
	}

	virtual void Update(float_t dt, std::vector<GameObject*>& gameObjects)
	{
		for (auto object : gameObjects) {
			PositionComponent * posComponent = object->GetComponent<PositionComponent>();
			MoveComponent * moveComponent = object->GetComponent<MoveComponent>();
			if (posComponent == nullptr || moveComponent == nullptr) continue;
			if (moveComponent->move) {
				posComponent->position.x -= 0.1f;
			} else {
				posComponent->position.x += 0.1f;
			}

			if (posComponent->position.x > 10.0f) {
				EventHandler::Get()->Send<MoveEvent>();
				moveComponent->move = true;
			}
			else if (posComponent->position.x < -10.0f) {
				EventHandler::Get()->Send<MoveEvent>();
				moveComponent->move = false;
			}
		}
	}
};
