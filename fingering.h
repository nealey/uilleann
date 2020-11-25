#pragma once
#include "tuning.h"

struct Fingering {
  Note note;
  bool alt;  // Alternate fingering: sounds more choked
};

#define n(note) \
  { note, false }
#define P(note) \
  { note, true }

#define CCCC n(NOTE_CS5), n(NOTE_CS5), n(NOTE_CS5), n(NOTE_CS5)
#define CCDD n(NOTE_CS5), n(NOTE_CS5), n(NOTE_D5), n(NOTE_D5)
#define CDCD n(NOTE_CS5), n(NOTE_D5), n(NOTE_CS5), n(NOTE_D5)
#define DDDD n(NOTE_D5), n(NOTE_D5), n(NOTE_D5), n(NOTE_D5)

struct Fingering uilleann_matrix[] = {
    // Open Back D
    n(NOTE_CS5), n(NOTE_CS5), n(NOTE_CS5), n(NOTE_D5),  // OOO OO..
    CCDD,                                               // OOO OX..
    CDCD,                                               // OOO XO..
    DDDD,                                               // OOO XX..
    CDCD,                                               // OOX OO..
    DDDD,                                               // OOX OX..
    CDCD,                                               // OOX XO..
    DDDD,                                               // OOX XX..
    CCDD,                                               // OXO OO..
    CCDD,                                               // OXO OX..
    DDDD,                                               // OXO XO..
    DDDD,                                               // OXO XX..
    DDDD,                                               // OXX OO..
    DDDD,                                               // OXX OX..
    DDDD,                                               // OXX XO..
    DDDD,                                               // OXX XX..
    CDCD,                                               // XOO OO..
    DDDD,                                               // XOO OX..
    CDCD,                                               // XOO XO..
    DDDD,                                               // XOO XX..
    CDCD,                                               // XOX OO..
    DDDD,                                               // XOX OX..
    CDCD,                                               // XOX XO..
    DDDD,                                               // XOX XX..
    DDDD,                                               // XXO OO..
    DDDD,                                               // XXO OX..
    DDDD,                                               // XXO XO..
    DDDD,                                               // XXO XX..
    DDDD,                                               // XXX OO..
    DDDD,                                               // XXX OX..
    n(NOTE_D5), n(NOTE_D5), n(NOTE_D5), P(NOTE_D5),     // XXX XO..
    DDDD,                                               // XXX XX..

    // Closed Back D
    CCCC,                                                // OOO OO...
    n(NOTE_CS5), n(NOTE_CS5), n(NOTE_CS5), P(NOTE_CS5),  // OOO OX..
    CCCC,                                                // OOO XO..
    CCCC,                                                // OOO XX..
    CCCC,                                                // OOX OO..
    n(NOTE_CS5), P(NOTE_CS5), n(NOTE_CS5), P(NOTE_CS5),  // OOX OX..
    CCCC,                                                // OOX XO..
    CCCC,                                                // OOX XX..
    CCCC,                                                // OXO OO..
    n(NOTE_CS5), P(NOTE_CS5), n(NOTE_CS5), P(NOTE_CS5),  // OXO OX..
    CCCC,                                                // OXO XO..
    CCCC,                                                // OXO XX..
    P(NOTE_C5), P(NOTE_C5), P(NOTE_C5), P(NOTE_C5),      // OXX OO..
    n(NOTE_C5), n(NOTE_C5), n(NOTE_C5), n(NOTE_C5),      // OXX OX..
    n(NOTE_C5), n(NOTE_C5), n(NOTE_C5), P(NOTE_C5),      // OXX XO..
    n(NOTE_C5), n(NOTE_C5), n(NOTE_C5), n(NOTE_CS5),     // OXX XX..
    n(NOTE_B4), n(NOTE_B4), n(NOTE_B4), n(NOTE_B4),      // XOO OO..
    P(NOTE_B4), P(NOTE_B4), n(NOTE_B4), P(NOTE_B4),      // XOO OX..
    n(NOTE_AS4), n(NOTE_B4), n(NOTE_AS4), n(NOTE_B4),    // XOO XO..
    n(NOTE_B4), n(NOTE_B4), n(NOTE_B4), n(NOTE_B4),      // XOO XX..
    P(NOTE_B4), P(NOTE_B4), P(NOTE_B4), P(NOTE_B4),      // XOX OO..
    P(NOTE_B4), P(NOTE_B4), P(NOTE_B4), P(NOTE_B4),      // XOX OX..
    P(NOTE_B4), P(NOTE_B4), P(NOTE_B4), P(NOTE_B4),      // XOX XO..
    P(NOTE_B4), P(NOTE_B4), P(NOTE_B4), P(NOTE_B4),      // XOX XX..
    n(NOTE_A4), n(NOTE_A4), P(NOTE_A4), n(NOTE_A4),      // XXO OO..
    P(NOTE_A4), P(NOTE_A4), P(NOTE_A4), P(NOTE_A4),      // XXO OX..
    n(NOTE_GS4), P(NOTE_GS4), n(NOTE_A4), n(NOTE_A4),    // XXO XO..
    P(NOTE_A4), P(NOTE_A4), P(NOTE_A4), n(NOTE_A4),      // XXO XX..
    n(NOTE_G4), n(NOTE_G4), P(NOTE_G4), n(NOTE_G4),      // XXX OO..
    P(NOTE_G4), P(NOTE_G4), P(NOTE_G4), P(NOTE_G4),      // XXX OX..
    n(NOTE_FS4), n(NOTE_FS4), n(NOTE_F4), P(NOTE_FS4),   // XXX XO..
    n(NOTE_E4), P(NOTE_E4), n(NOTE_DS4), n(NOTE_D4),     // XXX XX..
};
