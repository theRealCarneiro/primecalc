TARGET=prime

${TARGET}:
	mpicc prime.c -o ${TARGET}

run: ${TARGET}
	~/openmpi/bin/mpirun -n 67 --machinefile machinefile ${TARGET}
