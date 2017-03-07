/**
 *
 * Tree utilities
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tree-utils.h"

/**
 *  We need to include here "sentinel" since we use NIL in this file
 *
 */

extern Node sentinel;

/** 
 *
 *  Saves recursively the contents of each node. This function is called by
 *  saveTree. Do not call directly.
 *
 */

void saveTreeRecursive(Node *x, int sizeDb, FILE *fp)
{
  int len;

  len = strlen(x->data->key);

  fwrite(&len, sizeof(int), 1, fp);
  fwrite(x->data->key, sizeof(char), len, fp);
  fwrite(&(x->data->numFiles), sizeof(int), 1, fp);
  fwrite(x->data->numTimes, sizeof(int), sizeDb, fp); 

  if (x->right != NIL)
    saveTreeRecursive(x->right, sizeDb, fp);

  if (x->left != NIL)
    saveTreeRecursive(x->left, sizeDb, fp);
}

/**
 *
 * Save a tree in a file. Only saves contents of each node, does not save the
 * tree structure itself.
 *
 */

void saveTree(RBTree *tree, char *filename)
{
  FILE *fp;
  
  fp = fopen(filename, "w");
  if (!fp) {
    printf("No he pogut obrir el fitxer.\n");
    return;
  }

  fwrite(&(tree->numNodes), sizeof(int), 1, fp);
  fwrite(&(tree->sizeDb), sizeof(int), 1, fp);

  saveTreeRecursive(tree->root, tree->sizeDb, fp);

  fclose(fp);
}

/**
 *
 * Loads a tree in memory. The file only contains the information stored in
 * each tree, it does not contain the tree structure. The tree structure is
 * created by inserting the nodes within the tree structure.
 *
 */

RBTree *loadTree(char *filename)
{
  int i, len, numNodes;

  FILE *fp;

  RBTree *tree;
  RBData *data;

  fp = fopen(filename, "r");
  if (!fp) {
    printf("No he pogut obrir el fitxer.\n");
    return NULL;
  }

  tree = malloc(sizeof(RBTree));
  initTree(tree);

  fread(&numNodes, 1, sizeof(int), fp);
  fread(&(tree->sizeDb), 1, sizeof(int), fp);

  printf("Nombre de nodes de l'arbre: %d\n", numNodes);
  printf("Nombre de fitxers a la base de dades: %d\n", tree->sizeDb);

  for(i = 0; i < numNodes; i++)
  {
    data = malloc(sizeof(RBData));

    fread(&len, sizeof(int), 1, fp);
    data->key = malloc(sizeof(char) * (len + 1));

    fread(data->key, sizeof(char), len, fp);
    data->key[len] = 0;

    fread(&(data->numFiles), sizeof(int), 1, fp);

    data->numTimes = malloc(sizeof(int) * tree->sizeDb);
    fread(data->numTimes, sizeof(int), tree->sizeDb, fp);

    insertNode(tree, data);
  }

  fclose(fp);

  return tree;
}

/**
 *
 * Plots a histogram using gnuplot
 *
 */

void drawGnuplot(int len, double *vector)
{
  int i;
  char str[2];
  FILE *fp;

  fp = fopen("/tmp/vector.data", "w");
  for(i = 0; i < len; i++)
    fprintf(fp, "%d %f\n", i, vector[i]);
  fclose(fp);

  fp = popen("gnuplot", "w");
  fprintf(fp, "set key off\n");
  fprintf(fp, "plot \"/tmp/vector.data\" with lines\n");
  fflush(fp);
  sleep(2);

  printf("Polsa ENTER per continuar...\n");
  fgets(str, 2, stdin);

  fclose(fp);
}

/**
 * 
 * This function is called by the qsort function. It is used to compare two
 * elements of the vector to order.
 *
 */

int compareDoubles(const void *p1, const void *p2)
{
  double *num1, *num2;

  num1 = (double *) p1;
  num2 = (double *) p2;

  if (*num1 > *num2)
    return -1;
  if (*num1 < *num2)
    return 1;

  return 0;
}


/**
 * 
 * Computes the histogram of word and uses gnuplot
 * to plot the function
 *
 */

void wordProbabilityDistribution(RBTree *tree, char *word)
{
  int i, len;
  double *vector, sum;

  RBData *data;

  data = findNode(tree, word);

  if (!data) {
    printf("La paraula no s'ha trobat a l'arbre\n");
    return;
  }

  len = tree->sizeDb;
  vector = malloc(sizeof(double) * len);

  /* Copy numTimes to vector */

  for(i = 0; i < len; i++)
    vector[i] = (double) data->numTimes[i];

  /* Sort in descending order */

  qsort(vector, len, sizeof(double), compareDoubles);

  /* Normalize */

  sum = 0.0;
  for(i = 0; i < len; i++)
    sum += vector[i];

  for(i = 0; i < len; i++)
    vector[i] /= sum;

  /* And draw it using gnuplot */
     
  drawGnuplot(len, vector);

  free(vector);
}


