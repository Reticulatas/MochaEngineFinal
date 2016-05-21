#ifndef KEYBOARDBUTTONTYPES_H
#define KEYBOARDBUTTONTYPES_H

#ifdef FOREACH_BBUTTON
#undef FOREACH_BBUTTON
#endif

#define FOREACH_BBUTTON(BUTTON)\
        BUTTON(KBB_BACKSPACE)\
        BUTTON(KBB_TAB)\
        BUTTON(KBB_CLEAR)\
        BUTTON(KBB_RETURN)\
        BUTTON(KBB_SHIFT)\
        BUTTON(KBB_CONTROL)\
        BUTTON(KBB_ALT)\
        BUTTON(KBB_PAUSE)\
        BUTTON(KBB_CAPS)\
        BUTTON(KBB_ESCAPE)\
        BUTTON(KBB_SPACE)\
        BUTTON(KBB_PAGEUP)\
        BUTTON(KBB_PAGEDOWN)\
        BUTTON(KBB_END)\
        BUTTON(KBB_HOME)\
        BUTTON(KBB_LEFT)\
        BUTTON(KBB_UP)\
        BUTTON(KBB_RIGHT)\
        BUTTON(KBB_DOWN)\
        BUTTON(KBB_PRINTSCREEN)\
        BUTTON(KBB_INSERT)\
        BUTTON(KBB_DELETE)\
        BUTTON(KBB_0)\
        BUTTON(KBB_1)\
        BUTTON(KBB_2)\
        BUTTON(KBB_3)\
        BUTTON(KBB_4)\
        BUTTON(KBB_5)\
        BUTTON(KBB_6)\
        BUTTON(KBB_7)\
        BUTTON(KBB_8)\
        BUTTON(KBB_9)\
        BUTTON(KBB_A)\
        BUTTON(KBB_B)\
        BUTTON(KBB_C)\
        BUTTON(KBB_D)\
        BUTTON(KBB_E)\
        BUTTON(KBB_F)\
        BUTTON(KBB_G)\
        BUTTON(KBB_H)\
        BUTTON(KBB_I)\
        BUTTON(KBB_J)\
        BUTTON(KBB_K)\
        BUTTON(KBB_L)\
        BUTTON(KBB_M)\
        BUTTON(KBB_N)\
        BUTTON(KBB_O)\
        BUTTON(KBB_P)\
        BUTTON(KBB_Q)\
        BUTTON(KBB_R)\
        BUTTON(KBB_S)\
        BUTTON(KBB_T)\
        BUTTON(KBB_U)\
        BUTTON(KBB_V)\
        BUTTON(KBB_W)\
        BUTTON(KBB_X)\
        BUTTON(KBB_Y)\
        BUTTON(KBB_Z)\
        BUTTON(KBB_LWIN)\
        BUTTON(KBB_RWIN)\
        BUTTON(KBB_NUMPAD0)\
        BUTTON(KBB_NUMPAD1)\
        BUTTON(KBB_NUMPAD2)\
        BUTTON(KBB_NUMPAD3)\
        BUTTON(KBB_NUMPAD4)\
        BUTTON(KBB_NUMPAD5)\
        BUTTON(KBB_NUMPAD6)\
        BUTTON(KBB_NUMPAD7)\
        BUTTON(KBB_NUMPAD8)\
        BUTTON(KBB_NUMPAD9)\
        BUTTON(KBB_MULTIPLY)\
        BUTTON(KBB_ADD)\
        BUTTON(KBB_SEPARATOR)\
        BUTTON(KBB_SUBTRACT)\
        BUTTON(KBB_DECIMAL)\
        BUTTON(KBB_DIVIDE)\
        BUTTON(KBB_F1)\
        BUTTON(KBB_F2)\
        BUTTON(KBB_F3)\
        BUTTON(KBB_F4)\
        BUTTON(KBB_F5)\
        BUTTON(KBB_F6)\
        BUTTON(KBB_F7)\
        BUTTON(KBB_F8)\
        BUTTON(KBB_F9)\
        BUTTON(KBB_F10)\
        BUTTON(KBB_F11)\
        BUTTON(KBB_F12)\
        BUTTON(KBB_F13)\
        BUTTON(KBB_F14)\
        BUTTON(KBB_F15)\
        BUTTON(KBB_F16)\
        BUTTON(KBB_F17)\
        BUTTON(KBB_F18)\
        BUTTON(KBB_F19)\
        BUTTON(KBB_F20)\
        BUTTON(KBB_F21)\
        BUTTON(KBB_F22)\
        BUTTON(KBB_F23)\
        BUTTON(KBB_F24)\
        BUTTON(KBB_NUMLOCK)\
        BUTTON(KBB_SCROLL)\
        BUTTON(KBB_LSHIFT)\
        BUTTON(KBB_RSHIFT)\
        BUTTON(KBB_LCONTROL)\
        BUTTON(KBB_RCONTROL)\
        BUTTON(KBB_TOTAL)\

#define GENERATE_ENUM(ENUM) ENUM,

enum Keyboard_BButtons
{
    FOREACH_BBUTTON(GENERATE_ENUM)
};


#endif