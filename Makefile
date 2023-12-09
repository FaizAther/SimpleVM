.PHONY: all clean run debug

all: main.exe

main.exe: *.cpp *.hpp
	g++ -ggdb main.cpp -o main.exe

run: main.exe
	./main.exe

debug: main.exe
	gdb ./main.exe -q

clean:
	rm main.exe