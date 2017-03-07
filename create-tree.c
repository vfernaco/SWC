/**
 *
 * Tree creation 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "create-tree.h"

#define HASHSIZE 100

/**
 *  We need to include here "sentinel" since we use NIL in this file
 *
 */

extern Node sentinel;


/**
 *
 * Given a filename, this function read the dictionary words stored in it and
 * creates the tree structure.  Words are assumed to be separated by
 * newlines.
 *
 */

RBTree *processDictionary(char *filename)
{
  int i, len;

  FILE *fp;

  RBTree *tree;
  RBData *treeData;

  char *paraula, str[MAXCHAR];

  /* Allocate memory for tree */
  tree = (RBTree *) malloc(sizeof(RBTree));

  /* Initialize the tree */
  initTree(tree);

  /* Read file */
  fp = fopen(filename, "r");
  if (!fp) {
    printf("Could not open file '%s'\n", filename);
    exit(1);
  }

  while (fscanf(fp, "%s", str) != EOF)
  {
    len = strlen(str);
    paraula = (char *) malloc(sizeof(char) * (len + 1));

    for(i = 0; i < len; i++)
      paraula[i] = tolower(str[i]);

    paraula[i] = '\0';

    treeData = findNode(tree, paraula);

    if (treeData != NULL)
    {
      printf("Hey: I found treeData in the tree.\n");
      exit(1);
    }
    else
    {
      treeData = malloc(sizeof(RBData));
      treeData->key = paraula;
      treeData->numFiles = 0;
      treeData->numTimes = NULL;

      insertNode(tree, treeData);
    }
  }

  fclose(fp);

  /* Done reading file */

  return tree;
}

/**
 *
 * This function returns the hash value for a given string
 *
 */

int getHashValue(char *cadena)
{
  unsigned int i, len, seed, sum, hash;

  len = strlen(cadena); 
  sum = 0;
  seed = 131;
  for(i = 0; i < len; i++)
    sum = sum * seed + (int)cadena[i];

  hash = sum % HASHSIZE;

  return hash;
}

/**
 *
 * Allocates memory for the hashTable. The hash table is made up of a vector
 * of linked lists. Each list is initialized with zero elements.
 *
 */

List *allocHashTable()
{
  int i;
  List *hashTable;

  hashTable = malloc(sizeof(List) * HASHSIZE);
  for(i = 0; i < HASHSIZE; i++)
    initList(&(hashTable[i]));

  return hashTable;
}

/**
 *
 * Deletes the hash table
 *
 */

void deleteHashTable(List *hashTable)
{
  int i;

  for(i = 0; i < HASHSIZE; i++)
    deleteList(&(hashTable[i]));

  free(hashTable);
}

/**
 *
 * Given a filename, this function extracts all the valid words
 * within the file and inserts them in a hash table.
 *
 */

List *processPlainFile(char *filename)
{
  FILE *fp;
  ListData *listData;
  List *hashTable;

  struct stat st;

  char *bytes, paraula[MAXCHAR], *paraula_copy;
  int i, j, len, nbytes, is_word, valor_hash, done;

  printf("Processant fitxer %s\n", filename);

  fp = fopen(filename, "r");
  if (!fp) {
    printf("No s'ha pot obrir el fitxer '%s'\n", filename);
    exit(1);
  }

  /* Observe that we read the whole file at once */

  stat(filename, &st);
  nbytes = st.st_size;

  bytes = malloc(sizeof(char) * nbytes);
  if (!bytes) {
    printf("No he pogut reservar memoria.\n");
    exit(1);
  }

  fread(bytes, sizeof(char), nbytes, fp);
  fclose(fp);

  /* File has been read. Now process the data and extract words. */

  hashTable = allocHashTable();

  i = 0;

  /* Search for the beginning of a word */

  while ((i < nbytes) && (isspace(bytes[i]) || (ispunct(bytes[i]) && (bytes[i] != '\'')))) i++; 

  /* This is the main while that extracts all the words */

  while (i < nbytes)
  {
    j = 0;
    is_word = 1;
    done = 0;

    /* Extract the word including digits if they are present */

    while (!done) {

      if ((isalpha(bytes[i])) || (bytes[i] == '\''))
	paraula[j] = bytes[i];
      else 
	is_word = 0;

      j++; i++;

      /* Check if we arrive to an end of word: space or punctuation character */

      if ((i == nbytes) || (isspace(bytes[i])) || (ispunct(bytes[i]) && (bytes[i] != '\'')))
	done = 1;
    }

    /* If word insert in list */

    if (is_word) {

      /* Put a '\0' (end-of-word) at the end of the string*/
      paraula[j] = 0;

      /* Now transform to lowercase */
      len = j;

      for(j = 0; j < len; j++)
	paraula[j] = tolower(paraula[j]);

      /* Get hash value */
      valor_hash = getHashValue(paraula);

      /* Search if the key is in the tree */
      listData = findList(&(hashTable[valor_hash]), paraula); 

      if (listData != NULL) {

	/* We increment the number of times current item has appeared */
	listData->numTimes++;

      } else {

	/* If the key is not in the list, allocate memory for the data and
	 * insert it in the list */

	paraula_copy = malloc(sizeof(char) * (len+1));
	strcpy(paraula_copy, paraula);

	listData = malloc(sizeof(ListData));
	listData->primary_key = paraula_copy;
	listData->numTimes = 1;

	insertList(&(hashTable[valor_hash]), listData);
      }
    } /* if is_word */

    /* Search for the beginning of a word */

    while ((i < nbytes) && (isspace(bytes[i]) || (ispunct(bytes[i]) && (bytes[i] != '\'')))) i++; 

  } /* while (i < nbytes) */

  free(bytes);

  return hashTable;
}

/**
 *
 *  Function used to initialize the fiels of a tree. Do not call directly. 
 *
 */

void initializeFieldsTreeRecursive(Node *x, int nfiles)
{
  int i;
  RBData *treeData; 

  if (x->right != NIL)
    initializeFieldsTreeRecursive(x->right, nfiles);

  if (x->left != NIL)
    initializeFieldsTreeRecursive(x->left, nfiles);

  treeData = x->data;

  treeData->numTimes = malloc(sizeof(int) * nfiles);

  for(i = 0; i < nfiles; i++)
    treeData->numTimes[i] = 0;

  treeData->numFiles = 0;
}


/**
 *
 *  Initialize the fields of the tree so that the files with the words can be
 *  processed.
 *
 */

void initializeFieldsTree(RBTree *tree, int nfiles)
{
  tree->sizeDb = nfiles;

  if (tree->root != NIL)
    initializeFieldsTreeRecursive(tree->root, nfiles);
}


/**
 *
 * Updates the contents of the tree structure using the hash table
 *
 */

void updateTree(List *hastable, RBTree *tree, int idFile, int numFiles)
{
  RBData *data;
  ListItem *current;

  int i, j, numItems;

  for(i = 0; i < HASHSIZE; i++) {

    numItems = hastable[i].numItems;
    current = hastable[i].first;

    for(j = 0; j < numItems; j++) {

      /* Search if the key is in the tree */
      data = findNode(tree, current->data->primary_key);

      /* We transfer data only if word is in tree */
      if (data != NULL) {

	if (data->numTimes == NULL) {
	  data->numTimes = malloc(sizeof(int) * numFiles);

	  for(i = 0; i < numFiles; i++)
	    data->numTimes[i] = 0;
	}

	data->numFiles++;
	data->numTimes[idFile] = current->data->numTimes;
      } 

      current = current->next;
    }
  }
}

/**
 *
 *  Main function for processing database. Processes each
 *  file, creates local structure and dumps data to tree.
 *
 */ 

void processTextFiles(char *filename, RBTree *tree)
{
  FILE *fp;

  int i, nfiles;
  char line[MAXCHAR];

  List *hashTable;

  fp = fopen(filename, "r");
  if (!fp) {
    printf("No s'ha pogut obrir el fitxer '%s'\n", filename);
    exit(1);
  }

  /* Get number of files to process */

  fgets(line, MAXCHAR, fp);
  nfiles = atoi(line);

  if (nfiles < 1) {
    printf("El nombre d'arxius al fitxer '%s' no es correcte.\n", filename);
    fclose(fp);
    exit(1);
  }

  /* Initialize remaining fiels of the tree */

  initializeFieldsTree(tree, nfiles);

  /* Process files */
//#pragma omp parallel for schedule(static)
//Esto no, ejemplo de luis. Asigno las primeras iteraciones a un hilo, y las siguientes 
//las asignas al otro hilo, imaginad que el primer hilo hace el primer fgets, leera la //primera linea del fichero y ejecutara la linea, el otro hilo (en la iteracion 300) 
//cogerá y ejecutara la segunda linea del fichero, y en realidad tendria que coger el //fichero 300. Antes de comenzar a paralelizar, hay que leer todas las lineas del 
//fichero antes de comenzar a planificar. Con la planificacion estatica si coges el 
//fichero ordenado, habria que notar diferencia con una planificacion dinamica.

//Las otras dos pruebas, son cosas equivalentes con planificaciones de la tarea. En la //primera solucion seria, hacer una especie de #pragma omp task, leer el fichero, y 
//hacer esa secuencia antes del hashtable = processPlainFile(line) que será toda la 
//tarea. Si hago eso paso la linea como parametro a la tarea. Si lo hacemos asi, cada tasca accedera a disco en paralelo. Tambien estaria bien ver que diferencia hay al ejecutar diferentes hilos que tienen que acceder a disco en paralelo. 
//seguro que reduce la eficiencia.

//Tarea 3, nosotros leemos todo el fichero y pasamos como parametro el fichero a la tarea.

//Como leer el fichero entero antes de hacer nada? esta implementado en el processplainfile, se abre el fichero a traves del "stat()" se eobtienen los bytes del fichero, se reserva memoria para todos los bytes del fichero, y con el fread se lee todo el fichero entero. Finalmente, a traves del while se empieza el procesamiento del fichero, de extraer las palabras.

//Asegurarse que cuando se accede al arbol, solo una tarea lo modifique. Sección critica. En la teoria hay Construcción de regio critica, #pragma omp critical que implementa la seccion critica por tanto de asegurar que aquel trozo de código solo acceda al mismo tiempo un hilo

//Hay que hacer que la primera parte del programae sté asociada a un hilo,
//y que el resto de tareas se vayan ejecutando por diferentes hilos.

//for i in {1..3}; do ((time -p ./main < opcions.txt) 2>> fitxer.txt); done
//rm -r fitxer.txt; for i in {1..3}; do ((time -p ./main < opcions.txt) 2>> fitxer.txt); done

//Dinamica es mas rapida que estatica por defecto, el tamaño de hashsize afecta mucho en el tiempo de ejecucion, uno de 100 esta limitando la lista de hash, si se utiliza una hash mas grande influye mas el si es dinamico o estatico

  for(i = 0; i < nfiles; i++) {
    /* Read line */

    fgets(line, MAXCHAR, fp);
    line[strlen(line)-1]=0;

    /* Process file */

    hashTable = processPlainFile(line);

    /* Copy to tree */

    updateTree(hashTable, tree, i, nfiles);

    /* Delete hashTable */

    deleteHashTable(hashTable);
  }

  fclose(fp);
}

/**
 *
 * Main function. It accepts a file as command line argument. This file
 * contains a list with the files to be processed.
 *
 */

RBTree *createTree(char *words, char *dbfile)
{
  RBTree *tree;

  /* Read dictionary words and generate tree */

  tree = processDictionary(words);

  /* Process plain text files */

  processTextFiles(dbfile, tree);

  return tree;
}



