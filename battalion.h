#ifndef BATTALION_H
#define BATTALION_H

typedef struct Battalion {
    struct Entity *entities;
    unsigned int entities_count;
} Battalion;

#ifndef