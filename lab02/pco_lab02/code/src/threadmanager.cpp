#include <QCryptographicHash>
#include <QVector>
#include "pcosynchro/pcothread.h"
#include "threadmanager.h"
#include "mythread.h"


/*
 * std::pow pour les long long unsigned int
 */
/*
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
}*/

ThreadManager::ThreadManager(QObject *parent) :
    QObject(parent)
{}


void ThreadManager::incrementPercentComputed(double percentComputed)
{
    emit sig_incrementPercentComputed(percentComputed);
}

/*
 * Les paramètres sont les suivants:
 *
 * - charset:   QString contenant tous les caractères possibles du mot de passe
 * - salt:      le sel à concaténer au début du mot de passe avant de le hasher
 * - hash:      le hash dont on doit retrouver la préimage
 * - nbChars:   le nombre de caractères du mot de passe à bruteforcer
 * - nbThreads: le nombre de threads à lancer
 *
 * Cette fonction doit retourner le mot de passe correspondant au hash, ou une
 * chaine vide si non trouvé.
 */
QString ThreadManager::startHacking(
        QString charset,
        QString salt,
        QString hash,
        unsigned int nbChars,
        unsigned int nbThreads
)
{
     bool found = false;

    std::vector<std::unique_ptr<PcoThread>> threadList;

     QString *passwordFound = new QString("");


    static  unsigned long long totalComputed= 0;


    for (unsigned int i = 0; i < nbThreads; ++i) {
        threadList.emplace_back(new PcoThread(threadHack, charset, salt, hash, nbChars, nbThreads,  i,&totalComputed, &found, this, passwordFound));
    }


    for (auto& t : threadList) {
        t->join();

    }


    return *passwordFound;
}
