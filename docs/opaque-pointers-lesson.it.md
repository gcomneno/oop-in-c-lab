# Lezione: opaque pointer in C

Versione inglese: [opaque-pointers-lesson.md](opaque-pointers-lesson.md)

Questa lezione appartiene al laboratorio **Canis Polymorphicus Manualis**.

Spiega il pattern degli opaque pointer implementato in:

```text
examples/opaque/
```

Esegui la demo collegata:

```bash
make run-opaque
```

Leggi i sorgenti in questo ordine:

1. `opaque_animal.h` — API pubblica, tipo incompleto
2. `opaque_animal_internal.h` — dettagli interni solo per i tipi concreti
3. `opaque_animal.c` — definizione reale della struct (nascosta)
4. `opaque_dog.h` / `opaque_dog.c` — Dog concreto dietro un handle
5. `opaque_cat.h` / `opaque_cat.c` — Cat concreto dietro un handle
6. `opaque_demo.c` — codice client che usa solo l'API pubblica

Confronta con la demo principale nella root del repository, che usa **struct aperte**.

---

## 1. Cosa sai già

La demo principale mostra OOP manuale in C con **struct aperte**:

```c
struct Animal {
    const char *name;
    const AnimalVTable *vtable;
};
```

Chi include `animal.h` può:

- vedere ogni campo;
- allocare `Dog dog1;` sullo stack;
- scrivere direttamente `dog1.breed`;
- fare upcast manualmente con `&dog1.base`.

Questa trasparenza è ottima per imparare. Vedi ogni ingranaggio.

Ma molte librerie C reali fanno l'opposto: nascondono completamente il layout della struct.

---

## 2. L'idea centrale

Un **opaque pointer** è un puntatore a un tipo la cui definizione è nascosta al client.

Header pubblico:

```c
typedef struct Animal Animal;
```

File di implementazione:

```c
struct Animal {
    const char *name;
    const AnimalVTable *vtable;
};
```

Il client sa che `Animal` esiste.

Il client **non** sa:

- quanti campi ha;
- in che ordine sono;
- quanto è grande;
- come arrivare ai dati interni.

Quindi il client può interagire solo tramite funzioni:

```c
Animal *animal = dog_as_animal(dog);
animal_speak(animal);
animal_destroy(animal);
```

Questa è la versione C di **information hiding** e **incapsulamento**.

---

## 3. Tipi incompleti in C

Questa riga:

```c
typedef struct Animal Animal;
```

crea un **tipo incompleto**.

Il compilatore conosce il nome `Animal`, ma non il suo layout.

| Operazione | Consentita? | Perché |
|------------|-------------|--------|
| `Animal *p;` | sì | la dimensione di un puntatore è sempre nota |
| `Animal a;` | no | per lo stack serve il layout completo |
| `p->name` | no | il client non conosce i campi |
| `sizeof(Animal)` | no | la dimensione è sconosciuta |
| `void animal_speak(Animal *self);` | sì | per una dichiarazione di funzione basta il puntatore |

Non è un limite casuale. È il meccanismo che **obbliga** l'uso dell'API.

---

## 4. I tre strati del pattern

### Strato A — API pubblica (`.h` visto dal client)

Esempio: `opaque_animal.h`

- solo forward declaration;
- prototipi di funzione;
- nessun corpo di struct;
- nessuna vtable esposta.

### Strato B — implementazione nascosta (`.c`)

Esempio: `opaque_animal.c`

- definizione completa di `struct Animal { ... }`;
- dispatch virtuale;
- logica di destroy.

### Strato C — header interno (opzionale, per i tipi concreti)

Esempio: `opaque_animal_internal.h`

- condiviso tra `opaque_dog.c` e `opaque_cat.c`;
- espone vtable e helper di init;
- **non deve** essere incluso dal codice client.

Molte librerie reali hanno esattamente questa divisione:

```text
header API pubblico
header interno di implementazione
file .c con le struct reali
```

---

## 5. Create e destroy

Versione con struct aperta:

```c
Dog dog1;
dog_init(&dog1, "Fido", "Labrador");
/* memoria liberata automaticamente a fine scope */
```

Versione opaque:

```c
Dog *dog = dog_create("Fido", "Labrador");
Animal *animal = dog_as_animal(dog);
animal_speak(animal);
animal_destroy(animal);
```

Conseguenze importanti:

1. gli oggetti vivono di solito nell'**heap** (`malloc`);
2. il client riceve un **handle** (`Dog *`);
3. il client deve chiamare **`animal_destroy`**;
4. dimenticare destroy = memory leak.

In C non esistono costruttori o distruttori veri.

`create` e `destroy` sono il sostituto manuale.

---

## 6. Il polimorfismo resta uguale

Questo punto si fraintende spesso.

Gli opaque pointer cambiano la **visibilità**, non il meccanismo OOP.

Dentro `opaque_dog.c`:

- `Dog` contiene ancora `Animal base` come primo campo;
- esiste ancora `dog_vtable`;
- `dog_speak()` riceve ancora `Animal *` e fa downcast a `Dog *`;
- `animal_speak()` fa ancora dispatch tramite vtable.

Quindi:

```text
stessa vtable
stesso dispatch virtuale
stesso first-field inheritance trick
confine diverso tra API pubblica e implementazione privata
```

---

## 7. Upcast tramite API esplicita

Demo principale:

```c
Animal *animal = &dog1.base;
```

Demo opaque:

```c
Animal *animal = dog_as_animal(dog);
```

Internamente fanno la stessa cosa.

La differenza è **chi** può conoscere `base`:

- struct aperta: tutti;
- struct opaque: solo il codice di implementazione.

Per questo esiste `dog_as_animal()`.

Documenta l'upcast e tiene la conoscenza del layout dentro la libreria.

---

## 8. Confronto affiancato

| Argomento | Demo principale (struct aperte) | Demo opaque |
|-----------|----------------------------------|-------------|
| Layout struct | visibile in `.h` | nascosto in `.c` |
| Allocazione tipica | stack | heap |
| Creazione oggetto | `dog_init(&dog, ...)` | `dog = dog_create(...)` |
| Cleanup | automatico a fine scope | `animal_destroy(...)` |
| Accesso ai campi | diretto (`dog.breed`) | vietato dal client |
| Upcast | `&dog.base` | `dog_as_animal(dog)` |
| Downcast | ovunque | solo dentro i `.c` |
| Cambiare layout interno | può rompere i client | di solito i client non se ne accorgono |
| Valore didattico | vedi tutti gli interni | vedi uno stile API professionale |

---

## 9. Vantaggi

### Incapsulamento

Il client non può scrivere:

```c
dog->breed = "Hacked";
animal->vtable = NULL;
```

Quelle righe non devono compilare.

### Stabilità dell'API

Puoi aggiungere campi a `struct Dog` nel `.c` senza obbligare ogni client a ricompilare contro un nuovo layout pubblico.

Conta molto per librerie condivise e API C longeve.

### Familiarità con il mondo reale

Molte API C professionali usano handle opachi:

- `FILE *`
- `SDL_Window *`
- `SSL *` in OpenSSL
- `sqlite3 *` in SQLite

Quando vedi:

```c
typedef struct Foo Foo;
```

dovresti riconoscere subito il pattern.

---

## 10. Limiti

Gli opaque pointer non sono gratis.

### Niente oggetti incompleti sullo stack

Non puoi scrivere:

```c
Dog dog1;
```

Devi usare:

```c
Dog *dog = dog_create(...);
```

### Ownership esplicita

Il client è responsabile di distruggere ciò che crea.

### Più boilerplate

Ogni tipo tende ad avere bisogno di:

- `create`
- eventuale `as_animal`
- `destroy` condiviso o specifico

### Debug meno diretto

Dal client non ispezioni più i campi con la stessa facilità, a meno di entrare nei file di implementazione.

### Contesti embedded

In alcuni sistemi embedded si preferiscono struct aperte su storage statico perché la memoria è completamente controllata.

Gli handle opachi restano utili quando incapsuli un driver o nascondi una struct dispositivo complessa.

---

## 11. Come è organizzato questo lab

Questo repository tiene separati gli esperimenti apposta:

```text
demo principale     -> OOP manuale con struct aperte
demo container_of   -> recupero del contenitore quando base non è primo
demo opaque         -> incapsulamento e handle in stile professionale
```

Rispondono a domande diverse:

| Esperimento | Domanda |
|-------------|---------|
| Demo principale | Come si costruisce OOP manualmente in C? |
| `container_of` | Come si recupera il contenitore da un puntatore interno? |
| Demo opaque | Come si nascondono i dettagli di implementazione agli utenti dell'API? |

Non unificarli troppo presto. Un concetto alla volta.

---

## 12. Checklist di studio

Dopo aver letto la lezione e i sorgenti, dovresti saper rispondere:

1. Perché `Animal a;` fallisce nel client ma `Animal *a;` funziona?
2. Dove è definita la vera `struct Animal`?
3. Chi può fare downcast da `Animal *` a `Dog *`?
4. Perché la demo opaque usa `malloc` e `free`?
5. Cosa resta uguale tra demo principale e demo opaque?
6. Cita una libreria C reale che usa handle opachi.
7. Perché `opaque_animal_internal.h` non fa parte dell'API pubblica?

Esercizio suggerito:

- prova ad aggiungere una riga vietata in `opaque_demo.c`, come `dog->breed`, e verifica che il compilatore la rifiuti;
- confronta con la demo principale, dove la stessa riga è legale.

---

## 13. Ordine di lettura consigliato nel repository

1. [README.it.md](../README.it.md) — demo con struct aperte
2. questa lezione
3. `examples/opaque/*.c` e `examples/opaque/*.h`
4. [README.it.md](../README.it.md) — sezione `container_of`
5. `examples/container_of_demo.c`

Questo percorso va dagli interni visibili alle API nascoste fino al recupero low-level dei puntatori.

---

## 14. Regola d'oro

La demo principale insegna:

```text
Ecco esattamente come funziona la macchina.
```

La demo opaque insegna:

```text
Ecco come nascondere la macchina dietro un handle sicuro.
```

Entrambe le competenze contano in systems programming.

C ti lascia libertà di scelta.

Poi manda la fattura se scegli male.
