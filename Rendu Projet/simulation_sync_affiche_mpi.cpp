#include <cstdlib>
#include <random>
#include <iostream>
#include <fstream>
#include "contexte.hpp"
#include "individu.hpp"
#include "graphisme/src/SDL2/sdl2.hpp"
#include <chrono> //for clock
#include <mpi.h> //for mpi
#include <cassert> //nécéssaire pour utiliser assert.

/* Used to compile

make all

*/

void màjStatistique( épidémie::Grille& grille, std::vector<épidémie::Individu> const& individus )
{
    for ( auto& statistique : grille.getStatistiques() )
    {
        statistique.nombre_contaminant_grippé_et_contaminé_par_agent = 0;
        statistique.nombre_contaminant_seulement_contaminé_par_agent = 0;
        statistique.nombre_contaminant_seulement_grippé              = 0;
    }
    auto [largeur,hauteur] = grille.dimension();
    auto& statistiques = grille.getStatistiques();
    for ( auto const& personne : individus )
    {
        auto pos = personne.position();

        std::size_t index = pos.x + pos.y * largeur;
        if (personne.aGrippeContagieuse() )
        {
            if (personne.aAgentPathogèneContagieux())
            {
                statistiques[index].nombre_contaminant_grippé_et_contaminé_par_agent += 1;
            }
            else 
            {
                statistiques[index].nombre_contaminant_seulement_grippé += 1;
            }
        }
        else
        {
            if (personne.aAgentPathogèneContagieux())
            {
                statistiques[index].nombre_contaminant_seulement_contaminé_par_agent += 1;
            }
        }
    }
}







void afficheSimulation(sdl2::window& écran, épidémie::Grille const& grille, std::size_t jour, auto const& statistiques)
{
    auto [largeur_écran,hauteur_écran] = écran.dimensions();
    auto [largeur_grille,hauteur_grille] = grille.dimension();
    //auto const& statistiques = grille.getStatistiques();
    sdl2::font fonte_texte("./graphisme/src/data/Lato-Thin.ttf", 18);
    écran.cls({0x00,0x00,0x00});
    // Affichage de la grille :
    std::uint16_t stepX = largeur_écran/largeur_grille;
    unsigned short stepY = (hauteur_écran-50)/hauteur_grille;
    double factor = 255./15.;

    for ( unsigned short i = 0; i < largeur_grille; ++i )
    {
        for (unsigned short j = 0; j < hauteur_grille; ++j )
        {
            auto const& stat = statistiques[i+j*largeur_grille];
            int valueGrippe = stat.nombre_contaminant_grippé_et_contaminé_par_agent+stat.nombre_contaminant_seulement_grippé;
            int valueAgent  = stat.nombre_contaminant_grippé_et_contaminé_par_agent+stat.nombre_contaminant_seulement_contaminé_par_agent;
            std::uint16_t origx = i*stepX;
            std::uint16_t origy = j*stepY;
            std::uint8_t red = valueGrippe > 0 ? 127+std::uint8_t(std::min(128., 0.5*factor*valueGrippe)) : 0;
            std::uint8_t green = std::uint8_t(std::min(255., factor*valueAgent));
            std::uint8_t blue= std::uint8_t(std::min(255., factor*valueAgent ));
            écran << sdl2::rectangle({origx,origy}, {stepX,stepY}, {red, green,blue}, true);
        }
    }

    écran << sdl2::texte("Carte population grippée", fonte_texte, écran, {0xFF,0xFF,0xFF,0xFF}).at(largeur_écran/2, hauteur_écran-20);
    écran << sdl2::texte(std::string("Jour : ") + std::to_string(jour), fonte_texte, écran, {0xFF,0xFF,0xFF,0xFF}).at(0,hauteur_écran-20);
    écran << sdl2::flush;
}





void simulation(bool affiche, auto *adressArgc, auto *adressArgv)
{
    MPI_Init( adressArgc, adressArgv );
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
    assert(nbp == 2); //on veut deux proc! plus n'auraient pas de sens
	int rank;
	MPI_Comm_rank(globComm, &rank); //on ne sépare pas encore en deux comm...
    const int tag = 101; //Lazy tag creation
    MPI_Status status; //nécéssaire pour le MPI_Recv

    MPI_Datatype stat_struct;
        MPI_Type_contiguous(3, MPI_INT, &stat_struct);
        MPI_Type_commit(&stat_struct);
    
    constexpr const unsigned int largeur_écran = 1280, hauteur_écran = 1024;
    sdl2::window écran;

    if (rank==0){
    sdl2::window écran("Simulation épidémie de grippe", {largeur_écran,hauteur_écran});
    }
    unsigned int graine_aléatoire = 1;
    std::uniform_real_distribution<double> porteur_pathogène(0.,1.);

    épidémie::ContexteGlobal contexte; //initialise avec les valeurs de contexte.hpp
    // contexte.déplacement_maximal = 1; <= Si on veut moins de brassage
    // contexte.taux_population = 400'000;
    //contexte.taux_population = 1'000;

    if (rank==0){       //I get no freakouts keeping this here
    contexte.interactions.β = 60.;
    }

    std::vector<épidémie::Individu> population; //population est un vecteur d'individus, définition ici

    population.reserve(contexte.taux_population);   //Il y a 100.000 de population...
    épidémie::Grille grille{contexte.taux_population};  // Taille de la grille dépend de la taille de la population
    //                                                     Grille( std::size_t population ); est le constructeur
    auto [largeur_grille,hauteur_grille] = grille.dimension();
    
    
    // L'agent pathogène n'évolue pas et reste donc constant ?
    épidémie::AgentPathogène agent(graine_aléatoire++); //constructeur agentpathogene avec sa graine aléatoire

    //Je vais faire confiance a l'aléatoire... les graines devraient être identiques pour chaque rank, donc pas besoin d'avoir que un rank qui init puis qui broadcast aux autres (c'est du suicide)
    // Initialisation de la population initiale :----------------------
    for (std::size_t i = 0; i < contexte.taux_population; ++i )
    {
        std::default_random_engine motor(100*(i+1));
        population.emplace_back(graine_aléatoire++, contexte.espérance_de_vie, contexte.déplacement_maximal);
        population.back().setPosition(largeur_grille, hauteur_grille);
        if (porteur_pathogène(motor) < 0.2)
        {
            population.back().estContaminé(agent);   
        }
    }
    
    //Init variables
    std::size_t jours_écoulés = 0;
    int jour_apparition_grippe = 0;
    int nombre_immunisés_grippe= (contexte.taux_population*23)/100;
    sdl2::event_queue queue;
    

    bool quitting = false;
    
    //init écriture dans courbe.dat
    std::ofstream output("Courbe.dat");
    if (rank==0) {
        output << "# jours_écoulés \t nombreTotalContaminésGrippe \t nombreTotalContaminésAgentPathogène()" << std::endl;
    }
    
    épidémie::Grippe grippe(0); //création grippe, seed 0 (?)

    auto& statistiques = grille.getStatistiques(); //pour que rank>=1 ait un buffer en gros? C'est sous-opti surement.

    //Init Chrono
    std::chrono::time_point<std::chrono::system_clock> start, end;
    //Init Moyenne pour q2.2
    double average = 0;

    //-----------------------------------------------------------------------------------------Début boucle épidémie
    std::cout << "Début boucle épidémie " << rank << std::endl << std::flush;
    while (!quitting && jours_écoulés<365)
    {   
        if (0 == rank)
        {
            //Détection de fermeture de fenêtre.
            auto events = queue.pull_events();
            for ( const auto& e : events)
            {
                if (e->kind_of_event() == sdl2::event::quit)
                    quitting = true;
            }

            //début chrono
            start = std::chrono::system_clock::now();
        }

        
        if (rank >= 1)
        {
            if (jours_écoulés%365 == 0)// Si le premier Octobre / début de l'année
            {
                grippe = épidémie::Grippe(jours_écoulés/365);
                jour_apparition_grippe = grippe.dateCalculImportationGrippe();
                grippe.calculNouveauTauxTransmission();
                // 23% des gens sont immunisés. On prend les 23% premiers
                for ( int ipersonne = 0; ipersonne < nombre_immunisés_grippe; ++ipersonne)
                {
                    population[ipersonne].devientImmuniséGrippe();
                }
                for ( int ipersonne = nombre_immunisés_grippe; ipersonne < int(contexte.taux_population); ++ipersonne )
                {
                    population[ipersonne].redevientSensibleGrippe();
                }
            }
            if (jours_écoulés%365 == std::size_t(jour_apparition_grippe))
            {
                for (int ipersonne = nombre_immunisés_grippe; ipersonne < nombre_immunisés_grippe + 25; ++ipersonne )
                {
                    population[ipersonne].estContaminé(grippe);
                }
            }

            // Mise à jour des statistiques pour les cases de la grille :
            màjStatistique(grille, population);
            
            std::size_t compteur_grippe = 0, compteur_agent = 0, mouru = 0;
            //MET A JOUR LA POPULATION A PARTIR DE LA GRILLE MISE A JOUR. // On parcoure la population pour voir qui est contaminé et qui ne l'est pas, d'abord pour la grippe puis pour l'agent pathogène, à partir de la grille updatée.
            for ( auto& personne : population )
            {
                if (personne.testContaminationGrippe(grille, contexte.interactions, grippe, agent))
                {
                    compteur_grippe ++;
                    personne.estContaminé(grippe);
                }
                if (personne.testContaminationAgent(grille, agent))
                {
                    compteur_agent ++;
                    personne.estContaminé(agent);
                }
                // On vérifie si il n'y a pas de personne qui veillissent de veillesse et on génère une nouvelle personne si c'est le cas.
                if (personne.doitMourir())
                {
                    mouru++;
                    unsigned nouvelle_graine = jours_écoulés + personne.position().x*personne.position().y;
                    personne = épidémie::Individu(nouvelle_graine, contexte.espérance_de_vie, contexte.déplacement_maximal);
                    personne.setPosition(largeur_grille, hauteur_grille);
                }
                personne.veillirDUnJour();
                personne.seDéplace(grille);
            }



            auto& statistiques = grille.getStatistiques();
            //MPI envoi bloquant des statistiques au rank 0 == l'afficheur
            MPI_Send(statistiques.data(), largeur_grille*hauteur_grille, stat_struct, 0, tag, globComm);

        }

        if (rank == 0){
            //MPI reception bloquante de la grille
            MPI_Recv(statistiques.data(), largeur_grille*hauteur_grille, stat_struct, 1, tag, globComm, &status);

            //AFFICHAGE/ECRITURE DES RESULTATS POUR LA JOURNEE ACTUELLE
            if (affiche) afficheSimulation(écran, grille, jours_écoulés, statistiques.data()); //J'ai du changer affiche simulation
            //print in terminal instead of Courbe.dat
            // std::cout << "Cool tout fonctionne de ouf : " << jours_écoulés << "\t" << grille.nombreTotalContaminésGrippe() << "\t"
            //          << grille.nombreTotalContaminésAgentPathogène() << std::endl;
            output << jours_écoulés << "\t" << grille.nombreTotalContaminésGrippe() << "\t"
                << grille.nombreTotalContaminésAgentPathogène() << std::endl;
        

            end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            // std::cout << "Temps passé dans la simulation lors du jour "<< jours_écoulés <<" : " << elapsed_seconds.count() 
            //     << std::endl;
            average += elapsed_seconds.count();
        
        }

        jours_écoulés += 1;
        
    }// Fin boucle temporelle
    if (rank == 0){
        average /= 365;
        std::cout << "Temps moyen passé dans la simulation pendant les 365 premiers jours  "<< average
                << std::endl;
    }
    output.close();
    MPI_Finalize();
}






int main(int argc, char* argv[])
{   
    // parse command-line
    bool affiche = true;
    for (int i=0; i<argc; i++) {
      std::cout << i << " " << argv[i] << "\n";
      if (std::string("-nw") == argv[i]) affiche = false;
    }
  
    sdl2::init(argc, argv);
    {
        simulation(affiche, &argc, &argv );
    }

    sdl2::finalize();
    
    return EXIT_SUCCESS;
}







//Fin chrono
        //end = std::chrono::system_clock::now();
        //std::chrono::duration<double> elapsed_seconds = end-start;

        // std::cout << "Temps passé dans la simulation lors du jour "<< jours_écoulés <<" : " << elapsed_seconds.count() 
        //         << std::endl;
        
        //average += elapsed_seconds.count();

/* TEST COMPILATION ET FONCTIONNEMENT MPI
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
        std::cout << "Apres echange, je possede le jeton avec la valeur : " << jeton << std::flush << std::endl;
        */
