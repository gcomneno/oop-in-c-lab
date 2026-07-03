#include <stddef.h>
#include <stdio.h>

#include "../animal.h"

/*
 * Educational container_of-like macro.
 *
 * Given:
 *   - ptr:    pointer to a member inside a struct
 *   - type:   outer struct type
 *   - member: name of the member inside that outer struct
 *
 * It computes the address of the outer struct by subtracting the member offset.
 *
 * WARNING:
 * This is powerful low-level C. It is only valid when ptr really points to
 * that exact member inside an object of the requested type.
 */
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

typedef struct {
    const char *label;
    Animal base;
    int marker;
} WeirdDog;

int main(void)
{
    WeirdDog weird_dog = {
        .label = "WeirdDog wrapper",
        .base = {
            .name = "Offset Fido",
            .vtable = NULL,
        },
        .marker = 42,
    };

    Animal *animal = &weird_dog.base;
    WeirdDog *recovered = container_of(animal, WeirdDog, base);

    printf("container_of-style experiment:\n");
    printf("  weird_dog address       = %p\n", (void *)&weird_dog);
    printf("  weird_dog.base address  = %p\n", (void *)&weird_dog.base);
    printf("  same address?           = %s\n",
           (void *)&weird_dog == (void *)&weird_dog.base ? "yes" : "no");
    printf("\n");

    printf("  offsetof(WeirdDog, base) = %zu bytes\n",
           offsetof(WeirdDog, base));
    printf("  recovered address        = %p\n", (void *)recovered);
    printf("  recovered == weird_dog?  = %s\n",
           recovered == &weird_dog ? "yes" : "no");
    printf("\n");

    printf("Recovered data:\n");
    printf("  label  = %s\n", recovered->label);
    printf("  name   = %s\n", recovered->base.name);
    printf("  marker = %d\n", recovered->marker);

    return 0;
}
