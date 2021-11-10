#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "SFML/Audio/SoundBuffer.hpp"
#include "SFML/Audio/Sound.hpp"
#include "SFML/Audio/Music.hpp"

#include <string>
#include <unordered_map>

namespace systems
{
	class SoundSystem : public ecs::ISystem
	{
	public:
		SoundSystem(const std::string& soundPath, const std::vector<std::string>& bufferNames);
		void Execute() override;
		void PlaySound(
			const std::string& name,
			int priority,
			float volume,
			float pitch,
			float positionX,
			float positionY,
			float positionZ,
			bool isLooping);
		void StopSound(const std::string& name);
		void PlayMusic(const std::string& name, float volume, bool isLooping);
		void StopMusic();
		float GetMusicVolume();
		void SetMusicVolume(float value);
	private:
		struct SoundData
		{
			sf::Sound sound;
			int priority{ 0 };
			std::string name;
		};

		std::string m_soundPath;
		std::unordered_map<std::string, sf::SoundBuffer> m_soundBuffers;
		std::vector<SoundData> m_soundData;
		sf::Music m_music;
	};
}
