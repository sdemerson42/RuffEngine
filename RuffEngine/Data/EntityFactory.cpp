#include "EntityFactory.h"
#include "../Util/Logger.h"
#include "../Components/Components.h"
#include "../Systems/SpawnSystem.h"

#include <algorithm>

namespace data
{
	using namespace components;

	bool EntityFactory::Initialize(const std::string& entityDbPath, 
		asIScriptEngine* scriptEngine, systems::SpawnSystem* spawnSystem)
	{
		if (!LoadBlueprintData(entityDbPath, m_blueprints))
		{
			util::Logger::Log("Warning: EntityFactory failed to load blueprint data.");
			return false;
		}

		m_scriptEngine = scriptEngine;
		m_spawnSystem = spawnSystem;

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
		bool isActive,
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

		// First tag will always be the blueprint name
		entity.AddTag(blueprintName);

		// Add components
		AddRenderComponents(*blueprintIter, entity);
		AddAnimationComponents(*blueprintIter, entity);
		AddScriptComponents(*blueprintIter, entity);
		AddPhysicsComponents(*blueprintIter, entity);
		AddParticleComponents(*blueprintIter, entity);

		// Set position
		entity.SetPosition(positionX, positionY);

		entity.SetIsActive(isActive);

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
				RenderComponent* renderComponent = entity.GetComponent<RenderComponent>();
				if (renderComponent == nullptr)
				{
					renderComponent = entity.AddComponent<RenderComponent>();
					
				}
				renderComponent->Initialize();
				renderComponent->SetIsActive(true);
				
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

			AnimationComponent* animationComponent = entity.GetComponent<AnimationComponent>();
			if (animationComponent == nullptr)
			{
				animationComponent = entity.AddComponent<AnimationComponent>();
			}
			
			animationComponent->Initialize();
			animationComponent->SetIsActive(true);

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
					std::stoi(queryResult.at("total_frames")[i]));
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

			// Connect script components
			int totalComponents = queryResult.at("script_id").size();
			for (int i = 0; i < totalComponents; ++i)
			{
				ScriptComponent* scriptComponent = entity.GetComponent<ScriptComponent>();
				if (scriptComponent == nullptr)
				{
					scriptComponent = entity.AddComponent<ScriptComponent>();
				}
				std::string scriptPrefix = queryResult.at("main_prefix")[i];
				scriptComponent->PrepareScriptContext(m_scriptEngine, scriptPrefix);
				scriptComponent->Initialize();
				scriptComponent->SetIsActive(true);
				scriptComponent->AddSpawnSystem(m_spawnSystem);
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

			// Connect physics components
			int totalComponents = queryResult.at("physics_id").size();
			for (int i = 0; i < totalComponents; ++i)
			{
				PhysicsComponent* physicsComponent = entity.GetComponent<PhysicsComponent>();
				if (physicsComponent == nullptr)
				{
					physicsComponent = entity.AddComponent<PhysicsComponent>();
				}
				physicsComponent->Initialize();
				physicsComponent->SetIsActive(true);
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

	void EntityFactory::AddParticleComponents(
		const data::Blueprint& blueprint,
		/*out*/ecs::Entity& entity)
	{
		for (const auto& queryResult : blueprint.componentData)
		{
			if (queryResult.find("particle_id") == std::end(queryResult))
			{
				continue;
			}

			// Connect particle components
			int totalComponents = queryResult.at("particle_id").size();
			for (int i = 0; i < totalComponents; ++i)
			{
				ParticleComponent* particleComponent = entity.GetComponent<ParticleComponent>();
				if (particleComponent == nullptr)
				{
					particleComponent = entity.AddComponent<ParticleComponent>();
				}
				particleComponent->Initialize();
				particleComponent->SetIsActive(true);
				const std::string& sEmitterShape = queryResult.at("emitter_shape")[i];
				const std::string& sEmitterVars = queryResult.at("emitter_vars")[i];
				auto vars = ProcessMultiValueField(sEmitterVars);

				if (sEmitterShape == "CONE")
				{
					particleComponent->SetEmitterShape(ParticleComponent::EmitterShape::CONE);
					particleComponent->AddEmitterVar("offsetX", std::stof(vars[0]));
					particleComponent->AddEmitterVar("offsetY", std::stof(vars[1]));
					particleComponent->AddEmitterVar("startX", std::stof(vars[2]));
					particleComponent->AddEmitterVar("startY", std::stof(vars[3]));
					particleComponent->AddEmitterVar("angleDegrees", std::stof(vars[4]));
				}

				particleComponent->SetTexturePath(queryResult.at("texture_path")[i]);
				particleComponent->SetRenderLayer(queryResult.at("render_layer")[i]);
				particleComponent->SetTextureBox(
					std::stof(queryResult.at("texture_center_x")[i]),
					std::stof(queryResult.at("texture_center_y")[i]),
					std::stof(queryResult.at("texture_half_x")[i]),
					std::stof(queryResult.at("texture_half_y")[i]));
				particleComponent->SetIsUniformScaling(
					std::stoi(queryResult.at("uniform_scaling")[i]));
				particleComponent->SetScaleRange(
					std::stof(queryResult.at("scale_min_x")[i]),
					std::stof(queryResult.at("scale_max_x")[i]),
					std::stof(queryResult.at("scale_min_y")[i]),
					std::stof(queryResult.at("scale_max_y")[i]));
				particleComponent->SetColorRange(
					sf::Color(
						std::stoi(queryResult.at("color_min_r")[i]),
						std::stoi(queryResult.at("color_min_g")[i]),
						std::stoi(queryResult.at("color_min_b")[i]),
						std::stoi(queryResult.at("color_min_a")[i])),
					sf::Color(
						std::stoi(queryResult.at("color_min_r")[i]),
						std::stoi(queryResult.at("color_max_g")[i]),
						std::stoi(queryResult.at("color_max_b")[i]),
						std::stoi(queryResult.at("color_max_a")[i])));
				particleComponent->SetLifeRange(
					std::stof(queryResult.at("life_min")[i]),
					std::stof(queryResult.at("life_max")[i]));
				particleComponent->SetSpeedRange(
					std::stof(queryResult.at("speed_min")[i]),
					std::stof(queryResult.at("speed_max")[i]));
				particleComponent->SetSpawnRate(
					std::stof(queryResult.at("spawn_rate")[i]));
				
				particleComponent->PostStateSetup();
			}
		}
	}
};