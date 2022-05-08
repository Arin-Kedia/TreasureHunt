#OBJS specifies which files to compile as part of the project
OBJS = main2.cpp maze.cpp

#CC specifies which compiler we're using
CC = g++

#Source Paths


#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -w 

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -I /usr/include/SDL2

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = a.out

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) -g $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
