# Contribuire

English version: [CONTRIBUTING.md](CONTRIBUTING.md)

Grazie per contribuire a **OOP in C Lab**.

## Politica linguistica

Questo repository è bilingue.

- L'inglese è la lingua predefinita.
- L'italiano viene mantenuto come traduzione parallela.
- Codice sorgente, identificatori, API pubbliche, messaggi di commit, titoli delle issue e titoli delle pull request devono essere scritti in inglese.
- La documentazione didattica rivolta agli utenti deve essere disponibile sia in inglese sia in italiano.

## Nomi dei file

Usare queste convenzioni per i documenti tradotti:

| Inglese | Italiano |
|---|---|
| `README.md` | `README.it.md` |
| `docs/topic.md` | `docs/topic.it.md` |
| `CONTRIBUTING.md` | `CONTRIBUTING.it.md` |

Il documento inglese mantiene il nome canonico. La traduzione italiana aggiunge `.it` prima di `.md`.

## Collegamenti tra traduzioni

Ogni coppia di documenti tradotti deve contenere, vicino all'inizio, un collegamento reciproco ben visibile:

- il documento inglese collega la versione italiana;
- il documento italiano collega la versione inglese.

## Requisiti delle pull request

Quando una modifica riguarda la documentazione didattica:

1. aggiornare il documento inglese;
2. aggiornare nella stessa pull request il documento italiano corrispondente;
3. mantenere allineati titoli, esempi, comandi, tabelle e significato tecnico;
4. verificare che i collegamenti reciproci tra le lingue funzionino ancora.

Una modifica documentale è incompleta quando aggiorna una sola lingua, salvo che la pull request spieghi esplicitamente perché non sia necessaria una traduzione.

## Terminologia tecnica

Preferire i termini tecnici inglesi consolidati quando la loro traduzione ridurrebbe la precisione. Il testo italiano può spiegarli, ma identificatori del codice, comandi, nomi dei file, nomi delle funzioni e nomi dei tipi devono restare invariati.

## Checklist di verifica

Prima di aprire una pull request, verificare che:

- l'inglese resti il punto di ingresso predefinito;
- ogni nuovo documento Markdown didattico abbia la propria controparte italiana;
- i nomi dei file seguano la convenzione `.it.md`;
- i collegamenti reciproci siano presenti e corretti;
- gli esempi di codice siano equivalenti nelle due lingue;
- nessun documento tradotto sia rimasto silenziosamente indietro rispetto alla propria controparte.
