#include "mpi.h"
#include <iostream>
#include <cstring>

const int BUFFSIZE = 32;
//assume MAXPROCS > numprocs
const int MAXPROCS = 8;

//message: Hello from process *
void fill_buff(char buff[BUFFSIZE], int id)
{
	const char* message = "Hello from process ";
	strcpy(buff, message);
	buff[strlen(message)] = '0' + id;
	buff[strlen(message) + 1] = '\0';

}
int main(int argc, char *argv[])
{
	int id, numprocs;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Status status;

	char send_buff[BUFFSIZE];
	char recv_buff[MAXPROCS][BUFFSIZE];

	fill_buff(send_buff, id);

	for (int i = 0; i < numprocs; i++)
		if (i != id)    MPI_Send(send_buff, strlen(send_buff), MPI_CHAR, i, 0, MPI_COMM_WORLD);

	for (int i = 0; i < numprocs; i++)
		if (i != id)
		{
			MPI_Recv(recv_buff[i], BUFFSIZE, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			printf("process %d received: %s\n", id, recv_buff[i]);
		}
	MPI_Finalize();
	return 0;
}
