/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */


#ifndef SYNCHRO_H
#define SYNCHRO_H

#include <QDebug>

#include <pcosynchro/pcosemaphore.h>
#include <pcosynchro/pcothread.h>

#include "locomotive.h"
#include "ctrain_handler.h"
#include "synchrointerface.h"

/**
 * @brief La classe Synchro implémente l'interface SynchroInterface qui
 * propose les méthodes liées à la section partagée.
 */
class Synchro final : public SynchroInterface
{

public:

    /**
     * @brief Synchro Constructeur de la classe qui représente la section partagée.
     *
     */
    Synchro() {

    };


    /**
     * @brief access Méthode à appeler pour accéder à la section partagée
     *
     * Elle doit arrêter la locomotive et mettre son thread en attente si nécessaire.
     *
     * @param loco La locomotive qui essaie accéder à la section partagée
     */
    void access(Locomotive &loco) override {
        afficher_message(qPrintable(QString("The engine no. %1 accesses the shared section with a priority of %2").arg(loco.numero()).arg(loco.priority)));
        mutex.acquire();

        // on attend si la priorite est inférieure à 0
        if (loco.priority < 0) {
            nbWaiting++;
            loco.arreter();
            loco.priority = 1;
            mutex.release();
            // on attend que l'autre loco prioritaire quitte la section et release
            waiting.acquire();


            // WAITING FOR THE SHARED SECTION TO BE FREE

            mutex.acquire();
            nbWaiting--;
            loco.demarrer();
        }

        mutex.release();
        return;
    }

    /**
     * @brief leave Méthode à appeler pour indiquer que la locomotive est sortie de la section partagée
     *
     * Réveille les threads des locomotives potentiellement en attente.
     *
     * @param loco La locomotive qui quitte la section partagée
     */
    void leave(Locomotive& loco) override {

        mutex.acquire();
        // s'il y a une loco en attente on release
        if (nbWaiting > 0) {
             waiting.release();
        }

        mutex.release();
        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 leaves the shared section.").arg(loco.numero())));
    }

    /**
     * @brief stopAtStation Méthode à appeler quand la locomotive doit attendre à la gare
     * param loco La locomotive qui doit attendre à la gare
     */
    void stopAtStation(Locomotive& loco) override {
        afficher_message(qPrintable(QString("The engine no. %1 arrives at the station.").arg(loco.numero())));
        loco.arreter();
        mutex.acquire();
        if (!firstArrived) { // première loco arrive
            loco.priority = -10; // priorité négative car pas prioritaire


            firstArrived = true;
            mutex.release();
            afficher_message(qPrintable(QString("The engine no. %1 is the first arrived.").arg(loco.numero())));
            waitingStation.acquire();



        } else { // deuxième loco arrive
            loco.priority = 10; // priorité positive car loco arrivée en deuxième et prioritaire

            // on reset firstArrived
            firstArrived = false;
            // sleep(5s)
            PcoThread::usleep(5e6);
            mutex.release();
            afficher_message(qPrintable(QString("The engine no. %1 is the second arrived.").arg(loco.numero())));

            waitingStation.release();

        }

        // on redémarre après les 5 secondes d'attente
        loco.demarrer();

        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 leaves the station.").arg(loco.numero())));
    }


private:

    static inline PcoSemaphore waitingStation{0}; // semaphore pour l'attente à la gare
    static inline PcoSemaphore waiting{0}; // semaphore pour l'attente en section partagee
    static inline PcoSemaphore mutex{1};
    static inline unsigned int nbWaiting{0}; // pour savoir si un train attend d'accéder à la section partagée
    static inline bool firstArrived{false}; // bool pour savoir que la première loco est arrivée en gare
};


#endif // SYNCHRO_H
