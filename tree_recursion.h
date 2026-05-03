#ifndef TREERECURSION_H
#define TREERECURSION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
    int day, month, year;
} Date;

typedef struct {
    char fullName[50];      
    char definition[200];   
    Date birth;             
    Date death;             
} Person;

typedef struct {
    char eventName[100];
    Date eventDate;
} Event;

typedef struct {
    Person person;           
    struct personNode* next;     
} personNode, *TStack, *TList;

typedef struct {
    Person person;
    struct personTree* left;
    struct personTree* right;
} personTree, *TTree;

typedef struct {
    Event event;           
    eventNode* next;     
}eventNode;

typedef struct 
{
    Event event;
    eventTree* left;
    eventTree* right;
}eventTree;


TTree insertInBST(TTree root, Person p);
TTree toTree(TStack *stk);
TTree fillTree(FILE *f);
TTree getInfoNameTree(TTree tr, char *name);
TTree addNameBST(TTree tr, char *name, char *DoB, char *DoD);
TTree deleteNameBST(TTree root, char *name);
TTree updateNameBST(TTree tr, char *name, char *s, char *DoB, char *DoD);

void traversalBSTinOrder(TTree tr);
void traversalBSTpreOrder(TTree tr);
void traversalBSTpostOrder(TTree tr);

int height(TTree tr);
int size(TTree tr);
void heightSizeBST(TTree tr);
bool isBalancedBST(TTree tr);

TTree lowestCommonAncestor(TTree tr, char *word1, char *word2);
TTree inOrderSuccessor(TTree tr, char *word);
TTree BSTMirror(TTree tr);
int countNodesRange(TTree tr, int l, int h);

TTree findMin(TTree root);
void storeInOrder(TTree tr, Person nodes[], int *index);
TTree buildBalancedTree(Person nodes[], int start, int end);
TTree BTSMerge(TTree tr1, TTree tr2);

Date* stringToDate(char* string);

// _________________---------------------------------------------------_______________________


int countOccurence(FILE *f, char *name);
void recursiveFilter(FILE *src, FILE *dest, char *word);
FILE* removeOccurence(FILE *f, char *word);
void recursiveReplace(FILE *src, FILE *dest, char *name, char *DoB, char *DoD);
FILE* replaceOccurence(FILE *f, char *name, char *DoB, char *DoD);

void swap(char *x, char *y);
void permute(char *a, int l, int r);
void namePermutation(char *name);
void generateSubseq(char *word, char *current, int index, int currIndex);
void subseqName(char *word);

int extractYear(char *dateStr);
void longestSubyearRecursive(FILE *f, int y1, int y2);
void longestSubyear(char *date1, char *date2);

int countDistinct(char *str, int n, int lastPos[]);
int distinctSubseqWord(char *event);
int getYearOnly(char *dateStr);
bool checkOverlapRecursive(FILE *f, int eventYear);
bool isPalindromeWord(char *event);

#endif 