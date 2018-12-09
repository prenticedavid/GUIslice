//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 01 (Arduino):
//   - Display a box
//   - No touchscreen required
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//

#include "GUIslice.h"
#include "GUIslice_drv.h"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX, E_ELEM_TXT_XY, E_ELEM_TXT_PT, E_ELEM_TXT_Z, E_ELEM_TXT_MSG};
enum {E_FONT_TXT};

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_FONT            1
#define MAX_ELEM_PG_MAIN    12
#define MAX_STR             16

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

// Define debug message function
static int16_t DebugOut(char ch) {
    Serial.write(ch);
    return 0;
}


void helper(int x, int y, char *prompt, int ID, char *reply, int siz)
{
    int w, h = 8;
    w = strlen(prompt) * 6;  //7x5 char uses 6x8 box
    gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect){x, y, w, h}, prompt, 0, E_FONT_TXT);
    gslc_ElemCreateTxt(&m_gui, ID, E_PG_MAIN, (gslc_tsRect){x + 72, y, siz * 6, h}, reply, siz, E_FONT_TXT);
}


void setup()
{
    gslc_tsElemRef*  pElemRef = NULL;

    // Initialize debug output
    Serial.begin(9600);
    gslc_InitDebug(&DebugOut);
    //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

    // Initialize
    if (!gslc_Init(&m_gui, &m_drv, m_asPage, MAX_PAGE, m_asFont, MAX_FONT)) return;
    if (!gslc_FontAdd(&m_gui, E_FONT_TXT, GSLC_FONTREF_PTR, NULL, 1)) return;    // m_asFont[0]

    //Test display and touch rotations
    //4 rotations work with MCUFRIEND.
    //4 rotations work with TFT_eSPI directions ok, bad values in Portrait.
    //only 2 Landscape work with XPT2046_Touch, bad directions in Portrait
    gslc_DrvRotate(&m_gui, 0);

    gslc_PageAdd(&m_gui, E_PG_MAIN, m_asPageElem, MAX_ELEM_PG_MAIN, m_asPageElemRef, MAX_ELEM_PG_MAIN);

    // Background flat color
    gslc_SetBkgndColor(&m_gui, GSLC_COL_GRAY_DK2);

    // Create page elements
    gslc_tsRect box = { 10, 50, m_gui.nDispW - 20, m_gui.nDispH - 100 };
    pElemRef = gslc_ElemCreateBox(&m_gui, E_ELEM_BOX, E_PG_MAIN, box);
    gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);

    static char mstr2[10];
    sprintf(mstr2, "%d x %d", m_gui.nDispW, m_gui.nDispH);
    helper(20, 60, "Screen:", E_ELEM_TXT_XY, mstr2, 10); 
    helper(20, 80, "Touch:", E_ELEM_TXT_PT, "bum", MAX_STR); 
    helper(20, 100, "Pressure:", E_ELEM_TXT_Z, "Z", 6); 
    helper(20, 120, "Draw horiz: SWAP_XY if vert.", E_ELEM_TXT_MSG, "", 0); 
    helper(20, 140, "            FLIP_d  if rvs.", E_ELEM_TXT_MSG, "", 0); 

    // Start up display on main page
    gslc_SetPageCur(&m_gui, E_PG_MAIN);
    gslc_Update(&m_gui);

    delay(2000);     //show the initialised elements

    bool isTouchController = false;
#if !defined(DRV_TOUCH_NONE)
    isTouchController = gslc_InitTouch(&m_gui, "");
#endif
    pElemRef = gslc_PageFindElemById(&m_gui, E_PG_MAIN, E_ELEM_TXT_PT);
    gslc_ElemSetTxtStr(&m_gui, pElemRef, isTouchController ? "Stylus" : "No Touch");
    gslc_Update(&m_gui);
    
    delay(2000);     //show the Touch status
    
    if (isTouchController != true) {
        while (1) yield();     //tread water
    }
}

void loop()
{
    char                acTxt[MAX_STR];
    gslc_tsElemRef*     pElemRef = NULL;
    int16_t x, y, v;
    uint16_t z;
    gslc_teInputRawEvent e;
    bool down = false;
#if !defined(DRV_TOUCH_NONE)
    down =  gslc_GetTouch(&m_gui, &x, &y, &z, &e, &v);
#endif
    if (down && z) {
        gslc_DrawSetPixel(&m_gui, x, y, GSLC_COL_RED);
        snprintf(acTxt, MAX_STR, "(%3d, %3d)", x, y);
        pElemRef = gslc_PageFindElemById(&m_gui, E_PG_MAIN, E_ELEM_TXT_PT);
        gslc_ElemSetTxtStr(&m_gui, pElemRef, acTxt);
        snprintf(acTxt, MAX_STR, "%4d", z);
        pElemRef = gslc_PageFindElemById(&m_gui, E_PG_MAIN, E_ELEM_TXT_Z);
        gslc_ElemSetTxtStr(&m_gui, pElemRef, acTxt);
        gslc_Update(&m_gui);
    }
}


