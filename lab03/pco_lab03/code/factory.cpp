#include "factory.h"
#include "extractor.h"
#include "costs.h"
#include "wholesale.h"
#include <pcosynchro/pcothread.h>
#include <iostream>
#include <algorithm>

WindowInterface* Factory::interface = nullptr;


Factory::Factory(int uniqueId, int fund, ItemType builtItem, std::vector<ItemType> resourcesNeeded)
    : Seller(fund, uniqueId), resourcesNeeded(resourcesNeeded), itemBuilt(builtItem), nbBuild(0)
{
    assert(builtItem == ItemType::Chip ||
           builtItem == ItemType::Plastic ||
           builtItem == ItemType::Robot);

    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Factory created");
}

void Factory::setWholesalers(std::vector<Wholesale *> wholesalers) {
    Factory::wholesalers = wholesalers;

    for(Seller* seller: wholesalers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

ItemType Factory::getItemBuilt() {
    return itemBuilt;
}

int Factory::getMaterialCost() {
    return getCostPerUnit(itemBuilt);
}

bool Factory::verifyResources() {
    for (auto item : resourcesNeeded) {

        if (stocks[item] == 0) {
            return false;
        }
    }

    return true;
}

void Factory::buildItem() {

    int employeeCost = 0;

//    for (unsigned i = 0; i < resourcesNeeded.size(); i++) {
//        employeeCost += getEmployeeSalary(getEmployeeThatProduces(resourcesNeeded.at(i)));
//    }
    employeeCost = getEmployeeSalary(getEmployeeThatProduces(itemBuilt));
    mutex.lock();
    if (money < employeeCost) {
        mutex.unlock();
        PcoThread::usleep(1000U*TIME_CONTROLLER);
        return;
    }


    money -= employeeCost;
    for (unsigned i = 0; i < resourcesNeeded.size(); i++) {
        stocks[resourcesNeeded.at(i)] -= 1;
    }
    mutex.unlock();
    //Temps simulant l'assemblage d'un objet.
    PcoThread::usleep((rand() % 100) * 100000 * TIME_CONTROLLER);

    // TODO
    nbBuild++;
    mutex.lock();

    stocks[itemBuilt] += 1;
    //


    mutex.unlock();

    interface->consoleAppendText(uniqueId, "Factory have build a new object");
}

void Factory::orderResources() {

    // TODO - Itérer sur les resourcesNeeded et les wholesalers disponibles



    //Temps de pause pour éviter trop de demande
    PcoThread::usleep(10 * 100000 * TIME_CONTROLLER);

    for (unsigned i = 0; i < resourcesNeeded.size(); i++) {
        if (stocks[resourcesNeeded.at(i)] == 0) {
            int qty = 1;
            int price = qty * getCostPerUnit(resourcesNeeded.at(i));

                for (auto wholesale : wholesalers) {

                    mutex.lock();
                    if (price > money) {

                        mutex.unlock();
                        /* Not enough funds */
                        interface->consoleAppendText(uniqueId, QString("I don't have enough money to perform the transaction :("));
                        continue;
                    } else {


                        if (wholesale->trade(resourcesNeeded.at(i), qty)) {

                            /* Transaction succeeded */
                            stocks[resourcesNeeded.at(i)] += qty;
                            money -= price;
                            mutex.unlock();
                            interface->consoleAppendText(uniqueId, QString("I have bought %1 of ").arg(qty) % getItemName(resourcesNeeded.at(i))
                                                         % QString(" which have costed me %1").arg(price));
                            return;
                        } else {
                            mutex.unlock();
                            /* vendor cannot perform the transaction */
                            interface->consoleAppendText(uniqueId, QString("The vendor %1 cannot sell me what i asked for. ").arg(wholesale->getUniqueId())
                                                         % getItemName(resourcesNeeded.at(i)));
                            continue;
                        }
                    }
                }
        }
    }
}

void Factory::run() {
    if (wholesalers.empty()) {
        std::cerr << "You have to give to factories wholesalers to sales their resources" << std::endl;
        return;
    }
    interface->consoleAppendText(uniqueId, "[START] Factory routine");

    while (!PcoThread::thisThread()->stopRequested()) {

        if (verifyResources()) {
            buildItem();
        } else {
            orderResources();
        }

        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Factory routine");
}

std::map<ItemType, int> Factory::getItemsForSale() {
    return std::map<ItemType, int>({{itemBuilt, stocks[itemBuilt]}});
}

int Factory::trade(ItemType it, int qty) {
    // TODO

    interface->consoleAppendText(uniqueId, QString("Factory in trade"));

    if (qty <= 0 || getItemsForSale().find(it) == getItemsForSale().end()) {
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

int Factory::getAmountPaidToWorkers() {
    return Factory::nbBuild * getEmployeeSalary(getEmployeeThatProduces(itemBuilt));
}

void Factory::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

PlasticFactory::PlasticFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Plastic, {ItemType::Petrol}) {}

ChipFactory::ChipFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Chip, {ItemType::Sand, ItemType::Copper}) {}

RobotFactory::RobotFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Robot, {ItemType::Chip, ItemType::Plastic}) {}
