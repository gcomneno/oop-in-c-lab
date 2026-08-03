#include <stdio.h>
#include <stdlib.h>

#include "opaque_animal.h"
#include "opaque_cat.h"
#include "opaque_dog.h"

/*
 * Opaque pointer demo — client code.
 *
 * This file plays the same role as main.c in the root demo, but it is
 * written as if it were an external user of the Animal API.
 *
 * STUDY GOAL:
 * Notice what this file is allowed to do, and especially what it is NOT
 * allowed to do.
 *
 * Lesson:
 *   docs/opaque-pointers-lesson.md
 */

static void print_allocation_notes(const Dog *dog, const Cat *cat, Animal *dog_animal,
                                 Animal *cat_animal)
{
    printf("Allocation model demo:\n");
    printf("  Dog handle address         = %p\n", (const void *)dog);
    printf("  Cat handle address         = %p\n", (const void *)cat);
    printf("  dog_as_animal(...) address = %p\n", (const void *)dog_animal);
    printf("  cat_as_animal(...) address = %p\n", (const void *)cat_animal);

    printf("\n");
    printf("  same address for dog handle and its Animal handle? = %s\n",
           (const void *)dog == (const void *)dog_animal ? "yes" : "no");
    printf("  same address for cat handle and its Animal handle? = %s\n",
           (const void *)cat == (const void *)cat_animal ? "yes" : "no");

    printf("\n");
    printf("  These objects live on the heap because the client cannot write:\n");
    printf("    Dog dog1;   // incomplete type: size unknown\n");
    printf("  Instead it must call dog_create(), which uses malloc() internally.\n");
}

static void print_information_hiding_notes(void)
{
    printf("\nInformation hiding demo:\n");
    printf("  From this client file, direct field access is forbidden.\n");
    printf("  Examples that must NOT compile here:\n");
    printf("    dog->breed\n");
    printf("    cat->lives\n");
    printf("    animal->vtable\n");
    printf("\n");
    printf("  The compiler enforces the boundary between public API and private layout.\n");
    printf("  Compare with the main demo, where dog1.breed is legal.\n");

    /*
     * Uncomment ONE of the lines below to verify the compiler error:
     *
     * dog->breed = "Hacked";
     * cat->lives = 999;
     * animal->name;
     *
     * This is the practical proof of encapsulation in C.
     */
}

int main(void)
{
    Dog *dog1 = dog_create("Fido", "Labrador");
    Dog *dog2 = dog_create("Rex", "German Shepherd");
    Cat *cat1 = cat_create("Micia", 9);
    Cat *cat2 = cat_create("Nebbia", 7);

    if (dog1 == NULL || dog2 == NULL || cat1 == NULL || cat2 == NULL) {
        fprintf(stderr, "allocation failed\n");
        return EXIT_FAILURE;
    }

    Animal *dog1_animal = dog_as_animal(dog1);
    Animal *dog2_animal = dog_as_animal(dog2);
    Animal *cat1_animal = cat_as_animal(cat1);
    Animal *cat2_animal = cat_as_animal(cat2);

    print_allocation_notes(dog1, cat1, dog1_animal, cat1_animal);
    print_information_hiding_notes();

    printf("\nDog vtables:\n");
    /*
     * Even vtable addresses are no longer visible from client code.
     * In the main demo we could write:
     *
     *   dog1.base.vtable
     *
     * Here that field is hidden. We only call public methods.
     */
    printf("  (hidden from client code — inspect opaque_dog.c to see dog_vtable)\n");
    printf("  dog1 and dog2 still share the same virtual behavior at runtime.\n");

    printf("\nCat vtables:\n");
    printf("  (hidden from client code — inspect opaque_cat.c to see cat_vtable)\n");
    printf("  cat1 and cat2 still share the same virtual behavior at runtime.\n");

    printf("\nPolymorphic calls:\n");

    Animal *animals[] = {
        dog1_animal,
        cat1_animal,
        dog2_animal,
        cat2_animal,
    };

    for (int i = 0; i < 4; i++) {
        /*
         * Same public interface as the main demo:
         *
         *   animal_describe(...)
         *   animal_speak(...)
         *
         * Runtime dispatch still reaches dog_* or cat_* through the vtable.
         * The difference is that this file cannot see the vtable directly.
         */
        animal_describe(animals[i]);
        animal_speak(animals[i]);
    }

    printf("\nCleanup through animal_destroy():\n");

    for (int i = 0; i < 4; i++) {
        /*
         * Ownership reminder:
         * create() allocated memory, so destroy() must free it.
         *
         * In the main demo, stack objects disappear automatically.
         * Here cleanup is explicit and part of the API contract.
         */
        animal_destroy(animals[i]);
        printf("  destroyed animals[%d]\n", i);
    }

    return EXIT_SUCCESS;
}
