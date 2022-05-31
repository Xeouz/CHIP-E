mainsrc = main.cpp
mainobj = main.o
mainout = main.exe

sfmlsrc = D:/SFML/include
sfmllib = D:/SFML/lib
sfmllist= -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32

optimization = O0

run: clean $(mainobj)
	g++ -L$(sfmllib) $(mainobj) -o $(mainout)  $(sfmllist)
	./$(mainout)

$(mainobj):
	g++ -I$(sfmlsrc) -$(optimization) -DSFML_STATIC -c $(mainsrc)
clean:
	rm -f $(mainout)
	rm -f $(mainobj)
	clear