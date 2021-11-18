# include <chrono>
# include <random>
# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <mpi.h>


double approximate_pi( unsigned long nbSamples ) 
{
    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    myclock::duration d = beginning.time_since_epoch();
    unsigned seed = d.count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution <double> distribution ( -1.0 ,1.0);
    unsigned long nbDarts = 0;
    // Throw nbSamples darts in the unit square [-1 :1] x [-1 :1]
    for ( unsigned sample = 0 ; sample < nbSamples ; ++ sample ) {
        double x = distribution(generator);
        double y = distribution(generator);
        // Test if the dart is in the unit disk
        if ( x*x+y*y<=1 ) nbDarts ++;
    }
    // Number of nbDarts throwed in the unit disk
    double ratio = double(nbDarts)/double(nbSamples);
    return 4*ratio;
}

int main( int nargs, char* argv[] )
{
	MPI_Init( &nargs, &argv );
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	int rank;
	MPI_Status status;
	MPI_Request request;
	MPI_Comm_rank(globComm, &rank);

	// Création d'un fichier pour la sortie en écriture :
	std::stringstream fileName;
	fileName << "Output" << std::setfill('0') << std::setw(5) << rank << ".txt";
	std::ofstream output( fileName.str().c_str() );

	int nbpSample;
	double ratio;

	if (rank == 0) {
		double temp;
		ratio = approximate_pi(nbpSample);
		output << "I'm the processus " << rank << " on " << nbp << " processes." << std::endl;
		output << "mon approximation de pi est" << ratio << std::endl;

		for (int i = 1; i < nbp; ++i) {
			MPI_Irecv(&temp, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &request);
			ratio = ratio + temp;
			output << "je reçois de "<<i <<"la valeur : "
				<< temp
				<< std::endl;
		}
	
		output << "mon approximation total de pi est"
			<< ratio<<"/"<<nbp<<"="<<ratio/nbp
			<< std::endl;
		output.close();
	}

	ratio = approximate_pi(nbpSample);

	MPI_Send(&ratio, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

	output << "I'm the processus " << rank << " on " << nbp << " processes." << std::endl;
	output << "mon approximation de pi est"
		<< ratio
		<< std::endl;
	output.close();

	MPI_Finalize();
	return EXIT_SUCCESS;
}
