# Note: To run, you must enter the number of processes
	# Compile:         mpicc -g -Wall -o main main.c Lab4_IO.c -lm
	# Single Computer: mpiexec -n <# of processes> ./main
	# Cluster:         mpirun -np <# of processes> -f hosts ./main

# List of all our executables:
all: main

# Setting our compiler & Flags"
CC = mpicc
CFLAGS  = -g -Wall -o

# Executable 1: PageRank Implementation with MPI
main: main.c Lab4_IO.c timer.h 
	$(CC) $(CFLAGS) main main.c Lab4_IO.c -lm
