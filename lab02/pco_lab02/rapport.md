# Rapport du labo 2 Rachel Tranchida & Quentin Surdez

Auteurs : REDACTED & Quentin Surdez

## Introduction

Une application pouvant retrouver un mot de passe à partir d'un hash md5 nous est fournie. Nous devons rendre cette application multi-threadée afin d'améliorer la performance de cette dernière. Pour ce faire, nous avons à disposition la librairie PcoSynchro et nous devons utiliser PcoThread pour créer les threads nécessaires à l'amélioration de l'application. 

## Choix d'implémentation
### Séparation du travail entre threads
Pour séparer le travail en fonction des threads, nous avons décidé de divisier le nombre de mots de passe à tester par le nombre de thread. Le but de notre implémentation est que chaque thread puisse calculer lui-même son prochain mot de passe et que plusieurs threads ne calculent pas 
le même mot de passe. 

Pour ce faire, on a donné à chaque thread un ID allant de $0$ à $nbThreads-1$. Dans chaque thread, le premier mot de passe à tester est initialisé avec que le caractère en position $0$ sauf celui de "poids faible" (ici, on suppose qu'il n'y aura pas plus de `nbChars` threads.
Ensuite, chaque thread, au lieu d'incrémenter de 1 comme dans le programm originel, incrémentera de la chaîne à tester de `nbThreads`, il a donc fallu modifier légèrement la partie du code qui calcule le prochain mot de passe.

```cpp
while (i < (unsigned int)currentPasswordArray.size()) {
    currentPasswordArray[i]+= toIncrement;

    if (currentPasswordArray[i] >= nbValidChars) {
        toIncrement = currentPasswordArray[i]/nbValidChars;
        currentPasswordArray[i] = currentPasswordArray[i]%nbValidChars;

        i++;
    } else
        break;

}
```

Chaque fonction calcule son propre nombre de mot de passe à calculer. En effet, Le nombre de mot de passe total ne sera pas forcément un multiple du nombre de thread, pour associer le bon nombre à calculer on a utilisé cette formule :

```cpp
nbToCompute        = totalToCompute/nbThreads;
nbToCompute += (nbToCompute%nbThreads < threadID) ? 1 : 0;
```

### Fin des fonctions
Pour récupérer le mot de passe trouvé, on a choisi de passer aux threads un pointeur sur le string qui contiendra le résultat.
Lorsque l'un des threads trouve le mot de passe, il le met dans ce string. Ici, le multi-threading ne pose pas de problème car un seul thread peut trouver le mot de passe et il n'y aura qu'un seul thread qui modifiera cette variable ou aucun si le mot de passe n'est pas trouvé.


De la même manière, pour finir tous les threads lorqu'un thread a trouvé le mot de passe, une variable partagée passée en paramètre est mise à true et tous les threads testent à chaque boucle si le mot de passe est trouvée et finissent la fonction si c'est le cas. 

Ici, il n'était pas non plus nécesaire de protéger cette variable, car un thread
au plus pourra trouver le mot de passe et lui seul modifiera la variable le cas écheant.


### Barre de progression
La barre de progression nous a posé plus de questions, en effet, nous avons commencé par conceptualiser un code qui utiliserait une variable partagée `totalComputed` que chaque thread viendrait incrémenter.
Cette variable serait aussi utilisée pour appeler la fonction d'incrémentation afin d'update la barre de progression. 
L'implémentation de cette solution demandait des `mutex`, ce que nous avons ajouté au code. 


Cependant, après plusieurs essais et comparaison entre le nombre de thread et le temps de résolution des calculs, un problème évident existait. 
Le temps pris par le programme avec les `mutex` était beaucoup trop élevé, au point qu''à partir de 4 threads, le programme prenait plus de temps qu'avec moins de threads.
Cela nous a amener à repenser notre solution. 
Nous avons d'abord remplacé la variable `totalComputed` par un `std::atomic`.
Cela nous permettait de faire une incrémentation de manière atomique sans avoir besoin de `mutex`. 
Les changements n'ont pas été concluants. 

Nous avons alors réfléchi à une solution qui n'utiliserait pas les `mutex`. 
Nous avons alors conçu l'idée que chaque thread possède un sous-espace de hash semblable. Lorsque l'entier de son espace est calculé, alors il a rempli toute sa tâche. 
Les espaces sont très proche dans leur cardinalité. 
Avec ces affirmations, nous avons effectué le calcul pour update la barre de progresssion selon le nombre de hash calculé par le thread. 
On pourrait dire que le comportement de la barre n'est pas tout à fait le même car le pourcentage n'est pas augmenté tous les 1000 itérations totales, mais à la place, toutes les 1000 itérations d'un thread, mais 
nous avons estimé que ce comportement était suffisant visuellement et était tout de même cohérent avec la progression générale de la recherche.

Néanmoins, même si on utilise plus de mutex pour la variable partagée, il restait la question de l'appel de la fonction d'incrémentation. Sans doute que cet appel-ci devrait être protégé par des mutex. 


Après de nombreux tests, on a remarqué que sans mutex, la barre de progression avait le comportment escompté. Après quelques recherches, nous concluons que l'utilisation des signaux et du `connect` de **Qt** est thread safe et ne pose pas de problème pour la barre de progression. 

Dans tous les cas,
Le comportement de la barre de progression est cohérent et ne montre pas de problèmes

## Tests effectués
Nous avons effectué différents tests avec des mots de passe différents et un nombre de thread différent. Nous avons pour chaque combinaison de mot de passe et de nombre de thread, récolté quatre valeurs dont nous avons fait une moyenne pour connaître le temps moyen de résolution. 

Les mots de passes que nous utilisons sont de longueur 4. La longueur 5 et plus est possible mais prend beaucoup de temps. Afin d'avoir des tests rapides et efficaces nous avons choisi la longueur 4.


#### Chaîne en début de l'ensemble
La première chaîne testée est abcd. Cette chaîne se trouve au début de l'ensemble des chaînes à tester.
Les temps seront donc petits et nous pourrons observer comment notre répartition des différents threads est efficace dans ce cas.

| number of threads | temps moyen(ms) |
|:--------:|:-------------:|
| 1 | 1034 |
| 4 | 303.4 |
| 8 | 220 |

Le tableau ci-dessus nous montre une clair amélioration du temps en fonction du nombre de thread utilisé. Voyons si c'est le cas pour les chaînes se trouvant à d'autres endroits de notre set. 

#### Chaîne en milieu de l'ensemble
La deuxième chaîne testée est ABCD. Cette chaîne se trouve à peu près en milieu de l'ensemble des chaînes à tester. 
Nous devrions observer des temps plus longs que précédemment. Observons si les valeurs vont diminuer en fonction du nombre de thread utilisé.

| number of threads | temps moyen(ms) |
|:--------:|:-------------:|
| 1 | 9447.4 |
| 4 | 3006.5 |
| 8 | 2328.5 |

À nouveau nous voyons une amélioration du temps en fonction du nombre de thread que nous utilisons. 
Voyons si les chaînes en fin de l'ensemble confirme le comportement des chaînes testées jusqu'à maintenant. 

#### Chaîne en fin d'ensemble

La troisième chaîne testée se trouve en fin de l'ensemble et est ****.  Observons si le comportement change lorsque la chaîne est en fin d'ensemble.

| number of threads | temps moyen(ms) |
|:--------:|:-------------:|
| 1 | 24668.75 |
| 4 | 9168 |
| 8 | 7274.25 |

À nouveau le temps diminue en fonction du nombre de threads qu'on met en input. Enfin regardons si le sel a un impact sur les performances. 

#### Chaîne salée

Nous utilisons la même chaîne qu'au début pour observer si le fait d'ajouter un sel pose problème. La chaîne est donc abcd et le sel est xy.

| number of threads | temps moyen(ms) |
|:--------:|:-------------:|
| 1 | 1025.75 |
| 4 | 309.5 |
| 8 | 228 |

On peut voir que le sel ne change pas les performances. 

## Conclusion

Le but du labo étant de transformer l'application en une application multi-threadée, nous l'avons rempli. 
Notre implémentation donne des résultats concluants et logiques puisque le temps pour chaque chaîne diminue en fonction du nombre de thread. 

La subdivision de l'espace de recherche a été intéressante et montre que pour les mots de passe en fin ou en milieu de set les différences de temps sont attendues. 
Il y a une augmentation du temps de recherche ce qui est logique au vu de l'emplacement dans le dictionnaire des différentes chaînes. 

Les différentes itérations de développement nous ont permis d'appréhender les concepts de management des threads, le concept d'espace de solution qui doit être réparti et comment concevoir une barre de progression. 

Ce TP a permis d'acquérir ces connaissances. 


