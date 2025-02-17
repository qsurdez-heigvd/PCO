#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <pcosynchro/pcothread.h>
#include <QString>
#include <QObject>
#include <QCryptographicHash>
#include <QVector>
#include_next <qobjectdefs.h>
#include <QtCore/qchar.h>
#include "threadmanager.h"

void threadHack(        QString charset,
                   QString salt,
                   QString hash,
                   unsigned int nbChars,
                   unsigned int nbThreads, unsigned int threadID, long long unsigned int *totalComputed, bool *found, ThreadManager *thisThread, QString *passwordFound);

#endif // MYTHREAD_H
