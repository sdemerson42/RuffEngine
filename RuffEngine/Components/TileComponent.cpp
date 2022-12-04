#include "pch.h"
#include "TileComponent.h"
#include "../Util/Logger.h"
#include "../ECSPrimitives/Entity.h"
#include "../Util/Time.h"
#include "../Util/Events.h"
#include "../Components/ScriptComponent.h"

#include <unordered_set>

namespace components
{
	std::unordered_map<int, TileComponent::TileSet> TileComponent::s_tileSets;
	std::unordered_map<int, data::SqlQueryResult> TileComponent::s_dynamicTileMaps;

	TileComponent::TileComponent(ecs::Entity* parent, const std::string& sceneLayer) :
		ecs::ComponentBase{ parent }, Autolist<TileComponent>{ sceneLayer }
	{
		m_renderTextureData.reserve(10);
	}

	void TileComponent::SetDbPathName(const std::string& pathName)
	{
		m_dbPathName = pathName;
	}

	void TileComponent::PostInitialize(const std::vector<int>& tileMapIds, const std::vector<int>& dynamicTileMapIds)
	{
		m_renderTextureData.clear();

		bool isDynamic = false;
		if (!dynamicTileMapIds.empty())
		{
			for (int id : dynamicTileMapIds)
			{
				auto resultIter = s_dynamicTileMaps.find(id);
				if (resultIter != std::end(s_dynamicTileMaps))
				{
					BuildTileMap(resultIter->second);
					isDynamic = true;
				}
				else
				{
					util::Logger::Log("Warning: Tile Component failed to access dynamic tilemap data.");
				}
			}
		}

		if (isDynamic) return;

		data::SqlQueryResult result;
		for (int id : tileMapIds)
		{
			if (!data::SqlQuery::SubmitQuery(
				m_dbPathName, "SELECT * FROM Tile_Map_Data WHERE tile_map_id = " +
				std::to_string(id),result))
			{
				util::Logger::Log("Warning: Tile Component failed to access TileMap data.");
			}
			BuildTileMap(result);
		}
	}

	void TileComponent::Update()
	{
		const TileSet& tileSet = s_tileSets.at(m_tileSetIndex);
		for (auto& animation : m_tileAnimations)
		{
			animation.secCounter += util::Time::DeltaTime();
			if (animation.secCounter < animation.secPerFrame) continue;
			
			animation.secCounter = 0.0f;
			animation.frameCounter = (animation.frameCounter + 1) % animation.frameTotal;
			auto& rcRefs = m_animationRenderRefs[animation.tileId];
			for (RenderComponent* rc : rcRefs)
			{
				if (animation.frameCounter == 0)
				{
					rc->SetTextureBox(animation.startFrame);
				}
				else
				{
					auto box = rc->GetTextureBox();
					box.center.x += tileSet.tileHalfSize.x * 2.0f;
					rc->SetTextureBox(box);
				}
			}
		}
	}

	void TileComponent::BuildTileMap(const data::SqlQueryResult& data)
	{
		int tileSetId = std::stoi(data.at("using_tile_set_id")[0]);
		m_tileSetIndex = VerifyTileSet(tileSetId);
		if (m_tileSetIndex == -1) return;
		
		int rowSize = std::stoi(data.at("row_size")[0]);
		std::string renderLayer = data.at("render_layer")[0];
		std::string colors = data.at("tile_colors")[0];
		std::string tiles = data.at("tiles")[0];
		std::string animations = data.at("tile_animations")[0];
		BuildImageAndAnimations(tiles, animations, rowSize, renderLayer, colors);

		std::string physicsLayers = data.at("physics_layers")[0];
		if (physicsLayers != "")
		{
			BuildPhysics(tiles, physicsLayers, rowSize);
		}

		const auto& tileSet = s_tileSets[tileSetId];
		if (tileSet.physicsCellSize.x != 0 && tileSet.physicsCellSize.y != 0)
		{
			util::SetPhysicsCellSizeEvent event;
			event.x = tileSet.physicsCellSize.x;
			event.y = tileSet.physicsCellSize.y;
			SendEvent(&event);
		}

		StoreTileMapData(tiles, rowSize, tileSet.tileHalfSize);
		m_topRenderLayer = renderLayer;
	}

	int TileComponent::VerifyTileSet(int id)
	{
		auto tsIter = s_tileSets.find(id);
		if (tsIter != s_tileSets.end()) return id;

		data::SqlQueryResult result;
		if (!data::SqlQuery::SubmitQuery(
			m_dbPathName, "SELECT * FROM Tile_Set_Data WHERE tile_set_id = "
			+ std::to_string(id), result))
		{
			util::Logger::Log("Warning: Failed load TileSet with id " +
				std::to_string(id) + ".");
			return -1;
		}

		s_tileSets[id] = TileSet{};
		auto& tileSet = s_tileSets[id];
		tileSet.id = id;
		tileSet.texturePathName = result.at("texture_path_name")[0];
		tileSet.tileHalfSize = {
			std::stof(result.at("tile_half_x")[0]),
			std::stof(result.at("tile_half_y")[0]) };
		tileSet.rowSize = std::stoi(result.at("row_size")[0]);
		tileSet.columnSize = std::stoi(result.at("column_size")[0]);

		auto cellXstring = result.at("physics_static_cell_x")[0];
		auto cellYstring = result.at("physics_static_cell_y")[0];
		if (cellXstring != "" && cellYstring != "")
		{
			tileSet.physicsCellSize = { std::stoi(cellXstring), std::stoi(cellYstring) };
		}

		util::Logger::Log("Info: Successfully loaded TileSet with id "
			+ std::to_string(id) + ".");

		return id;
	}

	void TileComponent::BuildImageAndAnimations(const std::string& tiles, const std::string& animations, 
		int rowSize, const std::string& renderLayer, const std::string& colors)
	{
		const TileSet& tileSet = s_tileSets[m_tileSetIndex];
		sf::Texture texture;
		if (!texture.loadFromFile(tileSet.texturePathName))
		{
			util::Logger::Log("Warning: TileComponent failed to load texture " +
				tileSet.texturePathName + ".");
			return;
		}

		ecs::Entity* entity = GetParent();
		auto tileVec = data::Parse::ProcessMultiValueField(tiles);
		auto animationsVec = data::Parse::ProcessMultiValueField(animations);
		std::unordered_set<int> animatedTiles;
		const sf::Vector2f& tileHalfSize = tileSet.tileHalfSize;

		m_renderTextureData.push_back(RenderTextureData{});
		m_renderTextureData.back().renderTexture =
			std::make_unique<sf::RenderTexture>();
		m_renderTextureData.back().renderTexture->create(
			rowSize * 2 * (int)tileHalfSize.x,
			(tileVec.size() / rowSize) * 2 * (int)tileHalfSize.y);
		sf::VertexArray vertexArray{ sf::PrimitiveType::Quads };

		for (int i = 0; i < animationsVec.size(); i += 3)
		{
			animatedTiles.insert(std::stoi(animationsVec[i]));
		}

		int i = 0, j = 0;

		for (const std::string& tileStr : tileVec)
		{
			int tile = std::stoi(tileStr);

			// We use -1 for empty space
			if (tile == -1)
			{
				++i;
				if (i == rowSize)
				{
					i = 0;
					++j;
				}
				continue;
			}

			// Do common rendering calculations

			ecs::Box2f drawBox;
			drawBox.center = {
				(float)i * 2.0f * tileHalfSize.x + tileHalfSize.x,
				(float)j * 2.0f * tileHalfSize.y + tileHalfSize.y };
			drawBox.halfSize = tileHalfSize;

			ecs::Box2f texBox;
			texBox.center = {
				float(tile % tileSet.rowSize * 2) * tileHalfSize.x + tileHalfSize.x,
				float(tile / tileSet.rowSize * 2) * tileHalfSize.y + tileHalfSize.y };
			texBox.halfSize = drawBox.halfSize;

			sf::Color drawColor{ sf::Color::White };
			if (colors != "")
			{
				auto colorVals = data::Parse::ProcessMultiValueField(colors);
				for (int k = 0; k < colorVals.size(); k += 5)
				{
					int val = std::stoi(colorVals[k]);
					if (val != tile) continue;
					drawColor = {
						sf::Uint8(std::stoi(colorVals[k + 1])),
						sf::Uint8(std::stoi(colorVals[k + 2])),
						sf::Uint8(std::stoi(colorVals[k + 3])),
						sf::Uint8(std::stoi(colorVals[k + 4])) };
					break;
				}
			}
			
			if (animatedTiles.find(tile) == animatedTiles.end())
			{
				// Draw to render target
				vertexArray.append(sf::Vertex{
					sf::Vector2f{ drawBox.GetLeft(), drawBox.GetTop() },
					drawColor,
					sf::Vector2f{ texBox.GetLeft(), texBox.GetTop()} });
				vertexArray.append(sf::Vertex{
					sf::Vector2f{ drawBox.GetRight(), drawBox.GetTop() },
					drawColor,
					sf::Vector2f{ texBox.GetRight(), texBox.GetTop()} });
				vertexArray.append(sf::Vertex{
					sf::Vector2f{ drawBox.GetRight(), drawBox.GetBottom() },
					drawColor,
					sf::Vector2f{ texBox.GetRight(), texBox.GetBottom()} });
				vertexArray.append(sf::Vertex{
					sf::Vector2f{ drawBox.GetLeft(), drawBox.GetBottom() },
					drawColor,
					sf::Vector2f{ texBox.GetLeft(), texBox.GetBottom()} });

				++i;
				if (i == rowSize)
				{
					i = 0;
					++j;
				}
				continue;
			}
			else
			{
				// Create render component and animation
				ecs::Entity* parent = GetParent();
				auto rc = parent->AddComponent<components::RenderComponent>();

				rc->SetColor(drawColor);
				rc->SetDrawBox(drawBox);
				rc->SetRenderLayer(renderLayer);
				rc->SetTextureBox(texBox);
				rc->SetTexturePath(tileSet.texturePathName);
				rc->SetIsActive(true);

				m_animationRenderRefs[tile].push_back(rc);
				auto animationIter = std::find_if(
					m_tileAnimations.begin(), m_tileAnimations.end(),
					[&](const TileAnimation& animation)
					{
						return animation.tileId == tile;
					});
				if (animationIter == m_tileAnimations.end())
				{
					m_tileAnimations.push_back(TileAnimation{});
					auto& animation = m_tileAnimations.back();

					for (int i = 0; i < animationsVec.size(); i += 3);
					{
						int animTile = std::stoi(animationsVec[i]);
						if (tile == animTile)
						{
							animation.tileId = animTile;
							animation.frameCounter = 0;
							animation.frameTotal = std::stoi(animationsVec[i + 1]);
							animation.secCounter = 0.0f;
							animation.secPerFrame = std::stof(animationsVec[i + 2]);
							animation.startFrame = texBox;
						}
					}
				}
			}

			++i;
			if (i == rowSize)
			{
				i = 0;
				++j;
			}
		}

		if (vertexArray.getVertexCount() > 0)
		{
			sf::RenderStates states;
			states.texture = &texture;
			m_renderTextureData.back().renderTexture->draw(vertexArray, states);
			m_renderTextureData.back().renderTexture->display();
			m_renderTextureData.back().renderLayer = renderLayer;
		}
	}

	void TileComponent::BuildPhysics(const std::string& tilesStr, const std::string& physicsLayersStr, 
		int rowSize)
	{
		auto tilesTemp = data::Parse::ProcessMultiValueField(tilesStr);
		std::vector<int> tiles;
		for (const std::string& str : tilesTemp)
		{
			tiles.push_back(std::stoi(str));
		}

		int columnSize = tiles.size() / rowSize;
		auto physicsLayers = data::Parse::ProcessMultiValueField(physicsLayersStr);
		const auto& tileSet = s_tileSets[m_tileSetIndex];
		const auto& tileHalfSize = tileSet.tileHalfSize;
		std::vector<bool> pMap;
		pMap.resize(tiles.size());

		ecs::Entity* parent = GetParent();

		int logCount = 0;

		for (int j = 0; j < columnSize; ++j)
		{
			for (int i = 0; i < rowSize; ++i)
			{
				int index = j * rowSize + i;
				if (tiles[index] != -1 && !pMap[index])
				{
					// Trace a horizontal-first rectangle
					int areaWidth = 0;
					for (int m = i; m < rowSize; ++m)
					{
						int localIndex = j * rowSize + m;
						if (tiles[localIndex] != -1 && !pMap[localIndex])
						{
							++areaWidth;
							pMap[localIndex] = true;
						}
						else break;
					}

					int areaHeight = 1;
					for (int n = j + 1; n < columnSize; ++n)
					{
						bool pass = true;
						for (int m = i; m < i + areaWidth; ++m)
						{
							int localIndex = n * rowSize + m;
							if (tiles[localIndex] == -1 || pMap[localIndex])
							{
								pass = false;
								break;
							}
						}
						if (!pass) break;
						for (int m = i; m < i + areaWidth; ++m)
						{
							int localIndex = n * rowSize + m;
							pMap[localIndex] = true;
						}
						++areaHeight;
					}
					// Area found...
					++logCount;

					PhysicsComponent* pc = parent->AddComponent<PhysicsComponent>(true);
					pc->SetDrag(0.0f);
					pc->SetIsActive(true);
					pc->SetIsIgnoreGravity(true);
					pc->SetIsSolid(true);
					pc->SetMass(0.0f);
					pc->SetRestution(0.0f);
					pc->SetVelocity({ 0.0f, 0.0f });
					
					float boxHalfWidth = float(areaWidth) * tileHalfSize.x;
					float boxHalfHeight = float(areaHeight) * tileHalfSize.y;
					float boxCenterX = (float(i * 2) * tileHalfSize.x + boxHalfWidth);
					float boxCenterY = (float(j * 2) * tileHalfSize.y + boxHalfHeight);

					pc->SetAABB(boxCenterX, boxCenterY, boxHalfWidth, boxHalfHeight);
					

					for (const std::string& layer : physicsLayers)
					{
						pc->AddActiveCollisionLayer(layer);
					}
				}
			}
		}
		util::Logger::Log("Info: TileMap physics layer created " +
			std::to_string(logCount) + " physics components.");
	}

	void TileComponent::StoreTileMapData(const std::string& tiles, int rowSize, const sf::Vector2f& tileHalfSize)
	{
		auto tileVec = data::Parse::ProcessMultiValueField(tiles);
		
		components::ScriptComponent::s_tileMaps.push_back(std::vector<int>());
		auto& cache = components::ScriptComponent::s_tileMaps[components::ScriptComponent::s_tileMaps.size() - 1];

		for (const auto& s : tileVec)
		{
			cache.push_back(std::stoi(s));
		}

		components::ScriptComponent::s_tileMapRowSize = rowSize;
		components::ScriptComponent::s_tileHalfSize = tileHalfSize;
	}

	void TileComponent::AddDynamicTileMap(int id, const data::SqlQueryResult& tileMap)
	{
		s_dynamicTileMaps[id] = tileMap;
	}
}