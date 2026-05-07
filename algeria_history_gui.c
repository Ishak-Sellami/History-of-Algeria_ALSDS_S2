/*
 * Algeria History DB — Raylib GUI
 * Build:
 *   gcc algeria_history_gui.c -o algeria_history_gui \
 *       -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
 */

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/* ═══════════════════════════════════════════════════════════
   DATA STRUCTURES  (from your module)
   ═══════════════════════════════════════════════════════════ */

typedef struct TList {
    char name[40];
    char difinition[400];   /* personality definition */
    struct {
        char birth[10];
        char death[10];
    } date;
    char dateB[20];         /* event date string */
    char dateE[10];
    int  type;              /* 0 = personality, 1 = event */
    struct TList *next;
    struct TList *prv;
} TList;

/* ═══════════════════════════════════════════════════════════
   YOUR EXACT FUNCTIONS  (from your module)
   ═══════════════════════════════════════════════════════════ */

/* ---------- addEvents (your exact code) ------------------- */
void addEvents(TList **b, char *namEvente, char *date){
    TList *newnode = malloc(sizeof(TList));
    memset(newnode, 0, sizeof(TList));
    newnode->next = NULL;
    strcpy(newnode->name, namEvente);
    strcpy(newnode->dateB, date);
    newnode->type = 1;

    if(*b == NULL) *b = newnode;
    else{
        TList *ptr = *b;
        while(ptr->next)
            ptr = ptr->next;
        ptr->next = newnode;
    }

    FILE *f = fopen("Algeria_history.txt", "a");
    if(f){
        fprintf(f, "%s:{%s}\n", namEvente, date);
        fclose(f);
    }
}

/* ---------- personalities --------------------------------- */
static void addPersonality(TList **s, TList **a,
                            const char *name, const char *def,
                            const char *DoB, const char *DoD){
    TList *ns = calloc(1, sizeof(TList)); ns->type = 0;
    strncpy(ns->name,      name, 39);
    strncpy(ns->difinition, def, 399);
    if(!*s) *s = ns; else { TList *p=*s; while(p->next) p=p->next; p->next=ns; }

    TList *na = calloc(1, sizeof(TList)); na->type = 0;
    strncpy(na->name,       name, 39);
    strncpy(na->date.birth, DoB,   9);
    strncpy(na->date.death, DoD,   9);
    if(!*a) *a = na; else { TList *p=*a; while(p->next) p=p->next; p->next=na; }
}

static TList* mergePersonalities(TList *names, TList *dates){
    TList *head = NULL, *tail = NULL;
    for(TList *s = names; s; s = s->next){
        for(TList *a = dates; a; a = a->next){
            if(strcmp(s->name, a->name) == 0){
                TList *n = calloc(1, sizeof(TList)); n->type = 0;
                strcpy(n->name,       s->name);
                strcpy(n->difinition, s->difinition);
                strcpy(n->date.birth, a->date.birth);
                strcpy(n->date.death, a->date.death);
                if(!head){ head=n; tail=n; }
                else { tail->next=n; n->prv=tail; tail=n; }
                break;
            }
        }
    }
    return head;
}

static void deleteFromList(TList **list, const char *name){
    TList *ptr = *list, *prev = NULL;
    while(ptr && strcmp(ptr->name, name) != 0){ prev=ptr; ptr=ptr->next; }
    if(ptr){
        if(!prev) *list = ptr->next; else prev->next = ptr->next;
        if(ptr->next) ptr->next->prv = prev;
        free(ptr);
    }
}

static void updatePersonality(TList *list, const char *name,
                               const char *def, const char *b, const char *d){
    for(TList *p = list; p; p = p->next)
        if(strcmp(p->name, name) == 0){
            strncpy(p->difinition,  def, 399);
            strncpy(p->date.birth,  b,     9);
            strncpy(p->date.death,  d,     9);
            break;
        }
}

static void sortByName(TList *head){
    for(TList *i = head; i; i = i->next)
        for(TList *j = i->next; j; j = j->next)
            if(strcmp(i->name, j->name) > 0){
                char tn[40], td[400], tb[10], tde[10], tB[20];
                strcpy(tn,  i->name);        strcpy(i->name,        j->name);        strcpy(j->name,  tn);
                strcpy(td,  i->difinition);  strcpy(i->difinition,  j->difinition);  strcpy(j->difinition, td);
                strcpy(tb,  i->date.birth);  strcpy(i->date.birth,  j->date.birth);  strcpy(j->date.birth, tb);
                strcpy(tde, i->date.death);  strcpy(i->date.death,  j->date.death);  strcpy(j->date.death,tde);
                strcpy(tB,  i->dateB);       strcpy(i->dateB,       j->dateB);       strcpy(j->dateB, tB);
                int tt=i->type; i->type=j->type; j->type=tt;
            }
}

static void sortByAge(TList *head){
    for(TList *i = head; i; i = i->next)
        for(TList *j = i->next; j; j = j->next){
            int a1 = atoi(i->date.death) - atoi(i->date.birth);
            int a2 = atoi(j->date.death) - atoi(j->date.birth);
            if(a1 > a2){
                char tn[40], td[400], tb[10], tde[10];
                strcpy(tn,  i->name);       strcpy(i->name,       j->name);       strcpy(j->name,  tn);
                strcpy(td,  i->difinition); strcpy(i->difinition, j->difinition); strcpy(j->difinition,td);
                strcpy(tb,  i->date.birth); strcpy(i->date.birth, j->date.birth); strcpy(j->date.birth,tb);
                strcpy(tde, i->date.death); strcpy(i->date.death, j->date.death); strcpy(j->date.death,tde);
            }
        }
}

static int isPalindrome(const char *w){
    int i=0, j=(int)strlen(w)-1;
    while(i<j){ if(tolower((unsigned char)w[i])!=tolower((unsigned char)w[j])) return 0; i++; j--; }
    return 1;
}

static bool isKilled(const char *def){
    char tmp[400]; strncpy(tmp,def,399); tmp[399]=0;
    for(int i=0;tmp[i];i++) tmp[i]=tolower((unsigned char)tmp[i]);
    const char *kw[]={"killed","assassinated","murdered","executed","shot dead"};
    for(int i=0;i<5;i++) if(strstr(tmp,kw[i])) return true;
    return false;
}

static void freeList(TList *h){ while(h){ TList *n=h->next; free(h); h=n; } }
static int  listCount(TList *h){ int c=0; for(;h;h=h->next) c++; return c; }
static TList* listGet(TList *h, int idx){ for(int i=0;h&&i<idx;i++,h=h->next); return h; }

/* ═══════════════════════════════════════════════════════════
   FILE I/O
   ═══════════════════════════════════════════════════════════ */

#define DATA_FILE "Algeria_history.txt"

static TList *gPersonNames = NULL;
static TList *gPersonDates = NULL;
static TList *gMerged      = NULL;   /* personalities */
static TList *gEvents      = NULL;   /* events        */

/* Parse personalities:  Name=definition{YYYY-YYYY}  */
static void loadPersonalities(FILE *f){
    char line[600];
    while(fgets(line, sizeof(line), f)){
        /* skip event lines (contain ':{') */
        if(strstr(line, ":{")) continue;
        if(!strchr(line, '=')) continue;

        char name[40]={0}, def[400]={0}, birth[10]={0}, death[10]={0};
        sscanf(line, "%39[^=]=%399[^{]", name, def);
        /* trim trailing space from def */
        int dl=(int)strlen(def);
        while(dl>0 && (def[dl-1]==' '||def[dl-1]=='\n'||def[dl-1]=='\r')) def[--dl]=0;

        char *p = strchr(line, '{');
        if(p) sscanf(p, "{%9[^-]-%9[^}]}", birth, death);

        if(!name[0]) continue;

        /* names list */
        TList *ns = calloc(1, sizeof(TList)); ns->type=0;
        strcpy(ns->name, name); strcpy(ns->difinition, def);
        if(!gPersonNames) gPersonNames=ns;
        else{ TList *t=gPersonNames; while(t->next) t=t->next; t->next=ns; }

        /* dates list */
        TList *na = calloc(1, sizeof(TList)); na->type=0;
        strcpy(na->name, name);
        strcpy(na->date.birth, birth); strcpy(na->date.death, death);
        if(!gPersonDates) gPersonDates=na;
        else{ TList *t=gPersonDates; while(t->next) t=t->next; t->next=na; }
    }
}

/* Parse events:  Name:{date}  */
static void loadEvents(FILE *f){
    char line[300];
    while(fgets(line, sizeof(line), f)){
        if(!strstr(line, ":{")) continue;
        char name[40]={0}, date[20]={0};
        char *colon = strchr(line, ':');
        if(!colon) continue;
        int nl = (int)(colon - line);
        if(nl<=0||nl>=40) continue;
        strncpy(name, line, nl); name[nl]=0;
        char *p = strchr(colon, '{');
        if(p) sscanf(p, "{%19[^}]}", date);
        if(!name[0]) continue;

        TList *n = calloc(1, sizeof(TList)); n->type=1;
        strcpy(n->name,  name);
        strcpy(n->dateB, date);
        if(!gEvents) gEvents=n;
        else{ TList *t=gEvents; while(t->next) t=t->next; t->next=n; }
    }
}

static void loadFromFile(void){
    FILE *f = fopen(DATA_FILE, "r");
    if(!f) return;
    freeList(gPersonNames); gPersonNames=NULL;
    freeList(gPersonDates); gPersonDates=NULL;
    freeList(gMerged);      gMerged=NULL;
    freeList(gEvents);      gEvents=NULL;
    loadPersonalities(f);
    rewind(f);
    loadEvents(f);
    fclose(f);
    gMerged = mergePersonalities(gPersonNames, gPersonDates);
}

/* Rewrite personality entries only (events written by addEvents itself) */
static void savePersonalities(void){
    /* Read existing event lines, rewrite file with updated personalities + old events */
    char evLines[4096]={0};
    FILE *fr = fopen(DATA_FILE, "r");
    if(fr){
        char line[300];
        while(fgets(line, sizeof(line), fr))
            if(strstr(line,":{")) strncat(evLines, line, sizeof(evLines)-strlen(evLines)-1);
        fclose(fr);
    }
    FILE *f = fopen(DATA_FILE, "w");
    if(!f) return;
    for(TList *p = gMerged; p; p = p->next)
        fprintf(f, "%s=%s{%s-%s}\n", p->name, p->difinition, p->date.birth, p->date.death);
    fprintf(f, "%s", evLines);
    fclose(f);
}

/* ─── Seed ─── */
static void seedData(void){
    addPersonality(&gPersonNames,&gPersonDates,"Emir Abdelkader",
        "Algerian military and political leader who resisted French colonial rule for 15 years.",
        "1808","1883");
    addPersonality(&gPersonNames,&gPersonDates,"Larbi Ben M'hidi",
        "FLN founder and revolutionary leader executed by French paratroopers in Algiers.",
        "1923","1957");
    addPersonality(&gPersonNames,&gPersonDates,"Hassiba Ben Bouali",
        "Young FLN fighter killed during the Battle of Algiers at age 19.",
        "1938","1957");
    addPersonality(&gPersonNames,&gPersonDates,"Ferhat Abbas",
        "Nationalist politician and first President of the Algerian provisional government.",
        "1899","1985");
    addPersonality(&gPersonNames,&gPersonDates,"Ahmed Ben Bella",
        "First President of independent Algeria after the War of Independence.",
        "1916","2012");
    addPersonality(&gPersonNames,&gPersonDates,"Houari Boumediene",
        "Second President of Algeria who nationalised oil and led socialist industrialisation.",
        "1932","1978");
    addPersonality(&gPersonNames,&gPersonDates,"Krim Belkacem",
        "Key FLN military commander who played a central role in the War of Independence.",
        "1922","1970");
    gMerged = mergePersonalities(gPersonNames, gPersonDates);

    /* seed events using YOUR function */
    addEvents(&gEvents, "Start of Revolution",  "01/11/1954");
    addEvents(&gEvents, "Battle of Algiers",    "1956-1957");
    addEvents(&gEvents, "Setif Massacre",       "08/05/1945");
    addEvents(&gEvents, "Evian Accords",        "18/03/1962");
    addEvents(&gEvents, "Independence Day",     "05/07/1962");
}

/* ═══════════════════════════════════════════════════════════
   UI CONSTANTS & PALETTE
   ═══════════════════════════════════════════════════════════ */

#define WIN_W       1100
#define WIN_H        640
#define SIDEBAR_W    190
#define DETAIL_W     260
#define TOPBAR_H      46
#define STATUSBAR_H   26
#define ROW_H         36
#define FONT_SZ       14
#define FONT_SM       12

#define C_BG        CLITERAL(Color){245,244,240,255}
#define C_SURF      CLITERAL(Color){255,255,255,255}
#define C_BORDER    CLITERAL(Color){220,218,210,255}
#define C_ACCENT    CLITERAL(Color){ 29,158,117,255}
#define C_ACCENT2   CLITERAL(Color){ 15,110, 86,255}
#define C_TXT1      CLITERAL(Color){ 26, 25, 22,255}
#define C_TXT2      CLITERAL(Color){ 90, 88, 82,255}
#define C_TXT3      CLITERAL(Color){140,138,130,255}
#define C_SEL       CLITERAL(Color){225,245,238,255}
#define C_HOVER     CLITERAL(Color){240,239,234,255}
#define C_TAG_G     CLITERAL(Color){225,245,238,255}
#define C_TAG_A     CLITERAL(Color){250,238,218,255}
#define C_TAG_B     CLITERAL(Color){230,241,251,255}
#define C_TAG_P     CLITERAL(Color){238,230,251,255}
#define C_TF_G      CLITERAL(Color){ 15,110, 86,255}
#define C_TF_A      CLITERAL(Color){133, 79, 11,255}
#define C_TF_B      CLITERAL(Color){ 24, 95,165,255}
#define C_TF_P      CLITERAL(Color){ 83, 49,165,255}
#define C_RED_BG    CLITERAL(Color){252,235,235,255}
#define C_RED_FG    CLITERAL(Color){163, 45, 45,255}
#define C_RED_BR    CLITERAL(Color){247,193,193,255}
#define C_TOAST_BG  CLITERAL(Color){ 30, 30, 28,220}
#define C_TOAST_FG  CLITERAL(Color){240,238,232,255}

/* ═══════════════════════════════════════════════════════════
   UI STATE
   ═══════════════════════════════════════════════════════════ */

typedef enum { VIEW_LIST, VIEW_STACK, VIEW_QUEUE } ViewMode;
typedef enum { TAB_PERSONALITIES=0, TAB_EVENTS=1 } TabMode;
typedef enum {
    MODAL_NONE, MODAL_ADD_P, MODAL_EDIT_P,
    MODAL_ADD_E, MODAL_EDIT_E,
    MODAL_CONFIRM_DEL,
    MODAL_SEARCH_DATE, MODAL_COUNT, MODAL_PALINDROMES
} ModalMode;

static ViewMode gView      = VIEW_LIST;
static TabMode  gTab       = TAB_PERSONALITIES;
static ModalMode gModal    = MODAL_NONE;
static int  gSelected      = 0;
static int  gScrollY       = 0;
static char gSearch[80]    = {0};
static bool gSearchFocus   = false;

/* form fields */
static char fName[40]={0}, fDef[400]={0}, fBirth[20]={0}, fDeath[10]={0};
static int  fFocus=0, fEditIdx=-1;

/* toast */
static char  gToast[300]={0};
static float gToastTimer=0.f;
static char  gOpResult[600]={0};

/* sidebar items */
typedef struct { const char *label; int section; } SBItem;
static SBItem sbItems[]={
    {"Personalities", 0}, {"Events", 0},
    {"Stack view",    1}, {"Queue view", 1},
    {"Sort by name",  2}, {"Sort by age",2},
    {"Palindromes",   2}, {"Merge lists",2},
    {"Search by date",3}, {"Count occurrences",3},
};
#define SB_COUNT (int)(sizeof(sbItems)/sizeof(sbItems[0]))
static int gSBActive   = 0;
static int lastSection = -1;

static const char *secLabels[] = {
    "Linked Lists","Structures","Operations","Recursion"
};

/* ═══════════════════════════════════════════════════════════
   UTILITIES
   ═══════════════════════════════════════════════════════════ */

static TList* activeList(void){
    return gTab==TAB_EVENTS ? gEvents : gMerged;
}

static bool strCI(const char *hay, const char *ndl){
    if(!ndl[0]) return true;
    char h[500], n[80];
    strncpy(h,hay,499); h[499]=0;
    strncpy(n,ndl,79);  n[79]=0;
    for(int i=0;h[i];i++) h[i]=tolower((unsigned char)h[i]);
    for(int i=0;n[i];i++) n[i]=tolower((unsigned char)n[i]);
    return strstr(h,n)!=NULL;
}

static void showToast(const char *msg){
    strncpy(gToast, msg, 299); gToastTimer=3.f;
}

/* ─── draw primitives ─── */

static void drect(float x,float y,float w,float h,Color c){
    DrawRectangle((int)x,(int)y,(int)w,(int)h,c);
}

static void dtag(Font font,const char *txt,float x,float y,Color bg,Color fg){
    float tw = MeasureTextEx(font,txt,11,1).x;
    DrawRectangleRounded((Rectangle){x,y,tw+12,18},0.5f,4,bg);
    DrawTextEx(font,txt,(Vector2){x+6,y+3},11,1,fg);
}

static void dbtn(Font font,const char *txt,float x,float y,float w,float h,
                 Color bg,Color fg,Color br,bool hov){
    Color b2 = hov ? (Color){bg.r-12,bg.g-12,bg.b-12,255} : bg;
    DrawRectangleRounded((Rectangle){x,y,w,h},0.35f,4,b2);
    DrawRectangleRoundedLinesEx((Rectangle){x,y,w,h},0.35f,4,1,br);
    float tw = MeasureTextEx(font,txt,FONT_SM,1).x;
    DrawTextEx(font,txt,(Vector2){x+(w-tw)/2.f,y+(h-FONT_SM)/2.f},FONT_SM,1,fg);
}

static void dwrapped(Font font,const char *text,float x,float y,
                     float maxW,float fs,Color col){
    char buf[500]; strncpy(buf,text,499); buf[499]=0;
    char line[200]={0};
    float lh=fs+5, cy=y;
    char *tok=strtok(buf," ");
    while(tok){
        char tmp[200];
        snprintf(tmp,sizeof(tmp),"%s%s%s",line,line[0]?" ":"",tok);
        if(MeasureTextEx(font,tmp,fs,1).x>maxW && line[0]){
            DrawTextEx(font,line,(Vector2){x,cy},fs,1,col);
            cy+=lh; strncpy(line,tok,199);
        } else strncpy(line,tmp,199);
        tok=strtok(NULL," ");
    }
    if(line[0]) DrawTextEx(font,line,(Vector2){x,cy},fs,1,col);
}

static void dinput(Font font,const char *lbl,const char *val,
                   float x,float y,float w,bool focused,bool multi){
    DrawTextEx(font,lbl,(Vector2){x,y},11,1,C_TXT3);
    float fh = multi?72:28;
    DrawRectangleRounded((Rectangle){x,y+14,w,fh},0.25f,4,C_BG);
    DrawRectangleRoundedLinesEx((Rectangle){x,y+14,w,fh},0.25f,4,
                                 focused?1.5f:1.f,focused?C_ACCENT:C_BORDER);
    float ty = y+14+(multi?8:(fh-FONT_SM)/2.f);
    if(multi) dwrapped(font,val,x+8,ty,w-16,FONT_SM,C_TXT1);
    else      DrawTextEx(font,val,(Vector2){x+8,ty},FONT_SM,1,C_TXT1);
    if(focused){
        float cx=x+8+MeasureTextEx(font,val,FONT_SM,1).x;
        if((int)(GetTime()*2)%2==0)
            DrawLineEx((Vector2){cx,ty},(Vector2){cx,ty+FONT_SZ},1.5f,C_ACCENT);
    }
}

/* tag for a row */
static void rowTag(Font font,TList *r,float x,float y){
    if(r->type==1){
        dtag(font,"Event",x,y,C_TAG_P,C_TF_P);
    } else {
        if(isKilled(r->difinition)) dtag(font,"Martyr",x,y,C_TAG_A,C_TF_A);
        else                        dtag(font,"Military",x,y,C_TAG_G,C_TF_G);
    }
}

/* ═══════════════════════════════════════════════════════════
   MODAL
   ═══════════════════════════════════════════════════════════ */

static void drawModal(Font font){
    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),(Color){0,0,0,120});

    float mw=400;
    float mh = (gModal==MODAL_ADD_P||gModal==MODAL_EDIT_P) ? 360 :
               (gModal==MODAL_ADD_E||gModal==MODAL_EDIT_E) ? 240 :
               (gModal==MODAL_PALINDROMES)                  ? 280 : 200;
    float mx=(WIN_W-mw)/2, my=(WIN_H-mh)/2;

    DrawRectangleRounded((Rectangle){mx,my,mw,mh},0.08f,8,C_SURF);
    DrawRectangleRoundedLinesEx((Rectangle){mx,my,mw,mh},0.08f,8,1,C_BORDER);

    const char *titles[]={
        "","Add Personality","Edit Personality",
        "Add Event","Edit Event",
        "Confirm Delete","Search by Date","Count Occurrences","Palindromes"
    };
    DrawTextEx(font,titles[gModal],(Vector2){mx+20,my+18},FONT_SZ+1,1,C_TXT1);
    DrawLineEx((Vector2){mx,my+44},(Vector2){mx+mw,my+44},1,C_BORDER);

    float fy=my+56;
    Vector2 mp=GetMousePosition();

    /* ── ADD / EDIT PERSONALITY ── */
    if(gModal==MODAL_ADD_P||gModal==MODAL_EDIT_P){
        dinput(font,"Name",        fName,  mx+20,fy,mw-40,fFocus==0,false); fy+=50;
        dinput(font,"Description", fDef,   mx+20,fy,mw-40,fFocus==1,true);  fy+=90;
        dinput(font,"Birth Year",  fBirth, mx+20,fy,(mw-50)/2,fFocus==2,false);
        dinput(font,"Death Year",  fDeath, mx+20+(mw-50)/2+10,fy,(mw-50)/2,fFocus==3,false);
        fy+=50;

        bool hSave  =CheckCollisionPointRec(mp,(Rectangle){mx+20,   fy,mw/2-25,32});
        bool hCancel=CheckCollisionPointRec(mp,(Rectangle){mx+mw/2+5,fy,mw/2-25,32});
        dbtn(font,"Save",  mx+20,    fy,mw/2-25,32,C_ACCENT,WHITE,  C_ACCENT,hSave);
        dbtn(font,"Cancel",mx+mw/2+5,fy,mw/2-25,32,C_SURF,  C_TXT2,C_BORDER,hCancel);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if(hSave && fName[0]){
                if(gModal==MODAL_ADD_P){
                    addPersonality(&gPersonNames,&gPersonDates,fName,fDef,fBirth,fDeath);
                    freeList(gMerged);
                    gMerged=mergePersonalities(gPersonNames,gPersonDates);
                    savePersonalities();
                    showToast("Personality added.");
                } else {
                    TList *r=listGet(gMerged,fEditIdx);
                    if(r){
                        strncpy(r->name,r->name,39); /* keep name, update rest */
                        updatePersonality(gMerged,r->name,fDef,fBirth,fDeath);
                        strncpy(r->name,fName,39);
                        savePersonalities();
                        showToast("Personality updated.");
                    }
                }
                gModal=MODAL_NONE;
            }
            if(hCancel) gModal=MODAL_NONE;
        }
    }

    /* ── ADD EVENT ── */
    else if(gModal==MODAL_ADD_E||gModal==MODAL_EDIT_E){
        dinput(font,"Event Name",fName, mx+20,fy,mw-40,fFocus==0,false); fy+=50;
        dinput(font,"Date (e.g. 01/11/1954 or 1956-1957)",
               fBirth, mx+20,fy,mw-40,fFocus==1,false); fy+=50;

        bool hSave  =CheckCollisionPointRec(mp,(Rectangle){mx+20,   fy,mw/2-25,32});
        bool hCancel=CheckCollisionPointRec(mp,(Rectangle){mx+mw/2+5,fy,mw/2-25,32});
        dbtn(font,"Save",  mx+20,    fy,mw/2-25,32,C_ACCENT,WHITE,  C_ACCENT,hSave);
        dbtn(font,"Cancel",mx+mw/2+5,fy,mw/2-25,32,C_SURF,  C_TXT2,C_BORDER,hCancel);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if(hSave && fName[0]){
                if(gModal==MODAL_ADD_E){
                    /* ← YOUR FUNCTION */
                    addEvents(&gEvents, fName, fBirth);
                    showToast("Event added.");
                } else {
                    /* edit: update in place */
                    TList *r=listGet(gEvents,fEditIdx);
                    if(r){
                        strncpy(r->name, fName,  39);
                        strncpy(r->dateB,fBirth, 19);
                        /* rewrite file */
                        FILE *fw=fopen(DATA_FILE,"r");
                        char buf[8192]={0};
                        if(fw){
                            char line[300];
                            while(fgets(line,sizeof(line),fw))
                                if(!strstr(line,":{"))
                                    strncat(buf,line,sizeof(buf)-strlen(buf)-1);
                            fclose(fw);
                        }
                        fw=fopen(DATA_FILE,"w");
                        if(fw){ fprintf(fw,"%s",buf); fclose(fw); }
                        /* re-append all events */
                        fw=fopen(DATA_FILE,"a");
                        if(fw){
                            for(TList *p=gEvents;p;p=p->next)
                                fprintf(fw,"%s:{%s}\n",p->name,p->dateB);
                            fclose(fw);
                        }
                        showToast("Event updated.");
                    }
                }
                gModal=MODAL_NONE;
            }
            if(hCancel) gModal=MODAL_NONE;
        }
    }

    /* ── CONFIRM DELETE ── */
    else if(gModal==MODAL_CONFIRM_DEL){
        TList *r=listGet(activeList(),gSelected);
        char msg[120]="Delete: "; if(r){ strncat(msg,r->name,39); strcat(msg,"?"); }
        DrawTextEx(font,msg,(Vector2){mx+20,fy},FONT_SM,1,C_TXT2); fy+=40;

        bool hDel=CheckCollisionPointRec(mp,(Rectangle){mx+20,   fy,mw/2-25,32});
        bool hNo =CheckCollisionPointRec(mp,(Rectangle){mx+mw/2+5,fy,mw/2-25,32});
        dbtn(font,"Delete",mx+20,    fy,mw/2-25,32,C_RED_BG,C_RED_FG,C_RED_BR,hDel);
        dbtn(font,"Cancel",mx+mw/2+5,fy,mw/2-25,32,C_SURF,  C_TXT2,  C_BORDER,hNo);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if(hDel && r){
                char name[40]; strncpy(name,r->name,39);
                if(gTab==TAB_PERSONALITIES){
                    deleteFromList(&gMerged,      name);
                    deleteFromList(&gPersonNames,  name);
                    deleteFromList(&gPersonDates,  name);
                    savePersonalities();
                } else {
                    deleteFromList(&gEvents, name);
                    /* rewrite events in file */
                    FILE *fw=fopen(DATA_FILE,"r");
                    char pbuf[8192]={0};
                    if(fw){ char line[300];
                        while(fgets(line,sizeof(line),fw))
                            if(!strstr(line,":{")) strncat(pbuf,line,sizeof(pbuf)-strlen(pbuf)-1);
                        fclose(fw); }
                    fw=fopen(DATA_FILE,"w");
                    if(fw){ fprintf(fw,"%s",pbuf);
                        for(TList *p=gEvents;p;p=p->next)
                            fprintf(fw,"%s:{%s}\n",p->name,p->dateB);
                        fclose(fw); }
                }
                int cnt=listCount(activeList());
                if(gSelected>=cnt) gSelected=cnt-1;
                if(gSelected<0)    gSelected=0;
                showToast("Record deleted.");
            }
            if(hDel||hNo) gModal=MODAL_NONE;
        }
    }

    /* ── SEARCH BY DATE ── */
    else if(gModal==MODAL_SEARCH_DATE){
        dinput(font,"Year (e.g. 1957)",fBirth,mx+20,fy,mw-40,true,false); fy+=50;
        dwrapped(font,gOpResult,mx+20,fy,mw-40,FONT_SM,C_TXT2); fy+=60;

        bool hRun  =CheckCollisionPointRec(mp,(Rectangle){mx+20,   fy,mw/2-25,32});
        bool hClose=CheckCollisionPointRec(mp,(Rectangle){mx+mw/2+5,fy,mw/2-25,32});
        dbtn(font,"Search",mx+20,    fy,mw/2-25,32,C_ACCENT,WHITE,  C_ACCENT,hRun);
        dbtn(font,"Close", mx+mw/2+5,fy,mw/2-25,32,C_SURF,  C_TXT2,C_BORDER,hClose);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if(hRun && fBirth[0]){
                int cnt=0; char res[500]="Found: ";
                for(TList *p=gMerged;p;p=p->next)
                    if(strstr(p->date.birth,fBirth)||strstr(p->date.death,fBirth)){
                        if(cnt) strcat(res,", "); strncat(res,p->name,39); cnt++;
                    }
                for(TList *p=gEvents;p;p=p->next)
                    if(strstr(p->dateB,fBirth)){
                        if(cnt) strcat(res,", "); strncat(res,p->name,39); cnt++;
                    }
                if(!cnt) strcpy(res,"No records found.");
                strncpy(gOpResult,res,599);
            }
            if(hClose){ gModal=MODAL_NONE; gOpResult[0]=0; fBirth[0]=0; }
        }
    }

    /* ── COUNT OCCURRENCES ── */
    else if(gModal==MODAL_COUNT){
        dinput(font,"Search term",fBirth,mx+20,fy,mw-40,true,false); fy+=50;
        dwrapped(font,gOpResult,mx+20,fy,mw-40,FONT_SM,C_TXT2); fy+=60;

        bool hRun  =CheckCollisionPointRec(mp,(Rectangle){mx+20,   fy,mw/2-25,32});
        bool hClose=CheckCollisionPointRec(mp,(Rectangle){mx+mw/2+5,fy,mw/2-25,32});
        dbtn(font,"Count",mx+20,    fy,mw/2-25,32,C_ACCENT,WHITE,  C_ACCENT,hRun);
        dbtn(font,"Close",mx+mw/2+5,fy,mw/2-25,32,C_SURF,  C_TXT2,C_BORDER,hClose);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if(hRun && fBirth[0]){
                int cnt=0;
                for(TList *p=gMerged;p;p=p->next)
                    if(strCI(p->name,fBirth)||strCI(p->difinition,fBirth)) cnt++;
                for(TList *p=gEvents;p;p=p->next)
                    if(strCI(p->name,fBirth)||strCI(p->dateB,fBirth)) cnt++;
                snprintf(gOpResult,599,"'%s' appears in %d record(s).",fBirth,cnt);
            }
            if(hClose){ gModal=MODAL_NONE; gOpResult[0]=0; fBirth[0]=0; }
        }
    }

    /* ── PALINDROMES ── */
    else if(gModal==MODAL_PALINDROMES){
        /* check names in active list */
        char res[800]=""; int cnt=0;
        for(TList *p=activeList();p;p=p->next){
            /* check each word of definition */
            char tmp[400]; strncpy(tmp,p->difinition[0]?p->difinition:p->name,399); tmp[399]=0;
            char *tok=strtok(tmp," ,.-");
            while(tok){
                if(strlen(tok)>1 && isPalindrome(tok)){
                    if(cnt) strcat(res,", "); strncat(res,tok,39); cnt++;
                }
                tok=strtok(NULL," ,.-");
            }
        }
        if(!cnt) strcpy(res,"No palindromic words found.");
        dwrapped(font,res,mx+20,fy,mw-40,FONT_SM,C_TXT2); fy+=120;

        bool hClose=CheckCollisionPointRec(mp,(Rectangle){mx+20,fy,mw-40,32});
        dbtn(font,"Close",mx+20,fy,mw-40,32,C_SURF,C_TXT2,C_BORDER,hClose);
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&hClose) gModal=MODAL_NONE;
    }

    /* ── TEXT INPUT ROUTING ── */
    if(gModal==MODAL_ADD_P||gModal==MODAL_EDIT_P||
       gModal==MODAL_ADD_E||gModal==MODAL_EDIT_E||
       gModal==MODAL_SEARCH_DATE||gModal==MODAL_COUNT){

        bool isEvent=(gModal==MODAL_ADD_E||gModal==MODAL_EDIT_E);
        bool isSearch=(gModal==MODAL_SEARCH_DATE||gModal==MODAL_COUNT);
        char *fields[]={fName,fDef,fBirth,fDeath};
        int   maxLen[]={39,   399,  19,    9};
        int   af = isSearch?2 : (isEvent?(fFocus==0?0:2) : fFocus);
        if(af>3) af=3;
        char *tgt=fields[af];
        int   mxl=maxLen[af];

        int key=GetCharPressed();
        while(key>0){
            int l=(int)strlen(tgt);
            if(l<mxl&&key>=32){tgt[l]=(char)key;tgt[l+1]=0;}
            key=GetCharPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE)){int l=(int)strlen(tgt);if(l>0)tgt[l-1]=0;}
        if(IsKeyPressed(KEY_TAB) && !isSearch){
            int maxF=isEvent?1:3; /* event: 0=name,1=date */
            fFocus=(fFocus+1)%(maxF+1);
        }

        /* click-to-focus for personality form */
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&(gModal==MODAL_ADD_P||gModal==MODAL_EDIT_P)){
            float fy2=my+56;
            if(CheckCollisionPointRec(mp,(Rectangle){mx+20,fy2+14,mw-40,28})) fFocus=0; fy2+=50;
            if(CheckCollisionPointRec(mp,(Rectangle){mx+20,fy2+14,mw-40,72})) fFocus=1; fy2+=90;
            if(CheckCollisionPointRec(mp,(Rectangle){mx+20,fy2+14,(mw-50)/2,28})) fFocus=2;
            if(CheckCollisionPointRec(mp,(Rectangle){mx+20+(mw-50)/2+10,fy2+14,(mw-50)/2,28})) fFocus=3;
        }
        /* click-to-focus for event form */
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&(gModal==MODAL_ADD_E||gModal==MODAL_EDIT_E)){
            float fy2=my+56;
            if(CheckCollisionPointRec(mp,(Rectangle){mx+20,fy2+14,mw-40,28})) fFocus=0; fy2+=50;
            if(CheckCollisionPointRec(mp,(Rectangle){mx+20,fy2+14,mw-40,28})) fFocus=1;
        }
    }
}

/* ═══════════════════════════════════════════════════════════
   SIDEBAR
   ═══════════════════════════════════════════════════════════ */

static void drawSidebar(Font font){
    drect(0,0,SIDEBAR_W,WIN_H,C_BG);
    DrawLineEx((Vector2){SIDEBAR_W,0},(Vector2){SIDEBAR_W,WIN_H},1,C_BORDER);

    /* header */
    DrawTextEx(font,"Algeria History DB",(Vector2){14,14},FONT_SZ,1,C_TXT1);
    DrawTextEx(font,"NSCS · 2025/2026",  (Vector2){14,32},11,    1,C_TXT3);
    DrawLineEx((Vector2){0,52},(Vector2){SIDEBAR_W,52},1,C_BORDER);

    Vector2 mp=GetMousePosition();
    float y=56;

    for(int i=0;i<SB_COUNT;i++){
        int sec=sbItems[i].section;
        if(sec!=lastSection){
            lastSection=sec;
            DrawTextEx(font,secLabels[sec],(Vector2){14,y},10,1,C_TXT3);
            y+=20;
        }
        bool hov = CheckCollisionPointRec(mp,(Rectangle){0,y,SIDEBAR_W,28})&&gModal==MODAL_NONE;
        bool act = (i==gSBActive);
        drect(0,y,SIDEBAR_W,28, act?C_SURF:(hov?C_SURF:C_BG));
        if(act) drect(0,y,3,28,C_ACCENT);
        DrawCircle(17,(int)(y+14),3,C_TXT3);
        DrawTextEx(font,sbItems[i].label,(Vector2){28,y+7},FONT_SM,1,
                   act?C_TXT1:(hov?C_TXT1:C_TXT2));
        y+=28;
        if(i<SB_COUNT-1 && sbItems[i+1].section!=sec){
            DrawLineEx((Vector2){0,y},(Vector2){SIDEBAR_W,y},1,C_BORDER);
            y+=4; lastSection=-1;
        }
    }

    /* click handling */
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&gModal==MODAL_NONE){
        y=56; lastSection=-1;
        for(int i=0;i<SB_COUNT;i++){
            int sec=sbItems[i].section;
            if(sec!=lastSection){lastSection=sec;y+=20;}
            if(CheckCollisionPointRec(mp,(Rectangle){0,y,SIDEBAR_W,28})){
                gSBActive=i; gScrollY=0; gSelected=0;
                switch(i){
                    case 0: gView=VIEW_LIST; gTab=TAB_PERSONALITIES; break;
                    case 1: gView=VIEW_LIST; gTab=TAB_EVENTS;        break;
                    case 2: gView=VIEW_STACK; gTab=TAB_PERSONALITIES; break;
                    case 3: gView=VIEW_QUEUE; gTab=TAB_PERSONALITIES; break;
                    case 4: sortByName(activeList()); showToast("Sorted by name."); break;
                    case 5:
                        if(gTab==TAB_PERSONALITIES) sortByAge(gMerged);
                        showToast("Sorted by age (personalities only)."); break;
                    case 6: gModal=MODAL_PALINDROMES; break;
                    case 7:{
                        char m[120];
                        snprintf(m,119,"Total: %d personalities + %d events = %d nodes",
                            listCount(gMerged),listCount(gEvents),
                            listCount(gMerged)+listCount(gEvents));
                        showToast(m); break;
                    }
                    case 8: gModal=MODAL_SEARCH_DATE; fBirth[0]=0; gOpResult[0]=0; break;
                    case 9: gModal=MODAL_COUNT;       fBirth[0]=0; gOpResult[0]=0; break;
                }
            }
            y+=28;
            if(i<SB_COUNT-1&&sbItems[i+1].section!=sec){y+=4;lastSection=-1;}
        }
    }
}

/* ═══════════════════════════════════════════════════════════
   TABLE  (personalities or events)
   ═══════════════════════════════════════════════════════════ */

static void drawTable(Font font,int tx,int ty,int tw,int th){
    TList *src=activeList();

    /* filter */
    TList *fil[512]; int fc=0;
    for(TList *p=src;p&&fc<512;p=p->next)
        if(strCI(p->name,gSearch)) fil[fc++]=p;

    /* header row */
    drect(tx,ty,tw,28,C_BG);
    DrawLineEx((Vector2){tx,ty+28},(Vector2){tx+tw,ty+28},1,C_BORDER);

    bool isEv=(gTab==TAB_EVENTS);
    float cw[]={36,230,130,100,80};
    const char *ch[]={"#","Name",isEv?"Date":"Birth",isEv?"":     "Death","Type"};
    float cx=tx+8;
    for(int c=0;c<5;c++){
        if(c==3&&isEv) continue;
        DrawTextEx(font,ch[c],(Vector2){cx,ty+8},11,1,C_TXT3);
        cx+=cw[c];
    }

    int vis=(th-28)/ROW_H;
    int maxS=fc-vis; if(maxS<0) maxS=0;
    if(gScrollY<0) gScrollY=0; if(gScrollY>maxS) gScrollY=maxS;

    BeginScissorMode(tx,ty+28,tw,th-28);
    Vector2 mp=GetMousePosition();

    for(int i=gScrollY;i<fc&&(i-gScrollY)<vis+1;i++){
        TList *r=fil[i];
        float ry=ty+28+(i-gScrollY)*ROW_H;
        bool sel=(i==gSelected);
        bool hov=CheckCollisionPointRec(mp,(Rectangle){tx,ry,tw,ROW_H})&&gModal==MODAL_NONE;
        drect(tx,ry,tw,ROW_H, sel?C_SEL:(hov?C_HOVER:C_SURF));
        DrawLineEx((Vector2){tx,ry+ROW_H},(Vector2){tx+tw,ry+ROW_H},1,C_BORDER);

        cx=tx+8;
        char idx[16]; snprintf(idx,16,"%02d",i+1);
        DrawTextEx(font,idx,(Vector2){cx,ry+11},FONT_SM,1,C_TXT3); cx+=cw[0];
        DrawTextEx(font,r->name,(Vector2){cx,ry+11},FONT_SM,1,C_TXT1); cx+=cw[1];

        if(isEv){
            DrawTextEx(font,r->dateB[0]?r->dateB:"—",(Vector2){cx,ry+11},FONT_SM,1,C_TXT2);
            cx+=cw[2];
        } else {
            DrawTextEx(font,r->date.birth[0]?r->date.birth:"—",(Vector2){cx,ry+11},FONT_SM,1,C_TXT2); cx+=cw[2];
            DrawTextEx(font,r->date.death[0]?r->date.death:"—",(Vector2){cx,ry+11},FONT_SM,1,C_TXT2); cx+=cw[3];
        }
        rowTag(font,r,cx,ry+9);

        if(hov&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) gSelected=i;
    }
    EndScissorMode();

    float wh=GetMouseWheelMove();
    if(wh!=0&&CheckCollisionPointRec(mp,(Rectangle){tx,ty,tw,th}))
        gScrollY-=(int)wh*2;
}

/* ═══════════════════════════════════════════════════════════
   STACK / QUEUE VIEWS  (personalities only)
   ═══════════════════════════════════════════════════════════ */

static void drawStructView(Font font,int x,int y,int w,int h){
    TList *src=gMerged;
    int cnt=listCount(src);
    if(!cnt){DrawTextEx(font,"No personalities loaded.",(Vector2){x+20,y+20},FONT_SM,1,C_TXT3);return;}

    float bw=w-60,bh=44,bx=x+30,by=y+14,gap=10;
    int maxVis=(int)((h-20)/(bh+gap+14));

    TList *arr[256]; int ac=0;
    for(TList *p=src;p&&ac<256;p=p->next) arr[ac++]=p;

    int start=(gView==VIEW_STACK)?ac-1:0;
    int dir  =(gView==VIEW_STACK)?-1:1;

    if(gView==VIEW_STACK)
        DrawTextEx(font,"▲ TOP",(Vector2){bx+bw/2-18,by-14},11,1,C_ACCENT);

    int disp=0;
    for(int i=start;disp<maxVis&&i>=0&&i<ac;i+=dir,disp++){
        TList *r=arr[i];
        bool sel=((gView==VIEW_STACK)?(ac-1-i):i)==gSelected;
        DrawRectangleRounded((Rectangle){bx,by,bw,bh},0.15f,4,sel?C_SEL:C_SURF);
        DrawRectangleRoundedLinesEx((Rectangle){bx,by,bw,bh},0.15f,4,sel?1.5f:1.f,
                                     sel?C_ACCENT:C_BORDER);
        char lbl[120];
        int age=atoi(r->date.death)-atoi(r->date.birth);
        snprintf(lbl,120,"%s  [%s - %s]  age: %d",
                 r->name,r->date.birth,r->date.death,age>0?age:0);
        DrawTextEx(font,lbl,(Vector2){bx+12,by+15},FONT_SM,1,C_TXT1);

        if(disp<maxVis-1 && ((gView==VIEW_STACK&&i>0)||(gView!=VIEW_STACK&&i<ac-1))){
            float ax=bx+bw/2, ay=by+bh;
            DrawLineEx((Vector2){ax,ay},(Vector2){ax,ay+gap+2},2,C_TXT3);
            DrawTriangle((Vector2){ax,ay+gap+10},
                         (Vector2){ax-5,ay+gap+2},
                         (Vector2){ax+5,ay+gap+2},C_TXT3);
        }
        by+=bh+gap+14;
    }

    if(gView==VIEW_STACK)
        DrawTextEx(font,"▼ BOTTOM",(Vector2){bx+bw/2-24,by},11,1,C_TXT3);
    if(gView==VIEW_QUEUE){
        DrawTextEx(font,"HEAD →",(Vector2){bx-50,y+14+bh/2-6},11,1,C_ACCENT);
        if(disp>0)
            DrawTextEx(font,"→ TAIL",(Vector2){bx+bw+6,y+14+(disp-1)*(bh+gap+14)+bh/2-6},11,1,C_TXT3);
    }
}

/* ═══════════════════════════════════════════════════════════
   DETAIL PANEL
   ═══════════════════════════════════════════════════════════ */

static void drawDetail(Font font,int x,int w){
    drect(x,0,w,WIN_H,C_SURF);
    DrawLineEx((Vector2){x,0},(Vector2){x,WIN_H},1,C_BORDER);

    TList *r=listGet(activeList(),gSelected);

    /* header */
    drect(x,0,w,52,C_SURF);
    if(r){
        DrawTextEx(font,r->name,(Vector2){x+14,10},FONT_SZ,1,C_TXT1);
        char sub[80];
        if(r->type==0){
            int age=atoi(r->date.death)-atoi(r->date.birth);
            snprintf(sub,79,"%s - %s  ·  Age: %d",r->date.birth,r->date.death,age>0?age:0);
        } else {
            snprintf(sub,79,"Date: %s",r->dateB);
        }
        DrawTextEx(font,sub,(Vector2){x+14,30},11,1,C_TXT2);
    } else {
        DrawTextEx(font,"Select a record",(Vector2){x+14,18},FONT_SM,1,C_TXT3);
    }
    DrawLineEx((Vector2){x,52},(Vector2){x+w,52},1,C_BORDER);

    if(r){
        float dy=62; float lx=x+14; float fw=w-28;

        if(r->type==0){
            DrawTextEx(font,"DESCRIPTION",(Vector2){lx,dy},10,1,C_TXT3); dy+=14;
            dwrapped(font,r->difinition,lx,dy,fw,11,C_TXT2); dy+=76;

            DrawTextEx(font,"BIRTH",(Vector2){lx,dy},10,1,C_TXT3); dy+=14;
            DrawTextEx(font,r->date.birth[0]?r->date.birth:"—",(Vector2){lx,dy},FONT_SM,1,C_TXT2); dy+=24;
            DrawTextEx(font,"DEATH",(Vector2){lx,dy},10,1,C_TXT3); dy+=14;
            DrawTextEx(font,r->date.death[0]?r->date.death:"—",(Vector2){lx,dy},FONT_SM,1,C_TXT2); dy+=24;
        } else {
            DrawTextEx(font,"DATE",(Vector2){lx,dy},10,1,C_TXT3); dy+=14;
            DrawTextEx(font,r->dateB[0]?r->dateB:"—",(Vector2){lx,dy},FONT_SM,1,C_TXT2); dy+=24;
        }

        /* linked list pointers */
        TList *list=activeList(), *prev=NULL, *nxt=NULL, *p=list;
        while(p&&p!=r){prev=p;p=p->next;}
        if(p) nxt=p->next;
        DrawTextEx(font,"DATA STRUCTURE",(Vector2){lx,dy},10,1,C_TXT3); dy+=14;
        char ns[60],ps[60];
        snprintf(ns,59,"next -> %s",nxt?nxt->name:"NULL");
        snprintf(ps,59,"prev -> %s",prev?prev->name:"NULL");
        DrawTextEx(font,ns,(Vector2){lx,dy},11,1,C_TXT2); dy+=16;
        DrawTextEx(font,ps,(Vector2){lx,dy},11,1,C_TXT2); dy+=24;

        if(r->type==0){
            bool pal=isPalindrome(r->name);
            bool kil=isKilled(r->difinition);
            DrawTextEx(font,"FLAGS",(Vector2){lx,dy},10,1,C_TXT3); dy+=14;
            char fl[80]; snprintf(fl,79,"Palindrome: %s   Killed: %s",pal?"Yes":"No",kil?"Yes":"No");
            DrawTextEx(font,fl,(Vector2){lx,dy},11,1,C_TXT2);
        }
    }

    /* edit / delete buttons */
    int fy=WIN_H-STATUSBAR_H-42;
    DrawLineEx((Vector2){x,fy},(Vector2){x+w,fy},1,C_BORDER);
    Vector2 mp=GetMousePosition();
    bool hE=CheckCollisionPointRec(mp,(Rectangle){x+12,fy+8,w/2-18,26})&&gModal==MODAL_NONE;
    bool hD=CheckCollisionPointRec(mp,(Rectangle){x+w/2+6,fy+8,w/2-18,26})&&gModal==MODAL_NONE;
    dbtn(font,"Edit",  x+12,   fy+8,w/2-18,26,C_SURF,  C_TXT1,C_BORDER,hE);
    dbtn(font,"Delete",x+w/2+6,fy+8,w/2-18,26,C_RED_BG,C_RED_FG,C_RED_BR,hD);

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        if(hE&&r){
            fFocus=0; fEditIdx=gSelected;
            strncpy(fName,r->name,39);
            if(r->type==0){
                strncpy(fDef,  r->difinition, 399);
                strncpy(fBirth,r->date.birth,   9);
                strncpy(fDeath,r->date.death,   9);
                gModal=MODAL_EDIT_P;
            } else {
                fDef[0]=0;
                strncpy(fBirth,r->dateB,19);
                fDeath[0]=0;
                gModal=MODAL_EDIT_E;
            }
        }
        if(hD&&r) gModal=MODAL_CONFIRM_DEL;
    }
}

/* ═══════════════════════════════════════════════════════════
   MAIN
   ═══════════════════════════════════════════════════════════ */

int main(void){
    SetConfigFlags(FLAG_WINDOW_RESIZABLE|FLAG_MSAA_4X_HINT);
    InitWindow(WIN_W,WIN_H,"Algeria History DB — NSCS 2025/2026");
    SetTargetFPS(60);
    SetExitKey(0);

    /* load or seed */
    FILE *chk=fopen(DATA_FILE,"r");
    if(chk){ fclose(chk); loadFromFile(); }
    else    seedData();

    Font font=LoadFontEx("resources/JetBrainsMono-Regular.ttf",18,NULL,0);
    if(font.texture.id==0) font=GetFontDefault();

    while(!WindowShouldClose()){
        float dt=GetFrameTime();
        if(gToastTimer>0) gToastTimer-=dt;
        if(IsKeyPressed(KEY_ESCAPE)&&gModal!=MODAL_NONE) gModal=MODAL_NONE;

        /* main search input */
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
        ClearBackground(C_SURF);
        int W=GetScreenWidth(), H=GetScreenHeight();

        lastSection=-1;
        drawSidebar(font);

        int mainX=SIDEBAR_W, mainW=W-SIDEBAR_W-DETAIL_W;

        /* ── topbar ── */
        drect(mainX,0,mainW,TOPBAR_H,C_SURF);
        DrawLineEx((Vector2){mainX,TOPBAR_H},(Vector2){mainX+mainW,TOPBAR_H},1,C_BORDER);
        const char *titles[]={
            "Personalities · Linked List","Events · Linked List",
            "Personalities · Stack","Personalities · Queue"
        };
        int ti=gSBActive==1?1:(gView==VIEW_STACK?2:gView==VIEW_QUEUE?3:0);
        DrawTextEx(font,titles[ti],(Vector2){mainX+14,14},FONT_SZ,1,C_TXT1);

        /* search box */
        float sx=mainX+mainW-220;
        Vector2 mp=GetMousePosition();
        bool sHov=CheckCollisionPointRec(mp,(Rectangle){sx,10,162,26});
        drect(sx,10,162,26,C_BG);
        DrawRectangleLinesEx((Rectangle){sx,10,162,26},1,gSearchFocus?C_ACCENT:C_BORDER);
        DrawTextEx(font,gSearch[0]?gSearch:"Search...",(Vector2){sx+8,16},FONT_SM,1,
                   gSearch[0]?C_TXT1:C_TXT3);
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&gModal==MODAL_NONE) gSearchFocus=sHov;

        /* add button */
        float abx=sx+170;
        bool aHov=CheckCollisionPointRec(mp,(Rectangle){abx,10,50,26})&&gModal==MODAL_NONE;
        dbtn(font,"+ Add",abx,10,50,26,C_ACCENT,WHITE,C_ACCENT,aHov);
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&aHov){
            fName[0]=fDef[0]=fBirth[0]=fDeath[0]=0; fFocus=0;
            gModal=(gTab==TAB_EVENTS)?MODAL_ADD_E:MODAL_ADD_P;
        }

        /* ── content ── */
        int cy=TOPBAR_H, ch=H-TOPBAR_H-STATUSBAR_H;
        if(gView==VIEW_LIST){
            drawTable(font,mainX,cy,mainW,ch);
        } else {
            drect(mainX,cy,mainW,ch,C_SURF);
            drawStructView(font,mainX,cy,mainW,ch);
        }

        /* ── statusbar ── */
        drect(mainX,H-STATUSBAR_H,mainW,STATUSBAR_H,C_BG);
        DrawLineEx((Vector2){mainX,H-STATUSBAR_H},(Vector2){mainX+mainW,H-STATUSBAR_H},1,C_BORDER);
        int cnt=listCount(activeList());
        const char *tabs[]={"personalities","events"};
        const char *views[]={"bidirectional","stack (LIFO)","queue (FIFO)"};
        char st[160];
        snprintf(st,159,"%d %s loaded   ·   Structure: %s",cnt,tabs[gTab],views[gView]);
        DrawTextEx(font,st,(Vector2){mainX+14,H-STATUSBAR_H+6},11,1,C_TXT3);

        /* ── detail panel ── */
        drawDetail(font,W-DETAIL_W,DETAIL_W);

        /* ── modal ── */
        if(gModal!=MODAL_NONE) drawModal(font);

        /* ── toast ── */
        if(gToastTimer>0){
            float a=gToastTimer>0.5f?1.f:gToastTimer/0.5f;
            Color tb=C_TOAST_BG; tb.a=(unsigned char)(200*a);
            Color tf=C_TOAST_FG; tf.a=(unsigned char)(255*a);
            float tw=MeasureTextEx(font,gToast,FONT_SM,1).x+24;
            float ttx=(W-tw)/2.f, tty=H-STATUSBAR_H-52;
            DrawRectangleRounded((Rectangle){ttx,tty,tw,30},0.4f,6,tb);
            DrawTextEx(font,gToast,(Vector2){ttx+12,tty+8},FONT_SM,1,tf);
        }

        EndDrawing();
    }

    freeList(gMerged); freeList(gPersonNames);
    freeList(gPersonDates); freeList(gEvents);
    UnloadFont(font);
    CloseWindow();
    return 0;
}
    return 0;
}
