#ifndef PARCOURS_H
#define PARCOURS_H

#include <vector>
#include "etape.h"
#include "gare.h"

/**
 * @brief The Parcours class permet de définir un parcours pour un locomotive. Elle permet de donner les différents aiguillages à ràgler,
 * Les information sur la gare ainsi que le début et la fin de la section critique
 */
class Parcours
{
private:

    /**
     * @brief points représente le début et la fin de la section partagée
    */
    std::pair<int,int> points; 

    /**
     * @brief parcours ,les étapes à régler le long du parcours ( numéro aiguillage, sens aiguillage, contact avant aiguillage)
    */
    std::vector<Etape> parcours;

    /**
     * @brief gare représentant la gare du parcours
    */
    Gare gare; 

public:
    /**
     * @brief Parcours Constructeur de Parcours, pour connaître les spécificités du chemin pris par la loco
     * @param points la paire début et fin de section partagée, prend un contact avant et après pour permettre attente des trains
     * @param parcours vector d'étapes pour gérer les aiguillages, doivent être dans l'ordre qu'ils seront rencontrés durant le parcours
     * @param gare gare pour connaître la position de la gare
     */
    Parcours(std::pair<int, int> points, std::vector<Etape> parcours, Gare gare) : points(points), parcours(parcours), gare(gare) {};

    /**
     * @brief Parcours Constructeur de Parcours, pour connaître les spécificités du chemin pris par la loco, le parcours peut être ajouter plus tard avec un focntion add
     * @param points la paire début et fin de section partagée, prend un contact avant et après pour permettre attente des trains
     * @param parcours vector d'étapes pour gérer les aiguillages, doivent être dans l'ordre qu'ils seront rencontrés durant le parcours
     * @param gare gare pour connaître la position de la gare
     */
    Parcours(Gare gare, std::pair<int,int> points) : points(points), gare(gare) {};

    /**
     * @brief addEtape permet d'ajouter une Etape au parcours
     * @param numAiguillage_ le numéro de l'aiguillage à changer
     * @param sensAiguillage_ le sens de l'aiguillage
     * @param numContact_ le numero de contact avant l'aiguillage
     */
    void addEtape(int numAiguillage_, int sensAiguillage_, int numContact_) {
        parcours.emplace_back(Etape(numAiguillage_, sensAiguillage_, numContact_));
    }

    /**
     * @brief getEtape getter d'Etape
     * @param index index de l'étape
     * @return l'étape contenue à l'index index
     */
    Etape getEtape(int index) {
        return parcours[index];
    }

    /**
     * @brief getGare permet d'obtenir la gare
     * @return la gare
     */
    Gare getGare() {
        return gare;
    }

    /**
     * @brief getDebutSectionPartagee permet d'obtenir le numero de contact du début de la section partagee
     * @return numéro de contact du début de la section
     */
    int getDebutSectionPartagee() {
        return points.first;
    }
    /**
     * @brief getFinSectionPartagee permet d'obtenir le numero de contact de la fin de la section partagee
     * @return numéro de contact de la fin de la section
     */
    int getFinSectionPartagee() {
        return points.second;
    }

};

#endif // PARCOURS_H
