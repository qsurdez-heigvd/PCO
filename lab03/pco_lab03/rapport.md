# Titre du laboratoire

Auteurs: Quentin Surdez, REDACTED

## Description des fonctionnalités du logiciel
Le logiciel simule une vente entre différents acteurs pour afin de créer des ressources. 

Chaque acteur a différents moyens afin d'obtenir sa ressource. L'extracteur paye un employé. Le grossiste, peut acheter et revendre ses stocks. L'usine peut payer un employé et utiliser des ressources afin de construire une ressource plus avancée. 

Les achats et ventes entre ces différents acteurs est ce que le logiciel simule.

Il est important de noter que le logiciel utilise le concept de thread afin que les différents acteurs puisse agir de manière parallèle.


## Choix d'implémentation

On a commencé par gérer la fin de la simulation pour pouvoir tester proprement le programme.

Ensuite on a implémenté la logique du programme sans mutex pour commencer. Puis finalement, nous avons rajouté les mutex là où ils semblaient 
nécessaires.

### Fin de la simulation
Pour la fin de la simulation, on a utilisé la fonction requestStop() de `PcoThread`. Quand `endService` est appelé,
on fait un requestStop sur tous les threads. Cela permet de faire tourner l'intérieur des fonctions run des threads dans un
while loop dont on sort seulement si `stopRequested()` des threads est true.

### Ajout des mutex
Les problèmes principaux venaient de la gestion des stocks et de l'argent (i.e les attributs `stocks` et `money`), c'est les variables principales qu'il fallait protéger et ceci dans chaque type de `Seller`.
Nous avons donc commencé par ajouter un mutex comme attribut dans `Factory`, `Extractor` et `Wholesale`. 

Le mutex ajouté est un simple mutex. Il n'est pas static car les attributs à protéger ne sont pas partagés entre différentes instances de classes.

Nous avons décidé d'utiliser un seul mutex, car `money` et `stocks` sont souvent modifiés l'un après l'autre et il ne semblait pas y avoir de réelle plus-value à leur donner un mutex distinct.

#### Extractor
On a rajouté des mutex dans la fonction `run`et `trade`.
Dans `run`:

Entre mutex, on teste s'il y a assez d'argent pour payer l'employé. Sinon, on `unlock()` et `return`, si oui on paie l'employé, puis on `unlock()` pour le sleep simulant le temps de minage. On teste si l'argent est suffisant entre mutex pour éviter
que l'argent puisse être diminué entre le test et le paiement de l'employé. 

En théorie, dans le programme actuel, l'argent ne pourrait pas augmenter entre les deux, mais il est plus sûr de le mettre à cet endroit si le programme venait à évoluer et une diminution d'argent viendrait à être possible entre les deux. Cette logique sera utilisé à chaque fois, également lorsque l'on testera les stocks avant de les diminuer.


Dans `trade`, on vérifie avant le mutex que la quantité demandée est > 0 et demande une ressource qui est possédée. Puis on met entre mutex le test de stock et les différentes modification de `money` et `stocks`.

#### Factory
Dans factory on rajoute des mutex dans les fonctions `trade` et `orderRessources`et `buildItem`. `trade` est implémenté similairement à la même fonction dans `Extractor`. Dans `orderRessources` on utilise la même logique pour placer les mutex ainsi que leur lock et unlock. Pour `buildItem` également sauf qu'à la fin, une fois l'assemblage terminé on `lock()` pour mettre à jour le stock avec l'objet assemblé puis on `unlock()` derrière.

#### Wholesale
Pour `Wholesale`, on modifie `trade` et `buyRessources`. `trade` est similaire à précédemment. `buyRessources` est fait avec la même logique que `orderRessources` de `Factory`

## Tests effectués
Pour tester, on a ajouté dans `Seller.h`, une constante `TIME_CONTROLLER` pour pouvoir modifier les délais d'attente généraux et vérifier que le système a toujours le comportement escompté. 
Nous avons testé en multipliant le temps par un facteur 0.125, 0.25, 0.5. Le comportement restait celui attendu avec les mutex.

En effet, aucun argent n'était créé ou perdu. Les objets se construisent et s'échangent correctement. 

Nous n'avons pas créés d'autres tests que ce dernier. En effet, il est le seul efficace et intéressant dans notre cas. Il permet de générer des collisions de threads sur les ressources et de s'assurer que les mutex fonctionnent correctement.

## Conclusion

Nous avons pu implémenter les mutex afin de pouvoir garantir un accès aux données partagées. Dans l'implémentation du labo, cela a été difficile à voir.
 
En effet, il n'existe pas suffisamment de possibilités pour que les threads s'entrechoquent. Il nous est nécessaire de descendre drastiquement la valeur du temps afin de créer des possibilités pour les threads d'accéder à la même valeur et aisni d'être bloqué par le mutex.

Difficilement observable, nous avons quand même pu effectuer des tests qui nous prouvent que notre implémentation fonctionne même à des temps petits.


