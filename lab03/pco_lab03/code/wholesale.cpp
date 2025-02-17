#include "wholesale.h"
#include "factory.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>

WindowInterface* Wholesale::interface = nullptr;

Wholesale::Wholesale(int uniqueId, int fund)
    : Seller(fund, uniqueId)
{
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Wholesaler Created");

}

void Wholesale::setSellers(std::vector<Seller*> sellers) {
    this->sellers = sellers;

    for(Seller* seller: sellers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

void Wholesale::buyResources() {
    auto s = Seller::chooseRandomSeller(sellers);
    auto m = s->getItemsForSale();
    auto i = Seller::chooseRandomItem(m);

    if (i == ItemType::Nothing) {
        /* Nothing to buy... */
        return;
    }

    int qty = rand() % 5 + 1;
    int price = qty * getCostPerUnit(i);

    interface->consoleAppendText(uniqueId, QString("I would like to buy %1 of ").arg(qty) %
                                 getItemName(i) % QString(" which would cost me %1").arg(price));
    /* TODO */

    mutex.lock();
    if (price > money) {
        mutex.unlock();
        /* Not enough funds */
        interface->consoleAppendText(uniqueId, QString("I don't have enough money to perform the transaction :("));
        return;
    } else {
        if (s->trade(i, qty)) {
            /* Transaction succeeded */
            //mutex.lock();
            stocks[i] += qty;
            money -= price;
            mutex.unlock();
            interface->consoleAppendText(uniqueId, QString("I have bought %1 of ").arg(qty) % getItemName(i) % QString(" which have costed me %1").arg(price));
            return;
        } else {
            mutex.unlock();
            /* vendor cannot perform the transaction */
            interface->consoleAppendText(uniqueId, QString("The vendor %1 cannot sell me what i asked for.").arg(s->getUniqueId()));
            return;
        }

    }
}

void Wholesale::run() {

    if (sellers.empty()) {
        std::cerr << "You have to give factories and mines to a wholeseler before launching is routine" << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Wholesaler routine");
    while (!PcoThread::thisThread()->stopRequested()) {
        buyResources();
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
        //Temps de pause pour espacer les demandes de ressources
        PcoThread::usleep((rand() % 10 + 1) * 100000 * TIME_CONTROLLER);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Wholesaler routine");


}

std::map<ItemType, int> Wholesale::getItemsForSale() {
    return stocks;
}

int Wholesale::trade(ItemType it, int qty) {

    interface->consoleAppendText(uniqueId, "In trade of Wholesale");

    if(qty <= 0 || getItemsForSale().find(it) == getItemsForSale().end()) {
        return 0;
    }

    mutex.lock();
    if (qty > stocks[it]) {
        mutex.unlock();
        return 0;
    }


    stocks[it] -= qty;

    money += qty * getCostPerUnit(it);
    mutex.unlock();

    return qty * getCostPerUnit(it);
}

void Wholesale::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}
