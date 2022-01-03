# include <iostream>
# include <cstdlib>
# include <mpi.h>
# include <sstream>
# include <fstream>

int main( int nargs, char* argv[] )
{
    MPI_Init(&nargs, &argv); //init du mode parallèle
    //on clone le MPI_COMM_WORLD a chaque fois parceque tkt
    MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);

    int numero_du_processus, nombre_de_processus;
    MPI_Comm_rank(globComm, &numero_du_processus);
    MPI_Comm_size(globComm, &nombre_de_processus);

    //ici dans cette zone on peut faire de la programmation parallèle par échange de messages
    std::cout << "Bonjour, je suis la tâche n° " 
              << numero_du_processus << " sur "
              << nombre_de_processus << " tâches." 
              << std::endl;

    MPI_Finalize(); //Fin du mode parallèle
    return EXIT_SUCCESS; //pk pas "return MPI_SUCCESS;" ?
}
