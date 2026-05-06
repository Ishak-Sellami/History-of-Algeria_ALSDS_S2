/*
 * Algeria History DB — Raylib GUI
 * Wraps Modules_for_Linked_lists_and_Queues.c
 * Build: gcc algeria_history_gui.c -o algeria_history_gui -lraylib -lm
 * Or with full path to raylib:
 *   gcc algeria_history_gui.c -o algeria_history_gui \
 *       -I/usr/local/include -L/usr/local/lib -lraylib -lm -lpthread -ldl
 */

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

/* ─── Data structures (from your module) ─── */

typedef struct TList {
    char name[40];
    char difinition[400];
    struct date { char birth[10]; char death[10]; } date;
    char dateB[10];
    char dateE[10];
    int  type;          /* 0=personality, 1=event */
    struct TList *next;
    struct TList *prv;
} TList;

typedef struct TQueue { TList *head; TList *tail; } TQueue;

typedef struct TStack {
    struct TStack *link;
    char name[40];
    char definition[400];
    char birth[10];
    char death[10];
    int  type;
} TStack;

/* ─── Module helpers (from your .c) ─── */

static int countWords(const char *s){
    int c=1; for(;*s;s++) if(*s==' ') c++; return c;
}

static void to_lowercase(char *w){
    for(int i=0;w[i];i++) w[i]=tolower(w[i]);
}

static bool isPersonalityKilled(const char *word){
    char tmp[500]; strncpy(tmp,word,499); tmp[499]=0; to_lowercase(tmp);
    const char *kw[]={"killed","assassinated","murdered","executed","shot dead"};
    for(int i=0;i<5;i++) if(strstr(tmp,kw[i])) return true;
    return false;
}

static int is_pallindrome(const char *word){
    int j=(int)strlen(word)-1, i=0;
    while(i<j){ if(tolower(word[i])!=tolower(word[j])) return 0; i++; j--; }
    return 1;
}

/* Bubble-sort linked list by name */
static TList* sortWord(TList *head){
    if(!head) return NULL;
    char tn[40],td[400],tb[10],tde[10];
    for(TList *i=head;i;i=i->next)
        for(TList *j=i->next;j;j=j->next)
            if(i->type==0&&j->type==0&&strcmp(i->name,j->name)>0){
                strcpy(tn,i->name);strcpy(i->name,j->name);strcpy(j->name,tn);
                strcpy(td,i->difinition);strcpy(i->difinition,j->difinition);strcpy(j->difinition,td);
                strcpy(tb,i->date.birth);strcpy(i->date.birth,j->date.birth);strcpy(j->date.birth,tb);
                strcpy(tde,i->date.death);strcpy(i->date.death,j->date.death);strcpy(j->date.death,tde);
            }
    return head;
}

static TList* sortPersonality(TList *head){
    if(!head) return NULL;
    char tn[40],td[400],tb[10],tde[10];
    for(TList *i=head;i;i=i->next)
        for(TList *j=i->next;j;j=j->next){
            int a1=atoi(i->date.death)-atoi(i->date.birth);
            int a2=atoi(j->date.death)-atoi(j->date.birth);
            if(i->type==0&&j->type==0&&a1>a2){
                strcpy(tn,i->name);strcpy(i->name,j->name);strcpy(j->name,tn);
                strcpy(td,i->difinition);strcpy(i->difinition,j->difinition);strcpy(j->difinition,td);
                strcpy(tb,i->date.birth);strcpy(i->date.birth,j->date.birth);strcpy(j->date.birth,tb);
                strcpy(tde,i->date.death);strcpy(i->date.death,j->date.death);strcpy(j->date.death,tde);
            }
        }
    return head;
}

static TList* mergeNodes(TList *s, TList *a){
    TList *head=NULL,*tail=NULL;
    for(TList *p=s;p;p=p->next){
        for(TList *p1=a;p1;p1=p1->next){
            if(strcmp(p->name,p1->name)==0){
                TList *n=calloc(1,sizeof(TList));
                strcpy(n->name,p->name); strcpy(n->difinition,p->difinition);
                strcpy(n->date.birth,p1->date.birth); strcpy(n->date.death,p1->date.death);
                if(!head){head=n;tail=n;}else{tail->next=n;n->prv=tail;tail=n;}
                break;
            }
        }
    }
    return head;
}

static void addPersonality(TList **s, TList **a, const char *name, const char *def,
                            const char *DoB, const char *DoD){
    TList *ns=calloc(1,sizeof(TList)); ns->type=0;
    strcpy(ns->name,name); strcpy(ns->difinition,def);
    strcpy(ns->date.birth,DoB); strcpy(ns->date.death,DoD);
    if(!*s){*s=ns;}else{TList*p=*s;while(p->next)p=p->next;p->next=ns;}

    TList *na=calloc(1,sizeof(TList)); na->type=0;
    strcpy(na->name,name); strcpy(na->date.birth,DoB); strcpy(na->date.death,DoD);
    if(!*a){*a=na;}else{TList*p=*a;while(p->next)p=p->next;p->next=na;}
}

static void deletepersonality(TList **s, const char *name){
    TList *ptr=*s,*prev=NULL;
    while(ptr&&strcmp(ptr->name,name)!=0){prev=ptr;ptr=ptr->next;}
    if(ptr){
        if(!prev)*s=ptr->next; else prev->next=ptr->next;
        if(ptr->next) ptr->next->prv=prev;
        free(ptr);
    }
}

static void updatePersonality(TList *s, const char *name,
                               const char *def, const char *DoB, const char *DoD){
    for(TList *p=s;p;p=p->next)
        if(strcmp(p->name,name)==0){
            strcpy(p->difinition,def);
            strcpy(p->date.birth,DoB);
            strcpy(p->date.death,DoD);
            break;
        }
}

static TList* palindromesInDefs(TList *s){
    TList *result=NULL;
    for(TList *p=s;p;p=p->next){
        char tmp[400]; strncpy(tmp,p->difinition,399); tmp[399]=0;
        char *tok=strtok(tmp," ,.-");
        while(tok){
            if(is_pallindrome(tok)){
                TList *n=calloc(1,sizeof(TList)); strcpy(n->name,tok);
                if(!result){result=n;}else{
                    TList *e=result; while(e->next)e=e->next; e->next=n;
                }
            }
            tok=strtok(NULL," ,.-");
        }
    }
    return result;
}

static void freeList(TList *h){while(h){TList*n=h->next;free(h);h=n;}}

/* ─── File I/O ─── */

#define DATA_FILE "Algeria_history.txt"

static TList *gPersonNames = NULL;   /* definition list */
static TList *gPersonDates = NULL;   /* date list */
static TList *gMerged      = NULL;   /* working list */
static TList *gEvents      = NULL;

static TList* file_getPersonality(FILE *f){
    TList *head=NULL,*tail=NULL;
    char line[500];
    while(fgets(line,sizeof(line),f)){
        if(strchr(line,'=')){
            char name[40]={0}, def[400]={0};
            sscanf(line,"%39[^=]=%399[^{]",name,def);
            TList *n=calloc(1,sizeof(TList)); n->type=0;
            strcpy(n->name,name); strcpy(n->difinition,def);
            if(!head){head=n;tail=n;}else{tail->next=n;tail=n;}
        }
    }
    return head;
}

static TList* file_getDatePersonality(FILE *f){
    TList *head=NULL,*tail=NULL;
    char line[500];
    while(fgets(line,sizeof(line),f)){
        char *p=strchr(line,'{'); char *eq=strchr(line,'=');
        if(p&&eq&&strchr(p,'-')){
            char name[100]={0},birth[10]={0},death[10]={0};
            int len=(int)(eq-line); strncpy(name,line,len); name[len]=0;
            if(sscanf(p,"{%9[^-]-%9[^}]}",birth,death)==2){
                TList *n=calloc(1,sizeof(TList)); n->type=0;
                strcpy(n->name,name); strcpy(n->date.birth,birth); strcpy(n->date.death,death);
                if(!head){head=n;tail=n;}else{tail->next=n;tail=n;}
            }
        }
    }
    return head;
}

static void loadFromFile(void){
    FILE *f=fopen(DATA_FILE,"r");
    if(!f) return;
    freeList(gPersonNames); freeList(gPersonDates); freeList(gMerged);
    gPersonNames=file_getPersonality(f);
    rewind(f);
    gPersonDates=file_getDatePersonality(f);
    fclose(f);
    gMerged=mergeNodes(gPersonNames,gPersonDates);
}

static void saveToFile(void){
    FILE *f=fopen(DATA_FILE,"w");
    if(!f) return;
    for(TList *p=gMerged;p;p=p->next)
        fprintf(f,"%s=%s{%s-%s}\n",p->name,p->difinition,p->date.birth,p->date.death);
    fclose(f);
}

/* ─── Seed data (used if no file exists) ─── */

static void seedData(void){
    /* personalities */
    addPersonality(&gPersonNames,&gPersonDates,"Emir Abdelkader",
        "Algerian political and military leader who led the struggle against French colonial rule in the 19th century. Born in Mascara.",
        "1808","1883");
    addPersonality(&gPersonNames,&gPersonDates,"Larbi Ben M'hidi",
        "Revolutionary leader and founding member of the FLN. He was arrested and executed by French paratroops.",
        "1923","1957");
    addPersonality(&gPersonNames,&gPersonDates,"Ferhat Abbas",
        "Algerian nationalist politician and first President of the Provisional Government of the Algerian Republic.",
        "1899","1985");
    addPersonality(&gPersonNames,&gPersonDates,"Hassiba Ben Bouali",
        "Young revolutionary FLN fighter killed during the Battle of Algiers.",
        "1938","1957");
    addPersonality(&gPersonNames,&gPersonDates,"Ahmed Ben Bella",
        "First President of independent Algeria and a key figure of the Algerian War of Independence.",
        "1916","2012");
    addPersonality(&gPersonNames,&gPersonDates,"Houari Boumediene",
        "Second President of Algeria who nationalised oil and led socialist industrialisation.",
        "1932","1978");
    addPersonality(&gPersonNames,&gPersonDates,"Krim Belkacem",
        "Military commander who played a central role in the Algerian War of Independence as a leader of the FLN.",
        "1922","1970");
    gMerged=mergeNodes(gPersonNames,gPersonDates);
}

/* ─── UI constants ─── */

#define WIN_W   1100
#define WIN_H   640
#define SIDEBAR_W  190
#define DETAIL_W   260
#define TOPBAR_H    46
#define STATUSBAR_H 26
#define ROW_H       36
#define FONT_SIZE   14
#define FONT_SM     12

/* Palette */
#define COL_BG       CLITERAL(Color){245,244,240,255}
#define COL_SURFACE  CLITERAL(Color){255,255,255,255}
#define COL_BORDER   CLITERAL(Color){220,218,210,255}
#define COL_ACCENT   CLITERAL(Color){ 29,158,117,255}
#define COL_ACCENT2  CLITERAL(Color){ 15,110, 86,255}
#define COL_TXT_PRI  CLITERAL(Color){ 26, 25, 22,255}
#define COL_TXT_SEC  CLITERAL(Color){ 90, 88, 82,255}
#define COL_TXT_TER  CLITERAL(Color){140,138,130,255}
#define COL_SEL_BG   CLITERAL(Color){225,245,238,255}
#define COL_HOVER    CLITERAL(Color){240,239,234,255}
#define COL_TAG_MIL  CLITERAL(Color){225,245,238,255}
#define COL_TAG_MAR  CLITERAL(Color){250,238,218,255}
#define COL_TAG_POL  CLITERAL(Color){230,241,251,255}
#define COL_TAGF_MIL CLITERAL(Color){ 15,110, 86,255}
#define COL_TAGF_MAR CLITERAL(Color){133, 79, 11,255}
#define COL_TAGF_POL CLITERAL(Color){ 24, 95,165,255}
#define COL_RED_BG   CLITERAL(Color){252,235,235,255}
#define COL_RED_FG   CLITERAL(Color){163, 45, 45,255}
#define COL_RED_BR   CLITERAL(Color){247,193,193,255}
#define COL_TOAST_BG CLITERAL(Color){ 30, 30, 28,240}
#define COL_TOAST_FG CLITERAL(Color){240,238,232,255}

/* ─── UI state ─── */

typedef enum { VIEW_LIST, VIEW_STACK, VIEW_QUEUE, VIEW_BST } ViewMode;
typedef enum { MODAL_NONE, MODAL_ADD, MODAL_EDIT, MODAL_CONFIRM_DEL,
               MODAL_SEARCH_DATE, MODAL_COUNT, MODAL_PALINDROMES } ModalMode;

static ViewMode  gView        = VIEW_LIST;
static ModalMode gModal       = MODAL_NONE;
static int       gSelected    = 0;   /* index in gMerged */
static int       gScrollY     = 0;
static char      gSearch[80]  = {0};
static bool      gSearchFocus = false;

/* Form fields */
static char fName[40]={0}, fDef[400]={0}, fBirth[10]={0}, fDeath[10]={0};
static int  fFocus=0;  /* 0=name,1=def,2=birth,3=death */
static int  fEditIdx=-1;

/* Toast */
static char  gToast[300]={0};
static float gToastTimer=0.f;

/* Operations result */
static char gOpResult[1200]={0};

/* Sidebar items */
typedef struct { const char *label; int section; } SBItem;
static SBItem sbItems[]={
    {"Personalities",0},{"Events",0},{"Dates",0},
    {"Stack view",1},{"Queue view",1},{"BST view",1},
    {"Sort by name",2},{"Sort by age",2},{"Palindromes",2},{"Merge lists",2},
    {"Search by date",3},{"Count occurrences",3},
};
#define SB_COUNT (sizeof(sbItems)/sizeof(sbItems[0]))
static int gSBActive=0;

/* ─── Helpers ─── */

static int listCount(TList *h){int c=0;for(;h;h=h->next)c++;return c;}

static TList* listGet(TList *h, int idx){
    for(int i=0;h&&i<idx;i++,h=h->next);
    return h;
}

static int listFind(TList *h, const char *name){
    int i=0;
    for(;h;h=h->next,i++) if(strcmp(h->name,name)==0) return i;
    return 0;
}

static void showToast(const char *msg){
    strncpy(gToast,msg,299); gToastTimer=3.0f;
}

static bool strContains(const char *haystack, const char *needle){
    if(!needle[0]) return true;
    char h[500],n[80];
    strncpy(h,haystack,499); h[499]=0; to_lowercase(h);
    strncpy(n,needle,79);    n[79]=0;  to_lowercase(n);
    return strstr(h,n)!=NULL;
}

/* Wrap text into lines of at most maxW pixels */
static void drawTextWrapped(Font font, const char *text, float x, float y,
                             float maxW, float fontSize, Color col){
    char buf[400]; strncpy(buf,text,399); buf[399]=0;
    char line[200]={0};
    float lineH=fontSize+4;
    float cy=y;
    char *tok=strtok(buf," ");
    while(tok){
        char tmp[200];
        snprintf(tmp,sizeof(tmp),"%s%s%s",line,line[0]?" ":"",tok);
        float w=MeasureTextEx(font,tmp,fontSize,1).x;
        if(w>maxW&&line[0]){
            DrawTextEx(font,line,(Vector2){x,cy},fontSize,1,col);
            cy+=lineH;
            strncpy(line,tok,199);
        }else{
            strncpy(line,tmp,199);
        }
        tok=strtok(NULL," ");
    }
    if(line[0]) DrawTextEx(font,line,(Vector2){x,cy},fontSize,1,col);
}

/* ─── Draw primitives ─── */

static void drawRect(float x,float y,float w,float h,Color c){
    DrawRectangle((int)x,(int)y,(int)w,(int)h,c);
}
static void drawBorder(float x,float y,float w,float h,Color c){
    DrawRectangleLinesEx((Rectangle){x,y,w,h},1,c);
}
static void drawTag(Font font,const char *txt,float x,float y,Color bg,Color fg){
    float tw=MeasureTextEx(font,txt,11,1).x;
    DrawRectangleRounded((Rectangle){x,y,tw+12,18},0.5f,4,bg);
    DrawTextEx(font,txt,(Vector2){x+6,y+4},11,1,fg);
}
static void drawButton(Font font,const char *txt,float x,float y,float w,float h,
                        Color bg,Color fg,Color br,bool hover){
    Color bg2=hover?COL_HOVER:bg;
    DrawRectangleRounded((Rectangle){x,y,w,h},0.35f,4,bg2);
    DrawRectangleRoundedLinesEx((Rectangle){x,y,w,h},0.35f,4,1,br);
    float tw=MeasureTextEx(font,txt,FONT_SM,1).x;
    DrawTextEx(font,txt,(Vector2){x+(w-tw)/2,y+(h-14)/2},FONT_SM,1,fg);
}

/* Input field */
static void drawInputField(Font font,const char *label,const char *value,
                            float x,float y,float w,bool focused,bool multiline){
    DrawTextEx(font,label,(Vector2){x,y},11,1,COL_TXT_TER);
    float fh=multiline?72:28;
    Color br=focused?COL_ACCENT:COL_BORDER;
    DrawRectangleRounded((Rectangle){x,y+14,w,fh},0.25f,4,COL_BG);
    DrawRectangleRoundedLinesEx((Rectangle){x,y+14,w,fh},0.25f,4,focused?1.5f:1,br);
    /* clip-draw value */
    float tx=x+8, ty=y+14+7;
    if(multiline){
        drawTextWrapped(font,value,tx,ty,w-16,FONT_SM,COL_TXT_PRI);
    } else {
        DrawTextEx(font,value,(Vector2){tx,ty},FONT_SM,1,COL_TXT_PRI);
    }
    if(focused){
        float cx=tx+MeasureTextEx(font,value,FONT_SM,1).x;
        if((int)(GetTime()*2)%2==0)
            DrawLineEx((Vector2){cx,ty},(Vector2){cx,ty+FONT_SIZE},1.5f,COL_ACCENT);
    }
}

/* ─── Modal draw ─── */

static void drawModal(Font font){
    /* Overlay */
    DrawRectangle(0,0,WIN_W,WIN_H,(Color){0,0,0,120});

    float mw=400, mh=(gModal==MODAL_ADD||gModal==MODAL_EDIT)?340:
                     (gModal==MODAL_PALINDROMES||gModal==MODAL_COUNT)?280:200;
    float mx=(WIN_W-mw)/2, my=(WIN_H-mh)/2;

    DrawRectangleRounded((Rectangle){mx,my,mw,mh},0.08f,8,COL_SURFACE);
    DrawRectangleRoundedLinesEx((Rectangle){mx,my,mw,mh},0.08f,8,1,COL_BORDER);

    /* Title */
    const char *titles[]={
        "","Add Personality","Edit Personality","Delete Record",
        "Search by Date","Count Occurrences","Palindromes in Definitions"
    };
    DrawTextEx(font,titles[gModal],(Vector2){mx+20,my+18},FONT_SIZE+1,1,COL_TXT_PRI);
    DrawLineEx((Vector2){mx,my+44},(Vector2){mx+mw,my+44},1,COL_BORDER);

    float fy=my+56;

    if(gModal==MODAL_ADD||gModal==MODAL_EDIT){
        drawInputField(font,"Name",fName,mx+20,fy,mw-40,fFocus==0,false); fy+=50;
        drawInputField(font,"Definition",fDef,mx+20,fy,mw-40,fFocus==1,true); fy+=90;
        drawInputField(font,"Date of Birth (YYYY)",fBirth,mx+20,fy,(mw-50)/2,fFocus==2,false);
        drawInputField(font,"Date of Death (YYYY)",fDeath,mx+20+(mw-50)/2+10,fy,(mw-50)/2,fFocus==3,false);
        fy+=50;

        Vector2 mp=GetMousePosition();
        bool hSave=CheckCollisionPointRec(mp,(Rectangle){mx+20,fy,mw/2-25,32});
        bool hCancel=CheckCollisionPointRec(mp,(Rectangle){mx+mw/2+5,fy,mw/2-25,32});
        drawButton(font,"Save",mx+20,fy,mw/2-25,32,COL_ACCENT,WHITE,COL_ACCENT,hSave);
        drawButton(font,"Cancel",mx+mw/2+5,fy,mw/2-25,32,COL_SURFACE,COL_TXT_SEC,COL_BORDER,hCancel);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if(hSave&&fName[0]){
                if(gModal==MODAL_ADD){
                    addPersonality(&gPersonNames,&gPersonDates,fName,fDef,fBirth,fDeath);
                    freeList(gMerged);
                    gMerged=mergeNodes(gPersonNames,gPersonDates);
                    gSelected=listFind(gMerged,fName);
                    saveToFile();
                    showToast("Personality added.");
                } else {
                    TList *r=listGet(gMerged,fEditIdx);
                    if(r){
                        updatePersonality(gMerged,r->name,fDef,fBirth,fDeath);
                        strcpy(r->name,fName);
                        saveToFile();
                        showToast("Personality updated.");
                    }
                }
                gModal=MODAL_NONE;
            }
            if(hCancel) gModal=MODAL_NONE;
        }
    } else if(gModal==MODAL_CONFIRM_DEL){
        TList *r=listGet(gMerged,gSelected);
        char msg[120]="Delete ";
        if(r){strncat(msg,r->name,39);strcat(msg,"?");}
        DrawTextEx(font,msg,(Vector2){mx+20,fy},FONT_SM,1,COL_TXT_SEC); fy+=40;

        Vector2 mp=GetMousePosition();
        bool hDel=CheckCollisionPointRec(mp,(Rectangle){mx+20,fy,mw/2-25,32});
        bool hNo=CheckCollisionPointRec(mp,(Rectangle){mx+mw/2+5,fy,mw/2-25,32});
        drawButton(font,"Delete",mx+20,fy,mw/2-25,32,COL_RED_BG,COL_RED_FG,COL_RED_BR,hDel);
        drawButton(font,"Cancel",mx+mw/2+5,fy,mw/2-25,32,COL_SURFACE,COL_TXT_SEC,COL_BORDER,hNo);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if(hDel&&r){
                deletepersonality(&gMerged,r->name);
                deletepersonality(&gPersonNames,r->name);
                deletepersonality(&gPersonDates,r->name);
                int cnt=listCount(gMerged);
                if(gSelected>=cnt) gSelected=cnt-1;
                if(gSelected<0) gSelected=0;
                saveToFile();
                showToast("Record deleted.");
            }
            if(hDel||hNo) gModal=MODAL_NONE;
        }
    } else if(gModal==MODAL_SEARCH_DATE||gModal==MODAL_COUNT){
        const char *lbl=gModal==MODAL_SEARCH_DATE?"Year (e.g. 1957)":"Search term";
        drawInputField(font,lbl,fBirth,mx+20,fy,mw-40,true,false); fy+=50;
        DrawTextEx(font,gOpResult,(Vector2){mx+20,fy},FONT_SM,1,COL_TXT_SEC); fy+=50;

        Vector2 mp=GetMousePosition();
        bool hRun=CheckCollisionPointRec(mp,(Rectangle){mx+20,fy,mw/2-25,32});
        bool hClose=CheckCollisionPointRec(mp,(Rectangle){mx+mw/2+5,fy,mw/2-25,32});
        drawButton(font,"Run",mx+20,fy,mw/2-25,32,COL_ACCENT,WHITE,COL_ACCENT,hRun);
        drawButton(font,"Close",mx+mw/2+5,fy,mw/2-25,32,COL_SURFACE,COL_TXT_SEC,COL_BORDER,hClose);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if(hRun&&fBirth[0]){
                if(gModal==MODAL_SEARCH_DATE){
                    int cnt=0; char res[600]="Found: ";
                    for(TList *p=gMerged;p;p=p->next)
                        if(strstr(p->date.birth,fBirth)||strstr(p->date.death,fBirth)){
                            if(cnt) strcat(res,", "); strncat(res,p->name,39); cnt++;
                        }
                    if(!cnt) strcpy(res,"No records found.");
                    strncpy(gOpResult,res,299);
                } else {
                    /* Count (recursive via iteration) */
                    int cnt=0;
                    for(TList *p=gMerged;p;p=p->next)
                        if(strContains(p->name,fBirth)||strContains(p->difinition,fBirth)) cnt++;
                    snprintf(gOpResult,299,"'%s' found in %d record(s).",fBirth,cnt);
                }
            }
            if(hClose){ gModal=MODAL_NONE; gOpResult[0]=0; fBirth[0]=0; }
        }
    } else if(gModal==MODAL_PALINDROMES){
        TList *pals=palindromesInDefs(gMerged);
        char res[800]="";
        int cnt=0;
        for(TList *p=pals;p;p=p->next){ if(cnt) strcat(res,", "); strncat(res,p->name,39); cnt++; }
        freeList(pals);
        if(!cnt) strcpy(res,"No palindromic words found.");
        drawTextWrapped(font,res,mx+20,fy,mw-40,FONT_SM,COL_TXT_SEC); fy+=120;

        Vector2 mp=GetMousePosition();
        bool hClose=CheckCollisionPointRec(mp,(Rectangle){mx+20,fy,mw-40,32});
        drawButton(font,"Close",mx+20,fy,mw-40,32,COL_SURFACE,COL_TXT_SEC,COL_BORDER,hClose);
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&hClose) gModal=MODAL_NONE;
    }

    /* Text input handling */
    if(gModal==MODAL_ADD||gModal==MODAL_EDIT||
       gModal==MODAL_SEARCH_DATE||gModal==MODAL_COUNT){
        char *fields[]={fName,fDef,fBirth,fDeath,NULL};
        /* Which field to edit */
        int activeF=(gModal==MODAL_SEARCH_DATE||gModal==MODAL_COUNT)?2:fFocus;
        char *tgt=fields[activeF];
        int maxLen[]={39,399,9,9};
        int mxl=(gModal==MODAL_SEARCH_DATE||gModal==MODAL_COUNT)?79:maxLen[activeF];

        int key=GetCharPressed();
        while(key>0){
            int l=(int)strlen(tgt);
            if(l<mxl&&key>=32){tgt[l]=(char)key;tgt[l+1]=0;}
            key=GetCharPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE)){int l=(int)strlen(tgt);if(l>0)tgt[l-1]=0;}
        if(IsKeyPressed(KEY_TAB)&&(gModal==MODAL_ADD||gModal==MODAL_EDIT))
            fFocus=(fFocus+1)%4;
        /* click to focus */
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&(gModal==MODAL_ADD||gModal==MODAL_EDIT)){
            Vector2 mp=GetMousePosition();
            float fy2=my+56;
            if(CheckCollisionPointRec(mp,(Rectangle){mx+20,fy2+14,mw-40,28})) fFocus=0; fy2+=50;
            if(CheckCollisionPointRec(mp,(Rectangle){mx+20,fy2+14,mw-40,72})) fFocus=1; fy2+=90;
            if(CheckCollisionPointRec(mp,(Rectangle){mx+20,fy2+14,(mw-50)/2,28})) fFocus=2;
            if(CheckCollisionPointRec(mp,(Rectangle){mx+20+(mw-50)/2+10,fy2+14,(mw-50)/2,28})) fFocus=3;
        }
    }
}

/* ─── Sidebar ─── */

static const char *sectionLabels[]={"Linked Lists","Structures","Operations","Recursion"};
static int lastSection=-1;

static void drawSidebar(Font font){
    drawRect(0,0,SIDEBAR_W,WIN_H,COL_BG);
    DrawLineEx((Vector2){SIDEBAR_W,0},(Vector2){SIDEBAR_W,WIN_H},1,COL_BORDER);

    /* Header */
    drawRect(0,0,SIDEBAR_W,52,COL_BG);
    DrawTextEx(font,"Algeria History DB",(Vector2){14,14},FONT_SIZE,1,COL_TXT_PRI);
    DrawTextEx(font,"NSCS · 2025/2026",(Vector2){14,32},11,1,COL_TXT_TER);
    DrawLineEx((Vector2){0,52},(Vector2){SIDEBAR_W,52},1,COL_BORDER);

    Vector2 mp=GetMousePosition();
    float y=56;

    for(int i=0;i<(int)SB_COUNT;i++){
        int sec=sbItems[i].section;
        if(sec!=lastSection){
            lastSection=sec;
            DrawTextEx(font,sectionLabels[sec],(Vector2){14,y},10,1,COL_TXT_TER);
            y+=20;
        }
        bool hover=CheckCollisionPointRec(mp,(Rectangle){0,y,SIDEBAR_W,28})&&gModal==MODAL_NONE;
        bool active=(i==gSBActive);
        Color bg=active?COL_SURFACE:(hover?COL_SURFACE:COL_BG);
        drawRect(0,y,SIDEBAR_W,28,bg);
        if(active) drawRect(0,y,3,28,COL_ACCENT);
        Color tc=active?COL_TXT_PRI:(hover?COL_TXT_PRI:COL_TXT_SEC);
        /* dot */
        DrawCircle((int)(14+3),(int)(y+14),3,COL_TXT_TER);
        DrawTextEx(font,sbItems[i].label,(Vector2){26,y+7},FONT_SM,1,tc);
        y+=28;

        /* section separator */
        if(i<(int)SB_COUNT-1&&sbItems[i+1].section!=sec){
            DrawLineEx((Vector2){0,y},(Vector2){SIDEBAR_W,y},1,COL_BORDER);
            y+=4; lastSection=-1;
        }
    }

    /* click */
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&gModal==MODAL_NONE){
        y=56; lastSection=-1;
        for(int i=0;i<(int)SB_COUNT;i++){
            int sec=sbItems[i].section;
            if(sec!=lastSection){lastSection=sec;y+=20;}
            if(CheckCollisionPointRec(mp,(Rectangle){0,y,SIDEBAR_W,28})){
                gSBActive=i;
                /* map to action */
                switch(i){
                    case 0: gView=VIEW_LIST; break;
                    case 1: /* events — show subset */ gView=VIEW_LIST; break;
                    case 2: gView=VIEW_LIST; break;
                    case 3: gView=VIEW_STACK; break;
                    case 4: gView=VIEW_QUEUE; break;
                    case 5: gView=VIEW_BST; break;
                    case 6: gMerged=sortWord(gMerged); showToast("Sorted by name."); break;
                    case 7: gMerged=sortPersonality(gMerged); showToast("Sorted by age."); break;
                    case 8: gModal=MODAL_PALINDROMES; break;
                    case 9:{
                        int a=listCount(gMerged);
                        char msg[100]; snprintf(msg,99,"Merged list: %d nodes total.",a);
                        showToast(msg); break;
                    }
                    case 10: gModal=MODAL_SEARCH_DATE; fBirth[0]=0; gOpResult[0]=0; break;
                    case 11: gModal=MODAL_COUNT; fBirth[0]=0; gOpResult[0]=0; break;
                }
            }
            y+=28;
            if(i<(int)SB_COUNT-1&&sbItems[i+1].section!=sec){y+=4;lastSection=-1;}
        }
    }
}

/* ─── Table ─── */

static void drawTable(Font font, int tableX, int tableY, int tableW, int tableH){
    /* Build filtered list */
    TList *filtered[512]; int fCount=0;
    for(TList *p=gMerged;p&&fCount<512;p=p->next)
        if(strContains(p->name,gSearch)) filtered[fCount++]=p;

    /* Header */
    drawRect(tableX,tableY,tableW,28,COL_BG);
    DrawLineEx((Vector2){tableX,tableY+28},(Vector2){tableX+tableW,tableY+28},1,COL_BORDER);

    float colW[]={36,200,100,100,90};
    const char *colH[]={"#","Name","Birth","Death","Type"};
    float cx=tableX+8;
    for(int c=0;c<5;c++){
        DrawTextEx(font,colH[c],(Vector2){cx,tableY+8},11,1,COL_TXT_TER);
        cx+=colW[c];
    }

    /* Rows */
    int visRows=(tableH-28)/ROW_H;
    int maxScroll=fCount-visRows; if(maxScroll<0) maxScroll=0;
    if(gScrollY<0) gScrollY=0; if(gScrollY>maxScroll) gScrollY=maxScroll;

    BeginScissorMode(tableX,tableY+28,tableW,tableH-28);
    Vector2 mp=GetMousePosition();
    for(int i=gScrollY;i<fCount&&(i-gScrollY)<visRows+1;i++){
        TList *r=filtered[i];
        float ry=tableY+28+(i-gScrollY)*ROW_H;
        bool isSel=(i==gSelected);
        bool hover=CheckCollisionPointRec(mp,(Rectangle){tableX,ry,tableW,ROW_H})&&gModal==MODAL_NONE;
        Color bg=isSel?COL_SEL_BG:(hover?COL_HOVER:COL_SURFACE);
        drawRect(tableX,ry,tableW,ROW_H,bg);
        DrawLineEx((Vector2){tableX,ry+ROW_H},(Vector2){tableX+tableW,ry+ROW_H},1,COL_BORDER);

        cx=tableX+8;
        char idx[16]; snprintf(idx,16,"%02d",i+1);
        DrawTextEx(font,idx,(Vector2){cx,ry+11},FONT_SM,1,COL_TXT_TER); cx+=colW[0];
        DrawTextEx(font,r->name,(Vector2){cx,ry+11},FONT_SM,1,COL_TXT_PRI); cx+=colW[1];
        DrawTextEx(font,r->date.birth,(Vector2){cx,ry+11},FONT_SM,1,COL_TXT_SEC); cx+=colW[2];
        DrawTextEx(font,r->date.death,(Vector2){cx,ry+11},FONT_SM,1,COL_TXT_SEC); cx+=colW[3];

        /* Type tag */
        Color bg2,fg2;
        const char *typeLabel;
        bool killed=isPersonalityKilled(r->difinition);
        if(killed){bg2=COL_TAG_MAR;fg2=COL_TAGF_MAR;typeLabel="Martyr";}
        else if(r->type==1){bg2=COL_TAG_POL;fg2=COL_TAGF_POL;typeLabel="Event";}
        else{bg2=COL_TAG_MIL;fg2=COL_TAGF_MIL;typeLabel="Military";}
        drawTag(font,typeLabel,cx,ry+9,bg2,fg2);

        if(hover&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) gSelected=i;
    }
    EndScissorMode();

    /* Scroll */
    float wheel=GetMouseWheelMove();
    if(wheel!=0&&CheckCollisionPointRec(mp,(Rectangle){tableX,tableY,tableW,tableH}))
        gScrollY-=(int)wheel*2;
}

/* ─── Stack / Queue / BST views ─── */

static void drawStructureView(Font font, int x, int y, int w, int h){
    int cnt=listCount(gMerged);
    if(cnt==0){
        DrawTextEx(font,"No data.",(Vector2){x+20,y+20},FONT_SM,1,COL_TXT_TER);
        return;
    }

    /* For Stack: draw top-to-bottom, each node as a box with arrow */
    float bw=w-60, bh=44;
    float bx=x+30, by=y+10;
    float gap=10;
    int maxVis=(int)((h-20)/(bh+gap+14));

    TList *arr[256]; int ac=0;
    for(TList *p=gMerged;p&&ac<256;p=p->next) arr[ac++]=p;

    if(gView==VIEW_STACK){
        /* reverse order: top of stack = last inserted */
        DrawTextEx(font,"TOP",(Vector2){bx+bw/2-12,by-2},10,1,COL_ACCENT);
    }

    int start=(gView==VIEW_STACK)?ac-1:0;
    int dir=(gView==VIEW_STACK)?-1:1;
    int displayed=0;

    for(int i=start;displayed<maxVis&&i>=0&&i<ac;i+=dir,displayed++){
        TList *r=arr[i];
        bool isSel=((gView==VIEW_STACK)?(ac-1-i):(i)==gSelected);
        Color bg=isSel?COL_SEL_BG:COL_SURFACE;
        DrawRectangleRounded((Rectangle){bx,by,bw,bh},0.15f,4,bg);
        DrawRectangleRoundedLinesEx((Rectangle){bx,by,bw,bh},0.15f,4,isSel?1.5f:1,
                                     isSel?COL_ACCENT:COL_BORDER);

        char lbl[120];
        int age=atoi(r->date.death)-atoi(r->date.birth);
        snprintf(lbl,120,"%s  [%s - %s]  age: %d",r->name,r->date.birth,r->date.death,age>0?age:0);
        DrawTextEx(font,lbl,(Vector2){bx+10,by+14},FONT_SM,1,COL_TXT_PRI);

        /* Arrow */
        if(displayed<maxVis-1&&((gView==VIEW_STACK&&i>0)||(gView!=VIEW_STACK&&i<ac-1))){
            float ax=bx+bw/2; float ay=by+bh;
            DrawLineEx((Vector2){ax,ay},(Vector2){ax,ay+gap+2},2,COL_TXT_TER);
            DrawTriangle((Vector2){ax,ay+gap+10},
                         (Vector2){ax-5,ay+gap+2},
                         (Vector2){ax+5,ay+gap+2},COL_TXT_TER);
        }
        by+=bh+gap+14;
    }

    if(gView==VIEW_STACK)
        DrawTextEx(font,"BOTTOM",(Vector2){bx+bw/2-18,by},10,1,COL_TXT_TER);
    if(gView==VIEW_QUEUE){
        DrawTextEx(font,"HEAD",(Vector2){bx-28,y+10+bh/2-6},10,1,COL_ACCENT);
        DrawTextEx(font,"TAIL",(Vector2){bx+bw+6,y+10+(displayed-1)*(bh+gap+14)+bh/2-6},10,1,COL_ACCENT2);
    }
}

/* ─── Detail panel ─── */

static void drawDetail(Font font, int x, int y, int w, int h){
    drawRect(x,0,w,WIN_H,COL_SURFACE);
    DrawLineEx((Vector2){x,0},(Vector2){x,WIN_H},1,COL_BORDER);

    TList *r=listGet(gMerged,gSelected);

    /* Header */
    drawRect(x,y,w,52,COL_SURFACE);
    if(r){
        DrawTextEx(font,r->name,(Vector2){x+14,y+10},FONT_SIZE,1,COL_TXT_PRI);
        char sub[60];
        int age=atoi(r->date.death)-atoi(r->date.birth);
        snprintf(sub,59,"%s – %s · Age: %d",r->date.birth,r->date.death,age>0?age:0);
        DrawTextEx(font,sub,(Vector2){x+14,y+30},11,1,COL_TXT_SEC);
    } else {
        DrawTextEx(font,"Select a record",(Vector2){x+14,y+18},FONT_SM,1,COL_TXT_TER);
    }
    DrawLineEx((Vector2){x,y+52},(Vector2){x+w,y+52},1,COL_BORDER);

    /* Body */
    if(r){
        float dy=y+62; float lx=x+14; float fw=w-28;
        DrawTextEx(font,"DEFINITION",(Vector2){lx,dy},10,1,COL_TXT_TER); dy+=14;
        drawTextWrapped(font,r->difinition,lx,dy,fw,11,COL_TXT_SEC); dy+=68;

        DrawTextEx(font,"DATE OF BIRTH",(Vector2){lx,dy},10,1,COL_TXT_TER); dy+=14;
        DrawTextEx(font,r->date.birth,(Vector2){lx,dy},FONT_SM,1,COL_TXT_SEC); dy+=24;

        DrawTextEx(font,"DATE OF DEATH",(Vector2){lx,dy},10,1,COL_TXT_TER); dy+=14;
        DrawTextEx(font,r->date.death,(Vector2){lx,dy},FONT_SM,1,COL_TXT_SEC); dy+=24;

        /* Linked list pointers */
        TList *prev=NULL,*next=NULL;
        TList *p=gMerged; TList *pr=NULL;
        while(p&&p!=r){pr=p;p=p->next;}
        if(p){prev=pr;next=p->next;}
        DrawTextEx(font,"DATA STRUCTURE",(Vector2){lx,dy},10,1,COL_TXT_TER); dy+=14;
        char nxt[60],prv[60];
        snprintf(nxt,59,"next → %s",next?next->name:"NULL");
        snprintf(prv,59,"prev → %s",prev?prev->name:"NULL");
        DrawTextEx(font,nxt,(Vector2){lx,dy},11,1,COL_TXT_SEC); dy+=16;
        DrawTextEx(font,prv,(Vector2){lx,dy},11,1,COL_TXT_SEC); dy+=24;

        /* Flags */
        DrawTextEx(font,"FLAGS",(Vector2){lx,dy},10,1,COL_TXT_TER); dy+=14;
        bool pal=is_pallindrome(r->name);
        bool killed=isPersonalityKilled(r->difinition);
        char flags[80];
        snprintf(flags,79,"Palindrome: %s   Killed: %s",pal?"Yes":"No",killed?"Yes":"No");
        DrawTextEx(font,flags,(Vector2){lx,dy},11,1,COL_TXT_SEC);
    }

    /* Footer buttons */
    int fy=WIN_H-STATUSBAR_H-42;
    DrawLineEx((Vector2){x,fy},(Vector2){x+w,fy},1,COL_BORDER);

    Vector2 mp=GetMousePosition();
    bool hEdit=CheckCollisionPointRec(mp,(Rectangle){x+12,fy+8,w/2-18,26})&&gModal==MODAL_NONE;
    bool hDel=CheckCollisionPointRec(mp,(Rectangle){x+w/2+6,fy+8,w/2-18,26})&&gModal==MODAL_NONE;
    drawButton(font,"Edit",x+12,fy+8,w/2-18,26,COL_SURFACE,COL_TXT_PRI,COL_BORDER,hEdit);
    drawButton(font,"Delete",x+w/2+6,fy+8,w/2-18,26,COL_RED_BG,COL_RED_FG,COL_RED_BR,hDel);

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        if(hEdit&&r){
            gModal=MODAL_EDIT; fFocus=0; fEditIdx=gSelected;
            strncpy(fName,r->name,39);
            strncpy(fDef,r->difinition,399);
            strncpy(fBirth,r->date.birth,9);
            strncpy(fDeath,r->date.death,9);
        }
        if(hDel&&r) gModal=MODAL_CONFIRM_DEL;
    }
}

/* ─── Main ─── */

int main(void){
    SetConfigFlags(FLAG_WINDOW_RESIZABLE|FLAG_MSAA_4X_HINT);
    InitWindow(WIN_W, WIN_H, "Algeria History DB — NSCS 2025/2026");
    SetTargetFPS(60);
    SetExitKey(0);

    /* Load or seed data */
    FILE *f=fopen(DATA_FILE,"r");
    if(f){fclose(f); loadFromFile();}
    else seedData();

    /* Font */
    Font font=LoadFontEx("resources/JetBrainsMono-Regular.ttf",18,NULL,0);
    if(font.texture.id==0) font=GetFontDefault();

    while(!WindowShouldClose()){
        float dt=GetFrameTime();
        if(gToastTimer>0) gToastTimer-=dt;

        /* ESC closes modal */
        if(IsKeyPressed(KEY_ESCAPE)){
            if(gModal!=MODAL_NONE) gModal=MODAL_NONE;
        }

        /* Search input (when no modal) */
        if(gModal==MODAL_NONE&&gSearchFocus){
            int key=GetCharPressed();
            while(key>0){
                int l=(int)strlen(gSearch);
                if(l<79&&key>=32){gSearch[l]=(char)key;gSearch[l+1]=0;}
                key=GetCharPressed();
            }
            if(IsKeyPressed(KEY_BACKSPACE)){int l=(int)strlen(gSearch);if(l>0)gSearch[l-1]=0;}
        }

        BeginDrawing();
        ClearBackground(COL_SURFACE);

        int W=GetScreenWidth();
        int H=GetScreenHeight();

        /* ── Sidebar ── */
        lastSection=-1;
        drawSidebar(font);

        /* ── Main area ── */
        int mainX=SIDEBAR_W;
        int mainW=W-SIDEBAR_W-DETAIL_W;

        /* Topbar */
        drawRect(mainX,0,mainW,TOPBAR_H,COL_SURFACE);
        DrawLineEx((Vector2){mainX,TOPBAR_H},(Vector2){mainX+mainW,TOPBAR_H},1,COL_BORDER);

        const char *titles[]={
            "Personalities · Linked List","Events · Linked List","Dates · Linked List",
            "Personalities · Stack","Personalities · Queue","Personalities · BST"
        };
        int tidx=gSBActive<3?gSBActive:(gView==VIEW_STACK?3:gView==VIEW_QUEUE?4:5);
        if(tidx>5) tidx=0;
        DrawTextEx(font,titles[tidx],(Vector2){mainX+14,14},FONT_SIZE,1,COL_TXT_PRI);

        /* Search box */
        float sx=mainX+mainW-210;
        Vector2 mp=GetMousePosition();
        bool searchHover=CheckCollisionPointRec(mp,(Rectangle){sx,10,160,26});
        drawRect(sx,10,160,26,COL_BG);
        DrawRectangleLinesEx((Rectangle){sx,10,160,26},1,gSearchFocus?COL_ACCENT:COL_BORDER);
        DrawTextEx(font,gSearch[0]?gSearch:"Search...",(Vector2){sx+8,16},FONT_SM,1,
                   gSearch[0]?COL_TXT_PRI:COL_TXT_TER);
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&gModal==MODAL_NONE)
            gSearchFocus=searchHover;

        /* Add button */
        float abx=sx+168;
        bool addHover=CheckCollisionPointRec(mp,(Rectangle){abx,10,50,26})&&gModal==MODAL_NONE;
        drawButton(font,"+ Add",abx,10,50,26,COL_ACCENT,WHITE,COL_ACCENT,addHover);
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&addHover){
            gModal=MODAL_ADD; fFocus=0;
            fName[0]=fDef[0]=fBirth[0]=fDeath[0]=0;
        }

        /* Content area */
        int contentY=TOPBAR_H;
        int contentH=H-TOPBAR_H-STATUSBAR_H;

        if(gView==VIEW_LIST){
            drawTable(font,mainX,contentY,mainW,contentH);
        } else {
            drawRect(mainX,contentY,mainW,contentH,COL_SURFACE);
            drawStructureView(font,mainX,contentY,mainW,contentH);
        }

        /* Status bar */
        drawRect(mainX,H-STATUSBAR_H,mainW,STATUSBAR_H,COL_BG);
        DrawLineEx((Vector2){mainX,H-STATUSBAR_H},(Vector2){mainX+mainW,H-STATUSBAR_H},1,COL_BORDER);
        int cnt=listCount(gMerged);
        char status[120];
        const char *viewNames[]={"bidirectional","stack (LIFO)","queue (FIFO)","BST in-order"};
        snprintf(status,119,"%d personalities loaded   ·   List: %s   ·   Sorted: alphabetical",
                 cnt,viewNames[gView]);
        DrawTextEx(font,status,(Vector2){mainX+14,H-STATUSBAR_H+6},11,1,COL_TXT_TER);

        /* ── Detail panel ── */
        drawDetail(font,W-DETAIL_W,0,DETAIL_W,H);

        /* ── Modal ── */
        if(gModal!=MODAL_NONE) drawModal(font);

        /* ── Toast ── */
        if(gToastTimer>0){
            float alpha=gToastTimer>0.5f?1.f:gToastTimer/0.5f;
            Color tbg=COL_TOAST_BG; tbg.a=(unsigned char)(200*alpha);
            Color tfg=COL_TOAST_FG; tfg.a=(unsigned char)(255*alpha);
            float tw=MeasureTextEx(font,gToast,FONT_SM,1).x+24;
            float tx=(W-tw)/2; float ty=H-STATUSBAR_H-52;
            DrawRectangleRounded((Rectangle){tx,ty,tw,30},0.4f,6,tbg);
            DrawTextEx(font,gToast,(Vector2){tx+12,ty+8},FONT_SM,1,tfg);
        }

        EndDrawing();
    }

    freeList(gMerged);
    freeList(gPersonNames);
    freeList(gPersonDates);
    UnloadFont(font);
    CloseWindow();
    return 0;
}
