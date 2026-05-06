#ifndef HISTORY_H
#define HISTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


typedef struct TList{

    char name[40];
    char difinition[400];
    
     struct date{

        char birth[10];
        char death[10];
    }date;

    char dateB[10];
    char dateE[10];
    int type;   // type = 0 for personality

               // type  = 1 for events


    struct TList *next;
    struct TList *prv;
    

}TList;

typedef struct TQueue{

 TList *head;
 TList *tail;

}TQueue;

typedef struct TStack{

   struct TStack *link;
    char name[40];
    char definition[400];
    char birth[10];
    char death[10];
    char dateB[10];
    char dateE[10];

    int type;



}TStack;



/* ===================== FUNCTIONS ===================== */

/* Stack functions */
int countWords(char *name);

TStack* toStack(TList *merged);
TStack* getInfoPersonality(TStack *stk, char *name);
TStack* sortNameStack(TStack *s);
TStack* deleteName(TStack *stk, char *name);
TStack* updateStack(TStack *stk, char *name, char *def, char *DoB, char *DoD);
TStack* stackToQueue(TStack *stk);
TList*  stackToList(TStack *stk);
TStack* addNameStack(TStack *stk, char *name, char *definition, char *DoB, char *DoD);
TStack* definitionStack(TStack *stk);
char*   getSmallest(TStack *stk);
void    to_lowercase(char *word);
bool    isPersonalityKilled(char *word);
TStack* sortbydate(TStack *stk);
TStack* sort_difinition_len_Stack(TStack *s);
TStack* pronunciationStack(TStack *stk);
void    continuousSearch(TStack *head);
int     get_size(TStack *stk);
TStack* recRevStack(TStack *stk);

/* List functions */
TList* getPersonality(FILE *f);
TList* getDatePersonality(FILE *f);

void getInfoByDates(TList *s, TList *DoB);
void getInfoByDates2(TList *s, TList *DoD);

TList* sortWord(TList *head);
TList* sortWord2(TList *head);
TList* sortPersonality(TList *head);
TList* similarPersonality(TList *s, char *word);
TList* countPersonality(TList *s, char *dateB, char *dateE);
int    is_pallindrome(char *word);
TList* insert_sorted(TList *l, char *word);
TList* palindromeName(TList *s);
TList* mergeNodes(TList *s, TList *a);
TList* merge2Nodes(TList *s, TList *a);
void   addPersonality(TList **s, TList **a, char *name, char *DoB, char *DoD);
void   addEvents(TList **b, char *namEvente, char *date);

/* Queue functions */
void    insert_in_sorted_order(TQueue *queue, char *w);
void    insert_in_sorted_order_age(TQueue *queue, char *w, int age);
TQueue* sName(TList *s);
TQueue* ageP(TList *a);
TQueue* toQueue(TList *merged);

/* File operations */
void delete_from_file(const char *fileName, const char *name);
void deletepersonality(FILE *f, TList **s, TList **a, char *name);
void updateInFile(const char *filename, const char *name,
                  const char *definition, const char *DoB, const char *DoD);
void updatePersonality(FILE *f, TList **s, TList **a,
                       char *name, char *definition, char *DoB, char *DoD);

#endif