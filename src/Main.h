#ifndef MAIN_H
#define MAIN_H

struct SDL_Surface;
union SDL_Event;

class Main
{
private:
	SDL_Surface* mSurface;
	double mDeltaTime;
	bool mRunning;
	void OnUpdate();
	void DoOpenGLTests();
public:
	int Execute();
	void OnEvent(SDL_Event* event);
	bool Reset();
	Main() : mSurface(0), mDeltaTime(0), mRunning(true) {}
};

#endif