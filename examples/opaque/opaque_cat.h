#ifndef OPAQUE_CAT_H
#define OPAQUE_CAT_H

#include "opaque_animal.h"

/*
 * PUBLIC Cat API.
 *
 * Same opaque-handle pattern as Dog:
 * clients see Cat*, not struct Cat { ... }.
 */

typedef struct Cat Cat;

Cat *cat_create(const char *name, int lives);
Animal *cat_as_animal(Cat *cat);

#endif
