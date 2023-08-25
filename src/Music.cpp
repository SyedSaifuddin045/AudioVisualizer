#include <Music.h>

Music* Music::s_instance = nullptr;

Music::Music() :m_music(nullptr), looping(false) {
	if (!s_instance)
	{
		s_instance = this;
	}
	Mix_Init(MIX_INIT_MP3);
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		std::cout << "Audio Mixer Initialization Error : " << Mix_GetError() << std::endl;
	}
	channel = 0;
	volume = MIX_MAX_VOLUME;
}

Music::~Music() {
	Mix_FreeMusic(m_music);
	Mix_CloseAudio();
	s_instance = nullptr;
}

bool Music::load(const std::string& filePath, int _channel) {
	m_music = Mix_LoadMUS(filePath.c_str());
	return m_music != nullptr;
	channel = _channel;
}

void Music::play() {
	if (m_music)
	{
		Mix_PlayMusic(m_music, 1);
		Mix_HookMusicFinished(MusicFinishedCallback);
	}
}
void Music::toggleLooping()
{
	if (m_music)
	{
		looping = (looping == true) ? false : true;
		Mix_HookMusicFinished(nullptr);

		if (looping == true)
		{
			std::cout << "Music Looping is True" << std::endl;
			if (!Mix_Playing(channel))
			{
				play();
			}
		}
	}
}

void Music::MusicFinishedCallback()
{
	if (s_instance->looping)
	{
		s_instance->play();
	}
}

void Music::pause() {
	if (m_music)
	{
		Mix_PauseMusic();
	}
}



Mix_Music* Music::getMusic() const
{
	return nullptr;
}

void Music::resume() {
	if (Mix_PausedMusic()) {
		Mix_ResumeMusic();
	}
}

void Music::stop() {
	Mix_HaltMusic();
}

void Music::restart()
{
	if (m_music)
	{
		stop();
		play();
	}
}

void Music::setVolume(int _volume) {
	_volume = std::max(0, std::min(128, _volume));

	Mix_VolumeMusic(_volume);
	volume = _volume;
	std::cout << "Volume : " << volume << std::endl;
}