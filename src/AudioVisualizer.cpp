#include <AudioVisualizer.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include<kiss_fft.h>
#include <vector>
#include <cmath>
#include <mutex>

std::vector<float> fftData;
float fft_max = 0.0f;

void noEffect(int chan, void* stream, int len, void* udata)
{
	kiss_fft_cfg cfg = kiss_fft_alloc(len, 0, nullptr, nullptr);
	kiss_fft_cpx* fftIn = new kiss_fft_cpx[len];
	kiss_fft_cpx* fftOut = new kiss_fft_cpx[len];

	// Copy audio data to the input buffer
	for (int i = 0; i < len; i++) {
		fftIn[i].r = reinterpret_cast<Uint8*>(stream)[i];
		fftIn[i].i = 0;
	}

	// Perform FFT
	kiss_fft(cfg, fftIn, fftOut);

	// Analyze FFT results and apply custom effect logic
	fftData.clear();
	for (int i = 1; i < len; i++) {
		float magnitude = sqrt(fftOut[i].r * fftOut[i].r + fftOut[i].i * fftOut[i].i);
		if (magnitude > fft_max) { fft_max = magnitude; }
		fftData.push_back(magnitude);
	}

	// Clean up
	kiss_fft_free(cfg);
	delete[] fftIn;
	delete[] fftOut;
}

int main(int argc, char* argv[])
{
	// Initialize SDL with video
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	Mix_Init(MIX_INIT_MP3);

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		std::cerr << "SDL Mixer initialization failed: " << Mix_GetError() << std::endl;
		return 1;
	}
	Mix_Chunk* musicChunk = Mix_LoadWAV("Memory Reboot.mp3");

	Mix_PlayChannel(0, musicChunk, -1);

	if (!Mix_RegisterEffect(0, noEffect, NULL, NULL)) {
		printf("Mix_RegisterEffect: %s\n", Mix_GetError());
	}

	// Create an SDL window
	SDL_Window* window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);

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
	while (running)
	{
		// poll for events from SDL
		while (SDL_PollEvent(&event))
		{
			// determine if the user still wants to have the window open
			// (this basically checks if the user has pressed 'X')
			running = event.type != SDL_QUIT;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 200, 120, 55, SDL_ALPHA_OPAQUE);
		int window_height, window_width;
		SDL_GetWindowSize(window, &window_width, &window_height);
		//SDL_RenderDrawLine(renderer, 0, window_height, window_width, window_height);

		int rectWidth = fftData.size() / window_width;
		//std::cout << rectWidth << std::endl;
		for (int i = 0; i < fftData.size(); i+=rectWidth)
		{
			int rectHeight = static_cast<int>(fftData[i]);
			std::cout<<"rect height : " << (rectHeight / fft_max) * window_height<< std::endl;

			int newHeight = (rectHeight / fft_max) * window_height;
			SDL_Rect rect = { i * rectWidth, window_height- newHeight, rectWidth, newHeight};
			SDL_RenderFillRect(renderer, &rect);
		}

		fft_max = 0;

		SDL_RenderPresent(renderer);
	}

	Mix_HookMusic(nullptr, nullptr);

	//Destroy the renderer
	SDL_DestroyRenderer(renderer);
	// Destroy the window
	SDL_DestroyWindow(window);

	// And quit SDL
	SDL_Quit();

	return 0;
}