#ifndef OPAQUE_ANIMAL_H
#define OPAQUE_ANIMAL_H

/*
 * PUBLIC API for the opaque-pointer version of Animal.
 *
 * Read the lesson first:
 *   docs/opaque-pointers-lesson.md
 *
 * KEY IDEA:
 * This header exposes only a forward declaration of Animal.
 * The client knows that "Animal exists", but not what is inside it.
 *
 * That makes Animal an "incomplete type" in client translation units.
 *
 * Allowed in client code:
 *   Animal *animal;
 *   animal_speak(animal);
 *
 * Forbidden in client code:
 *   Animal animal;          // size unknown
 *   animal->name;            // fields unknown
 *   sizeof(Animal);          // layout unknown
 */

typedef struct Animal Animal;

const char *animal_get_name(const Animal *self);
void animal_speak(Animal *self);
void animal_describe(Animal *self);

/*
 * Destroy an object through its Animal handle.
 *
 * This function frees the memory block that was allocated in dog_create()
 * or cat_create().
 *
 * OWNERSHIP RULE:
 * Every object created with *_create() must eventually be passed here.
 * Forgetting this call creates a memory leak.
 */
void animal_destroy(Animal *self);

#endif
