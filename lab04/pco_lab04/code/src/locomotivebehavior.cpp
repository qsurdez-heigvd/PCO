/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */

#include "locomotivebehavior.h"
#include "ctrain_handler.h"
#include "synchro.h"

/**
 * @brief LocomotiveBehavior::run fonction principale qui permet de faire fonctionner la locomotive et
 * de gérer les arrêts en gare et les sections partagées
 */
void LocomotiveBehavior::run()
{
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    /* A vous de jouer ! */
    while(true) {

        Gare gare = parcours.getGare();


        // Attente à la gare
        // vitesse à atteindre un peu avant d'arriver en gare
        int vitesseBeforeStation = 3;

        int oldVitesse = loco.vitesse();


        // on attend le contact avant le début de la gare et on ralentit
        attendre_contact(gare.getNumContactAvantDebut());

        loco.fixerVitesse(vitesseBeforeStation);

        attendre_contact(gare.getNumContactDebut());

        sharedSection->stopAtStation(loco);


        loco.fixerVitesse(oldVitesse * 0.5);

        // Gestion de l'entrée à la section partagée
        attendre_contact(parcours.getDebutSectionPartagee());
        loco.fixerVitesse(oldVitesse);

        sharedSection->access(loco);

        // on attend le contact avant le changement d'aiguillage
        attendre_contact(parcours.getEtape(0).getNumContact());
        diriger_aiguillage(parcours.getEtape(0).getNumAiguillage(), parcours.getEtape(0).getSensAiguillage(), 0);

        // on attend la fin de la section partage et on quitte
        attendre_contact(parcours.getFinSectionPartagee());
        sharedSection->leave(loco);


    }
}

void LocomotiveBehavior::printStartMessage()
{
    qDebug() << "[START] Thread de la loco" << loco.numero() << "lancé";
    loco.afficherMessage("Je suis lancée !");
}

void LocomotiveBehavior::printCompletionMessage()
{
    qDebug() << "[STOP] Thread de la loco" << loco.numero() << "a terminé correctement";
    loco.afficherMessage("J'ai terminé");
}


