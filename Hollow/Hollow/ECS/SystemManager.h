#pragma once

#ifndef HW_SYSTEM_MANAGER_H
#define HW_SYSTEM_MANAGER_H

#include "ISystem.h"
#include <list>
#include "Hollow/Platform.h"
#include "Hollow/Core/CModule.h"

namespace Hollow {
	class HOLLOW_API SystemManager : public CModule<SystemManager>
	{
	private:
		std::list<ISystem*> m_Systems;
	public:
		void startUp()
		{
			setStartedUp();
		}

		void shutdown()
		{
			setShutdown();
		}
		template<class T>
		void AddSystem(T* system)
		{
			this->m_Systems.push_back((ISystem*)system);
		}

		template<class S>
		void RemoveSystem(S* system)
		{
			auto it = this->m_Systems.find(system);
			if (it != this->m_Systems.end())
			{
				this->m_Systems.remove(system);
			}
		}

		void PreUpdateSystems(float dt)
		{
			for (auto& it : m_Systems)
				it->PreUpdate(dt);
		}

		void PostUpdateSystems(float dt)
		{
			for (auto& it : m_Systems)
				it->PostUpdate(dt);
		}

		void UpdateSystems(float dt)
		{
			for (auto& it : m_Systems)
				it->Update(dt);
		}
	};
}

#endif