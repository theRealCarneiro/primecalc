sequencial:
	gcc sequencial.c -o sequencial

paralelo:
	mpicc main.c worker.c master.c -o paralelo

run: paralelo
	mpirun -n 4 --machinefile machinefile ${TARGET}
