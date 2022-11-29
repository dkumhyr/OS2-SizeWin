/* --------------------------------------------------------------------------
|
|     SIZEWIN.H
|
|     SizeWin - OS/2 Presentation Manager Window Sizing Tool
|
|     Copyright 1994 by David B. Kumhyr
|
|     All rights reserved under U.S. and international copyright laws.
|     FreeWare application, Free use and distribution is encouraged.
|
|     Copyright owner : David B. Kumhyr
|                       Fuquay-Varina, NC 27526
|
|     Author(s) ......: David B. Kumhyr
|     Date ...........: 04Nov93
|     Operating system: OS/2 2.1
|     Version control : SCCS
|     Compiler vers ..: Borland C++ 1.0
|     Make files .....: SIZEWIN.PRJ
|
+----------------------------------------------------------------------------
|
|     Date last update: 27Jun94
|     Programmer .....: David B. Kumhyr
|     Version control : SCCS
|     Change flag ....: None
|     Changes made ...: Update to Borland C++ 1.5
|
+--------------------------------------------------------------------------
|
|     Author(s) ......: David B. Kumhyr
|     Date ...........: 29Nov22
|     Version control : GIT
|
|     - Update contact information, add to GIT
|
|     Copyright owner : David B. Kumhyr
|                       david_kumhyr@yahoo.com
|                       Austin, TX, 78737
|
+-------------------------------------------------------------------------- */

#define ID_SIZEWIN       1
#define IDP_SIZEWIN      2
#define IDT_SIZEWIN      3

/* --< Menu defines >-- */

/* Tools menu */
#define M_TOOLS        100
#define M_MOUSEPOS     101
#define M_POINT        102
#define M_TARGET       103
#define M_SIZER        104
#define M_FRAME        105
#define M_CALCULATOR   106

/* Options menu */
#define M_OPTIONS      110
#define M_BITMAP       111
#define M_TOP          112
#define M_ZERO         113
#define M_MONITOR      114
#define M_COORDSYS     115
#define M_WINDOW       116
#define M_DIALOG       117

/* Help menu */
#define M_HELP         120
#define M_USEHELP      121
#define M_GENERAL      122
#define M_ABOUT        123

/* --< Help table defines >-- */
#define HT_SIZEWIN     400
#define HST_SIZEWIN    401

/* --< Stringtable defines >-- */
#define IDS_FRAMEDRAW  502
#define IDS_CLOSEFRAME 503

/* --< Dialog box defines >-- */
#define ID_ABOUT       600
#define ID_COORD       601
#define ID_CALCULATE   602
#define ID_MONITOR     603
#define ID_POINT       604
#define ID_FRAME       605

/* Dialog box controls */
#define D_TITLE        701
#define D_SYSTEM       702
#define D_X            703
#define D_Y            704
#define D_CX           705
#define D_CY           706

#define D_MOUSE        707
#define D_TARGET       801
#define D_WINX         802
#define D_WINY         803
#define D_CONVX        804
#define D_CONVY        805
#define D_WIN2DLG      806

#define D_PHYSICAL     809
#define D_VIRTUAL      810
#define D_NEWY         813
#define D_NEWX         814
#define D_MONGROUP     815
#define D_EGA          816
#define D_VGA          817
#define D_XGA          818
#define D_PS55         819

/* --< Types & defines >-- */

#define  USER_SIZEMSG   WM_USER + 1  /* Our dialog message */
                                     /* mp1  pswp */
                                     /* mp2  pptl */
#define  USER_RECALC    WM_USER + 2  /* Recalculate the coords (xlate) */

enum COORDS {WIN, DLG}; /* coord;*/
enum SELECT {POINT, WINDOW, TRACKFRAME} select;

COORDS Coords = WIN;                  /* Default coordinate system */
SELECT Select = WINDOW;


/* --< Global data >-- */

HAB             hAB;                        /* Anchor block */
HMQ             hMQ;                        // Message queue handle
HWND            hWndFrame,                  /* frame window handle*/
                hWndClient,                 /* client window handle */
                hWndHelp,                   /* help window handle */
                hWndSelect;                 /* target window handle */
LONG            lLogoDisplayTime;           /* About display time */
ERRORID         Err;
BOOL            fZeroed     = FALSE;        /* Track from zero option */
BOOL            fStayOnTop  = FALSE;        /* Pop SizeWin to the top */
BOOL            fBmpMenu    = TRUE;         /* Use Bitmap menu (vs. text menu) */
LONG            x,
                y,
                cx,
                cy;
static HWND     hWndCalcDlg = NULLHANDLE;   /* Calculator dialog hWnd */
static HWND     hWndSizeDlg = NULLHANDLE;
HWND            hWndFFrame;
HWND            hWndFClient;

static LONG     sx = 0;
static LONG     sy = 0;
static SWP      swp;
static POINTL   ptl;
static LONG     ScrX;                /* MLC Scaling stuff */
static LONG     ScrY;
static BOOL     bSelecting = FALSE;     /* Pointer capture */
static HPOINTER hSysPointer;
static HPOINTER hSelPointer;

static TRACKINFO tiStruct;

/* --< Function prototypes >-- */

MRESULT EXPENTRY SizeWinProc     (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY MFWProc         (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY AboutDlgProc    (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY SizeDlgProc     (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY CalcDlgProc     (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY ChgMonDlgProc   (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY GoToPtDlgProc   (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY FrameDlgProc    (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY _export process (HWND, ULONG,  MPARAM, MPARAM);

VOID             ExitProc        (USHORT);
BOOL             SetSystemMenu   (const HWND);
BOOL             CalculateCoord  (const HWND, const USHORT, const USHORT, const SHORT);
VOID             CentreDlgBox    (const HWND, const HWND);
VOID             SorryNoHelp     (const HAB);
BOOL             SetCheck        (const HWND, const SHORT, const BOOL);
BOOL             MouseButtonDown (const HWND, const MPARAM);

