glfacade
========

[Bigyama]: http://www.bigyama.net/
[Quell Memento]: http://youtu.be/RnemX6xn0Ss
[Fallen Tree Games]: http://www.fallentreegames.com/

Sketching out the OpenGL fixed function pipeline operations using shaders. For porting [Quell Memento] to PlayStation Vita.

[Bigyama] ported [Quell Memento] to PlayStation Vita for [Fallen Tree Games], this is some of the code we used to assist us. Quell Memento was released May/June 2013 (depending on your region) and has been well recieved!

Before we got the PlayStation Vita development kits, we started reading the docs and comparing it to the Quell engine. Quell was using a lot of OpenGL fixed function operations not available on the PlayStation Vita. Therefore we implemented these operations using a matrix stack and some shaders which would be easy to push to the Vita when the dev kits arrives.

Libraries
---------

I've not included the library directory. I used a existing project template to start up (also available in this repo) which is why Lua is one of the expected libraries.

Cg comes from somewhere on Nvidia's site. It's not particularly friendly to get.
./lib/cg.lib
./lib/cgGL.lib

./lib/glee
./lib/lua
./lib/soil
./lib/sdl

Make File
---------

I didn't bother making a make file for such a small project. I built it using a bat file, under mingw using g++.

