
### Accès au salon
Si le salon est ouvert, un client essaie d'accéder au salon avec un appel à accessSalon().
S'il y a de la place, il peut entrer et est ajouté dans la file d'attente, implémentée avec une std::queue.
Il vérifie si le barbier dort. si c'est le cas, il le réveille. Il attend ensuite d'être choisi par le babrier.
Si le barbier ne dort pas, c'est qu'il est occupé, il va donc prendre place sur une chaise.

### Le babrier va dormir
On a une variable isSleeping pour savoir si le babrier dort. On l'initialise à true avant l'animation du barbier qui va dormir pour éviter q'on le réveille avant qu'il soit effectivement en wait qui doit forcément se faire après l'animation.
### Gestion des chaises d'attente et de la fifo
La gestion de l'ordre est implémenté avec une fifo. Lorsqu'un client entre, sont id est ajouté dans un std::queue. Lorsque le babrier choisit le prochain client, il notifie tout le monde.
Dans goForHaircut(), les clients attendent d'être notifié dans un while loop qui teste si leur client id correspond à l'id du front de la pile, si non ils attendent de nouveau et seulement le client au bon id ira se faire coiffer.
Pour éviter des problèmes de concurrence, l'id du client choisit est retiré de la queue uniquement quand il a fini de se faire coiffer. C'est `beautiyClient()` qui effectue ce `fifo.pop()`.
Pour les sièges, nous avons utilisé un std::vector de booléens de la taille du nombre de sièges. Quand un client va s'asseoir, il prend la première chaise libre du vector.
NOus avons également rajouté une map qui prend comme clé l'id du client et la chaise d'attente attribuée. Cela nous permet 2. de libérer la bonne chaise du vector lorsque le client est choisi par le babrbier et 2. de vérifier dans la map si le client choisit était un client assis. Ainsi, dans le cas du client qui a réveillé le barbier, on s'assure de ne pas libérer une chaise de manière erronée.
### Terminaison du programme
Lorsque le client insère quelque chose dan le terminal, la fonction endService est appelée
Pour terminer le programme de manière à ce que tous les clients restant se fassent coiffer avant la fin, nous avons ajouté un booléen isOpen initialisé à true. La fonction isInService() retourne la valeur de ce booléen.
Chaque thread client fonctionne dans un loop qui teste si le salon est en service. Ainsi, lors de la fermeture du salon, plus aucun client n'entre.
Pour le barbier, il fonctionne dans un while(true). Lorsqu'il n'a plus de client, il vérifie si le salon est fermé, si c'est le cas, il break de sa boucle, si ce n'est pas le cas il va dormir.
Il restait à gérer le cas où la fermeture du salon se faisait lorsque le barbier dormait déjà. Dans ce cas-là, la fonction enService() vérifie si le barbier dort, si c'est le cas, il le notifie. Le barbier sort ensuite de sa boucle et finit son thread.