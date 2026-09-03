#ifndef BATTLEFIELD_INTERNAL_H
#define BATTLEFIELD_INTERNAL_H

typedef struct Padding {

    unsigned int left;
    unsigned int right;
    unsigned int top;
    unsigned int bottom;
    unsigned int in_between_armies;

} Padding;

/*typedef struct Cell {

    unsigned int w, h;

} Cell;*/

/*typedef struct Dimension {

    unsigned int x, y;

} Dimension;*/

/*typedef struct Size {

    unsigned int w, h;

} Size;*/

/*typedef struct Grid {

    //Cell cell;
    
    Dimension dimension;
    //Size size;

} Grid;*/

typedef struct Battlefield {

    //Grid grid;
    Padding padding;

} Battlefield;

#endif