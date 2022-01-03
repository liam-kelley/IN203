# include <iostream>
# include <cstdlib>
# include <mpi.h>

int main( int nargs, char* argv[] )
{
	MPI_Init(&nargs, &argv); //init du mode parallèle
    MPI_Comm globComm; //clone de MPI_COMM_WORLD
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
    int rank, nbp; //rang et nombre de taches
    MPI_Comm_rank(globComm, &rank);
    MPI_Comm_size(globComm, &nbp);

	MPI_Status Status; //Required variable for receive routines

	int jeton; //on doit avoir initialisé un int jeton pour eux tous

	if (rank==0){
		//génère un entier
		int jeton = 100;
		//l'envoie au processus suivant
		MPI_Send(&jeton,/*count*/1,/*type*/MPI_INT,/*dest*/rank+1,/*idtag*/rank,globComm);
		//recoit le jeton du processus nbp-1
		MPI_Recv(&jeton,/*count*/1,/*type*/MPI_INT,/*source*/nbp-1,/*idtag*/nbp-1,globComm,/*status*/&Status);
		//affiche le jeton
		jeton++;
		std::cout << "Salut, c'est moi " << rank << " et voici le jeton " << jeton << " que j'ai reçu de " << nbp-1 <<  ". (J'ai ajouté 1 hihi)" << std::endl;
	}
	else {
		//recoit le jeton
		MPI_Recv(&jeton,/*count*/1,/*type*/MPI_INT,/*source*/rank-1,/*idtag*/rank-1,globComm,/*status*/&Status);
		//l'incrémente de 1
		jeton++;
		//affiche le jeton
		std::cout << "Salut, c'est moi " << rank << " et voici le jeton " << jeton << " que j'ai reçu de " << rank-1 << ". (J'ai ajouté 1 hihi)" << std::endl;
		//envoie le jeton au processus rank + 1
		MPI_Send(&jeton,/*count*/1,/*type*/MPI_INT,/*dest*/(rank+1)%nbp,/*idtag*/rank,globComm);
	}
	
	MPI_Finalize();
	return EXIT_SUCCESS;
}

//rank 0 print en dernier car il est en attente du processus nbp-1 !!