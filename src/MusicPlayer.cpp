#include <MusicPlayer.h>

MusicPlayer* MusicPlayer::s_instance = nullptr;

MusicPlayer::MusicPlayer() :m_music(nullptr), looping(false) {
	if (!s_instance)
	{
		s_instance = this;
	}
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		std::cout << "Audio Mixer Initialization Error : " << Mix_GetError() << std::endl;
	}
	channel = 0;
}

MusicPlayer::~MusicPlayer() {
	Mix_CloseAudio();
	s_instance = nullptr;
}

bool MusicPlayer::loadMusic(const std::string& filePath,int _channel) {
	m_music = Mix_LoadMUS(filePath.c_str());
	return m_music != nullptr;
	channel = _channel;
}

void MusicPlayer::play() {
	if (m_music)
	{
		Mix_PlayMusic(m_music,1);
		Mix_HookMusicFinished(MusicFinishedCallback);
	}
}
void MusicPlayer::toggleLooping()
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

void MusicPlayer::MusicFinishedCallback()
{
	if (s_instance->looping)
	{
		s_instance->play();
	}
}

void MusicPlayer::pause() {
	if (m_music)
	{
		Mix_PauseMusic();
	}
}



Mix_Music* MusicPlayer::getMusic() const
{
	return nullptr;
}

void MusicPlayer::resume() {
	if (Mix_PausedMusic()) {
		Mix_ResumeMusic();
	}
}

void MusicPlayer::stop() {
	Mix_HaltMusic();
}

void MusicPlayer::restart()
{
	if (m_music)
	{
		stop();
		play();
	}
}

void MusicPlayer::setVolume(int volume) {
	Mix_VolumeMusic(volume);
}