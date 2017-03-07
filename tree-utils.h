/**
 *
 * Tree utils 
 *
 *
 */

#ifndef TREE_UTILS_H
#define TREE_UTILS_H

#include "red-black-tree.h"

void saveTree(RBTree *tree, char *filename);
RBTree *loadTree(char *filename);
void wordProbabilityDistribution(RBTree *tree, char *word);

#endif
