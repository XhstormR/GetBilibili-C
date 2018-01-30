#include <windows.h>

#define snew(type) (malloc(sizeof(type)))
#define snewn(n, type) (malloc((n) * sizeof(type)))
#define sfree(ptr) free(ptr)

#define lenof(x) (sizeof((x)) / sizeof(*(x)))

#define NULLTOEMPTY(s) ((s) ? (s) : "")

HINSTANCE hinst;

struct ctlpos {
    HWND hwnd;
    WPARAM font;
    int dlu4inpix;
    int ypos, width;
    int xoff;
    int boxystart, boxid;
    char *boxtext;
};

struct prefslist {
    int listid, upbid, dnbid;
    int srcitem;
    int dummyitem;
    int dragging;
};

void init_common_controls(void);
void ctlposinit(struct ctlpos *cp, HWND hwnd, int leftborder, int rightborder, int topborder);
HWND doctl(struct ctlpos *cp, RECT r, char *wclass, int wstyle, int exstyle, char *wtext, int wid);
void bartitle(struct ctlpos *cp, char *name, int id);
void beginbox(struct ctlpos *cp, char *name, int idbox);
void endbox(struct ctlpos *cp);
void editboxfw(struct ctlpos *cp, int password, char *text, int staticid, int editid);
void combobox(struct ctlpos *cp, char *text, int staticid, int listid);
void radioline(struct ctlpos *cp, char *text, int id, int nacross, ...);
void bareradioline(struct ctlpos *cp, int nacross, ...);
void radiobig(struct ctlpos *cp, char *text, int id, ...);
void checkbox(struct ctlpos *cp, char *text, int id);
char *staticwrap(struct ctlpos *cp, HWND hwnd, char *text, int *lines);
void statictext(struct ctlpos *cp, char *text, int lines, int id);
void paneltitle(struct ctlpos *cp, int id);
void staticbtn(struct ctlpos *cp, char *stext, int sid, char *btext, int bid);
void button(struct ctlpos *cp, char *btext, int bid, int defbtn);
void static2btn(struct ctlpos *cp, char *stext, int sid, char *btext1, int bid1, char *btext2, int bid2);
void staticedit(struct ctlpos *cp, char *stext, int sid, int eid, int percentedit);
void staticpassedit(struct ctlpos *cp, char *stext, int sid, int eid, int percentedit);
void staticddl(struct ctlpos *cp, char *stext, int sid, int lid, int percentlist);
void staticcombo(struct ctlpos *cp, char *stext, int sid, int lid, int percentlist);
void staticddlbig(struct ctlpos *cp, char *stext, int sid, int lid);
void bigeditctrl(struct ctlpos *cp, char *stext, int sid, int eid, int lines);
void listbox(struct ctlpos *cp, char *stext, int sid, int lid, int lines, int multi);
void ersatztab(struct ctlpos *cp, char *stext, int sid, int lid, int s2id);
void editbutton(struct ctlpos *cp, char *stext, int sid, int eid, char *btext, int bid);
void prefslist(struct prefslist *hdl, struct ctlpos *cp, int lines, char *stext, int sid, int listid, int upbid, int dnbid);
int pl_itemfrompt(HWND hwnd, POINT cursor, BOOL scroll);
int handle_prefslist(struct prefslist *hdl, int *array, int maxmemb, int is_dlmsg, HWND hwnd, WPARAM wParam, LPARAM lParam);
void progressbar(struct ctlpos *cp, int id);
