#include <cassert> //nécéssaire pour utiliser assert.

# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <mpi.h>

int main( int nargs, char* argv[] )
{

	MPI_Init( &nargs, &argv );
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
    
    assert(nbp == 2); //on veut deux proc! plus n'auraient pas de sens

	int rank;
	MPI_Comm_rank(globComm, &rank);
    
	// Création d'un fichier pour ma propre sortie en écriture :
	std::stringstream fileName;
	fileName << "Output" << std::setfill('0') << std::setw(5) << rank << ".txt";
	std::ofstream output( fileName.str().c_str() );

	int jeton; //Création jeton
	const int tag = 101;
    MPI_Status status;

    if (0 == rank)
	{
		jeton = 420;
		MPI_Send(&jeton, 1, MPI_INT, 1, tag, globComm);
	}
	else if (1 == rank)
	{
		MPI_Recv(&jeton, 1, MPI_INT, 0, tag, globComm, &status);
	}
	
    output << "Apres echange, je possede le jeton avec la valeur : " << jeton << std::flush << std::endl;

	output.close();
	MPI_Finalize();
	return EXIT_SUCCESS;
}
