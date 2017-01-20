example : example.c ../../oglconsole-sdl.o ../../oglconsole.h
	cc -framework OpenGL -I../../ -DOGLCONSOLE_USE_SDL `sdl-config --cflags --libs` -o $@ example.c ../../oglconsole-sdl.o

../../oglconsole-sdl.o : ../../oglconsole.c ../../oglconsole.h ../../ConsoleFont.c
	make -C ../../

clean ::
	-rm -f example.o example 2>/dev/null
