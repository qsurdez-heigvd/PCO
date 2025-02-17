# Labo 6 : Producteur-Consommateur pour calcul différé (Hoare)
Quentin Surdez et REDACTED

## Introduction

Le but de ce laboratoire est d'implémenter un système de type consommateur-producteur avec un moniteur de Hoare. Le cas pratique mis en place est un calculateur recevant des 
requêtes et ces requêtes doivent être renvoyées dans le même ordre que celui dans lequel les requêtes ont été appelées. Comme des threads sont implémentés pour représenter 
les différents types de requêtes, nous devons implémenter le moniteur de Hoare afin de n'avoir aucun problème de concurrence.

## Buffer de requêtes
Pour le buffer de requêtes, nous avons décidé d'utiliser une map prenant comme clé le type de computation et comme valeur une liste de requêtes.
Cela nous permet de stocker facilement de stocker dans l'ordre les requêtes par type, mais la liste nous permet tout de même d'en supprimer au milieu de la liste contrairement à une queue.

Ensuite, on peut utiliser des variables de type Condition pour gérer le cas où un thread attend de récupérer une requête, car le buffer est vide et de la même manière pour le cas où le buffer est plein.
On peut définir une Conditon par ComputationType dans les deux cas.

## Buffer de résultats
Nous avions besoin de connaître l'ordre d'arrivée des requêtes pour gérer l'ordre des résultats, indépendamment de leur type. Le plus simple est qu'à l'envoi d'une requête, l'id de la requête est insérée dans un conteneur dans l'ordre. L'idéal est d'avoir le futur résultat dans ce même conteneur pour faciliter le code.
Pour ce faire, nous avons décidé d'utiliser une liste de pair. La key sera l'id de la requête et la valeur le résultat. Pour pouvoir insérer dans ce tableau sans avoir déjà le résultat puis par la suite tester si le résultat est disponible nous avons décidié d'utiliser comme valeur `std::optional<Result>` 

Ensuite pour gérer les résultats, nous avons eu besoin d'une variable de condition pour permettre d'attendre tant que le résultat n'est pas disponible et notifier lorsqu'il devient disponible.

## Arrêt d'une requête
Pour l'arrêt d'une requête un peu comme demandé dans la donnée. On utilise l'attribut stopped que l'on met à true lors de l'appel à stop, dans cet appel, on libère également tous les threads.
Dans `abortComputation`, on supprime la requête de buffer de requête s'il y a besoin et on signale une potientielle requête en attente que le buffer se libère, ainsi que du buffer de résultat.
Dans `nextResult` on cherche le résultat tant que la requête n'est pas arrêtée. Dans `requestWork` et `getWork`, on teste avant et après
les appels de wait() si le système est arrêté. Si oui, on sort du moniteur et on appelle
throwStopException().

## Conclusion

L'implémentation du moniteur de Hoare nous a permis de comprendre et mettre en pratique la théorie vue en cours ainsi que les différents concepts qui y sont rattachés. Il a 
été intéressant de se poser des questions sur quelle structure de données, nous allions utiliser pour gérer le plus facilement possible ces différents cas. Le labo s'est déroulé sans 
trop de difficultés. L'implémentation du moniteur de Hoare étant relativement aisée, elle s'est bien déroulée.