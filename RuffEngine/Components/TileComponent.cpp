#include "pch.h"
#include "TileComponent.h"
#include "../Util/Logger.h"
#include "../ECSPrimitives/Entity.h"
#include "../Util/Time.h"

namespace components
{
	std::unordered_map<int, TileComponent::TileSet> TileComponent::s_tileSets;

	TileComponent::TileComponent(ecs::Entity* parent, const std::string& sceneLayer) :
		ecs::ComponentBase{ parent }, Autolist<TileComponent>{ sceneLayer }
	{
	}

	void TileComponent::SetDbPathName(const std::string& pathName)
	{
		m_dbPathName = pathName;
	}

	void TileComponent::PostInitialize(const std::vector<int>& tileMapIds)
	{
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
		BuildRenderComponents(tiles, rowSize, renderLayer, colors);

		std::string animations = data.at("tile_animations")[0];
		if (animations != "")
		{
			BuildAnimations(tiles, animations, rowSize);
		}

		std::string physicsLayers = data.at("physics_layers")[0];
		if (physicsLayers != "")
		{
			BuildPhysics(tiles, physicsLayers, rowSize);
		}
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

		util::Logger::Log("Info: Successfully loaded TileSet with id "
			+ std::to_string(id) + ".");

		return id;
	}

	void TileComponent::BuildRenderComponents(const std::string& tiles, int rowSize,
		const std::string& renderLayer, const std::string& colors)
	{
		ecs::Entity* entity = GetParent();
		auto tileVec = data::Parse::ProcessMultiValueField(tiles);

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

			const TileSet& tileSet = s_tileSets[m_tileSetIndex];
			const sf::Vector2f& tileHalfSize = tileSet.tileHalfSize;
			auto rc = entity->AddComponent<components::RenderComponent>();
			m_renderRefs.push_back(rc);
			rc->SetTexturePath(tileSet.texturePathName);
			
			ecs::Box2f drawBox;
			drawBox.center = {
				(float)i * 2.0f * tileHalfSize.x + tileHalfSize.x,
				(float)j * 2.0f * tileHalfSize.y + tileHalfSize.y };
			drawBox.halfSize = tileHalfSize;
			rc->SetDrawBox(drawBox);

			++i;
			if (i == rowSize)
			{
				i = 0;
				++j;
			}

			ecs::Box2f texBox;
			texBox.center = {
				float(tile % rowSize * 2) * tileHalfSize.x + tileHalfSize.x,
				float(tile / rowSize * 2) * tileHalfSize.y + tileHalfSize.y };
			texBox.halfSize = drawBox.halfSize;
			rc->SetTextureBox(texBox);

			rc->SetRenderLayer(renderLayer);

			rc->SetColor(sf::Color::White);                                                    
			if (colors != "")
			{
				auto colorVals = data::Parse::ProcessMultiValueField(colors);
				for (int k = 0; k < colorVals.size(); k += 5)
				{
					int val = std::stoi(colorVals[k]);
					if (val != tile) continue;
					rc->SetColor(
						sf::Uint8(std::stoi(colorVals[k + 1])),
						sf::Uint8(std::stoi(colorVals[k + 2])),
						sf::Uint8(std::stoi(colorVals[k + 3])),
						sf::Uint8(std::stoi(colorVals[k + 4])));
					break;
				}
			}
			rc->SetIsActive(true);
		}
	}

	void TileComponent::BuildAnimations(const std::string& tiles, const std::string& animations,
		int rowSize)
	{
		auto tileStr = data::Parse::ProcessMultiValueField(tiles);
		auto animationStr = data::Parse::ProcessMultiValueField(animations);
		const TileSet& tileSet = s_tileSets[m_tileSetIndex];
		
		for (int i = 0; i < animationStr.size(); i += 3)
		{
			int tileToAnimate = std::stoi(animationStr[i]);
			int frameTotal = std::stoi(animationStr[i + 1]);
			float seconds = std::stof(animationStr[i + 2]);
			const sf::Vector2f& tileHalfSize = tileSet.tileHalfSize;

			m_tileAnimations.push_back(TileAnimation());
			auto& animation = m_tileAnimations.back();
			animation.frameCounter = 0;
			animation.frameTotal = frameTotal;
			animation.secPerFrame = seconds;
			animation.secCounter = 0.0f;
			animation.tileId = tileToAnimate;

			animation.startFrame = {
				{ float(tileToAnimate % rowSize * 2) * tileHalfSize.x + tileHalfSize.x,
				float(tileToAnimate / rowSize * 2) * tileHalfSize.y + tileHalfSize.y },
				tileHalfSize };

			int rIndex = 0;
			for (int j = 0; j < tileStr.size(); ++j)
			{
				int tile = std::stoi(tileStr[j]);

				if (tile == -1)
				{
					continue;
				}

				if (tile != tileToAnimate)
				{
					++rIndex;
					continue;
				}

				m_animationRenderRefs[tileToAnimate].push_back(
					m_renderRefs[rIndex]);

				++rIndex;
			}
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

					PhysicsComponent* pc = parent->AddComponent<PhysicsComponent>();
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
}