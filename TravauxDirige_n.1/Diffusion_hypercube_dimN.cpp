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
    
    //pas d'assert faisable ici mais on veut un -np puissance de 2

	int rank;
	MPI_Comm_rank(globComm, &rank);
    
	// Création d'un fichier pour ma propre sortie en écriture :
	std::stringstream fileName;
	fileName << "Output" << std::setfill('0') << std::setw(5) << rank << ".txt";
	std::ofstream output( fileName.str().c_str() );

	int jeton; //Création jeton
	const int tag = 101; //lazy tag
    MPI_Status status;

    // Calcul dimension du cube, tout le monde doit connaitre la dim du cube...
	int ndim = 0;
	int nbnodes = nbp;
	while (nbnodes > 1)
	{
		nbnodes /= 2;
		++ ndim;
	}   //on divise autant de fois qu'il faut (on suppose np puissance de 2)

    // On vérifie qu'on avait bien une puissance de deux comme nombre de processus :
	if (nbp - (1<<ndim) > 0)
	{ // Si la différence est non nulle, c'est qu'on n'avait pas une puissance de deux !
		std::cerr << "Attention : le nombre de processus doit etre une puissance de deux !" << std::flush << std::endl;
		MPI_Abort(globComm, -1);
	}

    // if (0 == rank) 
    // {
	// 	jeton = 420;
    //     for(int i = 0; i < ndim; i++)
    //     {
    //         MPI_Send(&jeton, 1, MPI_INT, rank + 1<<i, tag, globComm);
    //     }
    //     //en dim 3
	// 	/*MPI_Send(&jeton, 1, MPI_INT, rank + 1, tag, globComm);
    //     MPI_Send(&jeton, 1, MPI_INT, rank + 2, tag, globComm);
    //     MPI_Send(&jeton, 1, MPI_INT, rank + 4, tag, globComm); */
	// }
    // int sum=0;
    // for (int i = 0; i < ndim; i++)
    //     for(int j = 1; j <= 1<<i)
    //         if (j+sum==rank)
    //     sum+=

	// else if (1 == rank) //2^0
	// {
	// 	MPI_Recv(&jeton, 1, MPI_INT, rank - 1, tag, globComm, &status);
    //     MPI_Send(&jeton, 1, MPI_INT, rank + 2, tag, globComm);
    //     MPI_Send(&jeton, 1, MPI_INT, rank + 4, tag, globComm);
	// }
    // else if (2 == rank) || (3 == rank) //2^1
	// {
    //     MPI_Recv(&jeton, 1, MPI_INT, rank - 2, tag, globComm, &status);
    //     MPI_Send(&jeton, 1, MPI_INT, rank + 4, tag, globComm);
    // }
    // else if (4 >= rank) //2^2
    // {
    //     MPI_Recv(&jeton, 1, MPI_INT, rank - 4, tag, globComm, &status);
    // }
	
    output << "Apres echange, je possede le jeton avec la valeur : " << jeton << std::flush << std::endl;

	output.close();
	MPI_Finalize();
	return EXIT_SUCCESS;
}
