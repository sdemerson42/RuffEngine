#include "EntityFactory.h"
#include "../Util/Logger.h"
#include "../Components/ComponentBank.h"
#include "../Components/Components.h"

#include <algorithm>

namespace data
{
	using namespace components;

	bool EntityFactory::Initialize(const std::string& entityDbPath, asIScriptEngine* scriptEngine)
	{
		if (!LoadBlueprintData(entityDbPath, m_blueprints))
		{
			util::Logger::Log("Warning: EntityFactory failed to load blueprint data.");
			return false;
		}

		m_scriptEngine = scriptEngine;

		return true;
	}

	std::vector<std::string> EntityFactory::ProcessMultiValueField(const std::string& values) const
	{
		std::vector<std::string> valueVector;
		std::string s;

		for (char c : values)
		{
			if (c == ',')
			{
				valueVector.push_back(s);
				s.clear();
				continue;
			}
			s += c;
		}
		valueVector.push_back(s);

		return valueVector;
	}

	bool EntityFactory::BuildEntityFromBlueprint(
		const std::string& blueprintName,
		float positionX,
		float positionY,
		/*out*/ecs::Entity& entity)
	{
		auto blueprintIter = std::find_if(
			std::begin(m_blueprints), 
			std::end(m_blueprints),
			[&](const Blueprint& blueprint)
			{
				return blueprint.name == blueprintName;
			});

		if (blueprintIter == std::end(m_blueprints))
		{
			util::Logger::Log("Warning: Failed to build blueprint - No blueprint with name " + blueprintName);
			return false;
		}

		// Add components
		AddRenderComponents(*blueprintIter, entity);
		AddAnimationComponents(*blueprintIter, entity);
		AddScriptComponents(*blueprintIter, entity);
		AddPhysicsComponents(*blueprintIter, entity);

		// Set position
		entity.SetPosition(positionX, positionY);

		return true;
	}

	void EntityFactory::AddRenderComponents(
		const data::Blueprint& blueprint,
		/*out*/ecs::Entity& entity)
	{
		for (const auto& queryResult : blueprint.componentData)
		{
			if (queryResult.find("render_id") == std::end(queryResult))
			{
				continue;
			}

			// Connect render components
			int totalComponents = queryResult.at("render_id").size();
			for (int i = 0; i < totalComponents; ++i)
			{
				if (!CONNECT_COMP(&entity, RenderComponent))
				{
					util::Logger::Log("Warning: Failed to connect RenderComponent to entity.");
					continue;
				}

				RenderComponent* renderComponent = entity.GetComponents<RenderComponent>().back();
				renderComponent->SetTexturePath(queryResult.at("texture_path")[i]);
				renderComponent->SetRenderLayer(queryResult.at("render_layer")[i]);
				renderComponent->SetTextureBox(
					std::stof(queryResult.at("texture_center_x")[i]),
					std::stof(queryResult.at("texture_center_y")[i]),
					std::stof(queryResult.at("texture_half_x")[i]),
					std::stof(queryResult.at("texture_half_y")[i]));
				renderComponent->SetDrawBox(
					std::stof(queryResult.at("draw_center_x")[i]),
					std::stof(queryResult.at("draw_center_y")[i]),
					std::stof(queryResult.at("draw_half_x")[i]),
					std::stof(queryResult.at("draw_half_y")[i]));
				renderComponent->SetColor(
					std::stoi(queryResult.at("color_r")[i]),
					std::stoi(queryResult.at("color_g")[i]),
					std::stoi(queryResult.at("color_b")[i]),
					std::stoi(queryResult.at("color_a")[i]));
			}
		}
	}

	void EntityFactory::AddAnimationComponents(
		const data::Blueprint& blueprint,
		/*out*/ecs::Entity& entity)
	{
		bool hasAnimationComponent = false;

		for (const auto& queryResult : blueprint.componentData)
		{
			if (queryResult.find("animation_id") == std::end(queryResult))
			{
				continue;
			}

			// Connect a single animation component
			if (!hasAnimationComponent)
			{
				if (!CONNECT_COMP(&entity, AnimationComponent))
				{
					util::Logger::Log("Warning: Failed to connect AnimationComponent to entity.");
					return;
				}
				hasAnimationComponent = true;
			}

			AnimationComponent* animationComponent = entity.GetComponent<AnimationComponent>();

			int totalAnimations = queryResult.at("animation_id").size();
			for (int i = 0; i < totalAnimations; ++i)
			{
				animationComponent->AddAnimation(
					queryResult.at("name")[i],
					ecs::Box2f
					{
						std::stof(queryResult.at("start_center_x")[i]),
						std::stof(queryResult.at("start_center_y")[i]),
						std::stof(queryResult.at("half_x")[i]),
						std::stof(queryResult.at("half_y")[i])
					},
					std::stoi(queryResult.at("frames_per_row")[i]),
					std::stoi(queryResult.at("total_frames")[i]),
					std::stoi(queryResult.at("render_index")[i]));
			}
		}
	}

	void EntityFactory::AddScriptComponents(
		const data::Blueprint& blueprint,
		/*out*/ecs::Entity& entity)
	{
		for (const auto& queryResult : blueprint.componentData)
		{
			if (queryResult.find("script_id") == std::end(queryResult))
			{
				continue;
			}

			// Connect render components
			int totalComponents = queryResult.at("script_id").size();
			for (int i = 0; i < totalComponents; ++i)
			{
				if (!CONNECT_COMP(&entity, ScriptComponent))
				{
					util::Logger::Log("Warning: Failed to connect ScriptComponent to entity.");
					continue;
				}

				ScriptComponent* scriptComponent = entity.GetComponents<ScriptComponent>().back();
				std::string scriptPrefix = queryResult.at("main_prefix")[i];
				scriptComponent->PrepareScriptContext(m_scriptEngine, scriptPrefix);
			}
		}
	}

	void EntityFactory::AddPhysicsComponents(
		const data::Blueprint& blueprint,
		/*out*/ecs::Entity& entity)
	{
		for (const auto& queryResult : blueprint.componentData)
		{
			if (queryResult.find("physics_id") == std::end(queryResult))
			{
				continue;
			}

			// Connect render components
			int totalComponents = queryResult.at("physics_id").size();
			for (int i = 0; i < totalComponents; ++i)
			{
				if (!CONNECT_COMP(&entity, PhysicsComponent))
				{
					util::Logger::Log("Warning: Failed to connect RenderComponent to entity.");
					continue;
				}

				PhysicsComponent* physicsComponent = entity.GetComponents<PhysicsComponent>().back();
				//queryResult.at("texture_path")[i]
				physicsComponent->SetVelocity(
					std::stof(queryResult.at("velocity_x")[i]),
					std::stof(queryResult.at("velocity_y")[i]));
				physicsComponent->SetMass(std::stof(queryResult.at("mass")[i]));
				physicsComponent->SetRestution(std::stof(queryResult.at("restitution")[i]));
				physicsComponent->SetDrag(std::stof(queryResult.at("drag")[i]));
				physicsComponent->SetAABB(
					std::stof(queryResult.at("aabb_center_x")[i]),
					std::stof(queryResult.at("aabb_center_y")[i]),
					std::stof(queryResult.at("aabb_half_x")[i]),
					std::stof(queryResult.at("aabb_half_y")[i]));
				physicsComponent->SetIsSolid(
					std::stoi(queryResult.at("solid")[i]));
				physicsComponent->SetIsIgnoreGravity(
					std::stoi(queryResult.at("ignore_gravity")[i]));

				auto collisionLayers =
					ProcessMultiValueField(queryResult.at("collision_layers")[i]);
				for (const auto& layer : collisionLayers)
				{
					physicsComponent->AddActiveCollisionLayer(layer);
				}
			}
		}
	}
};