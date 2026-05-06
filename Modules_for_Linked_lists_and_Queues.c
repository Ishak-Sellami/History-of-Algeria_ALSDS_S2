#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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

int countWords(char *name){
    int count = 1;

    for (int i = 0; name[i] != '\0'; i++) {
        if (name[i] == ' ')
            count++;
    }

    return count;
}

TStack* toStack(TList *merged){

    TList *ptr = merged;
    TStack *top = NULL;


    while(ptr){

        TStack *newnode = malloc(sizeof(TStack));
        
        strcpy(newnode->name,ptr->name);
        strcpy(newnode->birth,ptr->date.birth);
        strcpy(newnode->death,ptr->date.death);
        strcpy(newnode->definition,ptr->difinition);

        newnode->link = top;
        top = newnode;

        ptr = ptr->next;
    }

    return top;
}

TStack* getInfoPersonality(TStack *stk, char *name){

    TStack *ptr = stk;
    
      while(ptr){

        if(strcmp(ptr->name,name) == 0){

            return ptr;

        }
        ptr = ptr->link;

    }

    return NULL;

}

TStack* sortNameStack(TStack *s){

    if (!s) return NULL;

    TStack *i, *j;
    char tempName[40], tempDef[400], tempBirth[10], tempDeath[10];

    for(i=s ; i!=NULL ; i = i->link){

        for(j=i->link ; j!=NULL ; j = j->link){

              if(i->type ==  0 && j->type == 0 && strcmp(i->name, j->name) > 0){

                strcpy(tempName,i->name); strcpy(i->name,j->name); strcpy(j->name,tempName);
                
                strcpy(tempDef,i->definition); strcpy(i->definition,j->definition); strcpy(j->definition,tempDef);
                
                strcpy(tempBirth,i->birth); strcpy(i->birth,j->birth); strcpy(j->birth,tempBirth);
              
                strcpy(tempDeath,i->death); strcpy(i->death,j->death); strcpy(j->death,tempDeath);
            }



        }



    }
 return s;
}

TStack* deleteName(TStack *stk, char *name){ 


    if(!stk) return NULL;

    TStack *ptr = stk;
    TStack *prv = NULL;

    if (strcmp(ptr->name,name) == 0){
        stk = stk->link;
        free(ptr);
        ptr = NULL;
        return stk;
    }

    while (ptr != NULL && strcmp(ptr->name, name) != 0) {
        prv = ptr;
        ptr = ptr->link;
    }

   
    if (ptr != NULL) {
        prv->link = ptr->link;
        free(ptr);
    }


    return stk;
}

TStack* updateStack(TStack *stk, char *name, char *def, char *DoB, char *DoD){

  TStack *ptr = stk;
  
    while(ptr){ 

     if(strcmp(ptr->name,name) == 0){ 
       strcpy(ptr->birth,DoB);
       strcpy(ptr->death,DoD);
       strcpy(ptr->definition,def);
    }

    ptr = ptr->link;
}

    return stk;

}

TQueue* stackToQueue(TStack *stk){

    TStack *ptr = sortNameStack(stk);

    TQueue *queue = malloc(sizeof(TQueue));
    queue->head = NULL;
    queue->tail = NULL;
    
    while(ptr){
        TList *temp = malloc(sizeof(TList));
        strcpy(temp->name,ptr->name);
        strcpy(temp->date.birth,ptr->birth);
        strcpy(temp->date.death,ptr->death);
        strcpy(temp->difinition,ptr->definition);

        if(!queue->head){
            queue->head = temp;
            queue->tail = temp;
        }else{

            temp->next = queue->head;
            queue->head = temp;
        }
        ptr = ptr->link;
    }


    return queue;

}

TList* stackToList(TStack *stk){

    TStack *ptr = stk;
    
    TList *head = NULL;
    TList *tail = NULL;

    if (!stk) return NULL;

    while(ptr){

        TList *newnode = malloc(sizeof(TList));
        strcpy(newnode->name,ptr->name);
        strcpy(newnode->difinition,ptr->definition);
        strcpy(newnode->date.birth,ptr->birth);
        strcpy(newnode->date.death,ptr->death);
        newnode->next = NULL;

        if(!head){ 
        head = newnode;
        tail = newnode;
        }
        else{

            tail->next = newnode;
            tail = newnode;
        }

        ptr = ptr->link;

    }

    return head;
}

TStack* addNameStack(TStack *stk, char *name, char *definition, char *DoB, char *DoD){ 

    TStack *newnode = malloc(sizeof(TStack));
    newnode->link = NULL;
    strcpy(newnode->name,name);
    strcpy(newnode->definition,definition);
    strcpy(newnode->birth,DoB);
    strcpy(newnode->death,DoD);

    if (!stk){  stk = newnode;
    return stk;
    }

    if(strcmp(stk->name,newnode->name) > 0){

        newnode->link = stk;
        stk = newnode;
        return stk;
    }

    TStack *ptr = stk;
    TStack *prv = NULL;

    while(ptr->link != NULL && strcmp(ptr->link->name,newnode->name) < 0){

        prv = ptr;
        ptr = ptr->link;
    }

    prv->link = newnode;
    newnode->link = ptr;

    return stk;
}

TStack* definitionStack(TStack *stk){

        if (!stk) return NULL;

    TStack *i, *j;
    char tempName[40], tempDef[400], tempBirth[10], tempDeath[10];

    for(i=stk ; i!=NULL ; i = i->link){

        for(j=i->link ; j!=NULL ; j = j->link){

              if(countWords(i->definition) > countWords(j->definition)){

                strcpy(tempName,i->name); strcpy(i->name,j->name); strcpy(j->name,tempName);
                
                strcpy(tempDef,i->definition); strcpy(i->definition,j->definition); strcpy(j->definition,tempDef);
                
                strcpy(tempBirth,i->birth); strcpy(i->birth,j->birth); strcpy(j->birth,tempBirth);
              
                strcpy(tempDeath,i->death); strcpy(i->death,j->death); strcpy(j->death,tempDeath);
            }



        }



    }

    return stk;

}

char* getSmallest(TStack *stk){

    TStack *ptr = stk;
    TStack *node = stk;

    while(ptr){  

        if(strlen(node->definition) > strlen(ptr->definition)){

            node = ptr;
            
        }
        ptr = ptr->link;

    }

    return node->definition;

}

void to_lowercase(char *word){

    for(int i = 0 ; i < strlen(word) ; i++){

        word[i] = tolower(word[i]);

    }

}

bool isPersonalityKilled(char *word){


    char temp[500];
    strcpy(temp,word);

    to_lowercase(temp);


    char *keywords[] = {
        "killed",
        "assassinated",
        "murdered",
        "executed",
        "shot dead"
    };

    int n = sizeof(keywords) / sizeof(keywords[0]);

    for(int i = 0 ;i < n ; i++){

        if (strstr(temp, keywords[i]) != NULL) {
            return true;
        }

    }


 return false;

}
 
TStack* sortbydate(TStack *stk){

        if (!stk) return NULL;

    TStack *i, *j;
    char tempName[40], tempDef[400], tempBirth[10], tempDeath[10];

    for(i=stk ; i!=NULL ; i = i->link){

        for(j=i->link ; j!=NULL ; j = j->link){

              if(strcmp(i->birth, j->birth) > 0){

                strcpy(tempName,i->name); strcpy(i->name,j->name); strcpy(j->name,tempName);
                
                strcpy(tempDef,i->definition); strcpy(i->definition,j->definition); strcpy(j->definition,tempDef);
                
                strcpy(tempBirth,i->birth); strcpy(i->birth,j->birth); strcpy(j->birth,tempBirth);
              
                strcpy(tempDeath,i->death); strcpy(i->death,j->death); strcpy(j->death,tempDeath);
            }
        }
    }
    return stk;
}

TStack* sort_difinition_len_Stack(TStack *s){

    if (!s) return NULL;

    TStack *i, *j;
    char tempName[40], tempDef[400], tempBirth[10], tempDeath[10];

    for(i=s ; i!=NULL ; i = i->link){

        for(j=i->link ; j!=NULL ; j = j->link){

              if(strcmp(i->definition, j->definition) > 0){

                strcpy(tempName,i->name); strcpy(i->name,j->name); strcpy(j->name,tempName);
                
                strcpy(tempDef,i->definition); strcpy(i->definition,j->definition); strcpy(j->definition,tempDef);
                
                strcpy(tempBirth,i->birth); strcpy(i->birth,j->birth); strcpy(j->birth,tempBirth);
              
                strcpy(tempDeath,i->death); strcpy(i->death,j->death); strcpy(j->death,tempDeath);
            }



        }



    }
 return s;
}

TStack* pronunciationStack(TStack *stk){

    stk = sort_difinition_len_Stack(stk);

    return stk;
}

void continuousSearch(TStack *head){
    if (head == NULL) return;

     sortbydate(head);

    TStack *current = head;

    printf("Groups:\n");

    while (current != NULL) {

       
        printf("\n[ %s", current->name);

        int currentEnd = atoi(current->death);

        TStack *next = current->link;

        while (next != NULL) {

            int nextStart = atoi(next->birth);

           
            if (nextStart <= currentEnd) {

                printf(", %s", next->name);

                int nextEnd = atoi(next->death);

                if (nextEnd > currentEnd)
                    currentEnd = nextEnd;

                next = next->link;
            } else {
                break;
            }
        }

        printf(" ]\n");

        current = next;  
    }
}

int get_size(TStack *stk){

    int size = 0;

    TStack *ptr = stk;

    while(ptr){
        size++;
        ptr = ptr->link;
    }

    return size;
}

TStack* recRevStack(TStack *stk){

    if (stk == NULL || stk->link == NULL)
        return stk;

    TStack *newTop = recRevStack(stk->link);

    stk->link->link = stk;
    stk->link = NULL;

    return newTop;
}

//----====================Linked list and Queues=================----//

TList* getPersonality(FILE *f){

    TList *head = NULL;
    TList *tail = NULL;

    char name[40];
    char definition[400];
    char line[500];
    char birth[10];
    char death[10];

    while (fgets(line,sizeof(line),f))
    {


        if (strchr(line, '='))   
        {
            sscanf(line,"%39[^=]=%399[^{]{",name,definition);

            TList *newnode = malloc(sizeof(TList));
 
            newnode->type = 0;
            strcpy(newnode->name,name);
            strcpy(newnode->difinition,definition);
            newnode->next = NULL;

            if(head == NULL){
                head = newnode;
                tail = newnode;
            }
            else{
                tail->next = newnode;
                tail = newnode;
            }
        }
    }

    return head;
}

TList* getDatePersonality(FILE *f){

    TList *head = NULL;
    TList *tail = NULL;

    char birth[10];
    char death[10];
    char name[100];
    char line[500];

    while(fgets(line, sizeof(line), f)){

        char *p = strchr(line, '{');
        char *eq = strchr(line, '=');

        if(p != NULL && eq != NULL && strchr(p, '-') != NULL){

            int len = eq - line;
            strncpy(name, line, len);
            name[len] = '\0';

            if (sscanf(p, "{%9[^-]-%9[^}]}", birth, death) == 2){ 

                TList *newnode = malloc(sizeof(TList));

                newnode->type = 0;
                strcpy(newnode->name, name);
                strcpy(newnode->date.birth, birth);
                strcpy(newnode->date.death, death);


                newnode->next = NULL;

                if(head == NULL){
                    head = newnode;
                    tail = newnode;
                }
                else{
                    tail->next = newnode;
                    tail = newnode;
                }
            }
        }
    }

    return head;
}

void getInfoByDates(TList *s, TList *DoB){

    while(s){

     if (strcmp(s->date.birth,DoB->date.birth) == 0){

        printf("Name: %s\n", s->name);
        printf("date of birth: %s , date of death: %s\n",s->date.birth,s->date.death);
        printf("Info: %s\n\n", s->difinition);

        break;
     }
     s = s->next;


    }

}

void getInfoByDates2(TList *s, TList *DoD){

 while(s){

     if (strcmp(s->date.death,DoD->date.death) == 0){

        printf("Name: %s\n", s->name);
        printf("date of birth: %s , date of death: %s\n",s->date.birth,s->date.death);
        printf("Info: %s\n\n", s->difinition);

        break;
     }
     s = s->next;


    }

}

TList* sortWord(TList *head){

    if(!head) return NULL;

    TList *i, *j;
    char tempName[40], tempDef[400], tempBirth[10], tempDeath[10];

    for(i=head; i!=NULL; i=i->next){

        for(j=i->next; j!=NULL; j=j->next) {
            if( i->type == 0 && j->type == 0 &&  strcmp(i->name, j->name) > 0) {
        
                strcpy(tempName,i->name); strcpy(i->name,j->name); strcpy(j->name,tempName);
                
                strcpy(tempDef,i->difinition); strcpy(i->difinition,j->difinition); strcpy(j->difinition,tempDef);
                
                strcpy(tempBirth,i->date.birth); strcpy(i->date.birth,j->date.birth); strcpy(j->date.birth,tempBirth);
              
                strcpy(tempDeath,i->date.death); strcpy(i->date.death,j->date.death); strcpy(j->date.death,tempDeath);
            }
        }
    }

    return head;
}

TList* sortWord2(TList *head){
    if(!head) return NULL;

    int size1 = 0  , size2 = 0;

    TList *i, *j;
    char tempName[40], tempDef[400], tempBirth[10], tempDeath[10];

    for(i=head; i!=NULL; i=i->next) {
        for(j=i->next; j!=NULL; j=j->next) {

            size1 = strlen(i->name) + strlen(i->difinition);
            size2 = strlen(j->name) + strlen(j->difinition);

            if( size1 > size2) {
        
                strcpy(tempName,i->name); strcpy(i->name,j->name); strcpy(j->name,tempName);
                
                strcpy(tempDef,i->difinition); strcpy(i->difinition,j->difinition); strcpy(j->difinition,tempDef);
                
                strcpy(tempBirth,i->date.birth); strcpy(i->date.birth,j->date.birth); strcpy(j->date.birth,tempBirth);
              
                strcpy(tempDeath,i->date.death); strcpy(i->date.death,j->date.death); strcpy(j->date.death,tempDeath);
            }
        }
    }
    return head;
}

TList* sortPersonality(TList *head){

 if(!head) return NULL;

    int age1_1 = 0  , age2_1 = 0;
    

    TList *i, *j;
    char tempName[40], tempDef[400], tempBirth[10], tempDeath[10];

    for(i=head; i!=NULL; i=i->next) {
        for(j=i->next; j!=NULL; j=j->next) {

            if(i->type == 0 && j->type == 0){ 

           age1_1 = atoi(i->date.death) - atoi(i->date.birth);
           age2_1 = atoi(j->date.death) - atoi(j->date.birth);

            if (age1_1 > age2_1) {
        
                strcpy(tempName,i->name); strcpy(i->name,j->name); strcpy(j->name,tempName);
                
                strcpy(tempDef,i->difinition); strcpy(i->difinition,j->difinition); strcpy(j->difinition,tempDef);
                
                strcpy(tempBirth,i->date.birth); strcpy(i->date.birth,j->date.birth); strcpy(j->date.birth,tempBirth);
              
                strcpy(tempDeath,i->date.death); strcpy(i->date.death,j->date.death); strcpy(j->date.death,tempDeath);
            }
        }

    }

    }
    return head;

}

TList* similarPersonality(TList *s, char *word){

 TList *ptr = s;
 TList *head = NULL;
 TList *tail = NULL;

    while(ptr){

        if(ptr->type == 0 && ( strcmp(ptr->date.birth,word) == 0 || strcmp(ptr->date.death,word) == 0)){
            TList *newnode = malloc(sizeof(TList));

            newnode->type = 0;

             strcpy(newnode->name,ptr->name);
             strcpy(newnode->difinition,ptr->difinition);
             strcpy(newnode->date.birth,ptr->date.birth);
             strcpy(newnode->date.death,ptr->date.death);


             newnode->next = NULL;



             if (head == NULL) {
                head = newnode;
                tail = newnode;
            }else{
                tail->next = newnode;
                tail = newnode;
            }
           

        }
        ptr = ptr->next;

    }

    return head;

}

TList* countPersonality(TList *s ,char *dateB , char *dateE){

if(!s) return NULL;

 TList *temp = s;
 TList *head = NULL;
 TList *tail = NULL;

while(temp){

    TList *newnode = NULL;

    if(temp->type == 0 && (strcmp(temp->date.birth,dateB) == 0 || strcmp(temp->date.death,dateE) == 0)){

        newnode = malloc(sizeof(TList));

        if (!newnode) return head;

        newnode->type = 0;
        strcpy(newnode->name,temp->name);
        strcpy(newnode->difinition,temp->difinition);
        strcpy(newnode->date.birth,temp->date.birth);
        strcpy(newnode->date.death,temp->date.death);

           newnode->next = NULL;
            newnode->prv = NULL;

    }else if (temp->type == 1 && (strcmp(temp->dateB,dateB) == 0 || strcmp(temp->dateE,dateE) == 0)){

        newnode = malloc(sizeof(TList));

        if (!newnode) return head;


        newnode->type = 1;
        strcpy(newnode->name,temp->name);
        strcpy(newnode->difinition,temp->difinition);
        strcpy(newnode->dateB,temp->dateB);
        strcpy(newnode->dateE,temp->dateE);

           newnode->next = NULL;
            newnode->prv = NULL;

    }

     if (newnode != NULL)
        {
            if (head == NULL)
            {
                head = newnode;
                tail = newnode;
            }
            else
            {
                tail->next = newnode;
                newnode->prv = tail;
                tail = newnode;
            }
        }

        temp = temp->next;
    }
  
   return head;
}

int is_pallindrome(char *word){

  int j = strlen(word) - 1;
  int i = 0;

  while( i < j){

    if (tolower(word[i]) != tolower(word[j]))
    return 0;
    i++;
    j--;
  }

  return 1;
}

TList* insert_sorted(TList *l , char *word){ 

    TList *newnode = malloc(sizeof(TList));
    strcpy(newnode->name, word);


 TList *temp = l;

 if (l == NULL || strcmp(word,l->name) < 0){ 
       newnode->next = l;
    return newnode;
 }

 while (temp->next && strcmp(word, temp->next->name) > 0)
 temp = temp->next;

 newnode->next = temp->next;
 temp->next = newnode;

 return l;

}

TList* palindromeName(TList *s) {

    TList *result = NULL;
    TList *p = s;

    while (p != NULL) {

        char temp[200];
        strcpy(temp, p->difinition); 

        char *word = strtok(temp, " ,.-");

        while (word != NULL) {

            if (is_pallindrome(word)) {
                result = insert_sorted(result, word);
            }

            word = strtok(NULL, " ,.-");
        }

        p = p->next;
    }

    return result;
}

TList* mergeNodes(TList *s, TList *a){

    TList *ptr = s;
    TList *ptr1 = a;
    
    TList *newnode = NULL;

    TList *head = NULL;
    TList *tail = NULL;

    while(ptr){
 

        newnode = NULL;
        ptr1 = a;

        while(ptr1){

            if(strcmp(ptr->name,ptr1->name) == 0){
                newnode = malloc(sizeof(TList));
                if(!newnode)
                return head;

                newnode->next = NULL;
                newnode->prv = NULL;

                strcpy(newnode->name,ptr->name);
                strcpy(newnode->difinition,ptr->difinition);

                strcpy(newnode->date.birth,ptr1->date.birth);
                strcpy(newnode->date.death,ptr1->date.death);

                break;
            }

           ptr1 = ptr1->next;

        }

        if(newnode != NULL){
        if(!head){
            head = newnode;
            tail = newnode;
        }
        else{ 
        tail->next = newnode;
        newnode->prv = tail;
        tail = newnode;
        }


    }
    ptr = ptr->next;
    }

    return head;
}

TList* merge2Nodes(TList *s, TList *a)
{
    TList *head = NULL;
    TList *tail = NULL;

    TList *ptr = s;

    while (ptr)
    {
        TList *ptr1 = a;
        TList *newnode = NULL;

        while (ptr1)
        {
            if (strcmp(ptr->name, ptr1->name) == 0)
            {
                newnode = malloc(sizeof(TList));
                if (!newnode) return head;

                strcpy(newnode->name, ptr->name);
                strcpy(newnode->difinition, ptr->difinition);

                strcpy(newnode->date.birth, ptr1->date.birth);
                strcpy(newnode->date.death, ptr1->date.death);

                newnode->next = NULL;
                newnode->prv = NULL;

                break;
            }

            ptr1 = ptr1->next;
        }

        if (newnode)
        {
            if (!head)
            {
                head = newnode;
                tail = newnode;
            }
            else
            {
                tail->next = newnode;
                newnode->prv = tail;
                tail = newnode;
            }
        }

        ptr = ptr->next;
    }

    
    if (head && tail)
    {
        tail->next = head;
        head->prv = tail;
    }

    return head;
}

void addPersonality(TList **s, TList **a, char *name, char *DoB, char *DoD){


    //--------add name-------------//

 TList *newnode = malloc(sizeof(TList));

 newnode->next = NULL;
 newnode->type = 0;
 strcpy(newnode->name,name);
 strcpy(newnode->date.birth,DoB);
 strcpy(newnode->date.death,DoD);

 if(*s == NULL){ 
    *s = newnode;
 }else{

    TList *ptr = *s;

    while(ptr->next)
    ptr = ptr->next;

    ptr->next = newnode;

 }

    //-------add date-------------//

  if(*a == NULL){ 
    *a = newnode;
 }else{

    TList *ptr1 = *a;

    while(ptr1->next)
    ptr1 = ptr1->next;

    ptr1->next = newnode;

 }

 FILE *f = fopen("Algeria_history.txt", "a");
    if (f) {
        fprintf(f, "%s={%s-%s}\n", name, DoB, DoD);
        fclose(f);
    }



}

void addEvents(TList **b, char *namEvente, char *date){

    TList *newnode = malloc(sizeof(TList));
    
    newnode->next = NULL;
    strcpy(newnode->name,namEvente);
    strcpy(newnode->dateB,date);

    if(*b == NULL) *b = newnode;
    else{

        TList *ptr = *b;

        while(ptr->next)
        ptr = ptr->next;

        ptr->next = newnode;
    }

    FILE *f = fopen("Algeria_history.txt", "a");

    if (f) {
        fprintf(f, "%s:{%s}\n", namEvente,date);
        fclose(f);
    }

}

void insert_in_sorted_order(TQueue *queue,char *w){

    TList *newnode = malloc(sizeof(TList));

    strcpy(newnode->name,w);

    int Wcounter = countWords(w); 
    
    if (queue->head == NULL){
        queue->head = newnode;
        queue->tail = newnode;
    }else{

        if(Wcounter < countWords(queue->head->name)){
            newnode->next = queue->head;
            queue->head = newnode;
            return;
        }

        TList *ptr = queue->head;

        while( ptr->next != NULL &&  Wcounter > countWords(ptr->name))
        ptr = ptr->next;

        newnode->next = ptr->next;
        ptr->next = newnode;

        if(newnode->next == NULL)
        queue->tail = newnode;

    }

}

void insert_in_sorted_order_age(TQueue *queue,char *w , int age){

    TList *newnode = malloc(sizeof(TList));

     strcpy(newnode->name,w);
    
    if (queue->head == NULL){
        queue->head = newnode;
        queue->tail = newnode;
    }else{

        if(age < atoi(queue->head->date.death) - atoi(queue->head->date.birth)){
            newnode->next = queue->head;
            queue->head = newnode;
            return;
        }

        TList *ptr = queue->head;

        while( ptr->next != NULL && age > atoi(ptr->date.death) - atoi(ptr->date.birth))
        ptr = ptr->next;

        newnode->next = ptr->next;
        ptr->next = newnode;

        if(newnode->next == NULL)
        queue->tail = newnode;

    }

}

TQueue* sName(TList *s){

    TQueue *queue = malloc(sizeof(TQueue));
    queue->head = NULL; queue->tail = NULL;

    TList *temp = s;

    while(temp != NULL){

        insert_in_sorted_order(queue,temp->name);
        temp = temp->next;
    }


    return queue;


}

TQueue* ageP(TList *a){

    TQueue *queue2 = malloc(sizeof(TQueue));
    queue2->head = NULL; queue2->tail = NULL;

    TList *temp = a;

    while(temp != NULL){

        insert_in_sorted_order_age(queue2,temp->name,atoi(temp->date.death) - atoi(temp->date.birth));
        temp =temp->next;
    }

    return queue2;
}

TQueue* toQueue(TList *merged){

    TQueue *queue = malloc(sizeof(TQueue));
    
    queue->head = NULL;
    queue->tail = NULL;

    TList *ptr = merged;

    while(ptr != NULL){

        TList *newnode = malloc(sizeof(TList));
        if(newnode == NULL) return NULL;
        
        strcpy(newnode->name,ptr->name);
        strcpy(newnode->difinition,ptr->difinition);

        strcpy(newnode->date.birth,ptr->date.birth);
        strcpy(newnode->date.death,ptr->date.death);


        newnode->next = NULL;

        if(queue->head == NULL){
            queue->head = queue->tail  = newnode;

        }else{

            queue->tail->next = newnode;
            queue->tail = newnode;
        }

        ptr = ptr->next;
    }


    return queue;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////:::::
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////::::: 

void delete_from_file(const char *fileName , const char *name){

    FILE *f = fopen(fileName,"r");
    FILE *temp = fopen("temp.txt","w");

    if (f == NULL || temp == NULL ) {
        printf("ERROR opening file.");
        return;
    }

    char line[1000];

    while (fgets(line, sizeof(line), f)) {

       
        if (strstr(line, name) == NULL) {
            fputs(line, temp);
        }
    }

    fclose(f);
    fclose(temp);

    
    remove(fileName);
    rename("temp.txt", fileName);
}

void deletepersonality(FILE *f, TList **s, TList **a, char *name) {

    if (*s == NULL && *a == NULL) return;

    delete_from_file("Algeria_history.txt", name);

    TList *ptr, *prev;
    ptr = *s;
    prev = NULL;

    while (ptr != NULL && strcmp(ptr->name, name) != 0) {
        prev = ptr;
        ptr = ptr->next;
    }

    if (ptr != NULL) {
        if (prev == NULL) {
            *s = ptr->next;
            if (*s != NULL) (*s)->prv = NULL;
        } else {
            prev->next = ptr->next;
            if (ptr->next != NULL)
                ptr->next->prv = prev;
        }
        free(ptr);
    }

    ptr = *a;
    prev = NULL;

    while (ptr != NULL && strcmp(ptr->name, name) != 0) {
        prev = ptr;
        ptr = ptr->next;
    }

    if (ptr != NULL) {
        if (prev == NULL) {
            *a = ptr->next;
            if (*a != NULL) (*a)->prv = NULL;
        } else {
            prev->next = ptr->next;
            if (ptr->next != NULL)
                ptr->next->prv = prev;
        }
        free(ptr);
    }
}

void updateInFile(const char *filename, const char *name , const char *definition , const char *DoB , const char *DoD)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Error opening file\n");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        printf("Error creating temp file\n");
        fclose(f);
        return;
    }

    char line[500];
    int found = 0;

    while (fgets(line, sizeof(line), f)) {

        char currentName[100];
        char *eq = strchr(line, '=');

        if (eq) {
            int len = eq - line;

            strncpy(currentName, line, len);
            currentName[len] = '\0';

           
            while (len > 0 && (currentName[len-1] == ' ' || currentName[len-1] == '\n')) {
                currentName[len-1] = '\0';
                len--;
            }

           
            if (strcmp(currentName, name) == 0) {
                fprintf(temp, "%s=%s {%s-%s}\n",
                        name, definition, DoB, DoD);
                found = 1;
                continue;
            }
        }

  
        fputs(line, temp);
    }

    fclose(f);
    fclose(temp);

    
    remove(filename);
    rename("temp.txt", filename);

    if (found)
        printf("Updated successfully.\n");
    else
        printf("Personality not found.\n");
}

void updatePersonality(FILE *f, TList **s, TList **a , char *name, char *definition, char *DoB, char *DoD){

    if (*s == NULL && *a == NULL) return;

    TList *ptr = *s;

    while(ptr != NULL && strcmp(ptr->name,name) != 0){
        ptr = ptr->next;
    }

    if(ptr){

         strcpy(ptr->difinition,definition);

    }else{
             printf("personality not found");
    }


    ptr = *a;

    while(ptr != NULL && strcmp(ptr->name,name) != 0){
        ptr = ptr->next;
    }

    if(ptr){

         strcpy(ptr->date.birth,DoB);
         strcpy(ptr->date.death,DoD);

    }else{
        printf("personality not found");
    }
    
    updateInFile("Algeria_history.txt",name,definition,DoB,DoD);


}

int int_runtest(){

    FILE *f = fopen("Algeria_history.txt","r");

    TList *head1 = getPersonality(f);

    rewind(f);

    TList *head2 = getDatePersonality(f);

    TList *head = mergeNodes(head1,head2);

    TStack *stk = toStack(head);

    //stk = recRevStack(stk);


    
    while(stk){
        printf("%s - %s - %s - %s\n",stk->name,stk->definition,stk->birth,stk->death);
        stk = stk->link;
    }
    

   

    return 0;
}