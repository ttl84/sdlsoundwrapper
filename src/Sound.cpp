#include "Sound.h"
#include "SDL2/SDL.h"
#include <vector>
#include <atomic>
#include <iostream>

Sound::Sound(std::string path, SDL_AudioDeviceID d)
:	buf(nullptr, [](Uint8* ptr){SDL_FreeWAV(ptr);}),
	dev(d)
{
	Uint8* tmpbuf;
	good = SDL_LoadWAV(path.c_str(), &spec, &tmpbuf, &len) != nullptr;
	buf.reset(tmpbuf);
	
}
Sound::~Sound()
{
}
Playback Sound::play()
{
	return Playback{buf.get(), len, 0};
}
bool Sound::getSpec(std::string path, SDL_AudioSpec * spec)
{
	unsigned len;
	Uint8 * buf;
	bool good = SDL_LoadWAV(path.c_str(), spec, &buf, &len);
	if(good)
		SDL_FreeWAV(buf);
	return good;
}
void SoundSystem::init()
{
	Sound::getSpec("sound/checkspec.wav", &spec);
	spec.userdata = this;
	spec.callback = SoundSystem::callback;
	spec.samples = 512;
	dev = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr,0);
	std::cout << "samples: " << spec.samples << '\n';
	std::cout << "freq: " << spec.freq << '\n';
	
	if(dev != 0)
		SDL_PauseAudioDevice(dev, 0);
}
void SoundSystem::cleanup()
{
	if(dev != 0)
	{
		SDL_PauseAudioDevice(dev, 0);
		SDL_CloseAudioDevice(dev);
	}
}
void SoundSystem::callback(void *userdata, Uint8 *stream, int len)
{
	auto sys = (SoundSystem *) userdata;
	SDL_memset(stream, 0, len);
	sys->lock();
	for(Playback & p : sys->playbacks)
	{
		if(p.buf == nullptr)
			continue;
		unsigned left = p.pos + len > p.len ?  p.len - p.pos : len;
		SDL_MixAudioFormat(stream, p.buf + p.pos, sys->spec.format, left, SDL_MIX_MAXVOLUME);
		
		p.pos += left;
		if(p.pos == p.len)
			p = Playback{nullptr, 0, 0};
	}
	sys->unlock();
}