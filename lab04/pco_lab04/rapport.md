# PCO Lab04

Auteurs: REDACTED, Quentin Surdez

## Description des fonctionnalités du logiciel

Le logiciel simule une maquette de trains. 

Chaque train a un thread qui lui est attribué. Les trains ont des parcours différents avec une section de ce parcours qui est partagé. 

## Choix d'implémentation

### Arrêt d'urgence

Pour gérer l'arrêt d'urgence en tout temps, nous avons fixer la vitesse des trains à 0. Cela nous permet de nous assurer que les trains ne redémarrent pas même s'ils arrivent dans une condition qui les font appeler la méthode `loco.demarrer()`.

### Gestion de la section partagée

La section du parcours qui est partagée entre les différentes locomotives est l'équivalent d'une section critique. Son accès doit être protégé et le départ de cette section doit ête communiqué aux threads pouvant être en attente. 

Nous avons implémenté une communication entre threads entourant cette section partagée via les méthodes `access()` et `leave()`. Pour implémenter cette communication, nous avons implémenté des sémaphores ainsi que des mutex. 

Ces méthodes sont appelées lorsque la locomotive est passée sur le contact précédant la section partagée et le contact suivant la section partagée. 

#### Méthode access()

Dans la méthode `access()`, nous avons un mutex static qui permet de faire en sorte que la locomotive avec une priorité plus petite que zéro s'arrête et attende que celle avec une priorité plus grande que zéro ait fini de passer par la section critique. 

Le mutex permet aussi de garantir la non-corruption des variables partagées telles que `nbWaiting`. Nous avons intégré la gestion de l'attente dans un `if`. Ce choix a été fait après avoir implémenté une boucle `while` qui selon nous ne servait à rien puisque la variable qui était checkée était la priorité de la locomotive que nous changions dans la boucle `while`. Ainsi un `if` est suffisant. 

Nous appelons un sémaphore `waiting` avec la méthode `acquire()` afin d'attendre que la locomotive ayant la priorité ait terminé de passer dans la section partagée.

#### Méthode leave()

Dans la méthode `leave()`, nous avons le même mutex static qui est utilisé dans la méthode `access()` afin de garantir l'accès sans concurrence aux différentes variables. 

Dans cette méthode, la variable `nbWaiting` doit être au-delà de zéro pour qu'un `release()` sur le sémaphore `waiting` soit appelé. Cela permettra à la locomotive en attente de redémarrer et d'entrer dans la section partagée. 

### Gestion de la gare

La gestion de la gare se fait avec le même mutex static que les deux méthodes précédentes. Cependant, nous arrêtons la locomotive avant d'`acquire()` le mutex. Cela s'explique par le fait que nous voulons forcer la locomotive à s'arrêter même si une autre locomotive est en train d'utiliser le mutex. 

Pour que la locomotive s'arrête correctement, nous avons créé une classe `Gare` qui permet d'avoir le point de contact caractérisant la gare ainsi que le point de contact le précédant. Cela nous permet d'abaisser la vitesse des locomotives lorsqu'elles s'approchent de la gare.

#### Méthode stopAtStation()

Dans cette méthode, nous utilisons un mutex comme dit ci-dessus. Si la locomotive est la première à arriver, sa priorité sera assignée à un nombre négatif et attendra que la deuxième locomotive ait quitté la gare. 

Lorsque la deuxième locomotive arrive en gare, sa priorité sera assignée à un nombre positif. Elle attendra 5 secondes et relâchera le sémaphore qui bloquait la locomotive précédemment arrivée. Enfin, les deux locomotives vont redémarrer et appeler la méthode `access()` lorsqu'elles arriveront à leur point de contact.

La connaissance de la première locomotive arrivée est gérée avec un `bool` qui nous permet de déterminer l'information.


### Gestion des points de contacts

Pour gérer les différentes informations liées au parcours, une classe `Parcours` a été créée. Cette dernière possède plusieurs attributs nécessaire à la bonne réalisation du parcours. 

- Une `std::pair<int,int>` de points qui permettent de représenter le début et la fin de la section partagée pour chaque locomotive. 
- Une `Gare` qui représente la gare du parcours pour chaque locomotive
- Un `std::vector<Etape>` qui permet d'enregistrer tous les aiguillages qui devront être géré dans le parcours pour chaque locomotive

Des getters pour chaque attribut existe dans notre classe afin de pouvoir les utiliser avec la fonction `attendre_contact()`.


## Tests effectués

Plusieurs tests ont été effectués afin de vérifier si le cahier des charges était respecté. 

Le premier test a été de vérifier que la deuxième locomotive arrivée en gare était celle qui accédait en premier à la section
partagée. Cela est bien le cas. 

Le deuxième test a été de vérifier que lorsque la deuxième locomotive est arrivée en gare, les deux locomotives doivent attendre 5 secondes avant de redémarrer. Cela est bien le cas. 

Le troisième test a été de vérifier le bon fonctionnement du programme à différentes vitesse avec notre implémentation. Notre implémentation représente le cas critique où le point de contact du début de la section partagée est le plus proche du point de contact de la gare. Cette implémentation fonctionne à toutes les vitesses en desssous ou égales à 12. Au-delà le fonctionnement n'est pas garanti.

Le quatrième test a été de vérifier que lorsque l'arrêt d'urgence est appuyé, il y a aucun moyen pour que les locomotives puissent redémarrer.
- Le premier cas est le cas simple lorsque les locomotives sont dans leur section privée
- Le deuxième cas est lorsque l'une des locomotives est dans la section partagée
- Le troisième cas est lorsque la locomotive qui était dans le section partagée passe sur le contact de fin de section partagée par inertie
Cela est bien le cas. 


## Conclusion

Nous avons pu implémenter une gestion de section critique dans ce laboratoire avec des sémaphores pour créer une attente passive.

La gestion de la file d'attente a été la partie nécessitant le plus de travail. 

L'intégration du contexte du labo a pu se faire avec quelques complications, principalement liées à l'inertie. 

Nous avons pu créer un programme qui permet aux deux locomotives de posséder une section partagée et de ne pas s'entrechoquer.

