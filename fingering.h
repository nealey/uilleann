#pragma once
#include "notes.h"

#define CCCC NOTE_CS5, NOTE_CS5, NOTE_CS5, NOTE_CS5
#define CCDD NOTE_CS5, NOTE_CS5, NOTE_D5, NOTE_D5
#define CDCD NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_D5
#define DDDD NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5
#define P 0x80

uint8_t uilleann_matrix[] = {
  // Open Back D
  NOTE_CS5, NOTE_CS5, NOTE_CS5, NOTE_D5,  // OOO OO..
  CCDD,  // OOO OX..
  CDCD,  // OOO XO..
  DDDD,  // OOO XX..
  CDCD,  // OOX OO..
  DDDD,  // OOX OX..
  CDCD,  // OOX XO..
  DDDD,  // OOX XX..
  CCDD,  // OXO OO..
  CCDD,  // OXO OX..
  DDDD,  // OXO XO..
  DDDD,  // OXO XX..
  DDDD,  // OXX OO..
  DDDD,  // OXX OX..
  DDDD,  // OXX XO..
  DDDD,  // OXX XX..
  CDCD,  // XOO OO..
  DDDD,  // XOO OX..
  CDCD,  // XOO XO..
  DDDD,  // XOO XX..
  CDCD,  // XOX OO..
  DDDD,  // XOX OX..
  CDCD,  // XOX XO..
  DDDD,  // XOX XX..
  DDDD,  // XXO OO..
  DDDD,  // XXO OX..
  DDDD,  // XXO XO..
  DDDD,  // XXO XX..
  DDDD,  // XXX OO..
  DDDD,  // XXX OX..
  NOTE_D5,  NOTE_D5,  NOTE_D5,  NOTE_D5|P,  // XXX XO..
  DDDD,  // XXX XX..

  // Closed Back D
  CCCC, // OOO OO...
  NOTE_CS5, NOTE_CS5, NOTE_CS5, NOTE_CS5|P, // OOO OX..
  CCCC, // OOO XO..
  CCCC, // OOO XX..
  CCCC, // OOX OO..
  NOTE_CS5, NOTE_CS5|P, NOTE_CS5, NOTE_CS5|P, // OOX OX..
  CCCC, // OOX XO..
  CCCC, // OOX XX..
  CCCC, // OXO OO..
  NOTE_CS5, NOTE_CS5|P, NOTE_CS5, NOTE_CS5|P, // OXO OX..
  CCCC, // OXO XO..
  CCCC, // OXO XX..
  NOTE_C5|P,  NOTE_C5|P,  NOTE_C5|P, NOTE_C5|P,   // OXX OO..
  NOTE_C5,    NOTE_C5,    NOTE_C5,   NOTE_C5,     // OXX OX..
  NOTE_C5,    NOTE_C5,    NOTE_C5,   NOTE_C5|P,   // OXX XO..
  NOTE_C5,    NOTE_C5,    NOTE_C5,   NOTE_CS5,    // OXX XX..
  NOTE_B4,    NOTE_B4,    NOTE_B4,   NOTE_B4,     // XOO OO..
  NOTE_B4|P,  NOTE_B4|P,  NOTE_B4,   NOTE_B4|P,   // XOO OX..
  NOTE_AS4,   NOTE_B4,    NOTE_AS4,  NOTE_B4,     // XOO XO..
  NOTE_B4,    NOTE_B4,    NOTE_B4,   NOTE_B4,     // XOO XX..
  NOTE_B4|P,  NOTE_B4|P,  NOTE_B4|P, NOTE_B4|P,   // XOX OO..
  NOTE_B4|P,  NOTE_B4|P,  NOTE_B4|P, NOTE_B4|P,   // XOX OX..
  NOTE_B4|P,  NOTE_B4|P,  NOTE_B4|P, NOTE_B4|P,   // XOX XO..
  NOTE_B4|P,  NOTE_B4|P,  NOTE_B4|P, NOTE_B4|P,   // XOX XX..
  NOTE_A4,    NOTE_A4,    NOTE_A4|P, NOTE_A4,     // XXO OO..
  NOTE_A4|P,  NOTE_A4|P,  NOTE_A4|P, NOTE_A4|P,   // XXO OX..
  NOTE_GS4,   NOTE_GS4|P, NOTE_A4,   NOTE_A4,     // XXO XO..
  NOTE_A4|P,  NOTE_A4|P,  NOTE_A4|P, NOTE_A4,     // XXO XX..
  NOTE_G4,    NOTE_G4,    NOTE_G4|P, NOTE_G4,     // XXX OO..
  NOTE_G4|P,  NOTE_G4|P,  NOTE_G4|P, NOTE_G4|P,   // XXX OX..
  NOTE_FS4,   NOTE_FS4,   NOTE_F4,   NOTE_FS4|P,  // XXX XO..
  NOTE_E4,    NOTE_E4|P,  NOTE_DS4,  NOTE_D4,     // XXX XX..
};
