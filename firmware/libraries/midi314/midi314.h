
#ifndef MIDI314_H_
#define MIDI314_H_

#include <MIDIUSB.h>

/* --------------------------------------------------------------------------- *
 * Common MIDI definitions
 * --------------------------------------------------------------------------- */

// MIDI control change events.
enum {
    MIDI_CC_CHANNEL_VOLUME         = 7,
    MIDI_CC_PAN                    = 10,
    MIDI_CC_REVERB                 = 91,
    MIDI_CC_OTHER_EFFECT           = 92,

    // Looper events (non-standard).
    MIDI_CC_CUSTOM_RECORD          = 20,
    MIDI_CC_CUSTOM_PLAY            = 21,
    MIDI_CC_CUSTOM_MUTE            = 22,
    MIDI_CC_CUSTOM_DELETE          = 23,
    MIDI_CC_CUSTOM_SOLO            = 24,
    MIDI_CC_CUSTOM_ALL             = 25,
    MIDI_CC_CUSTOM_SET_MIN_PITCH   = 26,
    MIDI_CC_CUSTOM_SET_MIN_PROGRAM = 27,
    MIDI_CC_CUSTOM_PERCUSSION      = 28,
};

// The MIDI channel of the instrument.
#define DEFAULT_MIDI_CHANNEL 0

// The MIDI channel of the percussion instruments (channel 10).
#define PERC_MIDI_CHANNEL 9

/* --------------------------------------------------------------------------- *
 * Event queue
 * --------------------------------------------------------------------------- */

enum {
    POT_EVENT,
    PRESS_EVENT,
    RELEASE_EVENT
};

#define KEY_EVENT_QUEUE_LENGTH 16

typedef struct {
    int row;
    int col;
    int kind;
} Event;

class Midi314 {
    // Indicates that MIDI events are pending.
    bool midiSent;

    Event eventQueue[KEY_EVENT_QUEUE_LENGTH];
    int eventWriteIndex;
    int eventReadIndex;
    int eventCount;

public:
    Midi314();
    void reset();

    void noteOn(byte channel, byte pitch, byte velocity) {
        midiEventPacket_t mep = {0x09, 0x90 | channel, pitch, velocity};
        MidiUSB.sendMIDI(mep);
        midiSent = true;
    }

    void noteOff(byte channel, byte pitch, byte velocity) {
        midiEventPacket_t mep = {0x08, 0x80 | channel, pitch, velocity};
        MidiUSB.sendMIDI(mep);
        midiSent = true;
    }

    void controlChange(byte channel, byte control, byte value) {
        midiEventPacket_t mep = {0x0B, 0xB0 | channel, control, value};
        MidiUSB.sendMIDI(mep);
        midiSent = true;
    }

    void programChange(byte channel, byte value) {
        midiEventPacket_t mep = {0x0C, 0xC0 | channel, value, value};
        MidiUSB.sendMIDI(mep);
        midiSent = true;
    }

    void pitchBend(byte channel, int bend) {
        midiEventPacket_t mep = {0x0E, 0xE0 | channel, bend & 0x7F, bend >> 7};
        MidiUSB.sendMIDI(mep);
        midiSent = true;
    }

    void flushMidi() {
        if (midiSent) {
            MidiUSB.flush();
            midiSent = false;
        }
    }

    bool hasEvent();
    void pushEvent(int k, int r, int c = 0);
    void pullEvent(Event *evt);
};

extern Midi314 midi314;

#endif
