#include "mythread.h"
#include <string>
#include "qchar.h"
#include "pcosynchro/pcomutex.h"
#include "threadmanager.h"

long long unsigned int intPow (
    long long unsigned int number,
    long long unsigned int index)
{
    long long unsigned int i;

    if (index == 0)
        return 1;

    long long unsigned int num = number;

    for (i = 1; i < index; i++)
        number *= num;

    return number;
}


void threadHack( QString charset,
                   QString salt,
                   QString hash,
                   unsigned int nbChars,
                   unsigned int nbThreads, unsigned int threadID, long long unsigned *totalComputed, bool  *found, ThreadManager *thisThread, QString  *passwordFound) {
    unsigned int i;

    long long unsigned int nbToCompute;
    long long unsigned int nbComputed;
    static PcoMutex mutex;
    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    unsigned int nbValidChars;

    /*
     * Mot de passe à tester courant
     */
    QString currentPasswordString;

    /*
     * Tableau contenant les index dans la chaine charset des caractères de
     * currentPasswordString
     */
    QVector<unsigned int> currentPasswordArray;

    /*
     * Hash du mot de passe à tester courant
     */
    QString currentHash;

    /*
     * Object QCryptographicHash servant à générer des md5
     */
    QCryptographicHash md5(QCryptographicHash::Md5);

    /*
     * Calcul du nombre de hash à générer
     */
    unsigned long long totalToCompute = intPow(charset.length(),nbChars);
    nbToCompute        = totalToCompute/nbThreads;
    nbToCompute += (nbToCompute%nbThreads < threadID) ? 1 : 0;
    nbComputed = 0;
    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    nbValidChars       = charset.length();

    /*
     * On initialise le premier mot de passe à tester courant en le remplissant
     * de nbChars fois du premier caractère de charset
     */
    currentPasswordString.fill(charset.at(0),nbChars);
    currentPasswordArray.fill(0,nbChars);
    currentPasswordArray[0] += threadID;
    currentPasswordString[0] = currentPasswordArray.at(0);
    /*
     * Tant qu'on a pas tout essayé...
     */
    while (nbComputed < nbToCompute) {
        /* On vide les données déjà ajoutées au générateur */
        md5.reset();
        /* On préfixe le mot de passe avec le sel */
        md5.addData(salt.toLatin1());
        md5.addData(currentPasswordString.toLatin1());
        /* On calcul le hash */
        currentHash = md5.result().toHex();

        /*
         * Si on a trouvé, on retourne le mot de passe courant (sans le sel)
         */
        if(*found) {
            return;
        }
        if (currentHash == hash) {
            //currentHash une valeur unique et l'ensemble des valeurs testées par chaque thread est disjoint
            *found = true;
            *passwordFound = currentPasswordString;
            return;
        }



        /*
         * Tous les 1000 hash calculés, on notifie qui veut bien entendre
         * de l'état de notre avancement (pour la barre de progression)
         */



        if ((nbComputed % 1000) == 0) {
            thisThread->incrementPercentComputed((double)1000/totalToCompute);
        }




        /*
         * On récupère le mot de pass à tester suivant.
         *
         * L'opération se résume à incrémenter currentPasswordArray comme si
         * chaque élément de ce vecteur représentait un digit d'un nombre en
         * base nbValidChars.
         *
         * Le digit de poids faible étant en position 0
         */
        i = 0;

        /*while (i < (unsigned int)currentPasswordArray.size()) {
            currentPasswordArray[i]++;

            if (currentPasswordArray[i] >= nbValidChars) {
                currentPasswordArray[i] = 0;
                i++;
            } else
                break;
        }*/



        unsigned int toIncrement = nbThreads;


        while (i < (unsigned int)currentPasswordArray.size()) {
            currentPasswordArray[i]+= toIncrement;

            if (currentPasswordArray[i] >= nbValidChars) {
                toIncrement = currentPasswordArray[i]/nbValidChars;
                currentPasswordArray[i] = currentPasswordArray[i]%nbValidChars;

                i++;
            } else
                break;

        }
        /*
         * On traduit les index présents dans currentPasswordArray en
         * caractères
         */
        for (i=0;i<nbChars;i++)
            currentPasswordString[i]  = charset.at(currentPasswordArray.at(i));

        nbComputed++;

    }
    /*
     * Si on arrive ici, cela signifie que tous les mot de passe possibles ont
     * été testés, et qu'aucun n'est la préimage de ce hash.
     */
    return;
}

