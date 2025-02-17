#ifndef ETAPE_H
#define ETAPE_H

/**
 * @brief The Etape class permet de définir une Etape qui contient les infos pour les réglages d'aiguillage,
 * permet de sauver l'aiguillage à changer pour une loco, le sens de l'aiguillage après changement et après quel contact se situe l'aiguillage
 */
class Etape
{
private:
    int numAiguillage;
    int sensAiguillage;
    int numContact;
public:
    /**
     * @brief Etape permet d'ajouter une Etape au parcours
     * @param numAiguillage_ le numéro de l'aiguillage à changer
     * @param sensAiguillage_ le sens de l'aiguillage
     * @param numContact_ le numero de contact avant l'aiguillage
     */
    Etape(int numAiguillage, int sensAiguillage, int numContact) : numAiguillage(numAiguillage), sensAiguillage(sensAiguillage), numContact(numContact) {};


    /**
     * @brief getNumContact getter du numero de contact avant aiguillage
     * @return le numero du contact
     */
    int getNumContact() {
        return numContact;
    }
    /**
     * @brief getSensAiguillage le sens de l'aiguillage à obtenir
     * @return le sens de l'aiguillage
     */
    int getSensAiguillage() {
        return sensAiguillage;
    }

    /**
     * @brief getNumAiguillage permet d'obtenir le numéro de l'aiguillage
     * @return le numéro de l'aiguillage
     */
    int getNumAiguillage() {
        return numAiguillage;
    }

};

#endif // ETAPE_H
