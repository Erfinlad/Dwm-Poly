/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
/* appearance */
static unsigned int borderpx = 2; /* border pixel of windows */
static unsigned int gappx = 0;    /* gap size */
static unsigned int snap = 32;    /* snap pixel */

static int showbar = 1; /* 0 means no bar */
static int topbar = 1;  /* 0 means bottom bar */

static const int usealtbar = 1;             /* 1 means use non-dwm status bar */
static const char *altbarclass = "Polybar"; /* Alternate bar class name */
static const char *alttrayname = "tray";    /* Polybar tray instance name */
static const char *altbarcmd = "$HOME/.config/polybar/polylaunch.sh"; /* Alternate bar launch command */

static char font[] = "monospace:size 10";
static char dmenufont[] = "monospace:size 10";
static const char *fonts[] = {font};

static char normalbgcolor[] = "#222222";
static char normalbordercolor[] = "#444444";
static char normalfgcolor[] = "#bbbbbb";
static char selfgcolor[] = "#eeeeee";
static char selbordercolor[] = "#005577";
static char selbgcolor[] = "#005577";

static char *colors[][3] = {
    /*               fg         bg         border   */
    [SchemeNorm] = {normalfgcolor, normalbgcolor, normalbordercolor},
    [SchemeSel] = {selfgcolor, selbgcolor, selbordercolor},
    [SchemeTagsNorm] = {normalfgcolor, normalbgcolor, normalbordercolor},
    [SchemeTagsSel] = {selfgcolor, normalbgcolor, selbordercolor},
    [SchemeInfoNorm] = {normalfgcolor, normalbgcolor, normalbordercolor},
    [SchemeInfoSel] = {selfgcolor, selbgcolor, selbordercolor},
    [SchemeStatus] = {selfgcolor, selbgcolor, selbgcolor},
};

static const char *barcolors[] = {
    "#000000", "#ff7a93", "#b9f27c", "#FF9E64", "#BD93F9", "#FF79C6",
    "#7da6ff", "#BFBFBF", "#4D4D4D", "#FF6E67", "#5AF78E", "#F4F99D",
    "#CAA9FA", "#FF92D0", "#9AEDFE", "#E6E6E6",
};

/* tagging */
static const char *tags[] = {"レ", "コ", "エ", "ル", "フ", "ィ", "ン", "ラ", "ッ", "ド"};

static const unsigned int ulinepad =
    0; /* horizontal padding between the underline and tag */
static const unsigned int ulinestroke =
    1; /* thickness / height of the underline */
static const unsigned int ulinevoffset =
    0; /* how far above the bottom of the bar the line should appear */
static const int ulineall =
    0; /* 1 to show underline on all tags, 0 for just the active ones */

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     isfloating   monitor */
    {"Gimp", NULL, NULL, 1 << 4, 1, 0, -1},
    {"Firefox", NULL, NULL, 1 << 3, 1, 0, -1},
    {"chromium", NULL, NULL, 0, 1, 0, -1},
    {"Neovim", NULL, NULL, 1 << 2, 1, 0, -1},
    {"thunar", NULL, NULL, 0, 1, 1, -1},
};

/* layout(s) */
static float mfact = 0.55;  /* factor of master area size [0.05..0.95] */
static int nmaster = 1;     /* number of clients in master area */
static int resizehints = 1; /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen =
    1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
    /* symbol     arrange function */
    {"一", tile}, /* first entry is default */
    {"二", NULL}, /* no layout function means floating behavior */
    {"三", monocle},
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                      \
  {MODKEY, KEY, view, {.ui = 1 << TAG}},                                       \
      {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},               \
      {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                        \
      {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},    \
      {MODKEY | ShiftMask, KEY, swaptags, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }

/* commands */
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {"dmenu_run",   "-m",  dmenumon,       "-fn",
                                 dmenufont,     "-nb", normalbgcolor,  "-nf",
                                 normalfgcolor, "-sb", selbordercolor, "-sf",
                                 selfgcolor,    NULL};
static const char *clipmenucmd[] = {"clipmenu",     "-fn", dmenufont,     "-nb",
                                    normalbgcolor,  "-nf", normalfgcolor, "-sb",
                                    selbordercolor, "-sf", selfgcolor,    NULL};
static const char *firefox[] = {"firefox", "--new-tab", NULL};
static const char *termcmd[] = {"alacritty", NULL};
static const char *sleepcmd[] = {"loginctl", "suspend", NULL};
static const char *audioup[] = {"amixer", "set", "Master", "2%+", NULL};
static const char *audiodown[] = {"amixer", "set", "Master", "2%-", NULL};
static const char *brightup[] = {"xbacklight", "-inc", "5", NULL};
static const char *brightdown[] = {"xbacklight", "-dec", "5", NULL};
static const char *stcmd[] = {"st", NULL};
static const char *thunarcmd[] = {"thunar", NULL};

ResourcePref resources[] = {
    {"font", STRING, &font},
    {"dmenufont", STRING, &dmenufont},
    {"background", STRING, &normalbgcolor},
    {"background", STRING, &normalbordercolor},
    {"foreground", STRING, &normalfgcolor},
    {"selbg", STRING, &selbgcolor},
    {"selbg", STRING, &selbordercolor},
    {"selfg", STRING, &selfgcolor},
    {"borderpx", INTEGER, &borderpx},
    {"snap", INTEGER, &snap},
    {"showbar", INTEGER, &showbar},
    {"topbar", INTEGER, &topbar},
    {"nmaster", INTEGER, &nmaster},
    {"resizehints", INTEGER, &resizehints},
    {"mfact", FLOAT, &mfact},
    {"gappx", INTEGER, &gappx},
};

static Key keys[] = {

    //-------------------- Keys --------------------
    // Spawn Apps
    {MODKEY, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_space, spawn, {.v = dmenucmd}},
    {MODKEY | ShiftMask, XK_c, spawn, {.v = clipmenucmd}},
    {MODKEY, XK_b, spawn, {.v = firefox}},
    {MODKEY | ShiftMask, XK_f, spawn, {.v = thunarcmd}},

    // Cycle through tags
    {MODKEY,  XK_Tab, shiftview, {.i = +1}},
    {MODKEY | ShiftMask,  XK_Tab, shiftview, {.i = -1}},

    // Move Windows
    {MODKEY, XK_c, rotatestack, {.i = +1}},
    {MODKEY, XK_t, rotatestack, {.i = -1}},
    
    // Swich Layouts
    {MODKEY | ControlMask,  XK_w, setlayout,  {.v = &layouts[0]}},
    {MODKEY | ControlMask,  XK_v, setlayout,  {.v = &layouts[1]}},
    {MODKEY | ControlMask,  XK_m, setlayout,  {.v = &layouts[2]}},

    // Switch Windows
    {MODKEY, XK_h, focusstack, {.i = +1}},
    {MODKEY, XK_n, focusstack, {.i = -1}},

    // Change Window Size
    {MODKEY | ControlMask, XK_h, setmfact, {.f = -0.01}},
    {MODKEY | ControlMask, XK_n, setmfact, {.f = +0.01}},

    // Change Window Orientation
    {MODKEY | ShiftMask, XK_h, incnmaster, {.i = +1}},
    {MODKEY | ShiftMask, XK_n, incnmaster, {.i = -1}},

    {MODKEY, XK_w, focusmon, {.i = -1}},
    {MODKEY, XK_v, focusmon, {.i = +1}},
    {MODKEY, XK_w, tagmon, {.i = -1}},
    {MODKEY, XK_v, tagmon, {.i = +1}},

    // Legacy keys
    {0, XF86XK_AudioRaiseVolume, spawn, {.v = audioup}},
    {0, XF86XK_AudioLowerVolume, spawn, {.v = audiodown}},
    {0, XF86XK_MonBrightnessUp, spawn, {.v = brightup}},
    {0, XF86XK_MonBrightnessDown, spawn, {.v = brightdown}},

    // Etc
    {MODKEY | ControlMask, XK_u, togglefloating, {0}},
    {MODKEY | ShiftMask, XK_b, togglebar, {0}},
    {MODKEY, XK_comma, killclient, {0}},
    {MODKEY, XK_semicolon, quit, {1}},
    {MODKEY, XK_p, quit, {0}},
    {MODKEY | ControlMask, XK_semicolon, spawn, {.v = sleepcmd}},

    // Workspaces
    TAGKEYS(XK_ampersand, 0) TAGKEYS(XK_bracketleft, 1) TAGKEYS(XK_braceleft, 2)
        TAGKEYS(XK_braceright, 3) TAGKEYS(XK_parenleft, 4) TAGKEYS(XK_equal, 5)
            TAGKEYS(XK_asterisk, 6) TAGKEYS(XK_parenright, 7)
                TAGKEYS(XK_plus, 8)

};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function argument */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},
};

static const char *ipcsockpath = "/tmp/dwm.sock";
static IPCCommand ipccommands[] = {
    IPCCOMMAND(view, 1, {ARG_TYPE_UINT}),
    IPCCOMMAND(toggleview, 1, {ARG_TYPE_UINT}),
    IPCCOMMAND(tag, 1, {ARG_TYPE_UINT}),
    IPCCOMMAND(toggletag, 1, {ARG_TYPE_UINT}),
    IPCCOMMAND(tagmon, 1, {ARG_TYPE_UINT}),
    IPCCOMMAND(focusmon, 1, {ARG_TYPE_SINT}),
    IPCCOMMAND(focusstack, 1, {ARG_TYPE_SINT}),
    IPCCOMMAND(incnmaster, 1, {ARG_TYPE_SINT}),
    IPCCOMMAND(killclient, 1, {ARG_TYPE_SINT}),
    IPCCOMMAND(togglefloating, 1, {ARG_TYPE_NONE}),
    IPCCOMMAND(setmfact, 1, {ARG_TYPE_FLOAT}),
    IPCCOMMAND(setlayoutsafe, 1, {ARG_TYPE_PTR}),
    IPCCOMMAND(quit, 1, {ARG_TYPE_NONE})};
