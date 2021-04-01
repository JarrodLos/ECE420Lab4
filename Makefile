# List of all our executables:
all: main

# Setting our compiler & Flags"
CC = mpicc
CFLAGS  = -g -Wall -o

# Executable 1: PageRank Implementation with MPI
main: main.c Lab4_IO.c timer.h 
	$(CC) $(CFLAGS) main main.c Lab4_IO.c -lm
	# mpicc -g -Wall -o main main.c Lab4_IO.c -lm

# Clean:


# To RUN:
	# SerialTester:         gcc serialtester.c Lab4_IO.c -o serialtester -lm
	# SerialMain:           gcc main.c Lab4_IO.c -o main -lm
	# Single Computer:      mpiexec -n 4 ./main
	# Vm Cluster (On VM):   mpirun -np 4 -f hosts ./main

# Discuss your findings under the following problem sizes: (repeat x10 & average performance)
	# gcc datatrim.c -o datatrim
	# 1112 nodes   (./datatrim -b 5300 -o 1112_nodes_input)   // 1112 nodes & 208,960 edges
	# 5424 nodes   (./datatrim -b 13000 -o 5424_nodes_input)  // 5424 nodes & 3,629,571 edges
	# 10000 nodes  (./datatrim -b 18789 -o 10000_nodes_input) // 10000 nodes & 10,224,274