sequencial:
	gcc sequencial.c -o sequencial

paralelo:
	mpicc paralelo.c -fopenmp -o paralelo

run: paralelo
	~/openmpi/bin/mpirun -n 4 --machinefile machinefile ${TARGET}
