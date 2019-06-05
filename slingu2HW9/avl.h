//
//  avl.h
//  251
//
//  Created by Srinivas Lingutla on 10/14/16.
//  Copyright © 2016 Srinivas Lingutla. All rights reserved.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymem.h"
//****************************************************************************************

//Structures to contain the movie nodes
typedef struct node

{
    char case_id[15];
    char date[40];
    char IUCR[6];
    char arrest[6];
    char domestic[6];
    int community;
    struct node *next;
    
}NODE;


typedef struct hash {
    struct node *head;
}HASH;

typedef struct AVLElementType
{
    char IUCR[6];
    char primary[150];
    char secondary[150];
} AVLElementType;


typedef struct AVLNode
{
    AVLElementType value;
    int height;
    int crimes[100];
    long int count;
    struct AVLNode *left;
    struct AVLNode *right;
}AVLNode;

AVLNode *Contains(AVLNode *root, AVLElementType value);
AVLNode *Insert(AVLNode *root, AVLElementType value);

int Count(AVLNode *root);
int Height(AVLNode *root);

void PrintInorder(AVLNode *root);
