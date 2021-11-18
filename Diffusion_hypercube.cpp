# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <mpi.h>
#include <tgmath.h>

int puissance2(int value) {
	if (value == 0) { return 1; }
	return 2*puissance2(value - 1);
}

int puissancede2_inf(int value) {
	int retour = (int)floor(log(value) / log(2));
	return retour;
}

int puissancede2_sup(int value) {
	int retour = (int)ceil(log(value) / log(2));
	if (value == 0) { return 0; }
	else if (puissance2(retour)==value){return retour+1;}//si value est une puissance de 2
	else {
		return retour;
	}
}



int main( int nargs, char* argv[] )
{
	MPI_Init( &nargs, &argv );
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	MPI_Status status;
	int tag = 0;
	int rank;
	int dimension = puissancede2_inf(nbp);

	MPI_Comm_rank(globComm, &rank);

	// Création d'un fichier pour ma propre sortie en écriture :
	std::stringstream fileName;
	fileName << "Output" << std::setfill('0') << std::setw(5) << rank << ".txt";
	std::ofstream output( fileName.str().c_str() );

	int jeton = 2;

	if (rank == 0) {
		int i = puissancede2_sup(rank);
		output << "I'm the processus " << rank << " on " << nbp << " processes." << std::endl;
		while (rank + puissance2(i) < nbp) {

			MPI_Send(&jeton, 1, MPI_INT, rank + puissance2(i), 0, MPI_COMM_WORLD);

			output << "J'envoie à " << rank + puissance2(i) << std::endl;
			++i;
		}
	}
	else {
		
		int recu;
		int rang_recu = rank - puissance2(puissancede2_inf(rank));
		output << "I'm the processus " << rank << " on " << nbp << " processes." << std::endl;
		output << "je reçois de " << rang_recu << std::endl;

		MPI_Recv(&recu, 1, MPI_INT, rang_recu, tag, MPI_COMM_WORLD, &status);
		int i = puissancede2_sup(rank);
		while (rank + puissance2(i) < nbp) {
			MPI_Send(&recu, 1, MPI_INT, rank + puissance2(i), 0, MPI_COMM_WORLD);
			
			output << "J'envoie à " << rank + puissance2(i) << std::endl;
			++i;
		}
	}
	output.close();
	MPI_Finalize();
	return EXIT_SUCCESS;
}
