/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */

/**
 * Implémentation de la classe Barber
 * @author Quentin Surdez
 * @author Rachel Tranchida
 */

#include "barber.h"
#include <unistd.h>

#include <iostream>

Barber::Barber(GraphicSalonInterface *interface, std::shared_ptr<SalonBarberInterface> salon)
    : _interface(interface), _salon(salon)
{
    _interface->consoleAppendTextBarber("Salut, prêt à travailler !");
}

void Barber::run()
{

    while(true) {
        if (_salon->getNbClient() > 0) {

            /* Pick the next client */
            _salon->pickNextClient();
            _interface->consoleAppendTextBarber("Je prends le prochain client.");
            /* Wait for the client at the chair */
            _salon->waitClientAtChair();
            _interface->consoleAppendTextBarber("J'attends le client à la chaise.");
            /* Beautify the client */
            _salon->beautifyClient();
            _interface->consoleAppendTextBarber("Je coiffe le client.");


        } else {
            if(!_salon->isInService()) break;
            /* Go to sleep */
            _salon->goToSleep();
            _interface->consoleAppendTextBarber("Je vais dodo, il n'y a aucun client");

        }
    }
    /* Le salon est fermé, c'est fini */
    _interface->consoleAppendTextBarber("La journée est terminée, à demain !");
}
