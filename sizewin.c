/* --------------------------------------------------------------------------
|
|     SIZEWIN.C
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
+--------------------------------------------------------------------------
|
|     Author(s) ......: David B. Kumhyr
|     Date ...........: 06Jun94
|     Operating system: OS/2 2.1
|     Version control : SCCS
|     Compiler vers ..: Borland C++ 1.5
|     Make files .....: SIZEWIN.PRJ
|
|     - Added exit list processing
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
/*
 -=-= TODO's !!! =-=-

1 Create an init IPF function
2 Fix dialog coordinate system
3 make help file
5 add sizes into the framedraw window
6. keep min size and not allow client shrink below level

 HWND     hwndDlg;
 PPOINTL  aptlPoints;    // Coordinate points to be mapped
 ULONG    ulCount;       // Number of coordinate points
 BOOL     fOptions;      // Calculation control
 BOOL     fSuccess;      // Coordinates-mapped indicator

 fSuccess = WinMapDlgPoints(hwndDlg,
              aptlPoints, ulCount, fOptions);



 hwndDlg (HWND) - input
    Dialog-window handle.

 aptlPoints (PPOINTL) - input/output
    Coordinate points to be mapped. The mapped points are
    substituted.

 ulCount (ULONG) - input
    Number of coordinate points.

 fOptions (BOOL) - input
    Calculation control:

    TRUE
       The points are in dialog coordinates and are to be mapped
       into window coordinates relative to the window specified by
       the hwndDlg parameter.

    FALSE
       The points are in window coordinates relative to the window
       specified by the hwndDlg parameter and are to be mapped
       into dialog coordinates.



  This example calls
 WinMapDlgPoints to map a point
 from dialog coordinates to window
 coordinates relative to the dialog
 window.


 #define INCL_WINDIALOGS
 #include <os2.h>

 HWND  hwndDlg;          // handle of dialog window
 BOOL  fSuccess;         // success indicator
 POINTL aptlPoint;       // point to be mapped

 // map point to relative window coordinates
 fSuccess = WinMapDlgPoints(hwndDlg, &aptlPoint, 1, TRUE);

 //*****************************************
 // This function maps a set of points from
 // a coordinate space relative to one
 // window into a coordinate space relative
 // to another window.
 //******************************************

 #define INCL_WINWINDOWMGR
 #include <os2.h>

 HWND     hwndFrom;      // Handle of the window from whose coordinates points are to be mapped
 HWND     hwndTo;        // Handle of the window to whose coordinates points are to be mapped
 PPOINTL  aptlPoints;    // Points to be mapped to the new coordinate system
 LONG     lCount;        // Number of points to be mapped
 BOOL     fSuccess;      // Success indicator

 fSuccess = WinMapWindowPoints(hwndFrom,
              hwndTo, aptlPoints, lCount);

 hwndFrom (HWND) - input
    Handle of the window from whose coordinates points are to be
    mapped:

    HWND_DESKTOP
       Points are mapped from screen coordinates

    Other
       Points are mapped from window coordinates.

 hwndTo (HWND) - input
    Handle of the window to whose coordinates points are to be
    mapped:

    HWND_DESKTOP
       Points are mapped into screen coordinates

    Other
       Points are mapped into window coordinates.

 aptlPoints (PPOINTL) - input/output
    Points to be mapped to the new coordinate system.

 lCount (LONG) - input
    Number of points to be mapped. aptlPoints can be a RECTL
    structure, in which case this parameter should have the value 2.

    Note:  This is not supported in all languages.





  This example calls
 WinMapWindowPoints to map a
 mouse point on the desktop
 window to a mouse point in the
 client window and then checks
 whether the mouse pointer is inside
 the client area or not.


 #define INCL_WINWINDOWMGR
 #define INCL_WINRECTANGLES
 #define INCL_WINPOINTERS
 #include <os2.h>

 HAB     hab;            // anchor-block handle
 HWND  hwndClient;       // handle of client window
 BOOL  fSuccess;         // success indicator
 POINTL ptlMouse;        // mouse pointer position
 RECTL rclWork;          // client area

 // get current mouse pointer position
 WinQueryPointerPos(HWND_DESKTOP, &ptlMouse);

 // map from desktop to client window
 fSuccess = WinMapWindowPoints(HWND_DESKTOP, hwndClient,
                               &ptlMouse, 1);

 // check if new mouse position is inside the client area
 WinQueryWindowRect(hwndClient, &rclWork);
 if (WinPtInRect(hab, &rclWork, &ptlMouse))
    {
    // pointer is in client area
    }

*/

#define  INCL_WIN             // PM Windowing
#define  INCL_DOS             // Control program functions
#define  INCL_GPI             // GPI functions
#define  INCL_WINTRACKRECT    // Trancframing support

#include <os2.h>
#include <except.h>           // Exception handling
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "sizewin.h"

// --< Exception Handler >-----------------------------------------------




// --< main >------------------------------------------------------------
INT cdecl main()
{
   QMSG        qmsg;                     // Message queue handle
   HELPINIT    hinit;                    // Help init structure
   ULONG       ctlData;                  // Window style data
   SWCNTRL     SwitchData;               // switching control data struct
   PID         pid;                      // process ID
   TID         tid;                      // Thread ID

   Err = WinGetLastError(hAB);           // Clear error struct
   hAB = WinInitialize(0);               // Set up PM
   hMQ = WinCreateMsgQueue(hAB, 0);      // Create message queue

   if (DosExitList(EXLST_ADD, (PFNEXITLIST)ExitProc)) {
      WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
                    (PSZ)"SizeWin: Cannot load exitlist.",
                    (PSZ)"Error",
                     1, MB_OK | MB_APPLMODAL | MB_MOVEABLE);
      exit(EXIT_PROCESS);
      }


   // Set the logo display duration
   lLogoDisplayTime = PrfQueryProfileInt(HINI_PROFILE,
                                        (PSZ)"PM_ControlPanel",
                                        (PSZ)"LogoDisplayTime", -1L);
   // Call the logo display dialog
   WinDlgBox (HWND_DESKTOP, HWND_DESKTOP, (PFNWP)AboutDlgProc,
              (HMODULE)NULL, ID_ABOUT, NULL);

   // Reset logo display so we can see 'ABOUT' normally
   lLogoDisplayTime = -1L;

   // Initializing IPF Structure
   hinit.cb                       = sizeof(HELPINIT);
   hinit.ulReturnCode             = 0L;
   hinit.pszTutorialName          = (PSZ)NULL;
   hinit.phtHelpTable             = (PHELPTABLE)(0xffff0000 | HT_SIZEWIN);
   hinit.hmodAccelActionBarModule = (HMODULE)NULL;
   hinit.idAccelTable             = 0;
   hinit.idActionBar              = 0;
   hinit.pszHelpWindowTitle       = (PSZ)"SIZEWIN.HLP";
   hinit.hmodHelpTableModule      = (HMODULE)NULL;
   hinit.fShowPanelId             = 0;
   hinit.pszHelpLibraryName       = (PSZ)"SIZEWIN.HLP";

   // Create instance of IPF
   hWndHelp = WinCreateHelpInstance (hAB, &hinit);

   if (!hWndHelp)
      SorryNoHelp(hAB);
   else
      if (hinit.ulReturnCode) {

         WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
                       (PSZ)"IPF Help Ended",
                       (PSZ)"IPF Help error",
                        1, MB_OK | MB_APPLMODAL | MB_MOVEABLE);
         WinDestroyHelpInstance(hWndHelp);
         }

   // get the screen coordinates for MLC sizing
   ScrX = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
   ScrY = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);

   // Register the application class
   WinRegisterClass(hAB,
                   (PCH)"SIZEWIN",
                   (PFNWP)SizeWinProc,
                   CS_MOVENOTIFY | CS_SIZEREDRAW,
                   (USHORT)0);

   // Register the framer class
   WinRegisterClass(hAB,
                   (PCH)"FWIN",
                   (PFNWP)MFWProc,
                    CS_SIZEREDRAW | CS_MOVENOTIFY,
                   (USHORT)0);

   ctlData = FCF_ICON | FCF_BORDER | FCF_MENU | FCF_MINBUTTON | FCF_SIZEBORDER | FCF_SYSMENU | FCF_TITLEBAR;

   hWndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                  WS_VISIBLE,
                                  &ctlData,
                                  (PSZ)"SIZEWIN",
                                  (PSZ)"SizeWin",
                                  0L,
                                  (HMODULE)NULL,
                                  ID_SIZEWIN,
                                  &hWndClient);

   // Query the profile for our last position, set starting size and position
   x  = PrfQueryProfileInt(HINI_USERPROFILE, (PSZ)"SIZEWIN", (PSZ)"SWP.X",  68);
   y  = PrfQueryProfileInt(HINI_USERPROFILE, (PSZ)"SIZEWIN", (PSZ)"SWP.Y",  124);
   cx = PrfQueryProfileInt(HINI_USERPROFILE, (PSZ)"SIZEWIN", (PSZ)"SWP.CX", 196);
   cy = PrfQueryProfileInt(HINI_USERPROFILE, (PSZ)"SIZEWIN", (PSZ)"SWP.CY", 160);
   WinSetWindowPos(hWndFrame, HWND_TOP, x, y, cx, cy, SWP_MOVE | SWP_SIZE | SWP_ACTIVATE);

   // Add ourselves to the switch list
   SwitchData.hwnd          = hWndFrame;
   SwitchData.hwndIcon      = (HWND)WinSendMsg(hWndFrame, WM_QUERYICON, NULL, NULL);
   SwitchData.hprog         = (HPROGRAM)NULL;
   WinQueryWindowProcess (SwitchData.hwnd, &pid, &tid);
   SwitchData.idProcess     = pid;
   SwitchData.idSession     = (USHORT)NULL;
   SwitchData.uchVisibility = SWL_VISIBLE;
   SwitchData.fbJump        = SWL_JUMPABLE;
   sprintf(SwitchData.szSwtitle, "%s", "* SizeWin *");
   WinAddSwitchEntry ((PSWCNTRL)&SwitchData);

   if (!hWndFrame) return 0;

   // Dispatch loop
   while( WinGetMsg(hAB, (PQMSG)&qmsg, NULLHANDLE, 0, 0))
      WinDispatchMsg(hAB, (PQMSG)&qmsg);

   return (INT)TRUE;
}

/* --< ExitProc >------------------------------------------------------------
|
|     Handles application exit cleanup.
|
+-------------------------------------------------------------------------- */
VOID ExitProc(USHORT usTermCode) {

   if (WinIsWindow(hAB, hWndFrame))
      WinDestroyHelpInstance (hWndHelp);

   if (WinIsWindow(hAB, hWndFrame))
      WinDestroyWindow(hWndFrame);

   WinDestroyMsgQueue(hMQ);
   WinTerminate(hAB);
   DosExitList(EXLST_EXIT, (PFNEXITLIST)NULL);
}


// --< SizeWinProc >---------------------------------------------------
MRESULT EXPENTRY SizeWinProc(HWND hWnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
   HPS             hPS;                    // Presentation space
   CHAR            szTemp[40];             // Temporary buffer
   CHAR            szCaptureTitle[80];     // Title buffer
   HWND            hwnd;                   // Window handle
   RECTL           rc;                     // Window rectangle
   POINTL          ptrPos;                 // Pointer position
   SWP             swpPos;                 // Last window position
   SWP             swpWin;                 // Window SWP
   SWP             swpLB;                  // Dialog SWP
   CHAR            prfBuf[10];             // Profile buffer
   ULONG           ctlData,                // Window control data
                   ulHelpError;            // Help error return code

   switch(msg) {

   case  WM_CREATE:

         hSysPointer = WinQueryPointer(HWND_DESKTOP);

         // clear tracking rect structure with new window
         tiStruct.rclTrack.xLeft   = (LONG)NULL;
         tiStruct.rclTrack.yBottom = (LONG)NULL;
         tiStruct.rclTrack.xRight  = (LONG)NULL;
         tiStruct.rclTrack.yTop    = (LONG)NULL;

         // Restore menu value
         fBmpMenu = (BOOL)PrfQueryProfileInt(HINI_USERPROFILE,
                                            (PSZ)"SIZEWIN", (PSZ)"BMPMENU", 1);
         if (!fBmpMenu) {
            WinDestroyWindow(WinWindowFromID(hWndFrame, FID_MENU));
            WinLoadMenu(hWndFrame, (HMODULE)NULL, IDT_SIZEWIN);
            }
         // Restore float on top value
         fStayOnTop = (BOOL)PrfQueryProfileInt(HINI_USERPROFILE,
                                         (PSZ)"SIZEWIN", (PSZ)"FLOAT", 0);
         if (fStayOnTop) {
            SetCheck(hWnd, M_TOP, TRUE);
            WinStartTimer (hAB, hWnd, 1, 100) ;
            }

         // Check profile for coordinate default
         Coords = (COORDS)PrfQueryProfileInt (HINI_USERPROFILE,
                                           (PSZ)"SIZEWIN",
                                           (PSZ)"COORD_SYS",
                                           (SHORT)WIN);

         // Only need to set if not using the default WIN
         switch (Coords) {

            case WIN:

                 SetCheck(hWnd, M_WINDOW, TRUE);
                 SetCheck(hWnd, M_DIALOG, FALSE);                 break;

            case DLG:

                 SetCheck(hWnd, M_WINDOW, FALSE);
                 SetCheck(hWnd, M_DIALOG, TRUE);
                 break;

            }

         hWndSizeDlg = WinLoadDlg (hWnd, hWnd, (PFNWP)SizeDlgProc,
                                  (HMODULE)NULL, ID_COORD, NULL);
         break;

    case WM_SIZE:

         WinQueryWindowPos(hWnd, &swpWin);
         WinQueryWindowPos(WinWindowFromID(hWnd, ID_COORD), &swpLB);
         WinSetWindowPos(WinWindowFromID(hWnd, ID_COORD), HWND_TOP,
                         swpLB.x,
                         swpLB.y,
                         swpWin.cx,      // Use the win width for lb width
                         swpWin.cy, // - 15, // Win height - title height
                         SWP_SIZE | SWP_MOVE | SWP_SHOW);
        break;

   case  WM_COMMAND:

         switch(SHORT1FROMMP(mp1)) {

         // Coordinates menu
         case M_WINDOW:

              Coords = WIN;
              SetCheck(hWnd, M_WINDOW, TRUE);
              SetCheck(hWnd, M_DIALOG, FALSE);
              PrfWriteProfileString(HINI_USERPROFILE,
                                    (PSZ)"SIZEWIN",
                                    (PSZ)"SYS",
                                    (PSZ)"1");
              WinSendMsg(hWndSizeDlg, USER_RECALC, (MPARAM)NULL, (MPARAM)NULL);
              return (MRESULT)TRUE;

         case M_DIALOG:

              Coords = DLG;
              SetCheck(hWnd, M_WINDOW, FALSE);
              SetCheck(hWnd, M_DIALOG, TRUE);
              PrfWriteProfileString(HINI_USERPROFILE,
                                  (PSZ)"SIZEWIN",
                                  (PSZ)"SYS",
                                  (PSZ)"2");
              WinSendMsg(hWndSizeDlg, USER_RECALC, (MPARAM)NULL, (MPARAM)NULL);
              return (MRESULT)TRUE;

         case M_MOUSEPOS:

              // Capture the mouse for a point
              bSelecting = TRUE;
              Select     = POINT;
              WinSetWindowText(hWndFrame, (PSZ)"Pointer Position");
              WinSetCapture(HWND_DESKTOP, hWnd);
              hSysPointer = WinQueryPointer(HWND_DESKTOP);
              hSelPointer = WinLoadPointer(HWND_DESKTOP,
                                           (HMODULE)NULL,
                                           IDP_SIZEWIN);
              WinSetPointer(HWND_DESKTOP, hSelPointer);
              return (MRESULT)TRUE;

         case M_TARGET:

              // Capture the mouse for a window query
              bSelecting = TRUE;
              Select     = WINDOW;
              WinSetWindowText(hWndFrame, (PSZ)"Selecting Window");
              WinSetCapture(HWND_DESKTOP, hWnd);
              hSysPointer = WinQueryPointer(HWND_DESKTOP);
              hSelPointer = WinLoadPointer(HWND_DESKTOP,
                                           (HMODULE)NULL,
                                           IDP_SIZEWIN);
              WinSetPointer(HWND_DESKTOP, hSelPointer);
              return (MRESULT)TRUE;

         case M_SIZER:

              // Trackframe
              bSelecting = TRUE;
              Select     = TRACKFRAME;
              WinSetWindowText(hWndFrame, (PSZ)"Trackframing");
              WinSetCapture(HWND_DESKTOP, hWnd);
              hSysPointer = WinQueryPointer(HWND_DESKTOP);
              hSelPointer = WinQueryPointer(HWND_DESKTOP);
              WinSetPointer(HWND_DESKTOP, hSelPointer);
              return (MRESULT)TRUE;

         case M_FRAME:

              if (WinDlgBox (HWND_DESKTOP, hWnd, (PFNWP)FrameDlgProc,
                            (HMODULE)NULL, ID_FRAME, NULL)) {
                 ctlData = FCF_SIZEBORDER | FCF_SYSMENU | FCF_TITLEBAR;
                 hWndFFrame = WinCreateStdWindow(HWND_DESKTOP,
                                  WS_VISIBLE,
                                  &ctlData,
                                  (PSZ)"FWIN",
                                  (PSZ)"SizeWin Frame Draw Window",
                                  WS_VISIBLE,
                                  (HMODULE)NULL,
                                  (USHORT)0,
                                 &hWndFClient);

                 WinSetWindowPos(hWndFFrame, HWND_TOP, x, y, cx, cy,
                                 SWP_MOVE | SWP_SIZE | SWP_ACTIVATE);
                 }
              break;

         case M_POINT:

              WinDlgBox (HWND_DESKTOP, hWnd, (PFNWP)GoToPtDlgProc,
                        (HMODULE)NULL, ID_POINT, NULL);
              break;

         case M_CALCULATOR:

              hWndCalcDlg = WinLoadDlg (HWND_DESKTOP, hWnd, (PFNWP)CalcDlgProc,
                                       (HMODULE)NULL, ID_CALCULATE, NULL);
              break;

         case M_BITMAP:

              if (fBmpMenu) {
                 fBmpMenu = FALSE;
                 WinDestroyWindow(WinWindowFromID(hWndFrame, FID_MENU));
                 WinLoadMenu(hWndFrame, (HMODULE)NULL, IDT_SIZEWIN);
                 PrfWriteProfileString(HINI_USERPROFILE,(PSZ)"SIZEWIN",
                                       (PSZ)"BMPMENU",(PSZ)"0");
                 }
              else {
                 fBmpMenu = TRUE;
                 WinDestroyWindow(WinWindowFromID(hWndFrame, FID_MENU));
                 WinLoadMenu(hWndFrame, (HMODULE)NULL, ID_SIZEWIN);
                 PrfWriteProfileString(HINI_USERPROFILE,(PSZ)"SIZEWIN",
                                       (PSZ)"BMPMENU",(PSZ)"1");                 }
              WinSendMsg(hWndFrame, WM_UPDATEFRAME,(MPARAM)FCF_MENU,(MPARAM)NULL);
              break;

         case M_TOP:

              if (!fStayOnTop) {
                 fStayOnTop = TRUE;
                 SetCheck(hWnd, M_TOP, TRUE);
                 PrfWriteProfileString(HINI_USERPROFILE,
                                       (PSZ)"SIZEWIN",
                                       (PSZ)"FLOAT",
                                       (PSZ)"1");
                 WinStartTimer (hAB, hWnd, 1, 100) ;
                 }
              else {
                 fStayOnTop = FALSE;
                 SetCheck(hWnd, M_TOP, FALSE);
                 PrfWriteProfileString(HINI_USERPROFILE,
                                       (PSZ)"SIZEWIN",
                                       (PSZ)"FLOAT",
                                       (PSZ)"0");
                 WinStopTimer (hAB, hWnd, 1);
                 }
              break;

         case M_ZERO:

              // If we starting zeroing the x,y coords
              if (!fZeroed) {
                 fZeroed = TRUE;
                 SetCheck(hWnd, M_ZERO, TRUE);
                 }
              else {
                 fZeroed = FALSE;
                 SetCheck(hWnd, M_ZERO, FALSE);
                 }
              break;

         case M_MONITOR:

              WinDlgBox (HWND_DESKTOP, hWnd, (PFNWP)ChgMonDlgProc,
                        (HMODULE)NULL, ID_MONITOR, NULL);

              break;

         // Help menus
         case M_GENERAL:

              if (!hWndHelp)
                 SorryNoHelp(hAB);
              else
                 WinSendMsg (hWndHelp, HM_DISPLAY_HELP,
                             MPFROMP((PVOID)M_GENERAL),
                             MPFROMSHORT((USHORT)HM_RESOURCEID));
              break;

         case M_ABOUT:

              WinDlgBox (HWND_DESKTOP, hWnd, (PFNWP)AboutDlgProc,
                        (HMODULE)NULL, ID_ABOUT, NULL);
              break;
         }
         break;

       case WM_TIMER:

            // Make us stay on top, but we don't need focus
            WinSetWindowPos (hWndFrame, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER | SWP_SHOW);
            return (MRESULT)TRUE;

       case WM_CLOSE:
       case WM_QUIT:
       case WM_DESTROY:

            // Query the window position to save the last position in the profile
            WinQueryWindowPos(hWndFrame, (PSWP)&swpPos);
            // Query the profile for our last position
            PrfWriteProfileString(HINI_USERPROFILE, (PSZ)"SIZEWIN", (PSZ)"SWP.X",
                                 (PSZ)ltoa(swpPos.x, prfBuf, 10));
            PrfWriteProfileString(HINI_USERPROFILE, (PSZ)"SIZEWIN", (PSZ)"SWP.Y",
                                 (PSZ)ltoa(swpPos.y, prfBuf, 10));
            PrfWriteProfileString(HINI_USERPROFILE, (PSZ)"SIZEWIN", (PSZ)"SWP.CX",
                                 (PSZ)ltoa(swpPos.cx, prfBuf, 10));
            PrfWriteProfileString(HINI_USERPROFILE, (PSZ)"SIZEWIN", (PSZ)"SWP.CY",
                                 (PSZ)ltoa(swpPos.cy, prfBuf, 10));
            WinPostMsg(hWnd, WM_QUIT, 0L, 0L );
            break;

       case HM_ERROR:

            ulHelpError = (ULONG)mp1;
            sprintf(szTemp, "Help manager error %x", ulHelpError);
            WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
                          (PSZ)szTemp,
                          (PSZ)"SizeWin",
                           1, MB_OK | MB_ICONHAND | MB_MOVEABLE);            return (MRESULT)TRUE;

       case WM_CONTROL:

            break;

       case WM_PAINT:

            hPS = WinBeginPaint( hWnd, (HPS)NULL, (PRECTL)&rc);
            WinFillRect(hPS, &rc, CLR_CYAN);
            WinEndPaint(hPS);
            return (MRESULT)TRUE;

       case WM_BUTTON1DOWN:

            // Get the coordinates of the mouse click
            ptrPos.x = SHORT1FROMMP(mp1);
            ptrPos.y = SHORT2FROMMP(mp1);

            if (bSelecting) {

               // Turn off select mode
               bSelecting = FALSE;
               // Turn off mouse capture
               WinSetCapture(HWND_DESKTOP, NULLHANDLE);
               // Restore the sys pointer
               WinSetPointer(HWND_DESKTOP, hSysPointer);

               // Case based upon what mode we are in
               switch (Select) {

                  case POINT:

                       WinQueryPointerPos(HWND_DESKTOP, &ptl);
                       // fixup swp
                       swp.x = (SHORT)ptl.x;
                       swp.y = (SHORT)ptl.y;
                       swp.cx = 0;
                       swp.cy = 0;
                       break;

                  case WINDOW:

                       // Convert points
                       WinMapWindowPoints(hWnd, HWND_DESKTOP, (PPOINTL)&ptrPos, 1);
                       // Get the window we are pointing to
                       hWndSelect = WinWindowFromPoint(HWND_DESKTOP, (PPOINTL)&ptrPos, TRUE);
                       // Get the hWndFrame
                       hwnd = WinQueryWindow(hWndSelect, QW_PARENT);

                       // Query the title
                       WinQueryWindowText(hwnd, sizeof(szTemp), (PSZ)szTemp);
                       WinQueryWindowPos(hwnd, &swp);

                       // convert the values in cx & cy to size of box
//                       swp.cx; // = swp.cx - swp.x;
//                       swp.cy; // = swp.cy - swp.y;

                       // Set the dialogs title to reflect the captured window
                       sprintf(szCaptureTitle, "Title -> %s", szTemp);
                       WinSetWindowText(hWndSizeDlg, (PSZ)szCaptureTitle);
                       break;

                  case TRACKFRAME:

                       MouseButtonDown (HWND_DESKTOP, mp1);
                       WinSetWindowText(hWndSizeDlg, (PSZ)"Trackframe Coords");
                       break;
                  }
               }

            WinSendMsg(hWndSizeDlg, USER_SIZEMSG, (MPARAM)&swp, (MPARAM)&ptl);
            return (MRESULT)TRUE;

       case WM_MOUSEMOVE:

            if (bSelecting)
               WinSetPointer (HWND_DESKTOP, hSelPointer);
            else
               WinSetPointer (HWND_DESKTOP, hSysPointer);

            return (MRESULT)TRUE;

       default:

           return(WinDefWindowProc(hWnd,msg,mp1,mp2));
      }

   return (MRESULT)FALSE;
}


// --< MFWProc >----------------------------------------------------

// NTD - add a menu for Dialog or Window coords, stay on top
// and show our info on the client...

MRESULT EXPENTRY MFWProc(HWND hWnd,ULONG msg,MPARAM mp1,MPARAM mp2)

{
   LONG    x, y, cx, cy;

   switch(msg) {

   case WM_CLOSE: {

        HWND hWndFrame;

        hWndFrame = WinQueryWindow(hWnd, QW_PARENT);
        WinDestroyWindow(hWndFrame);
        }
        return (MRESULT)TRUE;

   case WM_MOVE:  // Let SizeWin know where we are.
   case WM_SIZE: {

        SWP   swp;

        WinQueryWindowPos(hWnd, &swp);
        x  = swp.x;
        y  = swp.y;
        cx = swp.cx;
        cy = swp.cy;
        }
        break;

   case WM_PAINT: {

        HPS    hPS;
        RECTL  rc;
        POINTL pt;

        hPS = WinBeginPaint(hWnd, 0L, &rc);
//        pt.x = 50; pt.y = 50;
        GpiSetColor(hPS, CLR_NEUTRAL);
        GpiSetBackColor(hPS, CLR_BACKGROUND);
        GpiSetBackMix(hPS, BM_OVERPAINT);
//       GpiCharStringAt(hPS, &pt, (LONG)strlen(szString), szString);
        WinEndPaint(hPS);
        }
        break;

   default:

        return(WinDefWindowProc(hWnd,msg,mp1,mp2));
   }
} // End of MFWProc

// --< AboutDlgProc >----------------------------------------------------
MRESULT EXPENTRY AboutDlgProc(HWND hWndDlg, ULONG message, MPARAM mp1, MPARAM mp2)
{
 BOOL  fTimerSet;

 switch(message) {

    case WM_INITDLG:

         switch((INT)lLogoDisplayTime) {

            case -1:  // Show as long as no control is pushed

                 break;

            case 0:   // No logo display

                 WinDismissDlg(hWndDlg, FALSE);
                 break;

            default:  // All non zero and not -1
                      // are timer
                 fTimerSet = TRUE;
                 WinStartTimer(hAB, hWndDlg, 1, lLogoDisplayTime);
                 break;

            }

         CentreDlgBox(HWND_DESKTOP, hWndDlg);
         break;

    case WM_TIMER:

         WinDismissDlg (hWndDlg, TRUE);
         break;

    case WM_HELP:

         if (!hWndHelp)
            SorryNoHelp(hAB);
         else
            WinSendMsg (hWndHelp, HM_DISPLAY_HELP,
                           MPFROMP((PVOID)M_ABOUT),
                           MPFROMSHORT((USHORT)HM_RESOURCEID));
         break;

    case WM_COMMAND:

         switch( SHORT1FROMMP(mp1)) {

         case DID_OK:

              if (fTimerSet)
                 WinStopTimer (hAB, hWndDlg, 1);

              WinDismissDlg(hWndDlg, TRUE);
              break;

         case DID_CANCEL:

              if (fTimerSet)
                 WinStopTimer (hAB, hWndDlg, 1);

              WinDismissDlg(hWndDlg, FALSE);
              break;
              }

         break;

    default:

         return(WinDefDlgProc(hWndDlg, message, mp1, mp2));
         }
 return (MRESULT)FALSE;
}



// --< CalcDlgProc >----------------------------------------------------

MRESULT EXPENTRY CalcDlgProc(HWND hWndDlg, ULONG message, MPARAM mp1, MPARAM mp2)
{
   static SHORT sSystem = D_WIN2DLG;

 switch(message) {

    case WM_INITDLG:

         CentreDlgBox(HWND_DESKTOP, hWndDlg);

         WinSendDlgItemMsg(hWndDlg, sSystem, BM_SETCHECK,
                           MPFROM2SHORT (TRUE, 0), NULL);

         WinSendDlgItemMsg(hWndDlg, D_WINX,
                           EM_SETTEXTLIMIT,
                           MPFROMSHORT(4L),0L);

         WinSendDlgItemMsg(hWndDlg, D_WINY,
                           EM_SETTEXTLIMIT,
                           MPFROMSHORT(4L),0L);

         break;

    case WM_HELP:

         if (!hWndHelp)
            SorryNoHelp(hAB);
         else
            WinSendMsg (hWndHelp, HM_DISPLAY_HELP,
                           MPFROMP((PVOID)M_CALCULATOR),
                           MPFROMSHORT((USHORT)HM_RESOURCEID));
         break;

    case WM_CONTROL:


         switch(SHORT1FROMMP(mp1)) {

           case D_WIN2DLG:

                sSystem = D_WIN2DLG;
                break;

           case D_WINX:

                CalculateCoord(hWndDlg, D_WINX, D_CONVX, sSystem);
                break;

           case D_WINY:

                CalculateCoord(hWndDlg, D_WINY, D_CONVY, sSystem);
                break;

           }

         break;


    case WM_COMMAND:

         switch( SHORT1FROMMP(mp1)) {

         case DID_CANCEL:

              hWndCalcDlg = NULLHANDLE;
              WinDismissDlg(hWndDlg, FALSE);
              break;
              }

         break;

    default:

         return(WinDefDlgProc(hWndDlg, message, mp1, mp2));
         }

 return (MRESULT)FALSE;
}

BOOL CalculateCoord(const HWND   hWnd,
                    const USHORT usIn,
                    const USHORT usOut,
                    const SHORT  sSystem)
{
   BOOL  fRC;
   CHAR  szBuf[10];
   int   iIn, iOut;

   // Get the input
   WinQueryDlgItemText(hWnd, usIn, sizeof(szBuf), (PSZ)szBuf);

   // NTD an isdigit test

   // Convert to numeric
   iIn = atoi(szBuf);


   switch (sSystem) {

      case D_WIN2DLG:

           // Is it X or Y?
           if (usIn == D_WINX)
              iOut = (int)((LONG)iIn/4L); // Dialog
           else
              iOut = (int)((LONG)iIn/8L); // Dialog

           // Output text
           sprintf(szBuf, "%d", iOut);
           WinSetDlgItemText (hWnd, usOut, (PSZ)szBuf);
           break;

      }

   return fRC;
}



// --< SizeDlgProc >----------------------------------------------------

MRESULT EXPENTRY SizeDlgProc(HWND hWndDlg, ULONG message, MPARAM mp1, MPARAM mp2)
{
   CHAR   szSystem[10];
   CHAR   szX[8];
   CHAR   szY[8];
   CHAR   szCX[8];
   CHAR   szCY[8];
   CHAR   szMouse[12];

   switch(message) {

   case WM_INITDLG:

       // WinQueryWindowPos( hWndDlg, (PSWP)&swp );
        WinSetWindowPos( hWndDlg, HWND_TOP, 0, 0, 0, 0, SWP_MOVE );

      //CentreDlgBox(HWND_DESKTOP, hWndDlg);
      //WinQueryWindowPos(hWndFrame, &swp);
      //WinQueryPointerPos(HWND_DESKTOP, &ptl);


        switch (Coords) {

                case DLG:

                     sprintf(szSystem,"%s",    "Dialog");
                     break;

                default:

                     sprintf(szSystem,"%s",    "Window");
                     break;
                }


        WinSetDlgItemText (hWndDlg, D_SYSTEM, (PSZ)szSystem);
        sprintf(szX,     "%d",    0);
        sprintf(szY,     "%d",    0);
        sprintf(szCX,    "%d",    0);
        sprintf(szCY,    "%d",    0);
        sprintf(szMouse, "%d:%d", 0,0);

        WinSetDlgItemText (hWndDlg, D_X,     (PSZ)szX);
        WinSetDlgItemText (hWndDlg, D_Y,     (PSZ)szY);
        WinSetDlgItemText (hWndDlg, D_CX,    (PSZ)szCX);
        WinSetDlgItemText (hWndDlg, D_CY,    (PSZ)szCY);
        WinSetDlgItemText (hWndDlg, D_MOUSE, (PSZ)szMouse);


        break;

   case USER_SIZEMSG:

        switch (Coords) {

        case WIN:

             sprintf(szSystem,"%s",    "Window");
             sprintf(szX,     "%d",    swp.x);
             sprintf(szY,     "%d",    swp.y);
             sprintf(szCX,    "%d",    swp.cx);
             sprintf(szCY,    "%d",    swp.cy);
             sprintf(szMouse, "%d:%d", (int)ptl.x, (int)ptl.y);
             break;

        case DLG:

             sprintf(szSystem,"%s",    "Dialog");
             sprintf(szX,     "%d",    swp.x/4L);
             sprintf(szY,     "%d",    swp.y/8L);
             sprintf(szCX,    "%d",    swp.cx/4L);
             sprintf(szCY,    "%d",    swp.cy/8L);
             sprintf(szMouse, "%d:%d", (int)(ptl.x/4L), (int)(ptl.y/8L));
             break;
             }

        WinSetDlgItemText (hWndDlg, D_SYSTEM, (PSZ)szSystem);
        WinSetDlgItemText (hWndDlg, D_X,      (PSZ)szX);
        WinSetDlgItemText (hWndDlg, D_Y,      (PSZ)szY);
        WinSetDlgItemText (hWndDlg, D_CX,     (PSZ)szCX);
        WinSetDlgItemText (hWndDlg, D_CY,     (PSZ)szCY);
        WinSetDlgItemText (hWndDlg, D_MOUSE,  (PSZ)szMouse);

        break;

   case USER_RECALC:

        switch (Coords) {

        case WIN:

             sprintf(szSystem,"%s",    "Window");
             sprintf(szX,     "%d",    swp.x);
             sprintf(szY,     "%d",    swp.y);
             sprintf(szCX,    "%d",    swp.cx);
             sprintf(szCY,    "%d",    swp.cy);
             sprintf(szMouse, "%d:%d", (int)ptl.x, (int)ptl.y);
             break;

        case DLG:

             sprintf(szSystem,"%s",    "Dialog");
             sprintf(szX,     "%d",    swp.x/4L);
             sprintf(szY,     "%d",    swp.y/8L);
             sprintf(szCX,    "%d",    swp.cx/4L);
             sprintf(szCY,    "%d",    swp.cy/8L);
             sprintf(szMouse, "%d:%d", (int)(ptl.x/4L), (int)(ptl.y/8L));
             break;
             }

        WinSetDlgItemText (hWndDlg, D_SYSTEM, (PSZ)szSystem);
        WinSetDlgItemText (hWndDlg, D_X,      (PSZ)szX);
        WinSetDlgItemText (hWndDlg, D_Y,      (PSZ)szY);
        WinSetDlgItemText (hWndDlg, D_CX,     (PSZ)szCX);
        WinSetDlgItemText (hWndDlg, D_CY,     (PSZ)szCY);
        WinSetDlgItemText (hWndDlg, D_MOUSE,  (PSZ)szMouse);

        break;

   default:

        return(WinDefDlgProc(hWndDlg, message, mp1, mp2));
         }
 return (MRESULT)FALSE;
}



// --< CentreDlgBox >----------------------------------------------------

VOID CentreDlgBox(const HWND hWndParent, const HWND hWndDlg)
{
  LONG  x,
        y;
  LONG  lWidth,
        lDepth;
  SWP   swp;

  // Query width and depth of screen device
  lWidth = WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
  lDepth = WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );

  // Query width and depth of dialog box
  WinQueryWindowPos(hWndDlg, (PSWP)&swp );

  // Center dialog box within the screen
  x = (lWidth  - swp.cx ) / 2;
  y = (lDepth  - swp.cy ) / 2;

  WinSetWindowPos(hWndDlg, HWND_TOP, x, y, 0, 0, SWP_MOVE );
}

// --< SorryNoHelp >-----------------------------------------------------

VOID SorryNoHelp(const HAB hAB)
{
   WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
                 (PSZ)"Sorry, but IPF help is unavailable.",
                 (PSZ)"SizeWin",
                  1, MB_OK | MB_ICONHAND | MB_MOVEABLE);
}


// --< SetCheck >--------------------------------------------------------

BOOL SetCheck(const HWND hWnd, const SHORT sID, const BOOL fState)
{
   HWND hwnd;

   hwnd = WinQueryWindow(hWnd, QW_PARENT);

   if (fState)
      WinSendMsg (WinWindowFromID(hwnd, FID_MENU),
                  MM_SETITEMATTR,
                  MPFROM2SHORT(sID, TRUE),
                  MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
   else
      WinSendMsg (WinWindowFromID(hwnd, FID_MENU),
                  MM_SETITEMATTR,
                  MPFROM2SHORT(sID, TRUE),
                  MPFROM2SHORT(MIA_CHECKED, FALSE));

   return TRUE;
}



/* --< MouseButtonDown >---------------------------------------------------
|
|  Code cut from CLIPBRD.C OS/2 PM Clipboard sample program, draw a
|  tracking rectangle.
|  Thanks to Dave Hock (HOCK @ CARVM7) for suggestion
|
+------------------------------------------------------------------------ */

BOOL MouseButtonDown (const HWND hwnd, const MPARAM mp1)
{
   HPS hPSTrack;
   HDC hDCTrack;
   PSIZEL psizl;

   tiStruct.cxBorder   = 2;
   tiStruct.cyBorder   = 2;
   tiStruct.cxGrid     = 0;
   tiStruct.cyGrid     = 0;
   tiStruct.cxKeyboard = 5;
   tiStruct.cyKeyboard = 5;
   tiStruct.rclTrack.xLeft   = 0;
   tiStruct.rclTrack.yBottom = 0;
   tiStruct.rclTrack.xRight  = 0;
   tiStruct.rclTrack.yTop    = 0;

   // Get the size of the drawing window
   WinQueryWindowRect (HWND_DESKTOP, &tiStruct.rclBoundary);

   tiStruct.rclBoundary.yBottom = 0;

   WinQueryPointerPos(HWND_DESKTOP, &ptl);
   WinSetRect (hAB, &tiStruct.rclTrack, (SHORT)ptl.x, (SHORT)ptl.y, 0, 0);

   tiStruct.ptlMinTrackSize.x = 1;
   tiStruct.ptlMinTrackSize.y = 1;
   tiStruct.ptlMaxTrackSize.x = ++tiStruct.rclBoundary.xRight;
   tiStruct.ptlMaxTrackSize.y = ++tiStruct.rclBoundary.yTop;
   tiStruct.fs  = TF_RIGHT | TF_TOP;

   // Create an alternate HPS for the trackrect
   hDCTrack = WinOpenWindowDC (hwnd);
   hPSTrack = GpiCreatePS (hAB, hDCTrack, psizl, GPIT_MICRO);
   WinTrackRect (hwnd, (HPS)NULL, &tiStruct);
   GpiDestroyPS (hPSTrack);

   // What is the size of the drawn rectangle
   if (fZeroed) {  // Start calc from zero
      swp.x  = (SHORT)0;
      swp.y  = (SHORT)0;
      swp.cx = (SHORT)tiStruct.rclTrack.xRight;
      swp.cy = (SHORT)tiStruct.rclTrack.yTop;

      }
   else {
      swp.x  = (SHORT) tiStruct.rclTrack.xLeft;
      swp.y  = (SHORT)tiStruct.rclTrack.yBottom;
      swp.cx = (SHORT)(tiStruct.rclTrack.xRight - tiStruct.rclTrack.xLeft);
      swp.cy = (SHORT)(tiStruct.rclTrack.yTop - tiStruct.rclTrack.yBottom);
      }

   return TRUE;
}

// --< ChgMonDlgProc >--------------------------------------------------

MRESULT EXPENTRY ChgMonDlgProc(HWND hWndDlg, ULONG message, MPARAM mp1, MPARAM mp2)
{

   CHAR     szText[10];
   LONG     lMonX,
            lMonY;

   switch(message) {

   case WM_INITDLG:

         CentreDlgBox(HWND_DESKTOP, hWndDlg);

         // Physical screen coordinates of monitor
         lMonX = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
         lMonY = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);

         sprintf(szText, "%d", lMonX);
         WinSetDlgItemText(hWndDlg, D_X, (PSZ)szText);
         sprintf(szText, "%d", lMonY);
         WinSetDlgItemText(hWndDlg, D_Y, (PSZ)szText);

         sprintf(szText, "%d", ScrX);
         WinSetDlgItemText(hWndDlg, D_NEWX, (PSZ)szText);
         sprintf(szText, "%d", ScrY);
         WinSetDlgItemText(hWndDlg, D_NEWY, (PSZ)szText);

         WinSendDlgItemMsg(hWndDlg, D_NEWX, EM_SETTEXTLIMIT, MPFROMSHORT(4L),0L);
         WinSendDlgItemMsg(hWndDlg, D_NEWY, EM_SETTEXTLIMIT, MPFROMSHORT(4L),0L);
         WinSendDlgItemMsg(hWndDlg, D_NEWX, EM_SETSEL,
                                    MPFROM2SHORT((USHORT)0, (USHORT)4), 0L);
         break;

    case WM_HELP:

         if (!hWndHelp)
            SorryNoHelp(hAB);
         else
            WinSendMsg (hWndHelp, HM_DISPLAY_HELP,
                           MPFROMP((PVOID)M_COORDSYS),
                           MPFROMSHORT((USHORT)HM_RESOURCEID));
         break;

    case WM_COMMAND:

         switch(SHORT1FROMMP(mp1)) {

         case D_EGA:

              WinSetDlgItemText(hWndDlg, D_NEWX, (PSZ)"640");
              WinSetDlgItemText(hWndDlg, D_NEWY, (PSZ)"350");
              return (MRESULT)TRUE;

         case D_VGA:

              WinSetDlgItemText(hWndDlg, D_NEWX, (PSZ)"768");
              WinSetDlgItemText(hWndDlg, D_NEWY, (PSZ)"640");
              return (MRESULT)TRUE;

         case D_XGA:

              WinSetDlgItemText(hWndDlg, D_NEWX, (PSZ)"1024");
              WinSetDlgItemText(hWndDlg, D_NEWY, (PSZ)"768");
              return (MRESULT)TRUE;

         case D_PS55:

              WinSetDlgItemText(hWndDlg, D_NEWX, (PSZ)"1024");
              WinSetDlgItemText(hWndDlg, D_NEWY, (PSZ)"1024");
              return (MRESULT)TRUE;

         case DID_OK:

              // Get the input
              WinQueryDlgItemText(hWndDlg, D_NEWX, sizeof(szText), (PSZ)szText);
              ScrX = atol(szText);

              WinQueryDlgItemText(hWndDlg, D_NEWY, sizeof(szText), (PSZ)szText);
              ScrY = atol(szText);

              WinDismissDlg(hWndDlg, TRUE);
              break;

         case DID_CANCEL:

              WinDismissDlg(hWndDlg, FALSE);
              break;

              }

         break;

    default:

         return(WinDefDlgProc(hWndDlg, message, mp1, mp2));
         }

 return (MRESULT)FALSE;
}


// --< GoToPtDlgProc >--------------------------------------------------

MRESULT EXPENTRY GoToPtDlgProc(HWND hWndDlg, ULONG message, MPARAM mp1, MPARAM mp2)
{

       // NTD - finish
   CHAR     szText[10];
   HPOINTER hSelPointer;

   switch(message) {

   case WM_INITDLG:

         CentreDlgBox(HWND_DESKTOP, hWndDlg);

         WinSendDlgItemMsg(hWndDlg, D_X, EM_SETTEXTLIMIT, MPFROMSHORT(4L),0L);
         sprintf(szText, "%d", sx);
         WinSetDlgItemText(hWndDlg, D_X, (PSZ)szText);

         WinSendDlgItemMsg(hWndDlg, D_Y, EM_SETTEXTLIMIT, MPFROMSHORT(4L),0L);
         sprintf(szText, "%d", sy);
         WinSetDlgItemText(hWndDlg, D_Y, (PSZ)szText);
         WinSendDlgItemMsg(hWndDlg, D_X, EM_SETSEL,
                                    MPFROM2SHORT((USHORT)0, (USHORT)4), 0L);
         break;

    case WM_HELP:

         if (!hWndHelp)
            SorryNoHelp(hAB);
         else
            WinSendMsg (hWndHelp, HM_DISPLAY_HELP,
                           MPFROMP((PVOID)ID_POINT),
                           MPFROMSHORT((USHORT)HM_RESOURCEID));
         break;

    case WM_COMMAND:

         switch( SHORT1FROMMP(mp1)) {

         case DID_OK:

              // Get the input
              WinQueryDlgItemText(hWndDlg, D_X, sizeof(szText), (PSZ)szText);
              sx = atol(szText);

              WinQueryDlgItemText(hWndDlg, D_Y, sizeof(szText), (PSZ)szText);
              sy = atol(szText);

              // Check for any value
              if ((sx + sy) > 0) {

                 // Capture the mouse for a point
                 WinSetWindowText(hWndFrame, (PSZ)"Pointer Position");
               //WinSetCapture(HWND_DESKTOP, hWndDlg);
                 hSelPointer = WinLoadPointer(HWND_DESKTOP,
                                              (HMODULE)NULL,
                                              IDP_SIZEWIN);
                 WinSetPointer(HWND_DESKTOP, hSelPointer);
                 // Move the pointer to the selected point
                 WinSetPointerPos(HWND_DESKTOP, sx, sy);
                 DosSleep(100);
                 }

              WinDismissDlg(hWndDlg, TRUE);
              break;

         case DID_CANCEL:

              WinDismissDlg(hWndDlg, FALSE);
              break;

              }

         break;

    default:

         return(WinDefDlgProc(hWndDlg, message, mp1, mp2));
         }

 return (MRESULT)FALSE;
}


// --< FrameDlgProc >---------------------------------------------------

MRESULT EXPENTRY FrameDlgProc(HWND hWndDlg, ULONG message, MPARAM mp1, MPARAM mp2)
{

       // NTD - finish
   CHAR     szText[10];

   switch(message) {

   case WM_INITDLG:

        CentreDlgBox(HWND_DESKTOP, hWndDlg);
        // Set the text limit
        WinSendDlgItemMsg(hWndDlg, D_X, EM_SETTEXTLIMIT, MPFROMSHORT(4L),0L);
        WinSendDlgItemMsg(hWndDlg, D_Y, EM_SETTEXTLIMIT, MPFROMSHORT(4L),0L);
        WinSendDlgItemMsg(hWndDlg, D_CX,EM_SETTEXTLIMIT, MPFROMSHORT(4L),0L);
        WinSendDlgItemMsg(hWndDlg, D_CY,EM_SETTEXTLIMIT, MPFROMSHORT(4L),0L);
        break;

   case WM_HELP:

        if (!hWndHelp)
           SorryNoHelp(hAB);
        else
           WinSendMsg (hWndHelp, HM_DISPLAY_HELP,
                          MPFROMP((PVOID)ID_FRAME),
                          MPFROMSHORT((USHORT)HM_RESOURCEID));
        break;

   case WM_COMMAND:

        switch( SHORT1FROMMP(mp1)) {

        case DID_OK:

             // Get the input
             WinQueryDlgItemText(hWndDlg, D_X, sizeof(szText), (PSZ)szText);
             x = atoi(szText);

             WinQueryDlgItemText(hWndDlg, D_Y, sizeof(szText), (PSZ)szText);
             y = atoi(szText);

             WinQueryDlgItemText(hWndDlg, D_CX, sizeof(szText), (PSZ)szText);
             cx = atoi(szText);

             WinQueryDlgItemText(hWndDlg, D_CY, sizeof(szText), (PSZ)szText);
             cy = atoi(szText);

             // Check for any value
             if ((cx + cy) > 0) {

                // cannot be window visible

                }

             WinDismissDlg(hWndDlg, TRUE);
             break;

        case DID_CANCEL:

             WinDismissDlg(hWndDlg, FALSE);
             break;

             }

        break;

   default:

        return(WinDefDlgProc(hWndDlg, message, mp1, mp2));
        }

 return (MRESULT)FALSE;
}


