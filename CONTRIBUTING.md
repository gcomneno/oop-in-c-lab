# Contributing

Italian version: [CONTRIBUTING.it.md](CONTRIBUTING.it.md)

Thank you for contributing to **OOP in C Lab**.

## Language policy

This repository is bilingual.

- English is the default language.
- Italian is maintained as a parallel translation.
- Source code, identifiers, public APIs, commit messages, issue titles, and pull request titles should be written in English.
- User-facing educational documentation should be available in both English and Italian.

## File naming

Use these conventions for translated documents:

| English | Italian |
|---|---|
| `README.md` | `README.it.md` |
| `docs/topic.md` | `docs/topic.it.md` |
| `CONTRIBUTING.md` | `CONTRIBUTING.it.md` |

The English document keeps the canonical filename. The Italian translation adds `.it` before `.md`.

## Translation links

Each translated document pair should contain a visible reciprocal link near the beginning:

- the English document links to the Italian version;
- the Italian document links to the English version.

## Pull request requirements

When a change affects educational documentation:

1. update the English document;
2. update the corresponding Italian document in the same pull request;
3. keep headings, examples, commands, tables, and technical meaning aligned;
4. verify that reciprocal language links still work.

A documentation change is incomplete when only one language is updated, unless the pull request explicitly explains why no translation is required.

## Technical terminology

Prefer established English technical terms when translating them would reduce precision. Italian prose may explain those terms, but code identifiers, commands, filenames, function names, and type names must remain unchanged.

## Validation checklist

Before opening a pull request, verify that:

- English remains the default entry point;
- every new educational Markdown document has its Italian counterpart;
- filenames follow the `.it.md` convention;
- reciprocal links are present and correct;
- code examples are equivalent in both languages;
- no translated document has silently fallen behind its counterpart.
