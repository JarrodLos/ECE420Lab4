# List of all our executables:
all: main

# Setting our compiler & Flags"
CC = mpicc
CFLAGS  = -g -Wall -o

# Executable 1: PageRank Implementation with MPI
main: main.c Lab4_IO.c timer.h 
	$(CC) $(CFLAGS) main main.c Lab4_IO.c

# Clean:


# To RUN:
	# Single Computer:      mpiexec -n <number of processes> ./main
	# Vm Cluster (On VM):   mpirun -np 4 -f hosts ./main

# Discuss your findings under the following problem sizes: (repeat x10 & average performance)
	# 1112 nodes   (./datatrim -b 5300)
	# 5424 nodes   (-b 13000)
	# 10000 nodes  (-b 18789)