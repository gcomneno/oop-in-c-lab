#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "animal.h"
#include "cat.h"
#include "dog.h"

static int failures = 0;

static const char *yes_or_no(bool value)
{
    return value ? "yes" : "no";
}

static void expect(bool condition, const char *description)
{
    if (condition) {
        return;
    }

    fprintf(stderr, "FAILED: %s\n", description);
    failures++;
}

int main(void)
{
    Dog dog;
    Cat cat;

    dog_init(&dog, "Fido", "Labrador");
    cat_init(&cat, "Micia", 9);

    Animal *dog_as_animal = &dog.base;
    Animal *cat_as_animal = &cat.base;

    printf("Runtime type queries:\n");
    printf("  dog is Dog? %s\n", yes_or_no(animal_is_dog(dog_as_animal)));
    printf("  dog is Cat? %s\n", yes_or_no(animal_is_cat(dog_as_animal)));
    printf("  cat is Cat? %s\n", yes_or_no(animal_is_cat(cat_as_animal)));
    printf("  cat is Dog? %s\n", yes_or_no(animal_is_dog(cat_as_animal)));

    expect(animal_get_type(dog_as_animal) == ANIMAL_TYPE_DOG,
           "Dog must expose ANIMAL_TYPE_DOG");
    expect(animal_get_type(cat_as_animal) == ANIMAL_TYPE_CAT,
           "Cat must expose ANIMAL_TYPE_CAT");
    expect(animal_is_dog(dog_as_animal),
           "Dog query must accept Dog");
    expect(!animal_is_cat(dog_as_animal),
           "Cat query must reject Dog");
    expect(animal_is_cat(cat_as_animal),
           "Cat query must accept Cat");
    expect(!animal_is_dog(cat_as_animal),
           "Dog query must reject Cat");

    printf("\nSuccessful checked downcasts:\n");

    Dog *checked_dog = dog_from_animal(dog_as_animal);
    Cat *checked_cat = cat_from_animal(cat_as_animal);

    expect(checked_dog == &dog,
           "Dog checked downcast must recover the original Dog");
    expect(checked_cat == &cat,
           "Cat checked downcast must recover the original Cat");

    if (checked_dog != NULL) {
        printf("  Dog accepted: %s, breed %s\n",
               checked_dog->base.name,
               checked_dog->breed);
    }

    if (checked_cat != NULL) {
        printf("  Cat accepted: %s, lives %d\n",
               checked_cat->base.name,
               checked_cat->lives);
    }

    printf("\nRejected checked downcasts:\n");

    Dog *cat_as_dog = dog_from_animal(cat_as_animal);
    Cat *dog_as_cat = cat_from_animal(dog_as_animal);

    printf("  Cat as Dog: %s\n",
           cat_as_dog == NULL ? "rejected safely" : "wrongly accepted");
    printf("  Dog as Cat: %s\n",
           dog_as_cat == NULL ? "rejected safely" : "wrongly accepted");

    expect(cat_as_dog == NULL,
           "Cat-to-Dog checked downcast must return NULL");
    expect(dog_as_cat == NULL,
           "Dog-to-Cat checked downcast must return NULL");

    printf("\nConst checked downcasts:\n");

    const Animal *const_dog_as_animal = dog_as_animal;
    const Animal *const_cat_as_animal = cat_as_animal;

    const Dog *const_dog =
        dog_from_animal_const(const_dog_as_animal);
    const Cat *const_cat =
        cat_from_animal_const(const_cat_as_animal);

    printf("  const Animal* to const Dog*: %s\n",
           const_dog != NULL ? "accepted safely" : "rejected");
    printf("  const Animal* to const Cat*: %s\n",
           const_cat != NULL ? "accepted safely" : "rejected");

    expect(const_dog == &dog,
           "Const Dog downcast must recover the original Dog");
    expect(const_cat == &cat,
           "Const Cat downcast must recover the original Cat");
    expect(dog_from_animal_const(const_cat_as_animal) == NULL,
           "Const Cat-to-Dog downcast must return NULL");
    expect(cat_from_animal_const(const_dog_as_animal) == NULL,
           "Const Dog-to-Cat downcast must return NULL");

    printf("\nNULL safety:\n");
    printf("  NULL runtime type: %s\n",
           animal_get_type(NULL) == ANIMAL_TYPE_UNKNOWN
               ? "ANIMAL_TYPE_UNKNOWN"
               : "unexpected");
    printf("  NULL as Dog: %s\n",
           dog_from_animal(NULL) == NULL ? "rejected safely" : "wrongly accepted");
    printf("  NULL as Cat: %s\n",
           cat_from_animal(NULL) == NULL ? "rejected safely" : "wrongly accepted");

    expect(animal_get_type(NULL) == ANIMAL_TYPE_UNKNOWN,
           "NULL must expose ANIMAL_TYPE_UNKNOWN");
    expect(!animal_is_dog(NULL),
           "NULL must not be identified as Dog");
    expect(!animal_is_cat(NULL),
           "NULL must not be identified as Cat");
    expect(dog_from_animal(NULL) == NULL,
           "NULL-to-Dog downcast must return NULL");
    expect(cat_from_animal(NULL) == NULL,
           "NULL-to-Cat downcast must return NULL");

    printf("\nCore lesson:\n");
    printf("  A raw cast would compile, but it would not validate the runtime type.\n");

    if (failures != 0) {
        fprintf(stderr,
                "\nChecked-downcast validation failed: %d expectation(s).\n",
                failures);
        return EXIT_FAILURE;
    }

    printf("\nValidation:\n");
    printf("  All checked-downcast expectations passed.\n");

    return EXIT_SUCCESS;
}
