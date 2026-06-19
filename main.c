#include <stdio.h>

#include "animal.h"
#include "cat.h"
#include "dog.h"

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

    printf("Dog vtables:\n");
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
         * Il main chiama sempre le stesse due funzioni:
         *
         *   animal_describe(...)
         *   animal_speak(...)
         *
         * Ma a runtime finirà nelle implementazioni concrete:
         *
         *   dog_describe / dog_speak
         *   cat_describe / cat_speak
         */
        animal_describe(animals[i]);
        animal_speak(animals[i]);
    }

    return 0;
}
