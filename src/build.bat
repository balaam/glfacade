g++ -Wall main.cpp vitamath.cpp vitaglfacade.cpp ../lib/glee/glee.c ../lib/lua/src/liblua.a ../lib/soil/lib/libSOIL.a -I../lib/soil/src -I../lib/glee -I../lib/lua/src -Dmain=SDL_main -L/usr/local/lib -o OpenGLFake -Wl,-Bstatic -L/mingw/lib -lmingw32 -lSDLmain -lSDL -liconv -lm -luser32 -lgdi32 -lwinmm -lopengl32 -lcg -lcgGL -static-libgcc -static-libstdc++ -lws2_32 -lwinmm

