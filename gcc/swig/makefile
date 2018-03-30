# Define compilers
CFLAGS_DEBUG := -g -Wall -Wno-unknown-pragmas -Wno-unused-result -Wno-write-strings -Wno-invalid-offsetof -Wno-strict-aliasing -Wno-format-security
CFLAGS := -O3 -Wno-unknown-pragmas -Wno-unused-result -Wno-write-strings -Wno-invalid-offsetof -Wno-strict-aliasing -Wno-format-security

# Set environment variables
SET_INCLUDE_DIRS := export CPLUS_INCLUDE_PATH="../../include/.,../interop/."

# Targets
.PHONY: all swig compile

all: swig compile

swig:
	rm -f *.cs
	swig -csharp -c++ IronSightEngine.i

compile:
	$(SET_INCLUDE_DIRS)
	rm -f libISMath.o libISMath.so
	g++ $(CFLAGS) -I../../include -I../interop -c -o libISMath.o IronSightEngine_wrap.cxx
	g++ -lm -shared -o libISMath.so libISMath.o ../lib/libISEngine.a ../lib/libISMath.a
