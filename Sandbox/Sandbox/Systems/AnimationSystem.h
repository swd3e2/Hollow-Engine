#pragma once

#include "Hollow/Common/Log.h"
#include <Hollow/ECS/System.h>
#include <Hollow/ECS/EntityManager.h>
#include "Sandbox/Components/AnimationComponent.h"
#include "Sandbox/Entities/GameObject.h"
#include <tuple>
#include <utility>

using namespace Hollow;

class AnimationSystem : public Hollow::System<AnimationSystem>
{
public:
	virtual void PreUpdate(double dt) override {}

	virtual void Update(double dt) override
	{
		for (auto& entity : EntityManager::instance()->container<GameObject>()) {
			if (entity.hasComponent<AnimationComponent>()) {
				AnimationComponent* animationComponent = entity.getComponent<AnimationComponent>();

				animationComponent->currentAnimationTime += dt / 1000.0;
				if (animationComponent->animations.size() > 0) {
					if (animationComponent->currentAnimationTime > 5.0) {
						animationComponent->currentAnimationTime = 0.0;
					}

					animate(
						animationComponent->currentAnimationTime, 
						animationComponent->rootNode, 
						Matrix4::identity(), 
						animationComponent->animations[animationComponent->currentAnimation], 
						animationComponent->nodeInfo
					);
				}
			}
		}
	}

	virtual void PostUpdate(double dt) override {}

	void animate(double time, Node* node, const Matrix4& parentTransform, Animation* animation, Hollow::Matrix4* container)
	{
		if (animation->data.find(node->id) != animation->data.end()) {
			AnimationNodeData* data = animation->data[node->id];

			std::pair<double, Hollow::Vector3> closestScale;
			std::pair<double, Hollow::Vector3> closestTranslation;
			std::pair<double, Hollow::Quaternion> closestRotation;

			std::tie(closestScale, closestTranslation, closestRotation) = getClosest(time, data);

			std::pair<double, Hollow::Vector3> nextClosestScale;
			std::pair<double, Hollow::Vector3> nextClosestTranslation;
			std::pair<double, Hollow::Quaternion> nextClosestRotation;

			std::tie(nextClosestScale, nextClosestTranslation, nextClosestRotation) = getNextClosest(time, data);

			Hollow::Vector3 scaling = interpolateScaling(closestScale, nextClosestScale, time);
			Hollow::Vector3 translation = interpolatePosition(closestTranslation, nextClosestTranslation, time);
			Hollow::Quaternion rotation = interpolateRotation(closestRotation, nextClosestRotation, time);

			Hollow::Matrix4 nodeTransformation = Matrix4::transpose(Matrix4::translation(translation));
			Hollow::Matrix4 globalTransformation = parentTransform * nodeTransformation;

			//container[node->id] = component->globalInverse * GlobalTransformation * bone->localTransform;
			container[node->id] = Matrix4::identity();

			for (auto& it : node->childs) {
				animate(time, it, globalTransformation, animation, container);
			}
		} else {
			container[node->id] = Matrix4::identity();

			for (auto& it : node->childs) {
				animate(time, it, node->localTransform, animation, container);
			}
		}
	}


	std::tuple<std::pair<double, Hollow::Vector3>, std::pair<double, Hollow::Vector3>, std::pair<double, Hollow::Quaternion>> getClosest(double time, AnimationNodeData* data)
	{
		std::pair<double, Hollow::Vector3> scale;
		std::pair<double, Hollow::Vector3> translation;
		std::pair<double, Hollow::Quaternion> rotation;

		for (auto& it : data->scale) {
			if (it.first < time) {
				scale = std::make_pair(it.first, it.second);
			}
		}

		for (auto& it : data->positions) {
			if (it.first < time) {
				translation = std::make_pair(it.first, it.second);
			}
		}

		for (auto& it : data->rotations) {
			if (it.first < time) {
				rotation = std::make_pair(it.first, it.second);
			}
		}

		return std::make_tuple(scale, translation, rotation);
	}

	std::tuple<std::pair<double, Hollow::Vector3>, std::pair<double, Hollow::Vector3>, std::pair<double, Hollow::Quaternion>> getNextClosest(double time, AnimationNodeData* data)
	{
		std::pair<double, Hollow::Vector3> scale;
		std::pair<double, Hollow::Vector3> translation;
		std::pair<double, Hollow::Quaternion> rotation;

		for (auto& it : data->scale) {
			if (it.first >= time) {
				scale = std::make_pair(it.first, it.second);
				break;
			}
		}

		for (auto& it : data->positions) {
			if (it.first >= time) {
				translation = std::make_pair(it.first, it.second);
				break;
			}
		}

		for (auto& it : data->rotations) {
			if (it.first >= time) {
				rotation = std::make_pair(it.first, it.second);
				break;
			}
		}

		return std::make_tuple(scale, translation, rotation);
	}

	Hollow::Vector3 interpolatePosition(std::pair<double, Hollow::Vector3>& prev, std::pair<double, Hollow::Vector3>& next, double time)
	{
		if (next.first == 0.0 && prev.first == 0.0) {
			return Hollow::Vector3(0.0f, 0.0f, 0.0f);
		}

		float deltaTime = (float)(next.first - prev.first);
		float factor = (time - (float)prev.first) / deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		Hollow::Vector3 start = prev.second;
		Hollow::Vector3 end = next.second;
		Hollow::Vector3 delta = end - start;
		Hollow::Vector3 out = start + (delta * factor);

		return out;
	}

	Hollow::Vector3 interpolateScaling(std::pair<double, Hollow::Vector3>& prev, std::pair<double, Hollow::Vector3>& next, double time)
	{
		if (next.first == 0.0 && prev.first == 0.0) {
			return Hollow::Vector3(1.0f, 1.0f, 1.0f);
		}

		float deltaTime = (float)(next.first - prev.first);
		float factor = (time - (float)prev.first) / deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		Hollow::Vector3 start = prev.second;
		Hollow::Vector3 end = next.second;
		Hollow::Vector3 delta = end - start;
		Hollow::Vector3 out = start + (delta * factor);

		return out;
	}

	Hollow::Quaternion interpolateRotation(std::pair<double, Hollow::Quaternion>& prev, std::pair<double, Hollow::Quaternion>& next, double time)
	{
		if (next.first == 0.0 && prev.first == 0.0) {
			return Hollow::Quaternion();
		}

		float deltaTime = (float)(next.first - prev.first);
		float factor = (time - (float)prev.first) / deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		Hollow::Quaternion out;
		Hollow::Quaternion::Interpolate(out, prev.second, next.second, factor);
		out = out.Normalize();

		return out;
	}
};
