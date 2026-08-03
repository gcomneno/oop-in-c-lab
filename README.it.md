# OOP in C Lab

Nome in codice: **Canis Polymorphicus Manualis** 🐕⚙️

English version: [README.md](README.md)

Mini laboratorio didattico per capire come simulare concetti object-oriented in C puro.

Obiettivo: vedere con mano cosa c'è sotto concetti come:

- classi;
- oggetti;
- ereditarietà;
- metodi virtuali;
- vtable;
- polimorfismo;
- `self` / `this`.

Il progetto usa un esempio volutamente semplice:

```text
Animal
├── Dog
└── Cat
```

## File principali

```text
.
├── animal.h   # classe base / interfaccia comune
├── animal.c   # dispatch virtuale pubblico
├── dog.h      # tipo concreto Dog
├── dog.c      # implementazione Dog + dog_vtable
├── cat.h      # tipo concreto Cat
├── cat.c      # implementazione Cat + cat_vtable
├── main.c     # uso polimorfico tramite Animal*
├── docs/      # lezioni autonome
├── examples/  # esperimenti separati
└── Makefile   # build minimale
```

## Concetto 1: struct come oggetto

In C una `struct` è un contenitore di dati.

Non ha:

- metodi veri;
- costruttori;
- distruttori;
- ereditarietà;
- `private`;
- `public`;
- `protected`.

Però possiamo costruire manualmente alcuni meccanismi OOP.

Nel nostro caso `Animal` contiene:

```c
struct Animal {
    const char *name;
    const AnimalVTable *vtable;
};
```

`name` è lo stato comune.

`vtable` è il puntatore alla tabella dei metodi virtuali.

## Concetto 2: ereditarietà simulata

`Dog` e `Cat` contengono `Animal base` come primo campo:

```c
typedef struct {
    Animal base;
    const char *breed;
} Dog;
```

```c
typedef struct {
    Animal base;
    int lives;
} Cat;
```

Questo simula l'idea:

```text
Dog is an Animal
Cat is an Animal
```

## Concetto 3: il barbatrucco del primo campo

Regola fondamentale:

```text
L'indirizzo di una struct coincide con l'indirizzo del suo primo membro.
```

Quindi, se `Animal base` è il primo campo di `Dog`:

```c
&dog == &dog.base
```

Questo permette di trattare un `Dog` come `Animal`:

```c
Animal *animal = &dog.base;
```

E, con cautela, tornare indietro:

```c
Dog *dog = (Dog *)animal;
```

Questo ritorno è sicuro solo se quell'`Animal *` punta davvero alla base di un `Dog`.

Se era un `Cat`, il C non ti salva: comportamento indefinito, botola aperta.

## Demo del layout in memoria

La demo stampa gli indirizzi di ogni oggetto concreto e del suo campo `base` incorporato:

```text
Memory layout demo:
  dog object address      = 0x...
  dog.base address        = 0x...
  same address?           = yes

  cat object address      = 0x...
  cat.base address        = 0x...
  same address?           = yes
```

Gli indirizzi precisi cambiano a ogni esecuzione.

La cosa importante è che l'indirizzo dell'oggetto coincida con l'indirizzo del suo primo campo.

Questo è il motivo pratico per cui nel lab possiamo passare `&dog.base` o `&cat.base` come `Animal *`.

## Esperimento in stile container_of

La demo principale usa il barbatrucco del primo campo: `Animal base` è il primo membro di `Dog` e `Cat`, quindi l'indirizzo dell'oggetto concreto e l'indirizzo del campo `base` coincidono.

L'esperimento separato su `container_of` mostra un caso più avanzato:

```c
typedef struct {
    const char *label;
    Animal base;
    int marker;
} WeirdDog;
```

Qui `Animal base` non è il primo campo.

Quindi:

```text
&weird_dog != &weird_dog.base
```

Per recuperare un `WeirdDog *` esterno partendo da un `Animal *` interno, la demo usa `offsetof` e una piccola macro in stile `container_of`:

```c
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
```

L'idea è:

1. `offsetof(WeirdDog, base)` ci dice quanti byte separano l'inizio di `WeirdDog` dal suo membro `base`.
2. l'aritmetica su puntatori `char *` permette di muoversi byte per byte.
3. sottraendo quell'offset dal puntatore al membro, otteniamo l'indirizzo della struct esterna.

È una tecnica potente di C a basso livello, tipica del codice di sistema e dello stile del kernel Linux.

Va usata con cautela. È valida solo quando il puntatore punta davvero a quel preciso membro dentro un oggetto del tipo richiesto.

Esegui l'esperimento separato con:

```bash
make run-container-of
```

## Esperimento opaque pointer

La demo principale lascia pubblici i layout delle struct apposta. Puoi vedere ogni campo, ogni vtable e ogni trucco di upcast.

L'esperimento opaque pointer mostra lo stile opposto: un'API C professionale che nasconde i dettagli di implementazione dietro handle opachi.

Inizia dalla lezione:

- Italiano: [docs/opaque-pointers-lesson.it.md](docs/opaque-pointers-lesson.it.md)
- Inglese: [docs/opaque-pointers-lesson.md](docs/opaque-pointers-lesson.md)

Poi leggi i sorgenti in:

```text
examples/opaque/
```

Esegui l'esperimento separato con:

```bash
make run-opaque
```

Confronto rapido:

| Argomento | Demo principale | Demo opaque |
|-----------|-----------------|-------------|
| Layout struct | visibile in `.h` | nascosto in `.c` |
| Allocazione | stack | heap |
| Accesso ai campi | diretto | solo via API |
| Upcast | `&dog.base` | `dog_as_animal(dog)` |
| Cleanup | automatico | `animal_destroy(...)` |

Il meccanismo OOP resta lo stesso. Cambia il confine tra API pubblica e implementazione privata.

## Concetto 4: upcast e downcast

Upcast:

```text
Dog* -> Animal*
```

Vuol dire guardare un oggetto concreto tramite il suo tipo base.

Esempio:

```c
Animal *animal = &dog.base;
```

Downcast:

```text
Animal* -> Dog*
```

Vuol dire tornare dal tipo base al tipo concreto.

Esempio:

```c
Dog *self = (Dog *)animal;
```

Nel nostro lab il downcast avviene dentro `dog_speak` e `dog_describe`.

## Concetto 5: metodi virtuali

In un linguaggio OOP scriveremmo qualcosa tipo:

```cpp
animal->speak();
```

In C non esiste questa sintassi.

Noi usiamo una funzione pubblica:

```c
void animal_speak(Animal *self)
{
    self->vtable->speak(self);
}
```

Questa funzione:

1. prende l'oggetto;
2. legge la sua vtable;
3. trova il metodo concreto;
4. chiama quel metodo passando `self`.

## Concetto 6: vtable

Una vtable è una tabella di puntatori a funzione.

Nel nostro caso:

```c
typedef struct {
    void (*speak)(Animal *self);
    void (*describe)(Animal *self);
} AnimalVTable;
```

`Dog` ha la sua tabella:

```c
static const AnimalVTable dog_vtable = {
    .speak = dog_speak,
    .describe = dog_describe,
};
```

`Cat` ha la sua tabella:

```c
static const AnimalVTable cat_vtable = {
    .speak = cat_speak,
    .describe = cat_describe,
};
```

Tutti i `Dog` condividono la stessa `dog_vtable`.

Tutti i `Cat` condividono la stessa `cat_vtable`.

## Concetto 7: self esplicito

In C++ / C# / Java il riferimento all'oggetto corrente viene passato in modo implicito.

In C dobbiamo passarlo noi.

Per questo le funzioni virtuali hanno questa forma:

```c
void dog_speak(Animal *animal)
{
    Dog *self = (Dog *)animal;

    printf("%s the %s says: Woof!\n", self->base.name, self->breed);
}
```

`self` è il nostro `this` manuale.

## Esecuzione

Compila ed esegui:

```bash
make run
```

Pulisci il binario:

```bash
make clean
```

## Output atteso

Gli indirizzi cambiano a ogni esecuzione, ma la cosa importante è che:

- i due `Dog` abbiano la stessa vtable;
- i due `Cat` abbiano la stessa vtable;
- Dog e Cat abbiano vtable diverse.

Esempio:

```text
Dog vtables:
  dog1.base.vtable = 0x...
  dog2.base.vtable = 0x...

Cat vtables:
  cat1.base.vtable = 0x...
  cat2.base.vtable = 0x...

Polymorphic calls:
Fido is a dog of breed Labrador.
Fido the Labrador says: Woof!
Micia is a cat with 9 lives left.
Micia has 9 lives and says: Meow!
Rex is a dog of breed German Shepherd.
Rex the German Shepherd says: Woof!
Nebbia is a cat with 7 lives left.
Nebbia has 7 lives and says: Meow!
```

## Idea centrale

Il C non ha OOP nativo, ma permette di costruire manualmente molti suoi meccanismi.

Il prezzo è che il compilatore non ti protegge molto.

La regola d'oro è:

```text
Il C ti lascia fare.
Poi ti presenta il conto.
```
