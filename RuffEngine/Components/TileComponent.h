#pragma once

#include "../ECSPrimitives/Autolist.h"
#include "../ECSPrimitives/ComponentBase.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "SFML/System/Vector2.hpp"
#include "../Data/Parse.h"
#include "../Data/SqlQuery.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace components
{
	class TileComponent : public ecs::ComponentBase, public ecs::Autolist<TileComponent>
	{
	public:
		struct TileSet
		{
			int id;
			std::string texturePathName;
			sf::Vector2f tileHalfSize;
			int rowSize;
			int columnSize;
		};

		struct TileAnimation
		{
			int tileId;
			ecs::Box2f startFrame;
			int frameTotal;
			int frameCounter;
			float secPerFrame;
			float secCounter;
		};
		
		TileComponent(ecs::Entity* parent, const std::string& sceneLayer);
		~TileComponent()
		{
			m_tileAnimations.clear();
			m_animationRenderRefs.clear();
		}
		void SetDbPathName(const std::string& pathName);
		void PostInitialize(const std::vector<int>& tileMapIds);
		void Update();
	private:
		void BuildTileMap(const data::SqlQueryResult& data);
		int VerifyTileSet(int index);
		void BuildRenderComponents(const std::string& tiles, int rowSize,
			const std::string& renderLayer, const std::string& colors);
		void BuildAnimations(const std::string& tiles, const std::string& animations,
			int rowSize);
		void BuildPhysics(const std::string& tiles, const std::string& physicsLayers, int rowSize);

		std::string m_dbPathName;
		int m_tileSetIndex;
		std::vector<TileAnimation> m_tileAnimations;
		std::vector<components::RenderComponent*> m_renderRefs;
		std::unordered_map<int, std::vector<components::RenderComponent*>> m_animationRenderRefs;

		static std::unordered_map<int, TileSet> s_tileSets;
	};
};
