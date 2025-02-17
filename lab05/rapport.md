# Rapport Lab05

Authors : REDACTED, Quentin Surdez

## Introduction

Le but de ce laboratoire est d'utiliser les moniteurs de Mesa afin de simuler le fonctionnement d'un salon de coiffure. Les différentes entités, le barbier, les clients, ont des threads et accèdent à des ressources qui sont partagées entre eux. 

L'implémentation du partage de ces ressources se fait par les moniteurs de Mesa.

## Choix d'implémentation

Pour réaliser ce laboratoire différents choix ont été faits au niveau de l'implémentation. Nous nous concentrons ici sur les choix principaux. Nous commencerons par discuter de l'accès au salon, puis de comment nous gérons le moment où le barbier va dormir. Ensuite, nous discuterons de la gestion des chaises dans la salle d'attente et de l'implémentation d'une fifo pour gérer l'accès à ces chaises. Enfin, nous discuterons de la terminaison du programme.

### Accès au salon
Si le salon est ouvert, un client essaie d'accéder au salon avec un appel à `accessSalon()`.
S'il y a de la place, il peut entrer et est ajouté dans la file d'attente, implémentée avec une `std::queue`.

Il vérifie si le barbier dort. si c'est le cas, il le réveille. Il attend ensuite d'être choisi par le babrier.
Si le barbier ne dort pas, c'est qu'il est occupé, il va donc prendre place sur une chaise.

### Le barbier va dormir
On a une variable `isSleeping` pour savoir si le babrier dort. On l'initialise à true avant l'animation du barbier qui va dormir pour éviter q'on le réveille avant qu'il soit effectivement en wait qui doit forcément se faire après l'animation. 

### Gestion des chaises d'attente et de la fifo
La gestion de l'ordre est implémenté avec une fifo. Lorsqu'un client entre, sont id est ajouté dans un `std::queue`. Lorsque le babrier choisit le prochain client, il notifie tout le monde.
Dans `goForHaircut()`, les clients attendent d'être notifié dans un while loop qui teste si leur client id correspond à l'id du front de la pile, si non ils attendent de nouveau et seulement le client au bon id ira se faire coiffer.
Pour éviter des problèmes de concurrence, l'id du client choisit est retiré de la queue uniquement quand il a fini de se faire coiffer. C'est `beautiyClient()` qui effectue ce `fifo.pop()`.

Pour les sièges, nous avons utilisé un `std::vector` de booléens de la taille du nombre de sièges. Quand un client va s'asseoir, il prend la première chaise libre du vector.

Nous avons également rajouté une map qui prend comme clé l'id du client et la chaise d'attente attribuée. Cela nous permet 2. de libérer la bonne chaise du vector lorsque le client est choisi par le babrbier et 2. de vérifier dans la map si le client choisit était un client assis. Ainsi, dans le cas du client qui a réveillé le barbier, on s'assure de ne pas libérer une chaise de manière erronée.

### Terminaison du programme
Lorsque le client insère quelque chose dan le terminal, la fonction endService est appelée
Pour terminer le programme de manière à ce que tous les clients restant se fassent coiffer avant la fin, nous avons ajouté un booléen isOpen initialisé à true. La fonction `isInService()` retourne la valeur de ce booléen.

Chaque thread client fonctionne dans un loop qui teste si le salon est en service. Ainsi, lors de la fermeture du salon, plus aucun client n'entre.
Pour le barbier, il fonctionne dans un `while(true)`. Lorsqu'il n'a plus de client, il vérifie si le salon est fermé, si c'est le cas, il break de sa boucle, si ce n'est pas le cas il va dormir.

Il restait à gérer le cas où la fermeture du salon se faisait lorsque le barbier dormait déjà. Dans ce cas-là, la fonction `enService()` vérifie si le barbier dort, si c'est le cas, il le notifie. Le barbier sort ensuite de sa boucle et finit son thread.

## Tests

Nous avons décidé de faire des tests avec les différents cas limite que le programme pouvait avoir lors de son exécution. 

Premièrement nous avons testé le programme lors d'une configuration basique, soit lorsque le nombre de chaises est plus grand que le nombre de client total. Le comportement est comme attendu. 

Ensuite, nous avons testé le programme lorsqu'il y avait moins de chaises que de client total. Le comportement est comme attendu.

Pour le troisième test, nous avons testé le programme lorsque le nombre de chaises était égal au nombre de client total. Le comportement est comme attendu. 

Pour le quatrième test, nous avons testé le programme lorsque nous avons exactement une seule chaise et un plus grand nombre de client total. Le comportement est comme attendu. 

Pour le cinquième test, nous avons testé le programme lorsqu'il n'y a aucune chaise dans la salle d'attente et des clients. Le comportement est comme attendu.

Pour le sixième test, nous avons testé le programme lorsqu'il n'y a aucun client. Le comportement est comme attendu. 

## Commentaires

Nous avons choisi de documenter uniquement les fichiers que nous avons changé. Les headers se trouvent également uniquement dans les fichiers que nous avons changé. 

## Conclusion

À travers ce laboratoire, nous avons pu implémenté des moniteurs de Mesa afin de réaliser un programme avec plusieurs threads accédant aux mêmes ressources. Cela nous a permis de comprendre les différents concepts derrière cette méthode de programmation et d'avoir de la pratique dans celle-ci.


























