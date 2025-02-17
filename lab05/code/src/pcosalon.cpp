/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */

/**
 * Implémentation de la Classe PcoSalon
 * @author Quentin Surdez
 * @author Rachel Tranchida
 */


#include "pcosalon.h"

#include <pcosynchro/pcothread.h>

#include <iostream>

PcoSalon::PcoSalon(GraphicSalonInterface *interface, unsigned int capacity)
    : _interface(interface), capacity(capacity)
{
    /* On initialise le vecteur d'occupation des chaises au nombre de chaises */
    sittingOccupancy = std::vector<bool>(capacity, false);

}

/********************************************
 * Méthodes de l'interface pour les clients *
 *******************************************/
unsigned int PcoSalon::getNbClient()
{
    return fifo.size();
}

/* Avoir un buffer pour que les clients soient pris les uns après les autres
 * dans le bon ordre */



bool PcoSalon::accessSalon(unsigned clientId)
{
    _mutex.lock();
    animationClientAccessEntrance(clientId);
    /*  si encore des places d'attentes vides ou
     * qu'il n'y plus de client à prendre en charge ou pris en charge
     */
    if (nbWaiting < capacity || fifo.empty()) {
        fifo.push(clientId);

       if(isSleeping) { /* le barbier dort, il n'y a personne */
           /* Wake uuuuup */
                barberSleeping.notifyOne();
                animationWakeUpBarber();


        } else { /* on va attendre sur une chaise */

            nbWaiting++;
            /* find first free chair number */
            auto it = std::find(sittingOccupancy.begin(), sittingOccupancy.end(), false);
            unsigned sitId = std::distance(sittingOccupancy.begin(), it);

            /* mark it as occupied */
            sittingOccupancy.at(sitId) = true;

            /* add clientId key associated with chair on map */
            sittings.insert({clientId, sitId});

            /* Go to the waiting room and sit on a chair */
            animationClientSitOnChair(clientId, sitId);
        }
        _mutex.unlock();
        return true;
    } else {
        /* Walk around until there are free chairs */
        animationClientWalkAround(clientId);
        _mutex.unlock();
        return false;
    }
}


void PcoSalon::goForHairCut(unsigned clientId)
{
    _mutex.lock();
        /* if the first client in queue is not you wait */

        while (clientId != fifo.front()) {
            goForHaircut.wait(&_mutex);
        }

        _interface->consoleAppendTextClient(clientId,"Je vais sur la chaise de travail");

        animationClientSitOnWorkChair(clientId);

        /* si le client était assis à une chaise, on la libère */
        if(sittings.find(clientId) != sittings.end()) {
            sittingOccupancy.at(sittings[clientId]) = false;
            sittings.erase(clientId);
            nbWaiting--;
        }


        /* on notifie le barbier que l'on est arrivé sur la chaise */
        waitAtChair.notifyOne();


    _mutex.unlock();
}

void PcoSalon::waitingForHairToGrow(unsigned clientId)
{
    _mutex.lock();
    _interface->consoleAppendTextClient(clientId, "J'attends la fin de la coupe");
    /* on attend la fin de la coupe pour attendre que nos cheveux repoussent (vraiment un cycle éternel !) */
    endCut.wait(&_mutex);
    animationClientWaitForHairToGrow(clientId);

    _mutex.unlock();
}


void PcoSalon::walkAround(unsigned clientId)
{
    _mutex.lock();
    animationClientWalkAround(clientId);
    _mutex.unlock();

}


void PcoSalon::goHome(unsigned clientId){

    _mutex.lock();
    animationClientGoHome(clientId);
    _mutex.unlock();

}


/********************************************
 * Méthodes de l'interface pour le barbier  *
 *******************************************/
void PcoSalon::goToSleep()
{
    _mutex.lock();
    /* On met isSleeping à true avant l'animation car le mutex sera relâché */
    isSleeping = true;
    animationBarberGoToSleep();

    /* on attend d'être réveillé */
    barberSleeping.wait(&_mutex);
    isSleeping = false;
    _mutex.unlock();
}


void PcoSalon::pickNextClient()
{
    _mutex.lock();
    goForHaircut.notifyAll();
    _mutex.unlock();
}


void PcoSalon::waitClientAtChair()
{
    _mutex.lock();
    waitAtChair.wait(&_mutex);
    _mutex.unlock();
}


void PcoSalon::beautifyClient()
{
    _mutex.lock();
    animationBarberCuttingHair();

    /* on enlève le client de la queue et on notifie qu'il peut aller attendre
     ses cheveux repoussent
     */
    fifo.pop();
    endCut.notifyOne();

    _mutex.unlock();

}

/********************************************
 *    Méthodes générales de l'interface     *
 *******************************************/
bool PcoSalon::isInService()
{
    return isOpen;
}


void PcoSalon::endService()
{
    _mutex.lock();
    isOpen = false;

    /* si le barbier dort, on le réveille */
    if(isSleeping) {
        barberSleeping.notifyOne();
    }
    _mutex.unlock();
}

/********************************************
 *   Méthodes privées pour les animations   *
 *******************************************/
void PcoSalon::animationClientAccessEntrance(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientAccessEntrance(clientId);
    _mutex.lock();
}

void PcoSalon::animationClientSitOnChair(unsigned clientId, unsigned clientSitNb)
{
    _mutex.unlock();
    _interface->clientSitOnChair(clientId, clientSitNb);
    _mutex.lock();
}

void PcoSalon::animationClientSitOnWorkChair(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientSitOnWorkChair(clientId);
    _mutex.lock();
}

void PcoSalon::animationClientWaitForHairToGrow(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientWaitHairToGrow(clientId, true);
    _mutex.lock();
}

void PcoSalon::animationClientWalkAround(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientWalkAround(clientId);
    _mutex.lock();
}

void PcoSalon::animationBarberGoToSleep()
{
    _mutex.unlock();
    _interface->barberGoToSleep();
    _mutex.lock();
}

void PcoSalon::animationWakeUpBarber()
{
    _mutex.unlock();
    _interface->clientWakeUpBarber();
    _mutex.lock();
}

void PcoSalon::animationBarberCuttingHair()
{
    _mutex.unlock();
    _interface->barberCuttingHair();
    _mutex.lock();
}

void PcoSalon::animationClientGoHome(unsigned clientId){
    _mutex.unlock();
    _interface->clientWaitHairToGrow(clientId, false);
    _mutex.lock();
}
