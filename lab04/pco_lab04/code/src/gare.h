#ifndef GARE_H
#define GARE_H

/**
 * @brief The Gare class permet d'avoir les informations sur la position de la gare permettant aux trains de s'arrêter au bon moment
 */
class Gare
{
private:
    int numContactAvantDebut;
    int numContactDebut;
public:
    /**
     * @brief Gare constructeur d'une gare
     * @param numContactAvantDebut contact avant le début de la gare pour permettre le ralentissement des trains
     * @param numContactDebut le numéro de contact du début de la gare
     */
    Gare(int numContactAvantDebut, int numContactDebut, int numContactFin) : numContactAvantDebut(numContactAvantDebut), numContactDebut(numContactDebut), numContactFin(numContactFin) {};

    /**
     * @brief getNumContactDebut permet d'obtenir le numéro de contact du début de la gare
     * @return le numéro de contact du début de la gare
     */
    int getNumContactDebut() {
        return numContactDebut;
    }

    /**
     * @brief getNumContactAvantDebut permet d'obtenir le numéro de contact avant le début de la gare
     * @return le numéro de contact avant le début de la gare
     */
    int getNumContactAvantDebut() {
        return numContactAvantDebut;
    }
};

#endif // GARE_H
