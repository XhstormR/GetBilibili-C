#include "winctrls.h"

#include <commctrl.h>

#define GAPBETWEEN 3
#define GAPWITHIN 1
#define GAPXBOX 7
#define GAPYBOX 4
#define DLGWIDTH 168
#define STATICHEIGHT 8
#define TITLEHEIGHT 12
#define CHECKBOXHEIGHT 8
#define RADIOHEIGHT 8
#define EDITHEIGHT 12
#define LISTHEIGHT 11
#define LISTINCREMENT 8
#define COMBOHEIGHT 12
#define PUSHBTNHEIGHT 14
#define PROGBARHEIGHT 14

typedef void ( *t_InitCommonControls) (void); static t_InitCommonControls p_InitCommonControls;
typedef BOOL ( *t_MakeDragList) (HWND); static t_MakeDragList p_MakeDragList;
typedef int ( *t_LBItemFromPt) (HWND, POINT, BOOL); static t_LBItemFromPt p_LBItemFromPt;
typedef void ( *t_DrawInsert) (HWND, HWND, int); static t_DrawInsert p_DrawInsert;

void init_common_controls(void)
{
    HMODULE comctl32_module = LoadLibrary("comctl32.dll");
    p_InitCommonControls = (t_InitCommonControls) GetProcAddress(comctl32_module, "InitCommonControls");
    p_MakeDragList = (t_MakeDragList) GetProcAddress(comctl32_module, "MakeDragList");
    p_LBItemFromPt = (t_LBItemFromPt) GetProcAddress(comctl32_module, "LBItemFromPt");
    p_DrawInsert = (t_DrawInsert) GetProcAddress(comctl32_module, "DrawInsert");
    p_InitCommonControls();
}

void ctlposinit(struct ctlpos *cp, HWND hwnd,
		int leftborder, int rightborder, int topborder)
{
    RECT r, r2;
    cp->hwnd = hwnd;
    cp->font = SendMessage(hwnd, WM_GETFONT, 0, 0);
    cp->ypos = topborder;
    GetClientRect(hwnd, &r);
    r2.left = r2.top = 0;
    r2.right = 4;
    r2.bottom = 8;
    MapDialogRect(hwnd, &r2);
    cp->dlu4inpix = r2.right;
    cp->width = (r.right * 4) / (r2.right) - 2 * GAPBETWEEN;
    cp->xoff = leftborder;
    cp->width -= leftborder + rightborder;
}

HWND doctl(struct ctlpos *cp, RECT r,
	   char *wclass, int wstyle, int exstyle, char *wtext, int wid)
{
    HWND ctl;
    /*
     * Note nonstandard use of RECT. This is deliberate: by
     * transforming the width and height directly we arrange to
     * have all supposedly same-sized controls really same-sized.
     */

    r.left += cp->xoff;
    MapDialogRect(cp->hwnd, &r);

    /*
     * We can pass in cp->hwnd == NULL, to indicate a dry run
     * without creating any actual controls.
     */
    if (cp->hwnd) {
	ctl = CreateWindowEx(exstyle, wclass, wtext, wstyle,
			     r.left, r.top, r.right, r.bottom,
			     cp->hwnd, (HMENU)(ULONG_PTR)wid, hinst, NULL);
	SendMessage(ctl, WM_SETFONT, cp->font, MAKELPARAM(TRUE, 0));

	if (!strcmp(wclass, "LISTBOX")) {
	    /*
	     * Bizarre Windows bug: the list box calculates its
	     * number of lines based on the font it has at creation
	     * time, but sending it WM_SETFONT doesn't cause it to
	     * recalculate. So now, _after_ we've sent it
	     * WM_SETFONT, we explicitly resize it (to the same
	     * size it was already!) to force it to reconsider.
	     */
	    SetWindowPos(ctl, NULL, 0, 0, r.right, r.bottom,
			 SWP_NOACTIVATE | SWP_NOCOPYBITS |
			 SWP_NOMOVE | SWP_NOZORDER);
	}
    } else
	ctl = NULL;
    return ctl;
}

/*
 * A title bar across the top of a sub-dialog.
 */
void bartitle(struct ctlpos *cp, char *name, int id)
{
    RECT r;

    r.left = GAPBETWEEN;
    r.right = cp->width;
    r.top = cp->ypos;
    r.bottom = STATICHEIGHT;
    cp->ypos += r.bottom + GAPBETWEEN;
    doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, name, id);
}

/*
 * Begin a grouping box, with or without a group title.
 */
void beginbox(struct ctlpos *cp, char *name, int idbox)
{
    cp->boxystart = cp->ypos;
    if (!name)
	cp->boxystart -= STATICHEIGHT / 2;
    if (name)
	cp->ypos += STATICHEIGHT;
    cp->ypos += GAPYBOX;
    cp->width -= 2 * GAPXBOX;
    cp->xoff += GAPXBOX;
    cp->boxid = idbox;
    cp->boxtext = name;
}

/*
 * End a grouping box.
 */
void endbox(struct ctlpos *cp)
{
    RECT r;
    cp->xoff -= GAPXBOX;
    cp->width += 2 * GAPXBOX;
    cp->ypos += GAPYBOX - GAPBETWEEN;
    r.left = GAPBETWEEN;
    r.right = cp->width;
    r.top = cp->boxystart;
    r.bottom = cp->ypos - cp->boxystart;
    doctl(cp, r, "BUTTON", BS_GROUPBOX | WS_CHILD | WS_VISIBLE, 0,
	  cp->boxtext ? cp->boxtext : "", cp->boxid);
    cp->ypos += GAPYBOX;
}

/*
 * A static line, followed by a full-width edit box.
 */
void editboxfw(struct ctlpos *cp, int password, char *text,
	       int staticid, int editid)
{
    RECT r;

    r.left = GAPBETWEEN;
    r.right = cp->width;

    if (text) {
	r.top = cp->ypos;
	r.bottom = STATICHEIGHT;
	doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, text, staticid);
	cp->ypos += STATICHEIGHT + GAPWITHIN;
    }
    r.top = cp->ypos;
    r.bottom = EDITHEIGHT;
    doctl(cp, r, "EDIT",
	  WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL |
	  (password ? ES_PASSWORD : 0),
	  WS_EX_CLIENTEDGE, "", editid);
    cp->ypos += EDITHEIGHT + GAPBETWEEN;
}

/*
 * A static line, followed by a full-width combo box.
 */
void combobox(struct ctlpos *cp, char *text, int staticid, int listid)
{
    RECT r;

    r.left = GAPBETWEEN;
    r.right = cp->width;

    if (text) {
	r.top = cp->ypos;
	r.bottom = STATICHEIGHT;
	doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, text, staticid);
	cp->ypos += STATICHEIGHT + GAPWITHIN;
    }
    r.top = cp->ypos;
    r.bottom = COMBOHEIGHT * 10;
    doctl(cp, r, "COMBOBOX",
	  WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
	  CBS_DROPDOWN | CBS_HASSTRINGS, WS_EX_CLIENTEDGE, "", listid);
    cp->ypos += COMBOHEIGHT + GAPBETWEEN;
}

struct radio { char *text; int id; };

static void radioline_common(struct ctlpos *cp, char *text, int id,
			     int nacross, struct radio *buttons, int nbuttons)
{
    RECT r;
    int group;
    int i;
    int j;

    if (text) {
	r.left = GAPBETWEEN;
	r.top = cp->ypos;
	r.right = cp->width;
	r.bottom = STATICHEIGHT;
	cp->ypos += r.bottom + GAPWITHIN;
	doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, text, id);
    }

    group = WS_GROUP;
    i = 0;
    for (j = 0; j < nbuttons; j++) {
	char *btext = buttons[j].text;
	int bid = buttons[j].id;

	if (i == nacross) {
	    cp->ypos += r.bottom + (nacross > 1 ? GAPBETWEEN : GAPWITHIN);
	    i = 0;
	}
	r.left = GAPBETWEEN + i * (cp->width + GAPBETWEEN) / nacross;
	if (j < nbuttons-1)
	    r.right =
		(i + 1) * (cp->width + GAPBETWEEN) / nacross - r.left;
	else
	    r.right = cp->width - r.left;
	r.top = cp->ypos;
	r.bottom = RADIOHEIGHT;
	doctl(cp, r, "BUTTON",
	      BS_NOTIFY | BS_AUTORADIOBUTTON | WS_CHILD |
	      WS_VISIBLE | WS_TABSTOP | group, 0, btext, bid);
	group = 0;
	i++;
    }
    cp->ypos += r.bottom + GAPBETWEEN;
}

/*
 * A set of radio buttons on the same line, with a static above
 * them. `nacross' dictates how many parts the line is divided into
 * (you might want this not to equal the number of buttons if you
 * needed to line up some 2s and some 3s to look good in the same
 * panel).
 *
 * There's a bit of a hack in here to ensure that if nacross
 * exceeds the actual number of buttons, the rightmost button
 * really does get all the space right to the edge of the line, so
 * you can do things like
 *
 * (*) Button1  (*) Button2  (*) ButtonWithReallyLongTitle
 */
void radioline(struct ctlpos *cp, char *text, int id, int nacross, ...)
{
    va_list ap;
    struct radio *buttons;
    int i, nbuttons;

    va_start(ap, nacross);
    nbuttons = 0;
    while (1) {
	char *btext = va_arg(ap, char *);
	if (!btext)
	    break;
	(void) va_arg(ap, int); /* id */
	nbuttons++;
    }
    va_end(ap);
    buttons = snewn(nbuttons, struct radio);
    va_start(ap, nacross);
    for (i = 0; i < nbuttons; i++) {
	buttons[i].text = va_arg(ap, char *);
	buttons[i].id = va_arg(ap, int);
    }
    va_end(ap);
    radioline_common(cp, text, id, nacross, buttons, nbuttons);
    sfree(buttons);
}

/*
 * A set of radio buttons on the same line, without a static above
 * them. Otherwise just like radioline.
 */
void bareradioline(struct ctlpos *cp, int nacross, ...)
{
    va_list ap;
    struct radio *buttons;
    int i, nbuttons;

    va_start(ap, nacross);
    nbuttons = 0;
    while (1) {
	char *btext = va_arg(ap, char *);
	if (!btext)
	    break;
	(void) va_arg(ap, int); /* id */
        nbuttons++;
    }
    va_end(ap);
    buttons = snewn(nbuttons, struct radio);
    va_start(ap, nacross);
    for (i = 0; i < nbuttons; i++) {
	buttons[i].text = va_arg(ap, char *);
	buttons[i].id = va_arg(ap, int);
    }
    va_end(ap);
    radioline_common(cp, NULL, 0, nacross, buttons, nbuttons);
    sfree(buttons);
}

/*
 * A set of radio buttons on multiple lines, with a static above
 * them.
 */
void radiobig(struct ctlpos *cp, char *text, int id, ...)
{
    va_list ap;
    struct radio *buttons;
    int i, nbuttons;

    va_start(ap, id);
    nbuttons = 0;
    while (1) {
	char *btext = va_arg(ap, char *);
	if (!btext)
	    break;
	(void) va_arg(ap, int); /* id */
        nbuttons++;
    }
    va_end(ap);
    buttons = snewn(nbuttons, struct radio);
    va_start(ap, id);
    for (i = 0; i < nbuttons; i++) {
	buttons[i].text = va_arg(ap, char *);
	buttons[i].id = va_arg(ap, int);
    }
    va_end(ap);
    radioline_common(cp, text, id, 1, buttons, nbuttons);
    sfree(buttons);
}

/*
 * A single standalone checkbox.
 */
void checkbox(struct ctlpos *cp, char *text, int id)
{
    RECT r;

    r.left = GAPBETWEEN;
    r.top = cp->ypos;
    r.right = cp->width;
    r.bottom = CHECKBOXHEIGHT;
    cp->ypos += r.bottom + GAPBETWEEN;
    doctl(cp, r, "BUTTON",
	  BS_NOTIFY | BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0,
	  text, id);
}

/*
 * Wrap a piece of text for a static text control. Returns the
 * wrapped text (a malloc'ed string containing \ns), and also
 * returns the number of lines required.
 */
char *staticwrap(struct ctlpos *cp, HWND hwnd, char *text, int *lines)
{
    HDC hdc = GetDC(hwnd);
    int width, nlines, j;
    INT *pwidths, nfit;
    SIZE size;
    char *ret, *p, *q;
    RECT r;
    HFONT oldfont, newfont;

    ret = snewn(1+strlen(text), char);
    p = text;
    q = ret;
    pwidths = snewn(1+strlen(text), INT);

    /*
     * Work out the width the text will need to fit in, by doing
     * the same adjustment that the `statictext' function itself
     * will perform.
     */
    SetMapMode(hdc, MM_TEXT);	       /* ensure logical units == pixels */
    r.left = r.top = r.bottom = 0;
    r.right = cp->width;
    MapDialogRect(hwnd, &r);
    width = r.right;

    nlines = 1;

    /*
     * We must select the correct font into the HDC before calling
     * GetTextExtent*, or silly things will happen.
     */
    newfont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
    oldfont = SelectObject(hdc, newfont);

    while (*p) {
	if (!GetTextExtentExPoint(hdc, p, strlen(p), width,
				  &nfit, pwidths, &size) ||
	    (size_t)nfit >= strlen(p)) {
	    /*
	     * Either GetTextExtentExPoint returned failure, or the
	     * whole of the rest of the text fits on this line.
	     * Either way, we stop wrapping, copy the remainder of
	     * the input string unchanged to the output, and leave.
	     */
	    strcpy(q, p);
	    break;
	}

	/*
	 * Now we search backwards along the string from `nfit',
	 * looking for a space at which to break the line. If we
	 * don't find one at all, that's fine - we'll just break
	 * the line at `nfit'.
	 */
	for (j = nfit; j > 0; j--) {
	    if (isspace((unsigned char)p[j])) {
		nfit = j;
		break;
	    }
	}

	strncpy(q, p, nfit);
	q[nfit] = '\n';
	q += nfit+1;

	p += nfit;
	while (*p && isspace((unsigned char)*p))
	    p++;

	nlines++;
    }

    SelectObject(hdc, oldfont);
    ReleaseDC(cp->hwnd, hdc);

    if (lines) *lines = nlines;

    sfree(pwidths);

    return ret;
}

/*
 * A single standalone static text control.
 */
void statictext(struct ctlpos *cp, char *text, int lines, int id)
{
    RECT r;

    r.left = GAPBETWEEN;
    r.top = cp->ypos;
    r.right = cp->width;
    r.bottom = STATICHEIGHT * lines;
    cp->ypos += r.bottom + GAPBETWEEN;
    doctl(cp, r, "STATIC",
	  WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP,
	  0, text, id);
}

/*
 * An owner-drawn static text control for a panel title.
 */
void paneltitle(struct ctlpos *cp, int id)
{
    RECT r;

    r.left = GAPBETWEEN;
    r.top = cp->ypos;
    r.right = cp->width;
    r.bottom = TITLEHEIGHT;
    cp->ypos += r.bottom + GAPBETWEEN;
    doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
	  0, NULL, id);
}

/*
 * A button on the right hand side, with a static to its left.
 */
void staticbtn(struct ctlpos *cp, char *stext, int sid,
	       char *btext, int bid)
{
    const int height = (PUSHBTNHEIGHT > STATICHEIGHT ?
			PUSHBTNHEIGHT : STATICHEIGHT);
    RECT r;
    int lwid, rwid, rpos;

    rpos = GAPBETWEEN + 3 * (cp->width + GAPBETWEEN) / 4;
    lwid = rpos - 2 * GAPBETWEEN;
    rwid = cp->width + GAPBETWEEN - rpos;

    r.left = GAPBETWEEN;
    r.top = cp->ypos + (height - STATICHEIGHT) / 2;
    r.right = lwid;
    r.bottom = STATICHEIGHT;
    doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, stext, sid);

    r.left = rpos;
    r.top = cp->ypos + (height - PUSHBTNHEIGHT) / 2;
    r.right = rwid;
    r.bottom = PUSHBTNHEIGHT;
    doctl(cp, r, "BUTTON",
	  BS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
	  0, btext, bid);

    cp->ypos += height + GAPBETWEEN;
}

/*
 * A simple push button.
 */
void button(struct ctlpos *cp, char *btext, int bid, int defbtn)
{
    RECT r;

    r.left = GAPBETWEEN;
    r.top = cp->ypos;
    r.right = cp->width;
    r.bottom = PUSHBTNHEIGHT;

    /* Q67655: the _dialog box_ must know which button is default
     * as well as the button itself knowing */
    if (defbtn && cp->hwnd)
	SendMessage(cp->hwnd, DM_SETDEFID, bid, 0);

    doctl(cp, r, "BUTTON",
	  BS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_TABSTOP |
	  (defbtn ? BS_DEFPUSHBUTTON : 0) | BS_PUSHBUTTON,
	  0, btext, bid);

    cp->ypos += PUSHBTNHEIGHT + GAPBETWEEN;
}

/*
 * Like staticbtn, but two buttons.
 */
void static2btn(struct ctlpos *cp, char *stext, int sid,
		char *btext1, int bid1, char *btext2, int bid2)
{
    const int height = (PUSHBTNHEIGHT > STATICHEIGHT ?
			PUSHBTNHEIGHT : STATICHEIGHT);
    RECT r;
    int lwid, rwid1, rwid2, rpos1, rpos2;

    rpos1 = GAPBETWEEN + (cp->width + GAPBETWEEN) / 2;
    rpos2 = GAPBETWEEN + 3 * (cp->width + GAPBETWEEN) / 4;
    lwid = rpos1 - 2 * GAPBETWEEN;
    rwid1 = rpos2 - rpos1 - GAPBETWEEN;
    rwid2 = cp->width + GAPBETWEEN - rpos2;

    r.left = GAPBETWEEN;
    r.top = cp->ypos + (height - STATICHEIGHT) / 2;
    r.right = lwid;
    r.bottom = STATICHEIGHT;
    doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, stext, sid);

    r.left = rpos1;
    r.top = cp->ypos + (height - PUSHBTNHEIGHT) / 2;
    r.right = rwid1;
    r.bottom = PUSHBTNHEIGHT;
    doctl(cp, r, "BUTTON",
	  BS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
	  0, btext1, bid1);

    r.left = rpos2;
    r.top = cp->ypos + (height - PUSHBTNHEIGHT) / 2;
    r.right = rwid2;
    r.bottom = PUSHBTNHEIGHT;
    doctl(cp, r, "BUTTON",
	  BS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
	  0, btext2, bid2);

    cp->ypos += height + GAPBETWEEN;
}

/*
 * An edit control on the right hand side, with a static to its left.
 */
static void staticedit_internal(struct ctlpos *cp, char *stext,
				int sid, int eid, int percentedit,
				int style)
{
    const int height = (EDITHEIGHT > STATICHEIGHT ?
			EDITHEIGHT : STATICHEIGHT);
    RECT r;
    int lwid, rwid, rpos;

    rpos =
	GAPBETWEEN + (100 - percentedit) * (cp->width + GAPBETWEEN) / 100;
    lwid = rpos - 2 * GAPBETWEEN;
    rwid = cp->width + GAPBETWEEN - rpos;

    r.left = GAPBETWEEN;
    r.top = cp->ypos + (height - STATICHEIGHT) / 2;
    r.right = lwid;
    r.bottom = STATICHEIGHT;
    doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, stext, sid);

    r.left = rpos;
    r.top = cp->ypos + (height - EDITHEIGHT) / 2;
    r.right = rwid;
    r.bottom = EDITHEIGHT;
    doctl(cp, r, "EDIT",
	  WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | style,
	  WS_EX_CLIENTEDGE, "", eid);

    cp->ypos += height + GAPBETWEEN;
}

void staticedit(struct ctlpos *cp, char *stext,
		int sid, int eid, int percentedit)
{
    staticedit_internal(cp, stext, sid, eid, percentedit, 0);
}

void staticpassedit(struct ctlpos *cp, char *stext,
		    int sid, int eid, int percentedit)
{
    staticedit_internal(cp, stext, sid, eid, percentedit, ES_PASSWORD);
}

/*
 * A drop-down list box on the right hand side, with a static to
 * its left.
 */
void staticddl(struct ctlpos *cp, char *stext,
	       int sid, int lid, int percentlist)
{
    const int height = (COMBOHEIGHT > STATICHEIGHT ?
			COMBOHEIGHT : STATICHEIGHT);
    RECT r;
    int lwid, rwid, rpos;

    rpos =
	GAPBETWEEN + (100 - percentlist) * (cp->width + GAPBETWEEN) / 100;
    lwid = rpos - 2 * GAPBETWEEN;
    rwid = cp->width + GAPBETWEEN - rpos;

    r.left = GAPBETWEEN;
    r.top = cp->ypos + (height - STATICHEIGHT) / 2;
    r.right = lwid;
    r.bottom = STATICHEIGHT;
    doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, stext, sid);

    r.left = rpos;
    r.top = cp->ypos + (height - EDITHEIGHT) / 2;
    r.right = rwid;
    r.bottom = COMBOHEIGHT*4;
    doctl(cp, r, "COMBOBOX",
	  WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
	  CBS_DROPDOWNLIST | CBS_HASSTRINGS, WS_EX_CLIENTEDGE, "", lid);

    cp->ypos += height + GAPBETWEEN;
}

/*
 * A combo box on the right hand side, with a static to its left.
 */
void staticcombo(struct ctlpos *cp, char *stext,
		 int sid, int lid, int percentlist)
{
    const int height = (COMBOHEIGHT > STATICHEIGHT ?
			COMBOHEIGHT : STATICHEIGHT);
    RECT r;
    int lwid, rwid, rpos;

    rpos =
	GAPBETWEEN + (100 - percentlist) * (cp->width + GAPBETWEEN) / 100;
    lwid = rpos - 2 * GAPBETWEEN;
    rwid = cp->width + GAPBETWEEN - rpos;

    r.left = GAPBETWEEN;
    r.top = cp->ypos + (height - STATICHEIGHT) / 2;
    r.right = lwid;
    r.bottom = STATICHEIGHT;
    doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, stext, sid);

    r.left = rpos;
    r.top = cp->ypos + (height - EDITHEIGHT) / 2;
    r.right = rwid;
    r.bottom = COMBOHEIGHT*10;
    doctl(cp, r, "COMBOBOX",
	  WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
	  CBS_DROPDOWN | CBS_HASSTRINGS, WS_EX_CLIENTEDGE, "", lid);

    cp->ypos += height + GAPBETWEEN;
}

/*
 * A static, with a full-width drop-down list box below it.
 */
void staticddlbig(struct ctlpos *cp, char *stext,
		  int sid, int lid)
{
    RECT r;

    if (stext) {
	r.left = GAPBETWEEN;
	r.top = cp->ypos;
	r.right = cp->width;
	r.bottom = STATICHEIGHT;
	doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, stext, sid);
	cp->ypos += STATICHEIGHT;
    }

    r.left = GAPBETWEEN;
    r.top = cp->ypos;
    r.right = cp->width;
    r.bottom = COMBOHEIGHT*4;
    doctl(cp, r, "COMBOBOX",
	  WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
	  CBS_DROPDOWNLIST | CBS_HASSTRINGS, WS_EX_CLIENTEDGE, "", lid);
    cp->ypos += COMBOHEIGHT + GAPBETWEEN;
}

/*
 * A big multiline edit control with a static labelling it.
 */
void bigeditctrl(struct ctlpos *cp, char *stext,
		 int sid, int eid, int lines)
{
    RECT r;

    if (stext) {
	r.left = GAPBETWEEN;
	r.top = cp->ypos;
	r.right = cp->width;
	r.bottom = STATICHEIGHT;
	cp->ypos += r.bottom + GAPWITHIN;
	doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, stext, sid);
    }

    r.left = GAPBETWEEN;
    r.top = cp->ypos;
    r.right = cp->width;
    r.bottom = EDITHEIGHT + (lines - 1) * STATICHEIGHT;
    cp->ypos += r.bottom + GAPBETWEEN;
    doctl(cp, r, "EDIT",
	  WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | ES_MULTILINE,
	  WS_EX_CLIENTEDGE, "", eid);
}

/*
 * A list box with a static labelling it.
 */
void listbox(struct ctlpos *cp, char *stext,
	     int sid, int lid, int lines, int multi)
{
    RECT r;

    if (stext != NULL) {
	r.left = GAPBETWEEN;
	r.top = cp->ypos;
	r.right = cp->width;
	r.bottom = STATICHEIGHT;
	cp->ypos += r.bottom + GAPWITHIN;
	doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, stext, sid);
    }

    r.left = GAPBETWEEN;
    r.top = cp->ypos;
    r.right = cp->width;
    r.bottom = LISTHEIGHT + (lines - 1) * LISTINCREMENT;
    cp->ypos += r.bottom + GAPBETWEEN;
    doctl(cp, r, "LISTBOX",
	  WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
	  LBS_NOTIFY | LBS_HASSTRINGS | LBS_USETABSTOPS |
	  (multi ? LBS_MULTIPLESEL : 0),
	  WS_EX_CLIENTEDGE, "", lid);
}

/*
 * A tab-control substitute when a real tab control is unavailable.
 */
void ersatztab(struct ctlpos *cp, char *stext, int sid, int lid, int s2id)
{
    const int height = (COMBOHEIGHT > STATICHEIGHT ?
			COMBOHEIGHT : STATICHEIGHT);
    RECT r;
    int bigwid, lwid, rwid, rpos;
    static const int BIGGAP = 15;
    static const int MEDGAP = 3;

    bigwid = cp->width + 2 * GAPBETWEEN - 2 * BIGGAP;
    cp->ypos += MEDGAP;
    rpos = BIGGAP + (bigwid + BIGGAP) / 2;
    lwid = rpos - 2 * BIGGAP;
    rwid = bigwid + BIGGAP - rpos;

    r.left = BIGGAP;
    r.top = cp->ypos + (height - STATICHEIGHT) / 2;
    r.right = lwid;
    r.bottom = STATICHEIGHT;
    doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, stext, sid);

    r.left = rpos;
    r.top = cp->ypos + (height - COMBOHEIGHT) / 2;
    r.right = rwid;
    r.bottom = COMBOHEIGHT * 10;
    doctl(cp, r, "COMBOBOX",
	  WS_CHILD | WS_VISIBLE | WS_TABSTOP |
	  CBS_DROPDOWNLIST | CBS_HASSTRINGS, WS_EX_CLIENTEDGE, "", lid);

    cp->ypos += height + MEDGAP + GAPBETWEEN;

    r.left = GAPBETWEEN;
    r.top = cp->ypos;
    r.right = cp->width;
    r.bottom = 2;
    doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ,
	  0, "", s2id);
}

/*
 * A static line, followed by an edit control on the left hand side
 * and a button on the right.
 */
void editbutton(struct ctlpos *cp, char *stext, int sid,
		int eid, char *btext, int bid)
{
    const int height = (EDITHEIGHT > PUSHBTNHEIGHT ?
			EDITHEIGHT : PUSHBTNHEIGHT);
    RECT r;
    int lwid, rwid, rpos;

    r.left = GAPBETWEEN;
    r.top = cp->ypos;
    r.right = cp->width;
    r.bottom = STATICHEIGHT;
    cp->ypos += r.bottom + GAPWITHIN;
    doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, stext, sid);

    rpos = GAPBETWEEN + 3 * (cp->width + GAPBETWEEN) / 4;
    lwid = rpos - 2 * GAPBETWEEN;
    rwid = cp->width + GAPBETWEEN - rpos;

    r.left = GAPBETWEEN;
    r.top = cp->ypos + (height - EDITHEIGHT) / 2;
    r.right = lwid;
    r.bottom = EDITHEIGHT;
    doctl(cp, r, "EDIT",
	  WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
	  WS_EX_CLIENTEDGE, "", eid);

    r.left = rpos;
    r.top = cp->ypos + (height - PUSHBTNHEIGHT) / 2;
    r.right = rwid;
    r.bottom = PUSHBTNHEIGHT;
    doctl(cp, r, "BUTTON",
	  BS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
	  0, btext, bid);

    cp->ypos += height + GAPBETWEEN;
}

/*
 * A special control for manipulating an ordered preference list
 * (eg. for cipher selection).
 * XXX: this is a rough hack and could be improved.
 */
void prefslist(struct prefslist *hdl, struct ctlpos *cp, int lines,
	       char *stext, int sid, int listid, int upbid, int dnbid)
{
    const static int percents[] = { 5, 75, 20 };
    RECT r;
    int xpos, percent = 0, i;
    int listheight = LISTHEIGHT + (lines - 1) * LISTINCREMENT;
    const int BTNSHEIGHT = 2*PUSHBTNHEIGHT + GAPBETWEEN;
    int totalheight, buttonpos;

    /* Squirrel away IDs. */
    hdl->listid = listid;
    hdl->upbid  = upbid;
    hdl->dnbid  = dnbid;

    /* The static label. */
    if (stext != NULL) {
	r.left = GAPBETWEEN;
	r.top = cp->ypos;
	r.right = cp->width;
	r.bottom = STATICHEIGHT;
	cp->ypos += r.bottom + GAPWITHIN;
	doctl(cp, r, "STATIC", WS_CHILD | WS_VISIBLE, 0, stext, sid);
    }

    if (listheight > BTNSHEIGHT) {
        totalheight = listheight;
	buttonpos = (listheight - BTNSHEIGHT) / 2;
    } else {
        totalheight = BTNSHEIGHT;
	buttonpos = 0;
    }

    for (i=0; i<3; i++) {
        int left, wid;
        xpos = (cp->width + GAPBETWEEN) * percent / 100;
        left = xpos + GAPBETWEEN;
        percent += percents[i];
        xpos = (cp->width + GAPBETWEEN) * percent / 100;
        wid = xpos - left;

        switch (i) {
          case 1:
            /* The drag list box. */
            r.left = left; r.right = wid;
            r.top = cp->ypos; r.bottom = listheight;
            {
                HWND ctl;
                ctl = doctl(cp, r, "LISTBOX",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP |
			    WS_VSCROLL | LBS_HASSTRINGS | LBS_USETABSTOPS,
                            WS_EX_CLIENTEDGE,
                            "", listid);
		p_MakeDragList(ctl);
            }
            break;

          case 2:
            /* The "Up" and "Down" buttons. */
	    /* XXX worry about accelerators if we have more than one
	     * prefslist on a panel */
            r.left = left; r.right = wid;
            r.top = cp->ypos + buttonpos; r.bottom = PUSHBTNHEIGHT;
            doctl(cp, r, "BUTTON",
                  BS_NOTIFY | WS_CHILD | WS_VISIBLE |
		  WS_TABSTOP | BS_PUSHBUTTON,
                  0, "&Up", upbid);

            r.left = left; r.right = wid;
            r.top = cp->ypos + buttonpos + PUSHBTNHEIGHT + GAPBETWEEN;
            r.bottom = PUSHBTNHEIGHT;
            doctl(cp, r, "BUTTON",
                  BS_NOTIFY | WS_CHILD | WS_VISIBLE |
		  WS_TABSTOP | BS_PUSHBUTTON,
                  0, "&Down", dnbid);

            break;

        }
    }

    cp->ypos += totalheight + GAPBETWEEN;

}

/*
 * Helper function for prefslist: move item in list box.
 */
static void pl_moveitem(HWND hwnd, int listid, int src, int dst)
{
    int tlen, val;
    char *txt;
    /* Get the item's data. */
    tlen = SendDlgItemMessage (hwnd, listid, LB_GETTEXTLEN, src, 0);
    txt = snewn(tlen+1, char);
    SendDlgItemMessage (hwnd, listid, LB_GETTEXT, src, (LPARAM) txt);
    val = SendDlgItemMessage (hwnd, listid, LB_GETITEMDATA, src, 0);
    /* Deselect old location. */
    SendDlgItemMessage (hwnd, listid, LB_SETSEL, FALSE, src);
    /* Delete it at the old location. */
    SendDlgItemMessage (hwnd, listid, LB_DELETESTRING, src, 0);
    /* Insert it at new location. */
    SendDlgItemMessage (hwnd, listid, LB_INSERTSTRING, dst,
			(LPARAM) txt);
    SendDlgItemMessage (hwnd, listid, LB_SETITEMDATA, dst,
			(LPARAM) val);
    /* Set selection. */
    SendDlgItemMessage (hwnd, listid, LB_SETCURSEL, dst, 0);
    sfree (txt);
}

int pl_itemfrompt(HWND hwnd, POINT cursor, BOOL scroll)
{
    int ret;
    POINT uppoint, downpoint;
    int updist, downdist, upitem, downitem, i;

    /*
     * Ghastly hackery to try to figure out not which
     * _item_, but which _gap between items_, the user
     * is pointing at. We do this by first working out
     * which list item is under the cursor, and then
     * working out how far the cursor would have to
     * move up or down before the answer was different.
     * Then we put the insertion point _above_ the
     * current item if the upper edge is closer than
     * the lower edge, or _below_ it if vice versa.
     */
    ret = p_LBItemFromPt(hwnd, cursor, scroll);
    if (ret == -1)
	return ret;
    ret = p_LBItemFromPt(hwnd, cursor, FALSE);
    updist = downdist = 0;
    for (i = 1; i < 4096 && (!updist || !downdist); i++) {
	uppoint = downpoint = cursor;
	uppoint.y -= i;
	downpoint.y += i;
	upitem = p_LBItemFromPt(hwnd, uppoint, FALSE);
	downitem = p_LBItemFromPt(hwnd, downpoint, FALSE);
	if (!updist && upitem != ret)
	    updist = i;
	if (!downdist && downitem != ret)
	    downdist = i;
    }
    if (downdist < updist)
	ret++;
    return ret;
}

/*
 * Handler for prefslist above.
 *
 * Return value has bit 0 set if the dialog box procedure needs to
 * return TRUE from handling this message; it has bit 1 set if a
 * change may have been made in the contents of the list.
 */
int handle_prefslist(struct prefslist *hdl,
                     int *array, int maxmemb,
                     int is_dlmsg, HWND hwnd,
		     WPARAM wParam, LPARAM lParam)
{
    int i;
    int ret = 0;

    if (is_dlmsg) {

        if ((int)wParam == hdl->listid) {
            DRAGLISTINFO *dlm = (DRAGLISTINFO *)lParam;
            int dest = 0;	       /* initialise to placate gcc */
            switch (dlm->uNotification) {
              case DL_BEGINDRAG:
		/* Add a dummy item to make pl_itemfrompt() work
		 * better.
		 * FIXME: this causes scrollbar glitches if the count of
		 *        listbox contains >= its height. */
		hdl->dummyitem =
		    SendDlgItemMessage(hwnd, hdl->listid,
				       LB_ADDSTRING, 0, (LPARAM) "");

                hdl->srcitem = p_LBItemFromPt(dlm->hWnd, dlm->ptCursor, TRUE);
		hdl->dragging = 0;
		/* XXX hack Q183115 */
		SetWindowLongPtr(hwnd, DWLP_MSGRESULT, TRUE);
                ret |= 1; break;
              case DL_CANCELDRAG:
		p_DrawInsert(hwnd, dlm->hWnd, -1);     /* Clear arrow */
		SendDlgItemMessage(hwnd, hdl->listid,
				   LB_DELETESTRING, hdl->dummyitem, 0);
		hdl->dragging = 0;
                ret |= 1; break;
              case DL_DRAGGING:
		hdl->dragging = 1;
		dest = pl_itemfrompt(dlm->hWnd, dlm->ptCursor, TRUE);
		if (dest > hdl->dummyitem) dest = hdl->dummyitem;
		p_DrawInsert (hwnd, dlm->hWnd, dest);
		if (dest >= 0)
		    SetWindowLongPtr(hwnd, DWLP_MSGRESULT, DL_MOVECURSOR);
		else
		    SetWindowLongPtr(hwnd, DWLP_MSGRESULT, DL_STOPCURSOR);
                ret |= 1; break;
              case DL_DROPPED:
		if (hdl->dragging) {
		    dest = pl_itemfrompt(dlm->hWnd, dlm->ptCursor, TRUE);
		    if (dest > hdl->dummyitem) dest = hdl->dummyitem;
		    p_DrawInsert (hwnd, dlm->hWnd, -1);
		}
		SendDlgItemMessage(hwnd, hdl->listid,
				   LB_DELETESTRING, hdl->dummyitem, 0);
		if (hdl->dragging) {
		    hdl->dragging = 0;
		    if (dest >= 0) {
			/* Correct for "missing" item. */
			if (dest > hdl->srcitem) dest--;
			pl_moveitem(hwnd, hdl->listid, hdl->srcitem, dest);
		    }
		    ret |= 2;
		}
                ret |= 1; break;
            }
        }

    } else {

        if (((LOWORD(wParam) == hdl->upbid) ||
             (LOWORD(wParam) == hdl->dnbid)) &&
            ((HIWORD(wParam) == BN_CLICKED) ||
             (HIWORD(wParam) == BN_DOUBLECLICKED))) {
            /* Move an item up or down the list. */
            /* Get the current selection, if any. */
            int selection = SendDlgItemMessage (hwnd, hdl->listid, LB_GETCURSEL, 0, 0);
            if (selection == LB_ERR) {
                MessageBeep(0);
            } else {
                int nitems;
                /* Get the total number of items. */
                nitems = SendDlgItemMessage (hwnd, hdl->listid, LB_GETCOUNT, 0, 0);
                /* Should we do anything? */
		if (LOWORD(wParam) == hdl->upbid && (selection > 0))
		    pl_moveitem(hwnd, hdl->listid, selection, selection - 1);
		else if (LOWORD(wParam) == hdl->dnbid && (selection < nitems - 1))
		    pl_moveitem(hwnd, hdl->listid, selection, selection + 1);
		ret |= 2;
            }

        }

    }

    if (array) {
	/* Update array to match the list box. */
	for (i=0; i < maxmemb; i++)
	    array[i] = SendDlgItemMessage (hwnd, hdl->listid, LB_GETITEMDATA,
					   i, 0);
    }

    return ret;
}

/*
 * A progress bar (from Common Controls). We like our progress bars
 * to be smooth and unbroken, without those ugly divisions; some
 * older compilers may not support that, but that's life.
 */
void progressbar(struct ctlpos *cp, int id)
{
    RECT r;

    r.left = GAPBETWEEN;
    r.top = cp->ypos;
    r.right = cp->width;
    r.bottom = PROGBARHEIGHT;
    cp->ypos += r.bottom + GAPBETWEEN;

    doctl(cp, r, PROGRESS_CLASS, WS_CHILD | WS_VISIBLE
#ifdef PBS_SMOOTH
	  | PBS_SMOOTH
#endif
	  , WS_EX_CLIENTEDGE, "", id);
}
