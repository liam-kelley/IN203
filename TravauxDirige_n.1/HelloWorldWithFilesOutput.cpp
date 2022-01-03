# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <mpi.h>

int main( int nargs, char* argv[] )
{
	MPI_Init(&nargs, &argv); //init du mode parallèle
    //on clone le MPI_COMM_WORLD a chaque fois parceque tkt
    MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
    int numero_du_processus, nombre_de_processus;
    MPI_Comm_rank(globComm, &numero_du_processus);
    MPI_Comm_size(globComm, &nombre_de_processus);

	numero_du_processus++; //Comme ça on commence à 1 et on va jusqu'à 4

	// Création d'un fichier pour la sortie en écriture de ce processus:
	std::stringstream fileName;
	fileName << "Output" << std::setfill('0')
			<< std::setw(5) << numero_du_processus << ".txt";
	std::ofstream output( fileName.str().c_str() );

	output << "Bonjour, je suis la tâche n° " 
              << numero_du_processus << " sur "
              << nombre_de_processus << " tâches." 
              << std::endl;

	output.close();

	MPI_Finalize();  //Fin du mode parallèle
	return EXIT_SUCCESS;
}
