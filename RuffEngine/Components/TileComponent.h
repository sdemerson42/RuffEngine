#pragma once

#include "../ECSPrimitives/Autolist.h"
#include "../ECSPrimitives/ComponentBase.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "SFML/System/Vector2.hpp"
#include "../Data/Parse.h"
#include "../Data/SqlQuery.h"
#include "../Util/EventHandler.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace components
{
	class TileComponent : public ecs::ComponentBase, public ecs::Autolist<TileComponent>, public util::EventHandler
	{
	public:
		struct TileSet
		{
			int id;
			std::string texturePathName;
			sf::Vector2f tileHalfSize;
			int rowSize;
			int columnSize;
			sf::Vector2i physicsCellSize;
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

		struct RenderTextureData
		{
			std::unique_ptr<sf::RenderTexture> renderTexture;
			std::string renderLayer;
		};
		
		TileComponent(ecs::Entity* parent, const std::string& sceneLayer);
		~TileComponent()
		{
			m_tileAnimations.clear();
			m_animationRenderRefs.clear();
		}
		void SetDbPathName(const std::string& pathName);
		void PostInitialize(const std::vector<int>& tileMapIds, const std::vector<int>& dynamicTileMapIds);
		void Update();
		const std::vector<RenderTextureData>& GetRenderTextures()
		{
			return m_renderTextureData;
		}
		
		static void AddDynamicTileMap(int id, const data::SqlQueryResult& tileMap);
	private:
		void BuildTileMap(const data::SqlQueryResult& data);
		int VerifyTileSet(int index);
		void BuildImageAndAnimations(const std::string& tiles, const std::string& animations, int rowSize,
			const std::string& renderLayer, const std::string& colors);
		void BuildPhysics(const std::string& tiles, const std::string& physicsLayers, int rowSize);

		std::string m_dbPathName;
		int m_tileSetIndex;
		std::vector<TileAnimation> m_tileAnimations;
		std::unordered_map<int, std::vector<components::RenderComponent*>> m_animationRenderRefs;
		std::vector<RenderTextureData> m_renderTextureData;

		static std::unordered_map<int, TileSet> s_tileSets;
		static std::unordered_map<int, data::SqlQueryResult> s_dynamicTileMaps;
	};
};
