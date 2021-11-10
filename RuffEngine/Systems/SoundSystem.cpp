#include "pch.h"
#include "SoundSystem.h"
#include "../Util/Logger.h"

#include <algorithm>

namespace systems
{
	SoundSystem::SoundSystem(const std::string& soundPath, 
		const std::vector<std::string>& bufferNames) :
		m_soundPath{ soundPath }
	{
		for (const auto& name : bufferNames)
		{
			const std::string fName = m_soundPath + name;
			m_soundBuffers[name] =
				sf::SoundBuffer{};
			if (!m_soundBuffers[name].loadFromFile(fName))
			{
				util::Logger::Log("Warning: Unable to load sound buffer with path " +
					fName + ".");
			}
		}
		m_soundData.resize(8);
	}

	void SoundSystem::Execute()
	{
		
	}

	void SoundSystem::PlaySound(
		const std::string& name,
		int priority,
		float volume,
		float pitch,
		float positionX,
		float positionY,
		float positionZ,
		bool isLooping)
	{
		if (m_soundBuffers.find(name) == m_soundBuffers.end())
		{
			util::Logger::Log("Warning: Couldn't play sound " + name + ". SoundBuffer not found.");
			return;
		}
		sf::SoundBuffer& buffer = m_soundBuffers[name];

		int priorityDelta = -1;
		SoundData* soundToReplace = nullptr;
		for (auto& soundData : m_soundData)
		{
			if (soundData.sound.getStatus() == sf::Sound::Status::Stopped)
			{
				soundToReplace = &soundData;
				break;
			}

			int localDelta = priority - soundData.priority;
			if (localDelta > priorityDelta)
			{
				priorityDelta = localDelta;
				soundToReplace = &soundData;
			}
		}

		if (soundToReplace == nullptr) return;

		soundToReplace->sound.setBuffer(buffer);
		soundToReplace->priority = priority;
		soundToReplace->sound.setVolume(volume);
		soundToReplace->sound.setPitch(pitch);
		soundToReplace->sound.setPosition(positionX, positionY, positionZ);
		soundToReplace->sound.setLoop(isLooping);
		soundToReplace->name = name;

		soundToReplace->sound.play();
	}

	void SoundSystem::StopSound(const std::string& name)
	{
		auto iter = std::find_if(m_soundData.begin(), m_soundData.end(),
			[&](SoundData& data)
			{
				return data.name == name &&
					data.sound.getStatus() == sf::Sound::Status::Playing;
			});
		if (iter == m_soundData.end())
		{
			return;
		}
		iter->sound.stop();
	}

	void SoundSystem::PlayMusic(const std::string& name, float volume, bool isLooping)
	{
		m_music.stop();
		if (!m_music.openFromFile(m_soundPath + name))
		{
			util::Logger::Log("Warning: Music file " + name + " could not be loaded.");
		}
		m_music.setVolume(volume);
		m_music.setLoop(isLooping);
		m_music.play();
	}
	
	void SoundSystem::StopMusic()
	{
		m_music.stop();
	}

	float SoundSystem::GetMusicVolume()
	{
		return m_music.getVolume();
	}

	void SoundSystem::SetMusicVolume(float value)
	{
		m_music.setVolume(value);
	}
}