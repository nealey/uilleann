#include "pipe.h"
#include "fingering.h"

// Kludge time: this is something I did just to make my breadboard look nicer.
#define KEY_OFFSET 2

#define CLOSEDVAL 0x30
#define OPENVAL 0x70
#define GLISSANDO_STEPS (OPENVAL - CLOSEDVAL)

Pipe::Pipe() {
}

bool Pipe::Init() {
    // Capacative touch sensor
    if (!capSensor.begin(0x5A)) {
        return false;
    }

    // Proximity sensor
    if (paj7620Init()) {
        return false;
    }

    // Bag button
    bagSensor.begin();
    // This library takes the entire program out if you poll it 5-40 times without anything connected
    bag_enabled = bagSensor.isConnected();

    return true;
}

void Pipe::Update() {
    uint8_t glissandoKeys = 0;

    // Read the bag state, if there's a bag.
    // if there isn't a bag, don't try, or this library will crash the program.
    if (bag_enabled) {
        bag = bagSensor.isPressed();
    } else {
        bag = false;
    }

    // 0x6c is actually 8 bytes, but all 8 are always the same...
    paj7620ReadReg(0x6c, 1, &kneeClosedness);

    keys = 0;
    glissandoKeys = 0;
    for (int i=0; i<8; i++) {
        uint16_t val = max(capSensor.filteredData(i+KEY_OFFSET), CLOSEDVAL);
        float openness = ((val - CLOSEDVAL) / float(GLISSANDO_STEPS));

        // keys = all keys which are at least touched
        // glissandoKeys = all keys which are fully closed
        // The glissando operation computes the difference.
        if (openness < 1.0) {
            glissandoOpenness = max(glissandoOpenness, openness);
            bitSet(keys, i);
        }
        if (openness == 0.0) {
            bitSet(glissandoKeys, i);
        } 
    }

    // Look up notes in the big table
    note = uilleann_matrix[keys];
    glissandoNote = uilleann_matrix[glissandoKeys];

    // Was the high bit set? That indicates "alternate fingering", which sounds different.
    altFingering = (note & 0x80);

    note &= 0x7f;
    glissandoNote &= 0x7f;

    // If the bag is squished, jump up an octave
    // But only if the left thumb is down!
    if (bag && (keys & bit(7))) {
        note += 12;
        glissandoNote += 12;
    }

    // All keys closed + knee = no sound
    silent = ((kneeClosedness > 240) && (keys == 0xff));
}
