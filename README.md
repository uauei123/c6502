# c6502

**c6502** è un emulatore del microprocessore **6502** scritto interamente in C, eseguibile da terminale. Mostra in tempo reale la memoria, i registri, e lo stato del processore, offrendo un ambiente di debug interattivo per programmi 6502.

## Indice

1. [Requisiti](#requisiti)  
2. [Esecuzione](#esecuzione)  
3. [Licenza](#licenza)

## Requisiti

Per compilare ed eseguire **c6502**, sono necessari:  

- Un compilatore C standard (es. **GCC**).  
- Un ambiente terminale UNIX-like (Linux, macOS, WSL su Windows).

## Esecuzione

1. Compilare il progetto:  
    ```bash
    ./compile.sh
    ```

2. Eseguire l'emulatore con:  
    ```bash
    ./c6502 <file_binario> [indirizzo_iniziale] [pc_iniziale]
    ```
    - `<file_binario>`: Il file da caricare nella memoria.  
    - `[indirizzo_iniziale]`: (Opzionale) Indirizzo di memoria dove caricare il file (default: `0x0000`).  
    - `[pc_iniziale]`: (Opzionale) Valore iniziale del Program Counter (default: `0x0000`).

3. Una volta avviato, il terminale mostrerà:  
    - **Memoria**: La memoria del 6502 visualizzata a blocchi di 16 byte.  
    - **Registri**: Program Counter (`PC`), Stack Pointer (`SP`), Accumulatore (`A`), registri X e Y.  

## Licenza

Questo progetto è distribuito sotto licenza MIT. Consulta il file [LICENSE](LICENSE) per ulteriori dettagli.
