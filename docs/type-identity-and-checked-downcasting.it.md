# Lezione: identità di tipo e downcast controllato in C

Versione inglese: [type-identity-and-checked-downcasting.md](type-identity-and-checked-downcasting.md)

Questa lezione appartiene al laboratorio **Canis Polymorphicus Manualis**.

Spiega il meccanismo di identità dinamica e downcast controllato implementato in:

```text
animal.h
animal.c
dog.h
dog.c
cat.h
cat.c
examples/checked_downcast_demo.c
```

Esegui la demo collegata:

```bash
make run-checked-downcast
```

---

## 1. Il problema

Il tipo statico di un puntatore è quello conosciuto dal compilatore.

Per esempio:

```c
Animal *animal = &dog.base;
```

Il compilatore vede un `Animal *`.

L'oggetto reale in memoria resta però un `Dog`.

Questa identità concreta a runtime viene chiamata anche **tipo dinamico**.

Un downcast prova a recuperare il puntatore concreto:

```text
Animal * -> Dog *
```

Un cast C grezzo può esprimere la conversione:

```c
Dog *dog = (Dog *)animal;
```

Ma il cast non esegue alcuna verifica a runtime.

Cambia il modo in cui il compilatore interpreta l'indirizzo. Non dimostra che lì esista davvero un `Dog`.

---

## 2. Perché un cast errato è pericoloso

Supponiamo che `animal` punti in realtà alla parte base di un `Cat`:

```c
Animal *animal = &cat.base;
Dog *dog = (Dog *)animal;
```

Il compilatore accetta il codice.

Il puntatore continua a riferirsi all'oggetto `Cat`, ma il programma interpreta adesso quei byte usando il layout di `Dog`.

Leggere:

```c
dog->breed
```

significherebbe interpretare memoria estranea come puntatore a una stringa.

Il comportamento è indefinito.

C non conserva automaticamente abbastanza informazioni sul tipo dinamico per rifiutare l'errore.

---

## 3. Identità dinamica esplicita

Il laboratorio aggiunge un tag di tipo:

```c
typedef enum {
    ANIMAL_TYPE_UNKNOWN = 0,
    ANIMAL_TYPE_DOG,
    ANIMAL_TYPE_CAT,
} AnimalType;
```

Ogni `Animal` conserva la propria identità concreta:

```c
struct Animal {
    const char *name;
    const AnimalVTable *vtable;
    AnimalType type;
};
```

`dog_init()` assegna `ANIMAL_TYPE_DOG`.

`cat_init()` assegna `ANIMAL_TYPE_CAT`.

Il valore deve essere mantenuto manualmente perché il C non fornisce metadati di classe automatici.

---

## 4. Perché UNKNOWN vale zero

Il primo valore dell'enum è:

```c
ANIMAL_TYPE_UNKNOWN = 0
```

È una scelta intenzionale.

Un `Animal` azzerato non deve sembrare accidentalmente un `Dog` o un `Cat` valido.

L'esperimento separato `container_of` incorpora un `Animal` soltanto per mostrare il recupero del contenitore. Non possiede comportamento virtuale e non è un vero oggetto Dog/Cat, quindi il suo tag viene assegnato esplicitamente a `ANIMAL_TYPE_UNKNOWN`.

Consente inoltre ad `animal_get_type(NULL)` di restituire un risultato sicuro:

```c
AnimalType animal_get_type(const Animal *self)
{
    if (self == NULL) {
        return ANIMAL_TYPE_UNKNOWN;
    }

    return self->type;
}
```

---

## 5. Query pubbliche sul tipo

L'API base espone predicati semplici:

```c
bool animal_is_dog(const Animal *self);
bool animal_is_cat(const Animal *self);
```

La domanda sul tipo dinamico diventa esplicita:

```c
if (animal_is_dog(animal)) {
    /* il tipo dinamico è Dog */
}
```

Le funzioni restituiscono `false` anche quando ricevono `NULL`.

---

## 6. Downcast controllato

`Dog` espone:

```c
Dog *dog_from_animal(Animal *animal);
const Dog *dog_from_animal_const(const Animal *animal);
```

L'implementazione verifica il tag prima del cast:

```c
Dog *dog_from_animal(Animal *animal)
{
    if (!animal_is_dog(animal)) {
        return NULL;
    }

    return (Dog *)animal;
}
```

Il cast continua a esistere.

La differenza è che avviene soltanto dopo avere verificato l'identità dinamica.

Gli helper di `Cat` applicano la stessa regola.

---

## 7. Il fallimento è rappresentato da NULL

Un downcast rifiutato restituisce `NULL`:

```c
Dog *dog = dog_from_animal(cat_as_animal);

if (dog == NULL) {
    /* rifiutato in sicurezza */
}
```

Questo impedisce al chiamante di dereferenziare l'oggetto usando il tipo concreto sbagliato.

Il risultato deve essere controllato prima di accedere ai campi specifici.

---

## 8. Downcast const-correct

Il laboratorio fornisce una variante modificabile e una in sola lettura:

```c
Dog *dog_from_animal(Animal *animal);
const Dog *dog_from_animal_const(const Animal *animal);
```

L'helper const conserva la promessa originale:

```c
const Animal *animal = &dog.base;
const Dog *dog = dog_from_animal_const(animal);
```

Un `const Animal *` non deve trasformarsi silenziosamente in un `Dog *` modificabile.

---

## 9. Layout, identità e comportamento sono concetti diversi

Tre meccanismi collaborano.

| Meccanismo | Domanda a cui risponde |
|------------|------------------------|
| Layout col primo campo | Questo indirizzo può rappresentare fisicamente la parte base? |
| Tag di tipo | Quale tipo concreto è memorizzato qui? |
| Vtable | Quale implementazione deve eseguire una chiamata virtuale? |

Il barbatrucco del primo campo rende possibile questa relazione:

```text
&dog == &dog.base
```

Il tag verifica che l'oggetto sia davvero un `Dog`.

La vtable seleziona `dog_speak()` oppure `cat_speak()` durante il dispatch virtuale.

Nessuno dei tre meccanismi sostituisce gli altri.

---

## 10. Cast controllati nei metodi virtuali

Un metodo virtuale di Dog riceve `Animal *`, perché tutte le funzioni della vtable condividono la firma base.

L'implementazione esegue ora una conversione controllata:

```c
static void dog_speak(Animal *animal)
{
    Dog *self = dog_from_animal(animal);

    if (self == NULL) {
        return;
    }

    /* accesso concreto sicuro */
}
```

Con un oggetto inizializzato correttamente il controllo ha successo.

La verifica rende inoltre visibile la precondizione nascosta e protegge da oggetti assemblati manualmente in modo incoerente.

---

## 11. Demo collegata

La demo mostra:

1. query sul tipo dinamico;
2. downcast Dog e Cat riusciti;
3. conversioni Cat-verso-Dog e Dog-verso-Cat rifiutate;
4. downcast che preserva `const`;
5. differenza tra un cast C e una prova a runtime.

Eseguila con:

```bash
make run-checked-downcast
```

---

## 12. Confronto con dynamic_cast

I linguaggi dotati di runtime type information possono offrire una conversione controllata integrata.

Un esempio C++ potrebbe usare:

```cpp
Dog *dog = dynamic_cast<Dog *>(animal);
```

Il C puro non possiede un equivalente incorporato nel linguaggio.

Questo laboratorio ne costruisce manualmente una piccola parte tramite:

- enum usata come tag;
- funzioni di query;
- helper di cast concreti;
- `NULL` in caso di fallimento.

È runtime type information didattica, non un sistema completo di reflection.

---

## 13. Limiti

L'approccio basato su enum è volutamente piccolo ed esplicito.

Quando la gerarchia cresce:

- ogni tipo concreto richiede un valore enum unico;
- ogni inizializzatore deve assegnare il tag corretto;
- aumentano query e helper di cast;
- i plugin non possono aggiungere facilmente tipi senza cambiare l'enum comune;
- gerarchie più profonde richiedono un modello più espressivo;
- un tag corrotto o falsificato manualmente può comunque mentire.

Sistemi reali possono usare descrittori più ricchi, record di classe, indirizzi univoci, tabelle di interfacce o generazione di codice.

Il laboratorio evita questi meccanismi per lasciare visibile l'idea essenziale.

---

## 14. Lezione centrale

Un cast C risponde:

> Come deve interpretare il compilatore questo indirizzo?

Un downcast controllato chiede anche:

> L'oggetto a questo indirizzo possiede davvero il tipo dinamico richiesto?

Il cast cambia la vista.

Il tag fornisce la prova mancante.
