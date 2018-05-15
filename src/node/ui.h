//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/ui.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 30-04-18

#ifndef UI_H
#define	UI_H

void ui_printHeader();
void ui_refreshHeader(systemState_t* state);
void ui_printBootMenu();
void ui_printMenu();
uint8_t ui_browse(systemState_t* state);

#endif	/* UI_H */

