/**
 *
 * Main file 
 * 
 * Lluis Garrido, 2015.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "create-tree.h"
#include "tree-utils.h"

/**
 *
 *  Displays menu to screen. Returns selected option.
 *
 */

int menu() 
{
  char str[5];
  int opcio;

  printf("\n\nMenu\n\n");
  printf(" 1 - Creacio de l'arbre\n");
  printf(" 2 - Emmagatzemar arbre a disc\n");
  printf(" 3 - Llegir arbre de disc\n");
  printf(" 4 - Grafica de distributio de probabilitat d'una paraula\n");
  printf(" 5 - Sortir\n\n");
  printf("   Escull opcio: ");

  fgets(str, 5, stdin);
  opcio = atoi(str); 

  return opcio;
}

/**
 * 
 *  Main procedure
 *
 */

int main(int argc, char **argv)
{
  char filename[MAXCHAR], filename2[MAXCHAR];
  int opcio;

  RBTree *tree;

  if (argc != 1)
    printf("Opcions de la linia de comandes ignorades\n");

  tree = NULL;

  do {
    opcio = menu();
    printf("\n\n");

    switch (opcio) {
      case 1:
	if (tree) {
	  printf("Alliberant arbre actual\n");
	  deleteTree(tree);
	  free(tree);
	}

	printf("Introdueix fitxer que conte diccionari: ");
	fgets(filename2, MAXCHAR, stdin);

	filename2[strlen(filename2)-1]=0;

	printf("Introdueix fitxer amb base de dades: ");
	fgets(filename, MAXCHAR, stdin);

	filename[strlen(filename)-1]=0;

	printf("Creant arbre...\n");
	tree = createTree(filename2, filename);
	break;

      case 2:
	if (!tree) {
	  printf("No hi ha cap arbre a memoria\n");
	  break;
	}

	printf("Introdueix el nom de fitxer en el qual es desara l'arbre: ");
	fgets(filename, MAXCHAR, stdin);

	filename[strlen(filename)-1]=0;

	printf("Desant arbre...\n");
	saveTree(tree, filename);
	break;

      case 3:
	if (tree) {
	  printf("Alliberant arbre actual\n");
	  deleteTree(tree);
	  free(tree);
	}

	printf("Introdueix nom del fitxer amb l'arbre: ");
	fgets(filename, MAXCHAR, stdin);

	filename[strlen(filename)-1]=0;

	printf("Llegint arbre de disc...\n");
	tree = loadTree(filename);
	break;

      case 4:

	if (tree) {

	  printf("Introdueix la paraula: ");
	  fgets(filename, MAXCHAR, stdin);

	  filename[strlen(filename)-1]=0;

	  if (strlen(filename) == 0) {
	    printf("No s'ha introduit cap paraula\n");
	    break;
	  }

	  wordProbabilityDistribution(tree, filename);
	} else 
	  printf("No hi ha arbre carregat a memoria.\n");

	break;

      case 5:
	if (tree) {
	  printf("Alliberant memoria\n");
	  deleteTree(tree);
	  free(tree);
	}
	break;

      default:
	printf("Opcio no valida\n");

    } /* switch */
  }
  while (opcio != 5);

  return 0;
}


