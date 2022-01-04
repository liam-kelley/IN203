
# Question 2.1

Temps moyen passé par pas de temps AVEC affichage pendant les 365 premiers jours  0.0392283

Temps moyen passé par pas de temps SANS affichage pendant les 365 premiers jours  0.0202578

En moyenne 0.0189705 le temps passé à afficher presque égal au temps passé a simuler!

Séparer les taches en 2 et les parraléliser devrait presque doubler notre vitesse!

# Question 2.2

Temps moyen passé par pas de temps AVEC affichage pendant les 365 premiers jours  0.0258345 --> Comparé à 0.0392283, c'est 1.518 fois plus rapide!!

Temps moyen passé par pas de temps SANS affichage pendant les 365 premiers jours  0.0201779 --> Comparé à 0.0202578, écart négligeable.

Notre théorie se confirme!, séparer les taches a presque fait doubler notre vitesse.
Les Envois et les Réceptions, bloquantes, et les temps d'attente qui en découlent expliquent l'écart à 2 de notre accélération.

# Question 2.3

Temps moyen passé par pas de temps de SIMULATION pendant les 365 premiers jours 0.0264994 C'est un peu plus lent, dû aux différents Isend et Irecv, le petit traitement en plus. Néanmoins, la simulation a le temps d'avancer sur le prochain traitement


# Question 2.4

On a les mêmes seeds pour tout nombre de threads!
Temps avec #pragma ..  et OMP_NUM_THREADS=1 exactement le même qu'à la question 2.3

threads = 1     0.0255671
threads = 2     0.0146301
threads = 4     0.00911959
threads = 16    0.00735605




Il y a beaucoup de commentaires dans les programmes. Je tacherai de les inscrire ici.
