#pragma once

#include <stdint.h>
#include <SparkFun_Qwiic_Button.h>
#include <Adafruit_MPR121.h>
#include <paj7620.h>

class Pipe {
    public:
        // kneeClosedness indicates how "closed" the knee sensor is. 0 = wide open.
        uint8_t kneeClosedness;

        // keys are which keys are being pressed.
        uint8_t keys;

        // note holds the note being played, according to the fingering chart.
        uint8_t note;

        // silent is true if all keys and the knee are closed.
        bool silent;

        // bag is true if the bag is being squished.
        bool bag;

        // altFingering is true if the "alternate fingering" is being played.
        // This should sound different than the standard fingering.
        bool altFingering;

        // glissandoNote is the note that would be played if partially open keys were fully open.
        uint8_t glissandoNote;

        // glissandoOpenness is how "open" the holes are in the direction of the glissandoNote.
        float glissandoOpenness;

        Pipe();

        // Init initializes everything.
        //
        // Returns true if it all worked. You can run it again if it didn't.
        bool Init();

        // Update reads sensors and updates pipe state.
        //
        // It should be run once per loop.
        void Update();

    private:
        Adafruit_MPR121 capSensor;
        QwiicButton bagSensor;
        bool bag_enabled;    
};
