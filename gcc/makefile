# Define compilers
CFLAGS_DEBUG := -g -Wall -Wno-unknown-pragmas -Wno-unused-result -Wno-write-strings -Wno-invalid-offsetof -Wno-strict-aliasing -Wno-format-security -std=c++0x
CFLAGS := -O3 -Wno-unknown-pragmas -Wno-unused-result -Wno-write-strings -Wno-invalid-offsetof -Wno-strict-aliasing -Wno-format-security -std=c++0x

# Set environment variables
SET_INCLUDE_DIRS := export CPLUS_INCLUDE_PATH="../include/.,./interop/."

# Targets
.PHONY: all isengine ismath

all: isengine ismath

isengine:
	$(SET_INCLUDE_DIRS)
	rm -f windows.o Console.o Engine.o FilePathEx.o GlobalPtr.o Interfaces.o Memory.o Result.o ScriptDefinitions.o String.o StringEx.o Time.o lib/libISEngine.a
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ./interop/windows.cpp
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ../ISEngine/Console.cpp
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ../ISEngine/Engine.cpp
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ../ISEngine/FilePathEx.cpp
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ../ISEngine/GlobalPtr.cpp
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ../ISEngine/Interfaces.cpp
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ../ISEngine/Memory.cpp
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ../ISEngine/Result.cpp
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ../ISEngine/ScriptDefinitions.cpp
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ../ISEngine/String.cpp
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ../ISEngine/StringEx.cpp
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ../ISEngine/Time.cpp
	ar rvs lib/libISEngine.a windows.o Console.o Engine.o FilePathEx.o GlobalPtr.o Interfaces.o Memory.o Result.o ScriptDefinitions.o String.o StringEx.o Time.o

ismath:
	$(SET_INCLUDE_DIRS)
	rm -f windows.o Math.o lib/libISMath.a
	g++ $(CFLAGS) -I../ISEngine/. -I../include/. -I./interop/. -c ./interop/windows.cpp
	g++ $(CFLAGS) -I../ISMath/. -I../include/. -I./interop/. -c ../ISMath/Math.cpp
	ar rvs lib/libISMath.a windows.o Math.o
