#include <AudioVisualizer.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include<kiss_fft.h>
#include <vector>
#include <complex>
#include <cmath>
#include <mutex>
#include <random>

#define ARRAY_lEN(xs) sizeof(xs)/sizeof(xs[0])

std::mutex fft_mutex;
float fft_max = 0.0f;
#define samples 128
kiss_fft_cpx* fftOut;
int times_updated = 0, times_displayed = 0;

float amp(std::complex<float> z)
{
	float a = (std::abs(std::real(z)));
	float b = (std::abs(std::imag(z)));
	//std::cout << "A = " << a << " , B = " << b << std::endl;
	return a > b ? a : b;
}

void noEffect(int chan, void* stream, int len, void* udata)
{
	if (times_updated > times_displayed)
		return;
	if (len < samples)
		return;

	std::lock_guard<std::mutex> lock(fft_mutex);
	kiss_fft_cfg cfg = kiss_fft_alloc(len, 0, nullptr, nullptr);
	kiss_fft_cpx* fftIn = new kiss_fft_cpx[len];
	fftOut = new kiss_fft_cpx[len];

	/*for (int i = 0; i < samples; ++i) {
		float avgSum = 0.0f;

		for (int j = 0; j < average_count; ++j) {
			int index = i * average_count + j;
			float value = reinterpret_cast<Uint8*>(stream)[index];
			avgSum += value;
		}

		float average = avgSum / static_cast<float>(average_count);
		fftIn[i].r = average;
		fftIn[i].i = 0.0f;
	}*/

	for (int i = 0; i < len; i++) {
		fftIn[i].r = reinterpret_cast<Uint8*>(stream)[i];
		fftIn[i].i = 0.0f;
	}

	// Perform FFT
	kiss_fft(cfg, fftIn, fftOut);

	for (int i = 1; i < len; ++i)
	{
		std::complex<float> z = std::complex(fftOut[i].r, fftOut[i].i);
		int c = amp(z);
		if (c > fft_max)
			fft_max = c;
	}
	times_updated++;
	if (fft_max < -1)
	{
		fft_max = -1;
	}
	// Clean up
	kiss_fft_free(cfg);
	delete[] fftIn;
}


int main(int argc, char* argv[])
{
	// Initialize SDL with video
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 2048) == -1) {
		std::cerr << "SDL Mixer initialization failed: " << Mix_GetError() << std::endl;
		return 1;
	}
	Mix_Chunk* musicChunk = Mix_LoadWAV("Water Resistant.mp3");

	Mix_PlayChannel(0, musicChunk, -1);

	if (!Mix_RegisterEffect(0, noEffect, NULL, NULL)) {
		printf("Mix_RegisterEffect: %s\n", Mix_GetError());
	}

	// Create an SDL window
	SDL_Window* window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	// if failed to create a window
	if (!window)
	{
		// we'll print an error message and exit
		std::cerr << "Error failed to create window!\n";
		return 1;
	}
	SDL_Renderer* renderer = nullptr;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Event event;	 // used to store any events from the OS
	bool running = true; // used to determine if we're running the game

	glClearColor(0, 0, 0, 1);

	SDL_Color baseColor = { 0,220,90 };
	std::random_device rd;
	std::mt19937 gen(rd());

	// Define the range for the random number
	std::uniform_int_distribution<> distribution(-254, 254);
	while (running)
	{
		// poll for events from SDL
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_DROPFILE:
				char* droppedFile = event.drop.file;
				std::string file = droppedFile;
				std::cout << "File : " << droppedFile << std::endl;
				if (file.find(".mp3") != std::string::npos || file.find(".ogg") != std::string::npos || file.find(".wav") != std::string::npos)
				{
					Mix_FreeChunk(musicChunk);
					std::cout << "Loading your audio file" << std::endl;
					musicChunk = Mix_LoadWAV(file.c_str());
					Mix_FadeOutChannel(0, 2000);
					Mix_UnregisterEffect(0, noEffect);
					Mix_PlayChannel(0, musicChunk, -1);
					if (!Mix_RegisterEffect(0, noEffect, NULL, NULL)) {
						printf("Mix_RegisterEffect: %s\n", Mix_GetError());
					}
				}
				else
				{
					std::cout << "Sorry Unsupported file format" << std::endl;
				}
				break;
			}

		}

		if (fft_max >= 0)
		{
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(renderer);

			int window_height, window_width;
			SDL_GetWindowSize(window, &window_width, &window_height);
			//SDL_RenderDrawLine(renderer, 0, window_height, window_width, window_height);

			int rectWidth = window_width / samples;
			//std::cout << rectWidth << std::endl;
			std::lock_guard<std::mutex> lock(fft_mutex);
			for (int i = 1; i < samples; i++)
			{
				int randomColor = distribution(gen);
				std::complex<float> z = std::complex<float>(fftOut[i].r, fftOut[i].i);
				double rectHeight = amp(z);
				if (fft_max > 0)
					rectHeight /= std::abs(fft_max);

				int newHeight = (window_height * 0.8f) * (rectHeight);
				//std::cout << "fft_max : " << fft_max << "rect height : " << newHeight << std::endl;
				SDL_SetRenderDrawColor(renderer, baseColor.r + i, baseColor.g, baseColor.b + i, SDL_ALPHA_OPAQUE);
				SDL_Rect rect = { (i-1) * rectWidth, window_height - newHeight, rectWidth, newHeight };
				SDL_RenderFillRect(renderer, &rect);
			}
			times_displayed++;
			SDL_RenderPresent(renderer);
			fft_max = -1;
			delete[] fftOut;
		}

	}

	Mix_HookMusic(nullptr, nullptr);

	//Destroy the renderer
	SDL_DestroyRenderer(renderer);
	// Destroy the window
	SDL_DestroyWindow(window);

	// And quit SDL
	SDL_Quit();

	std::cout << "Updated : " << times_updated << "Displayed : " << times_displayed << std::endl;

	return 0;
}