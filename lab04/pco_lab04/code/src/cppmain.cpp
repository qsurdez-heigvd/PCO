/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */

#include "ctrain_handler.h"

#include "locomotive.h"
#include "locomotivebehavior.h"
#include "synchrointerface.h"
#include "synchro.h"
#include "parcours.h"

// Locomotives :
// Vous pouvez changer les vitesses initiales, ou utiliser la fonction loco.fixerVitesse(vitesse);
// Laissez les numéros des locos à 0 et 1 pour ce laboratoire

// Locomotive A
static Locomotive locoA(0 /* Numéro (pour commande trains sur maquette réelle) */, 13 /* Vitesse */);
// Locomotive B
static Locomotive locoB(1 /* Numéro (pour commande trains sur maquette réelle) */, 13 /* Vitesse */);

//Arret d'urgence
void emergency_stop()
{

    locoA.fixerVitesse(0);
    locoB.fixerVitesse(0);



    afficher_message("\nSTOP!");
}


//Fonction principale
int cmain()
{
    /************
     * Maquette *
     ************/

    //Choix de la maquette (A ou B)
    selection_maquette(MAQUETTE_A /*MAQUETTE_B*/);

    /**********************************
     * Initialisation des aiguillages *
     **********************************/

    // Initialisation des aiguillages
    // Positiion de base donnée comme exemple, vous pouvez la changer comme bon vous semble
    // Vous devrez utiliser cette fonction pour la section partagée pour aiguiller les locos
    // sur le bon parcours (par exemple à la sortie de la section partagée) vous pouvez l'
    // appeler depuis vos thread des locos par ex.
    diriger_aiguillage(1,  TOUT_DROIT, 0);
    diriger_aiguillage(2,  DEVIE     , 0);
    diriger_aiguillage(3,  DEVIE     , 0);
    diriger_aiguillage(4,  DEVIE, 0);
    diriger_aiguillage(5,  TOUT_DROIT, 0);
    diriger_aiguillage(6,  TOUT_DROIT, 0);
    diriger_aiguillage(7,  TOUT_DROIT, 0);
    diriger_aiguillage(8,  DEVIE     , 0);
    diriger_aiguillage(9,  DEVIE     , 0);
    diriger_aiguillage(10, TOUT_DROIT, 0);
    diriger_aiguillage(11, TOUT_DROIT, 0);
    diriger_aiguillage(12, TOUT_DROIT, 0);
    diriger_aiguillage(13, TOUT_DROIT, 0);
    diriger_aiguillage(14, DEVIE     , 0);
    diriger_aiguillage(15, DEVIE     , 0);
    diriger_aiguillage(16, TOUT_DROIT, 0);
    diriger_aiguillage(17, TOUT_DROIT, 0);
    diriger_aiguillage(18, TOUT_DROIT, 0);
    diriger_aiguillage(19, TOUT_DROIT, 0);
    diriger_aiguillage(20, DEVIE     , 0);
    diriger_aiguillage(21, DEVIE     , 0);
    diriger_aiguillage(22, TOUT_DROIT, 0);
    diriger_aiguillage(23, TOUT_DROIT, 0);
    diriger_aiguillage(24, TOUT_DROIT, 0);
    // diriger_aiguillage(/*NUMERO*/, /*TOUT_DROIT | DEVIE*/, /*0*/);

    /********************************
     * Position de départ des locos *
     ********************************/

    // Loco 0
    // Exemple de position de départ
    locoA.fixerPosition(25, 32);

    // Loco 1
    // Exemple de position de départ
    locoB.fixerPosition(22, 28);

    /***********
     * Message *
     **********/

    // Affiche un message dans la console de l'application graphique
    afficher_message("Hit play to start the simulation...");

    /*********************
     * Threads des locos *
     ********************/

    // définition des gares
    Gare gare_1(15, 14); // loco 1
    Gare gare_2(11, 10); // loco 2

    // definition des sections partagées
    std::pair<int,int> premier(7, 32); // loco 1
    std::pair<int,int> second(4, 28); // loco 2

    // création du parcours de la loco 1 et ajout de l'étape pour le changement d'aiguillage
    Parcours parcours_1(gare_1, premier);

    parcours_1.addEtape(20, DEVIE, 34);
    // création du parcours de la loco 2 et ajout de l'étape pour le changement d'aiguillage
    Parcours parcours_2(gare_2, second);

    parcours_2.addEtape(20, TOUT_DROIT, 34);



    // Création de la section partagée
    std::shared_ptr<SynchroInterface> sharedSection = std::make_shared<Synchro>();

    // Création du thread pour la loco 0
    std::unique_ptr<Launchable> locoBehaveA = std::make_unique<LocomotiveBehavior>(locoA, sharedSection, parcours_1/*, autres paramètres ...*/);
    // Création du thread pour la loco 1
    std::unique_ptr<Launchable> locoBehaveB = std::make_unique<LocomotiveBehavior>(locoB, sharedSection, parcours_2 /*, autres paramètres ...*/);

    // Lanchement des threads
    afficher_message(qPrintable(QString("Lancement thread loco A (numéro %1)").arg(locoA.numero())));
    locoBehaveA->startThread();
    afficher_message(qPrintable(QString("Lancement thread loco B (numéro %1)").arg(locoB.numero())));
    locoBehaveB->startThread();

    // Attente sur la fin des threads
    locoBehaveA->join();
    locoBehaveB->join();

    //Fin de la simulation
    mettre_maquette_hors_service();

    return EXIT_SUCCESS;
}
