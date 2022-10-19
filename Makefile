sequencial:
	gcc sequencial.c -o sequencial

paralelo:
	mpicc paralelo.c -o paralelo

run: paralelo
	~/openmpi/bin/mpirun -n 67 --machinefile machinefile ${TARGET}
