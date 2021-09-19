#include "EntityFactory.h"
#include "../Logging/Logger.h"
#include "../Components/ComponentBank.h"
#include "../Components/Components.h"

#include <algorithm>

namespace data
{
	using namespace components;

	bool EntityFactory::Initialize(const std::string& entityDbPath)
	{
		if (!LoadBlueprintData(entityDbPath, m_blueprints))
		{
			util::Logger::Log("Warning: EntityFactory failed to load blueprint data.");
			return false;
		}

		return true;
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
};