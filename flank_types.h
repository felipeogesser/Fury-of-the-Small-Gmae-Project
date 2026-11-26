#ifndef FLANK_TYPES_H
#define FLANK_TYPES_H

// where an attack lands relative to the facing of the battalion it hits
enum FlankState {

    FLANK_UNKNOWN = -1,
    FLANK_NONE,     // front, or no deliberate attack
    FLANK_SIDE,
    FLANK_REAR

};

#endif
