/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */

/**
 * Gestion du salon avec les fonctions nécessaires utilisées par le barbier et les clients et appels aux animations.
 * @author Quentin Surdez
 * @author Rachel Tranchida
 */

#ifndef PCOSALON_H
#define PCOSALON_H

#include <pcosynchro/pcoconditionvariable.h>
#include <pcosynchro/pcohoaremonitor.h>
#include <pcosynchro/pcomanager.h>
#include <vector>

#include "salon.h"
#include "utils/graphicsaloninterface.h"

class PcoSalon : public Salon, PcoHoareMonitor
{
public:
    PcoSalon(GraphicSalonInterface *interface, unsigned int capacity);

    /********************************************
     * Méthodes de l'interface pour les clients *
     *******************************************/
    /*!
     * \brief accessSalon Permet au client de savoir si le salon est plein
     * \return retourne true si le client peut accéder au salon, false s'il est plein
     * \pair
     */
    bool accessSalon(unsigned clientId) override;

    /*!
     *  \brief moveToWorkingChair Le client se dirige vers la chaise pour se faire embellir
     *  \pair pickNextClient
     */
    void goForHairCut(unsigned clientId) override;

    /*!
     *  \brief waitingForHairToGrow Après s'être fait coiffé, le client part et ne revient
     *  pas tant que ses cheveux n'ont pas repoussé
     *  \pair
     */
    void waitingForHairToGrow(unsigned clientId) override;

    /*!
     *  \brief walkAround Si le salon est plein, le client va se balader pendant un certains
     *  temps et revenir après a voir pris un petit kawa
     */
    void walkAround(unsigned clientId) override;

    /*!
     * \brief goHome Permet au client de partir et de quitter la scène
     */
    void goHome(unsigned clientId) override;

    /********************************************
     * Méthodes de l'interface pour le barbier  *
     *******************************************/
    /*!
     * \brief getNbClient Permet de savoir le nombre de clients qui attendent
     * \return retourne le nombre de client dans la salle d'attente
     */
    unsigned int getNbClient() override;

    /*!
     *  \brief goToSleep va envoyer le barbier dormir lorsqu'il n'y a pas
     *  de client dans son salon et qu'il a terminé d'embellir le dernier client
     */
    void goToSleep() override;

    /*!
     *  \brief pickNextClient permet au barbier de prendre le client suivant
     */
    void pickNextClient() override;

    /*!
     *  \brief waitClientAtChair permet au barbier d'attendre l'arrivée du client
     *  au siège de travail
     */
    void waitClientAtChair() override;

    /*!
     *  \brief beautifyClient permet au barbier de réaliser sa tâche sur le client
     */
    void beautifyClient() override;

    /********************************************
     *    Méthodes générales de l'interface     *
     *******************************************/
    /*!
     * \brief isInService Permet de savoir si le salon est ouvert
     * \return retourne vrai si le salon est ouvert
     */
    bool isInService() override;

    /*!
     * \brief endService Permet au barbier de fermer le salon
     */
    void endService() override;

protected:
    /*!
     * \brief animationClientAccessEntrance déclenche l'animation du client qui
     * accède à l'entrée du salon. Lorsqu'il est possible pour un client d'entrer
     * dans le salon, cette méthode doit être appelée.
     * \param unsigned ID du client
     */
    void animationClientAccessEntrance(unsigned clientId);

    /*!
     * \brief animationClientSitOnChair déclenche l'animation pour que le client
     * ciblé aille s'assoir s'une un des sièges de la salle d'attente
     * \param unsigned ID du client
     * \param unsigned ID du siège sur lequel le client va s'assoir
     */
    void animationClientSitOnChair(unsigned clientId, unsigned clientSitNb);

    /*!
     * \brief animationClientSitOnWorkChair déclenche l'animation du client qui va se
     * placer sur le siège de travail du barbier
     * \param unsigned ID du client
     */
    void animationClientSitOnWorkChair(unsigned clientId);

    /*!
     * \brief animationClientWaitForHairToGrow déclenche l'animation du client qui
     * qui part loin le temps que ses cheveux repoussent
     * \param unsigned ID du client
     */
    void animationClientWaitForHairToGrow(unsigned clientId);

    /*!
     * \brief animationClientWalkAround déclenche l'animation du client qui
     * se balade le temps qu'une place se libère dans le salon
     * \param unsigned ID du client
     */
    void animationClientWalkAround(unsigned clientId);

    /*!
     * \brief animationBarberGoToSleep déclenche l'animation du barbier qui
     * part dans l'arrière salle pour taper un petit somme
     */
    void animationBarberGoToSleep();

    /*!
     * \brief animationWakeUpBarber déclenche l'animation du barbier qui
     * retourne derrière son siège de travail, prêt à bosser
     */
    void animationWakeUpBarber();

    /*!
     * \brief animationWakeUpBarber déclenche l'animation du barbier qui
     * coupe les cheveux du client présent sur le siège de travail
     */
    void animationBarberCuttingHair();


    /*!
     * \brief animationClientGoHome déclenche l'animation pour que le client
     * parte de la scène et rentre chez lui après la fermeture du salon
     * \param unsigned ID du client
     */
    void animationClientGoHome(unsigned clientId);

protected:
    GraphicSalonInterface *_interface;

    PcoMutex _mutex;

    /**
     * Client attend d'être choisi
     */
    PcoConditionVariable goForHaircut;

    /**
     * Barbier attend que le client arrive à la chaise
     */
    PcoConditionVariable waitAtChair;

    /**
     * Client attend la fin de la coupe
     * */
    PcoConditionVariable endCut;

    /**
     * Barbier attend d'être réveillé
     * */
    PcoConditionVariable barberSleeping;

    /**
     * pour savoir si le barbier dort
     */
    bool isSleeping = false;

    /**
     * Le nombre de client en attente sur une chaise
     */
    unsigned int nbWaiting{};

    /** Le nombre de chaises dans la salle d'attente */
    unsigned int capacity;

    /**
     * Associe la clé client id avec le numéro de chaise de la salle d'attente
     */
    std::map<unsigned, unsigned> sittings;

    /**
     * vecteur des chaises avec leur occupation
     */
    std::vector<bool> sittingOccupancy;

    /**
     * prend des id de client pour savoir leur ordre de passage
     */
    std::queue<unsigned> fifo;

    /**
     * pour savoir si le salon est ouvert
     * */
    bool isOpen = true;

};


#endif // PCOSALON_H
