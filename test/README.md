# test

Contiene una serie di programmi di test, sia in formato sorgente che binario, per verificare il corretto funzionamento dell'emulatore. I test sono progettati per controllare l'accuratezza delle operazioni del processore e confrontare l'output dell'emulatore con quello atteso.

## Contenuto

- **Sorgenti**: I file `.s` contengono i programmi di test scritti in assembly 6502.
- **Binari**: I file `.bin` sono i programmi compilati e pronti per essere caricati nell'emulatore.

## Utilizzo

1. Per eseguire un test, compila ed esegui l'emulatore con uno dei file binari inclusi:

   ```bash
   ./c6502 <file_binario>
   ```

2. Ispeziona i file sorgente per comprendere il comportamento atteso di ogni programma. Gli output previsti sono descritti nei commenti all'interno del codice stesso.

## Assembler

I programmi sono scritti utilizzando l'assembler 6502 di Masswerk, disponibile al seguente [link](https://www.masswerk.at/6502/assembler.html). Puoi utilizzarlo per compilare i sorgenti `.s` e generare i file binari se necessario.

## Verifica dell'output

Controlla l'output dell'emulatore per confrontarlo con i comportamenti descritti nei commenti dei file sorgente. Ogni programma di test è progettato per verificare una specifica funzionalità dell'emulatore, come la gestione dei registri, delle operazioni aritmetiche e logiche, e l'interazione con la memoria.
