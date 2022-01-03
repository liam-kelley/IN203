# include <chrono>
# include <random>
# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <mpi.h>

// Attention , ne marche qu'en C++ 11 ou supérieur :
double approximate_pi( unsigned long nbSamples ) 
{
    typedef std::chrono::high_resolution_clock myclock; //Timing
    myclock::time_point beginning = myclock::now();
    myclock::duration d = beginning.time_since_epoch();

    unsigned seed = d.count();	//Randoming
    std::default_random_engine generator(seed);
    std::uniform_real_distribution <double> distribution ( -1.0 ,1.0);

    unsigned long nbDarts = 0; //Variable du nombre de darts dans le cercle

    // Throw nbSamples darts in the unit square [-1 :1] x [-1 :1]
    for ( unsigned sample = 0 ; sample < nbSamples ; ++ sample ) {
        
		//Place un point
		double x = distribution(generator);
        double y = distribution(generator);

		//si norme < 1, c'est contenu dans le cercle
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
	MPI_Comm_rank(globComm, &rank);

	// Création d'un fichier pour ma propre sortie en écriture : Inutile ici.
	std::stringstream fileName;
	fileName << "Output" << std::setfill('0') << std::setw(5) << rank << ".txt";
	std::ofstream output( fileName.str().c_str() );

	unsigned long nbSamples = 100000;
    if (nargs > 1 ) nbSamples = std::atoi(argv[1]); //on peut set le nombre de points voulu avec la commande

	MPI_Request* requests = nullptr; //Necessary for I stuff
	double* other_results = nullptr;

	const int tag = 101; // Lazy tag

	if (0 == rank)
	{
        // On prépare déjà les réceptions pour le rang 0 :

        requests = new MPI_Request[nbp-1]; //new : Allocates size bytes of storage, suitably aligned to represent any object of that size, and returns a non-null pointer to the first byte of this block
        other_results = new double[nbp-1];

		//les rank 1 à nbp-1 enverront leurs résultats ici
        for ( int p = 1; p < nbp; ++p )
			//MPI_Irecv ( &buf , count , datatype , source , tag , comm , & request )
			//request permet de connaitre l'état de l'envoi
			//dans les I send/receive, les var sont stoquees dans un buffer ici other_results
            MPI_Irecv(other_results+p-1, 1, MPI_DOUBLE, p, tag, globComm, requests + p - 1);
    }

	//rank 0 sera pret a recevoir dans other_results.

	//on sépare le travail à faire: NOTE on ne divise pas par nbp-1 car rank 0 fera aussi des calculs
	unsigned long nbSamples_loc = nbSamples/nbp;
	// Pour traiter le cas où ce n'est pas divisible, on réparti les samplings non encore affectés
    // sur les premiers processus (nombre de samplings non affectés = reste de la division entière)
    if (nbSamples_loc%nbp < (unsigned long)rank) nbSamples_loc += 1; //Mais on en donne pas plus au 0! que au 1, 2, 3...

	double partial_result =  approximate_pi(nbSamples_loc); //Chaque rank fait son approx de pi sur un samplesize plus petit (y compris rank 0!) (la valeur retournée est linéaire on peut faire une moyenne)

	if (rank > 0)
		//MPI_Send (&buf ,count ,datatype ,dest ,tag ,comm), envoi bloquant! en même temps why not
        MPI_Send(&partial_result, 1, MPI_DOUBLE, 0, tag, globComm);
	else //Pour rank 0
    {
        MPI_Status* array_of_statuses; //nécéssaire pour Waitall
        array_of_statuses = new MPI_Status[nbp-1];
		MPI_Waitall(nbp-1, requests, array_of_statuses); //Cette fonction bloque jusqu'a que l'on ait TOUT reçu and les IRecv
    
		//On fait la moyenne des résultats
		//partial_result est déjà défini pour 0, on peut sommer dessus puis diviser pour moyenner.
		for ( int i = 0; i < nbp - 1; ++i )
            partial_result = partial_result + other_results[i];
        partial_result = partial_result / nbp;

		//libérer mémoire attribuée.
		delete [] array_of_statuses;
        delete [] other_results;
        delete [] requests;

		std::cout << "Valeur approchee de pi trouvee : " << partial_result << std::flush << std::endl;
    }

	output.close();
	
	MPI_Finalize();
	return EXIT_SUCCESS;
}
