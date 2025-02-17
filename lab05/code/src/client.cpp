/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */

/**
 * Implémentation de la Classe Client
 * @author Quentin Surdez
 * @author Rachel Tranchida
 */

#include "client.h"
#include <unistd.h>

#include <iostream>

int Client::_nextId = 0;

Client::Client(GraphicSalonInterface *interface, std::shared_ptr<SalonClientInterface> salon)

    : _interface(interface), _salon(salon),  _clientId(_nextId++)
{
    _interface->consoleAppendTextClient(_clientId, "Salut, prêt pour une coupe !");
}

void Client::run()
{
    //
    while(_salon->isInService()) {
        if (_salon->accessSalon(_clientId)) { /* S'il y a de la place dans le salon */
            _interface->consoleAppendTextClient(_clientId, "Yes ! Il y a de la place dans le salon");
            _salon->goForHairCut(_clientId);
            _interface->consoleAppendTextClient(_clientId, "Je vais me faire couper les cheveux");
            _salon->waitingForHairToGrow(_clientId);
            _interface->consoleAppendTextClient(_clientId, "J'attends que mes cheveux repoussent");

        } else { /* Plus de place dans le salon */

            _salon->walkAround(_clientId);
            _interface->consoleAppendTextClient(_clientId, "Il n'y a plus de place, je vais faire un tour");
        }
    }
        /* Le salon est fermé, le client rentre chez lui */
        _salon->goHome(_clientId);

        _interface->consoleAppendTextClient(_clientId, "Le salon est fermé... Zut !");



}
