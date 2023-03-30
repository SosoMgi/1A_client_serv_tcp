#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#define TAILLE_LIGNE 1024
#define TAILLE_NOM 30
#define MAX_C 15

/*
struct Client {
    struct sockaddr_in;
    char* user;
    int sockclient;
};

*/



int max(int *tab) {
    int maxi = tab[0];
    for (int i = 0; i < 10; i++) {
        if (tab[i] > maxi)
            maxi = tab[i];
    }
    return maxi;
}


char** decoupeChaine (const char *str, const char *separateur) 
{ 
  char **a = NULL; 
  int taille = 0; 
  int longueur = 0; 
  int i; 
  int longueur_separateur = strlen (separateur); 
  const char *p0; 
  char *p1; 
 
  /* Initialisations */ 
  if (NULL == (a = malloc (TAILLE_LIGNE * sizeof *a)))
  {
    fprintf(stderr, "erreur malloc decoupeChaîne");
    exit(EXIT_FAILURE);
  } 
  taille = TAILLE_LIGNE; 
  longueur = 0; 
  p0 = str; 
   
  // Récupération des sous-chaînes  
  while (p0 && (p1 = strstr (p0, separateur))) 
    { 
      if (longueur >= (taille+2)) 
        { 
        // L'espace initialement prévu n'est pas suffisant
        // On agrandit le tableau  
          taille += TAILLE_LIGNE; 
          if (NULL == (a = realloc (a, taille * sizeof *a))) 
            return (NULL); 
        } 
      if (NULL == (a[longueur] = calloc (p1 - p0 + 1, sizeof **a))) 
        { 
          for (i = 0; i < longueur; i++) 
            free (a[i]); 
          free (a); 
          return (NULL); 
        } 
      memcpy (a[longueur], p0, p1 - p0); 
      p0 = p1 + longueur_separateur; 
      longueur++; 
      } 
  // On stocke la dernière chaîne  
  if (NULL == (a[longueur] = strdup (p0))) 
    { 
      for (i = 0; i < longueur; i++) 
        free (a[i]); 
      free (a); 
      return (NULL); 
    } 
  longueur++; 
  a[longueur] = NULL; 
 
  return (a); 
 
}




