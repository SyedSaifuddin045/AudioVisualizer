#pragma once
#include <SDL2/SDL_mixer.h>
#include <string>
#include <iostream>

class Music {
public:
	Music();
	~Music();

	bool load(const std::string& filePath,int _channel);
	void play();
	void pause();
	void resume();
	void stop();
	void restart();
	void setVolume(int volume);
	void toggleLooping();
	static void MusicFinishedCallback();
	Mix_Music* getMusic() const;
	Music* getInstance()const { return s_instance; }
private:
	bool looping;
	Mix_Music* m_music;
	int channel;

	static Music* s_instance;
};