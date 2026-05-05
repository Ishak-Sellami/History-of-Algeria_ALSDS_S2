#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "tree_recursion.h"

TTree insertInBST(TTree root, Person p) {
    if (root == NULL) {
        
        TTree newNode = (TTree)malloc(sizeof(TTree));
        newNode->person = p;
        newNode->left = newNode->right = NULL;
        return newNode;
    }

    // compair strings
    // e.g. Ishak(3)   Samir(5)    Ishak is the first so to the LEFT...
    if (strcmp(p.fullName, root->person.fullName) < 0) {
        root->left = insertInBST(root->left, p);
    } else if (strcmp(p.fullName, root->person.fullName) > 0) {
        root->right = insertInBST(root->right, p);
    }
    return root;
}

TTree toTree(TStack *stk) {  // To change its value
    TTree root = NULL;
    Person tempPerson;

    
    if (*stk == NULL) return NULL;

    while (*stk != NULL) {
        tempPerson = (*stk)->person;
        
        root = insertInBST(root, tempPerson);
        
        TStack tempNode = *stk;
        *stk = (*stk)->next;
        free(tempNode); 
    }

    return root; 
}

TTree fillTree(FILE *f) {
    if (f == NULL) return NULL;

    TTree root = NULL;
    char line[500]; 
    
    while (fgets(line, sizeof(line), f)) { 
        Person p;
        
        char *token = strtok(line, "=");
        if (token != NULL) {
            strcpy(p.fullName, token);
            
            
            token = strtok(NULL, "{");
            if (token != NULL) {
                strcpy(p.definition, token);
            }

            token = strtok(NULL, "/"); 
            if (token != NULL) {
                p.birth.day = atoi(token);
                
                token = strtok(NULL, "/");
                if (token != NULL) {
                    p.birth.month = atoi(token);
            
                    token = strtok(NULL, "-}"); 
                    if (token != NULL) {
                        p.birth.year = atoi(token);
                    }
                }
            }

            token = strtok(NULL, "/"); 
            if (token != NULL) {
                p.death.day = atoi(token);
                
                token = strtok(NULL, "/"); 
                if (token != NULL) {
                    p.death.month = atoi(token);
                    
                    token = strtok(NULL, "-}"); 
                    if (token != NULL) {
                        p.death.year = atoi(token);
                    }
                }
            }
            
            root = insertInBST(root, p);
        }
    }
    return root; 
}

TTree getInfoNameTree(TTree tr, char *name) {
    
    if (tr == NULL) {
        return NULL; 
    }
    int comparison = strcmp(name, tr->person.fullName);
    if (comparison == 0) {
        return tr;
    } 
    else if (comparison < 0) {
        return getInfoNameTree(tr->left, name);
    } 
    else {
        return getInfoNameTree(tr->right, name);
    }
}

Date* stringToDate(char* string) {
    Date* d = (Date*)malloc(sizeof(Date));
    if (d == NULL) return NULL; 

    d->day = 0; d->month = 0; d->year = 0;
    if (string == NULL) {
        return d;
    }
    char temp[30];
    strcpy(temp, string);

    char* token = strtok(temp, "/");
    if (token != NULL) {
        d->day = atoi(token); 
        
        token = strtok(NULL, "/");
        if (token != NULL) {
            d->month = atoi(token);
            
            token = strtok(NULL, "-}");
            if (token != NULL) {
                d->year = atoi(token);
            }
        }
    }
    return d; 
}

TTree addNameBST(TTree tr, char *name, char *DoB, char *DoD) {
    Person p;
    strcpy(p.fullName, name);
    
    strcpy(p.definition, "New personality added manually"); 

    Date* tempDateB = stringToDate(DoB);
    if (tempDateB) {
        p.birth = *tempDateB;
        free(tempDateB);
    }
    Date* tempDateD = stringToDate(DoD);
    if (tempDateD) {
        p.death = *tempDateD;
        free(tempDateD);
    }

    tr = insertInBST(tr, p);
    
    return tr; 
}

TTree findMin(TTree root) {
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

TTree deleteNameBST(TTree root, char *name) {
    if (root == NULL) return root; 
    
    int cmp = strcmp(name, root->person.fullName);
    
    if (cmp < 0) {
        root->left = deleteNameBST(root->left, name);
    } else if (cmp > 0) {
        root->right = deleteNameBST(root->right, name);
    } 
   
    else {
        //  0   1   child
        if (root->left == NULL) {
            TTree temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            TTree temp = root->left;
            free(root);
            return temp;
        }

        TTree temp = findMin(root->right);
        
        root->person = temp->person;

        root->right = deleteNameBST(root->right, temp->person.fullName);
    }
    return root;
}

TTree updateNameBST(TTree tr, char *name, char *s, char *DoB, char *DoD) {
    
    if (tr == NULL) {
        return NULL;
    }

    int cmp = strcmp(name, tr->person.fullName);

    if (cmp < 0) {
        tr->left = updateNameBST(tr->left, name, s, DoB, DoD);
    }
    else if (cmp > 0) {
        tr->right = updateNameBST(tr->right, name, s, DoB, DoD);
    }
    else {
        // the desert node

        // update definition
        strcpy(tr->person.definition, s);

        // update birth
        Date* d1 = stringToDate(DoB);
        if (d1 != NULL) {
            tr->person.birth = *d1;
            free(d1);
        }

        // update death
        Date* d2 = stringToDate(DoD);
        if (d2 != NULL) {
            tr->person.death = *d2;
            free(d2);
        }
    }

    return tr;
}

void traversalBSTinOrder(TTree tr) {
    if (tr == NULL) return;

    // 1. Left
    traversalBSTinOrder(tr->left);

    // 2. Root (print)
    printf("Name: %s\n", tr->person.fullName);
    printf("Definition: %s\n", tr->person.definition);
    printf("Birth: %02d/%02d/%d\n",
           tr->person.birth.day,
           tr->person.birth.month,
           tr->person.birth.year);
    printf("Death: %02d/%02d/%d\n",
           tr->person.death.day,
           tr->person.death.month,
           tr->person.death.year);
    printf("-------------------------\n");

    // 3. Right
    traversalBSTinOrder(tr->right);
}

void traversalBSTpreOrder(TTree tr) {
    if (tr == NULL) return;

    // 1. Root
    printf("Name: %s\n", tr->person.fullName);
    printf("Definition: %s\n", tr->person.definition);
    printf("Birth: %02d/%02d/%d\n",
           tr->person.birth.day,
           tr->person.birth.month,
           tr->person.birth.year);
    printf("Death: %02d/%02d/%d\n",
           tr->person.death.day,
           tr->person.death.month,
           tr->person.death.year);
    printf("-------------------------\n");

    // 2. Left
    traversalBSTpreOrder(tr->left);

    // 3. Right
    traversalBSTpreOrder(tr->right);
}

void traversalBSTpostOrder(TTree tr) {
    if (tr == NULL) return;

    // 1. Left
    traversalBSTpostOrder(tr->left);

    // 2. Right
    traversalBSTpostOrder(tr->right);

    // 3. Root
    printf("Name: %s\n", tr->person.fullName);
    printf("Definition: %s\n", tr->person.definition);
    printf("Birth: %02d/%02d/%d\n",
           tr->person.birth.day,
           tr->person.birth.month,
           tr->person.birth.year);
    printf("Death: %02d/%02d/%d\n",
           tr->person.death.day,
           tr->person.death.month,
           tr->person.death.year);
    printf("-------------------------\n");
}

int height(TTree tr) {
    if (tr == NULL) return 0;

    int leftH = height(tr->left);
    int rightH = height(tr->right);

    return 1 + (leftH > rightH ? leftH : rightH);
}

int size(TTree tr) {
    if (tr == NULL) return 0;

    return 1 + size(tr->left) + size(tr->right);
}

void heightSizeBST(TTree tr) {
    if (tr == NULL) {
        printf("Tree is empty\n");
        return;
    }

    int h = height(tr);
    int s = size(tr);

    printf("Height: %d\n", h);
    printf("Size: %d\n", s);
}

TTree lowestCommonAncestor(TTree tr, char *word1, char *word2) {
    if (tr == NULL) return NULL;

    int cmp1 = strcmp(word1, tr->person.fullName);
    int cmp2 = strcmp(word2, tr->person.fullName);

    // both in left
    if (cmp1 < 0 && cmp2 < 0) {
        return lowestCommonAncestor(tr->left, word1, word2);
    }

    // both in right 
    if (cmp1 > 0 && cmp2 > 0) {
        return lowestCommonAncestor(tr->right, word1, word2);
    }

    // the goal: one in right and one in left  -----> this is the goal
    // one of them equals node -----> this is the goal
    return tr;
}

int countNodesRange(TTree tr, int l, int h) {
    if (tr == NULL) return 0;
    int count = (tr->person.birth.year >= l && tr->person.birth.year <= h) ? 1 : 0;
    return count + countNodesRange(tr->left, l, h) + countNodesRange(tr->right, l, h);
}

TTree inOrderSuccessor(TTree tr, char *word) {
    if (tr == NULL) return NULL;

    TTree successor = NULL;
    TTree current = tr;

    
    while (current != NULL) {
        int cmp = strcmp(word, current->person.fullName);

        if (cmp < 0) {

            successor = current;
            current = current->left;
        } 
        else if (cmp > 0) {

            current = current->right;
        } 
        else {
            
            if (current->right != NULL) {
                successor = findMin(current->right); 
            }
            break;
        }
    }

    return successor;
}

TTree BSTMirror(TTree tr) {

    if (tr == NULL) {
        return NULL;
    }

    
    TTree tempLeft = BSTMirror(tr->left);
    TTree tempRight = BSTMirror(tr->right);

    
    tr->left = tempRight;
    tr->right = tempLeft;

    return tr;
}

bool isBalancedBST(TTree tr) {
    
    if (tr == NULL) {
        return true;
    }

    
    int leftH = height(tr->left);
    int rightH = height(tr->right);

    
    if (abs(leftH - rightH)  <= 1 && 
        isBalancedBST(tr->left) && 
        isBalancedBST(tr->right)) {
        return true;
    }

    
    return false;
}

// tree to array 
void storeInOrder(TTree tr, Person nodes[], int *index) {
    if (tr == NULL) return;
    storeInOrder(tr->left, nodes, index);
    nodes[(*index)++] = tr->person;
    storeInOrder(tr->right, nodes, index);
}

// from sorted array to InOrer tree
TTree buildBalancedTree(Person nodes[], int start, int end) {
    if (start > end) return NULL;

    int mid = (start + end) / 2;
    TTree root = (TTree)malloc(sizeof(personTree));
    root->person = nodes[mid];

    root->left = buildBalancedTree(nodes, start, mid - 1);
    root->right = buildBalancedTree(nodes, mid + 1, end);

    return root;
}


TTree BTSMerge(TTree tr1, TTree tr2) {
    int size1 = size(tr1); // count the size Up
    int size2 = size(tr2);
    
    Person *allNodes = (Person *)malloc((size1 + size2) * sizeof(Person));
    int index = 0;

    storeInOrder(tr1, allNodes, &index);
    storeInOrder(tr2, allNodes, &index);

    TTree result = buildBalancedTree(allNodes, 0, index - 1);
    
    free(allNodes);
    return result;
}

// -------------------------------------------------------------------------------------------

int countOccurence(FILE *f, char *name) {
    char line[500];

    if (fgets(line, sizeof(line), f) == NULL) {
        return 0;
    }

    int found ;
    if (strstr(line, name) != NULL)
        found = 1;
    else
        found = 0;

    // strstr look for name in line 

    return found + countOccurence(f, name);
}

void recursiveFilter(FILE *src, FILE *dest, char *word) {
    char line[500];

    if (fgets(line, sizeof(line), src) == NULL) {// return a pointer to line in the file otherwise NULL
        return;
    }

    if (strstr(line, word) == NULL) {// return pointer to the word from string line atherwise NULL
        fputs(line, dest);
    }

    recursiveFilter(src, dest, word);
}

FILE* removeOccurence(FILE *f, char *word) {
    if (f == NULL) return NULL;

    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) return f;

    rewind(f);  // To initialize any preExecution

    recursiveFilter(f, tempFile, word);

    fclose(f);
    fclose(tempFile);

    remove("history.txt"); 
    rename("temp.txt", "history.txt");

    return fopen("history.txt", "r+");
}

void recursiveReplace(FILE *src, FILE *dest, char *name, char *DoB, char *DoD) {
    char line[500];

    if (fgets(line, sizeof(line), src) == NULL) {
        return;
    }

    if (strstr(line, name) != NULL) {
        fprintf(dest, "%s={%s-%s}\n", name, DoB, DoD);
    } else {
        fputs(line, dest);
    }
    recursiveReplace(src, dest, name, DoB, DoD);
}

FILE* replaceOccurence(FILE *f, char *name, char *DoB, char *DoD) {
    if (f == NULL) return NULL;

    FILE *tempFile = fopen("temp_replace.txt", "w");
    if (tempFile == NULL) return f;

    rewind(f);

    recursiveReplace(f, tempFile, name, DoB, DoD);

    fclose(f);
    fclose(tempFile);

    remove("history.txt");
    rename("temp_replace.txt", "history.txt");

    return fopen("history.txt", "r+");
}

// defficult
void swap(char *x, char *y) {
    char temp = *x;
    *x = *y;
    *y = temp;
}

void permute(char *a, int l, int r) {
    
    if (l == r) {
        printf("%s\n", a);
    } else {
        for (int i = l; i <= r; i++) {
            swap((a + l), (a + i));       
            permute(a, l + 1, r);        
            swap((a + l), (a + i));       
        }
    }
}

void namePermutation(char *name) {
    int n = strlen(name);
    permute(name, 0, n - 1);
}

//defficult
void generateSubseq(char *word, char *current, int index, int currIndex) {
    if (word[index] == '\0') {
        current[currIndex] = '\0'; 
        if (currIndex > 0) {  
            printf("%s\n", current);
        }
        return;
    }

    generateSubseq(word, current, index + 1, currIndex);

    current[currIndex] = word[index];
    generateSubseq(word, current, index + 1, currIndex + 1);
}

void subseqName(char *word) {
    int n = strlen(word);
    char current[n + 1]; 
    
    printf("Subsequences of '%s':\n", word);
    generateSubseq(word, current, 0, 0);
}


int extractYear(char *dateStr) {
    char *lastSlash = strrchr(dateStr, '/');
    if (lastSlash) 
    return atoi(lastSlash + 1);
    return atoi(dateStr); 
}

void longestSubyearRecursive(FILE *f, int y1, int y2) {
    char line[500];
    
    if (fgets(line, sizeof(line), f) == NULL) {
        return;
    }

    if (line[0] == ':') {
        char *start = strchr(line, '{');
        char *end = strchr(line, '}');
        
        if (start && end) {
            char datePart[20] = {0};
            strncpy(datePart, start + 1, end - start - 1);
            int eventYear = extractYear(datePart);

            if (eventYear >= y1 && eventYear <= y2) {
                printf("Event Overlap Found: %s", line);
            }
        }
    }
    longestSubyearRecursive(f, y1, y2);
}

void longestSubyear(char *date1, char *date2) {
    FILE *f = fopen("history.txt", "r");
    if (f == NULL) return;

    int y1 = extractYear(date1);
    int y2 = extractYear(date2);

    printf("Events overlapping between %d and %d:\n", y1, y2);
    longestSubyearRecursive(f, y1, y2);

    fclose(f);
}


int countDistinct(char *str, int n, int lastPos[]) {
    if (n == 0) {
        return 1;
    }

    int count = countDistinct(str, n - 1, lastPos);

    int currentCount = 2 * count;

    int charIdx = (unsigned char)str[n - 1];

    if (lastPos[charIdx] != -1) {
        currentCount = currentCount - countDistinct(str, lastPos[charIdx] - 1, lastPos);
    }

    lastPos[charIdx] = n;

    return currentCount;
}

int distinctSubseqWord(char *event) {
    int n = strlen(event);
    int lastPos[256]; 
    
    for (int i = 0; i < 256; i++) {
        lastPos[i] = -1;
    }

    return countDistinct(event, n, lastPos) - 1;
}






int getYearOnly(char *dateStr) {
    char *lastSlash = strrchr(dateStr, '/');
    if (lastSlash) return atoi(lastSlash + 1);
    return atoi(dateStr);
}

bool checkOverlapRecursive(FILE *f, int eventYear) {
    char line[500];
    
    if (fgets(line, sizeof(line), f) == NULL) {
        return false;
    }

    if (strchr(line, '=') != NULL) {
        char *start = strchr(line, '{');
        char *end = strchr(line, '}');

        if (start && end) {
            char dates[40];
            strncpy(dates, start + 1, end - start - 1);
            dates[end - start - 1] = '\0';

            char *birthPart = strtok(dates, "-");
            char *deathPart = strtok(NULL, "-");

            if (birthPart && deathPart) {
                int bYear = getYearOnly(birthPart);
                int dYear = getYearOnly(deathPart);

                if (eventYear >= bYear && (dYear == 0 || eventYear <= dYear)) {
                    return true; 
                }
            }
        }
    }
    return checkOverlapRecursive(f, eventYear);
}

bool isPalindromeWord(char *event) {
    int eventYear = atoi(event); 
    
    FILE *f = fopen("history.txt", "r");
    if (f == NULL) return false;

    bool result = checkOverlapRecursive(f, eventYear);
    
    fclose(f);
    return result;
}
