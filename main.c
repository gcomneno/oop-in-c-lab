#include <stdio.h>

#include "animal.h"
#include "cat.h"
#include "dog.h"

/*
 * Memory layout demo.
 *
 * In this lab, Dog and Cat "inherit" from Animal by placing
 * Animal base as the first field of their structs.
 *
 * The important C rule is:
 *
 *   the address of a struct is the same as the address
 *   of its first member.
 *
 * Therefore:
 *
 *   &dog == &dog.base
 *   &cat == &cat.base
 *
 * This is what makes our simple upcast/downcast trick work.
 */
static void print_memory_layout(const Dog *dog, const Cat *cat)
{
    printf("Memory layout demo:\n");

    printf("  dog object address      = %p\n", (const void *)dog);
    printf("  dog.base address        = %p\n", (const void *)&dog->base);
    printf("  same address?           = %s\n",
           (const void *)dog == (const void *)&dog->base ? "yes" : "no");

    printf("\n");

    printf("  cat object address      = %p\n", (const void *)cat);
    printf("  cat.base address        = %p\n", (const void *)&cat->base);
    printf("  same address?           = %s\n",
           (const void *)cat == (const void *)&cat->base ? "yes" : "no");
}

int main(void)
{
    Dog dog1;
    Dog dog2;
    Cat cat1;
    Cat cat2;

    dog_init(&dog1, "Fido", "Labrador");
    dog_init(&dog2, "Rex", "German Shepherd");

    cat_init(&cat1, "Micia", 9);
    cat_init(&cat2, "Nebbia", 7);

    print_memory_layout(&dog1, &cat1);

    printf("\nDog vtables:\n");
    printf("  dog1.base.vtable = %p\n", (const void *)dog1.base.vtable);
    printf("  dog2.base.vtable = %p\n", (const void *)dog2.base.vtable);

    printf("\nCat vtables:\n");
    printf("  cat1.base.vtable = %p\n", (const void *)cat1.base.vtable);
    printf("  cat2.base.vtable = %p\n", (const void *)cat2.base.vtable);

    printf("\nPolymorphic calls:\n");

    Animal *animals[] = {
        &dog1.base,
        &cat1.base,
        &dog2.base,
        &cat2.base,
    };

    for (int i = 0; i < 4; i++) {
        /*
         * The main function always calls the same public interface:
         *
         *   animal_describe(...)
         *   animal_speak(...)
         *
         * At runtime, the vtable dispatch reaches the concrete functions:
         *
         *   dog_describe / dog_speak
         *   cat_describe / cat_speak
         */
        animal_describe(animals[i]);
        animal_speak(animals[i]);
    }

    return 0;
}
