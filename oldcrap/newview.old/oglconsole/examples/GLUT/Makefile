example : example.c ../../oglconsole-glut.o
	cc -I../ -framework GLUT -framework OpenGL -framework Cocoa -lobjc  -o $@ example.c ../../oglconsole.o

../../oglconsole-glut.o : ../../oglconsole.c ../../oglconsole.h ../../ConsoleFont.c
	make -C ../../ oglconsole-glut.o

clean ::
	-rm -f example.o example 2> /dev/null
