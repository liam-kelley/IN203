# include <iostream>
# include <cstdlib>
# include <mpi.h>
#include<random>

int main(int nargs, char* argv[])
{
	MPI_Init(&nargs, &argv);
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	int rank;
	MPI_Comm_rank(globComm, &rank);
	MPI_Status status;

	int jeton;
	int jeton2;
	int tag = 0;

	std::cout << "Hello World, I'm processus " << rank << " on " << nbp << " processes.\n";

	if (rank == 0)
	{
		jeton = rand();
		MPI_Send(&jeton, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(&jeton2, 1, MPI_INT, nbp -1 , tag, MPI_COMM_WORLD, &status);
		jeton2 = jeton2 + 1;
		std::cout << "Jeton reçu de " << nbp - 1 << " par " << rank << " est " << jeton2 << std::endl;
	}

	else {
		MPI_Recv(&jeton, 1, MPI_INT, rank - 1, tag, MPI_COMM_WORLD, &status);
		jeton = jeton + 1;
		std::cout << "Jeton reçu de " << rank - 1 << " par " << rank << " est " << jeton << std::endl;
		jeton2 = rand()%rank;
		MPI_Send(&jeton2, 1, MPI_INT, (rank + 1) % nbp, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return EXIT_SUCCESS;
}
