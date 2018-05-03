// =----------------------------------------------------------=
// =                           AVISO:                         =
// =                   MODIFIQUE ESTE ARQUIVO!                =
// = Aqui, você deve implementar, obrigatoriamente, essas     =
// = duas funções:                                            =
// =     - `a_menor_que_b`: deve retornar quando uma string   =
// =        é menor que a outra (ambos com o mesmo tamanho);  =
// =     - `external_sort`: aqui, você deverá implementar o   =
// =        seu trabalho. Ordene os registros em `input_file` =
// =        e escreva-os em `output_file`, usando somente     =
// =        `memory` KB de memória.                           =
// =                                                          =
// = Instruções:                                              =
// =     - use a função `mathias_malloc` ao invés de          =
// =       `malloc`;                                          =
// =     - use a função `mathias_free` ao invés de `free`;    =
// =     - lembre-se que você tem somente `memory` KB de      =
// =       memória;                                           =
// =     - note que a sua implementação de `a_menor_que_b`    =
// =       afetará o resultado da verificação da resposta.    =
// =                                                          =
// = Dicas:                                                   =
// =     - você pode usar a sua função `a_menor_que_b` em     =
// =       sua implementação da função `external_sort`;       =
// =                                                          =
// =                      BOM TRABALHO!                       =
// =----------------------------------------------------------=

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"

// Parâmetros:
//     - a: uma cadeia de caracteres de tamanho `len`;
//     - b: uma cadeia de caracteres de tamanho `len`;
//     - len: o tamanho (quantidade de bytes) de `a` e `b`.
// Essa função deve retornar 1, se `a` é menor que `b` (`a` deve vir antes de `b` no
// arquivo), e 0 caso contrário.
// Ela também é usada para verificação da ordenação! Então, se você implementá-la
// erroneamente, a verificação também estará errada! 
int a_menor_que_b(char* a, char* b, int len) {
    if (strcmp(a, b) < 0) {
        return 1;
    }
    return 0;
}

// Parâmetros:
//     - input_file: o nome do arquivo com os registros de entrada;
//     - output_file: o nome do arquivo com os registros de entrada ordenados;
//     - memory: o limite de memória a ser usado.
// Essa função deverá ler todos registros do arquivo `input_file`, ordená-los e
// escrevê-los em `output_file`, usando memória menor ou igual a `memory`. Lembre-se
// de fechar todos os arquivos que você usar por aqui!!
void external_sort(const char* input_file, const char* output_file, unsigned int memory) {
    int i;

    FILE* ptr_input_file = fopen(input_file, "r");
    FILE* ptr_output_file = fopen(output_file, "w+");

    unsigned int line_size;
    fscanf(ptr_input_file, "%u%*c", &line_size);
    printf("line_size: %u\n", line_size);

    unsigned int line_size_bytes = sizeof(char) * (line_size + 1);

    // buffer and heap lines amount
    unsigned int m = (
        // Size in bytes
        (memory * 1000) /
        // Size of a line in bytes
        (
            (line_size_bytes) +
            sizeof(char *) +
            sizeof(int *)
        )
    ) - 1; // discounting the buffer aux at 0
    printf("m: %u\n", m);

    // -------- Breaking input file into blocks --------

    int *heap = (int *) mathias_malloc(sizeof(int *) * m);

    // 0 position of the buffer is used as aux
    char **buffer = (char **) mathias_malloc(sizeof(char *) * (m + 1));
    for (i = 0; i < m + 1; i++) {
        buffer[i] = (char *) mathias_malloc(line_size_bytes);
    }

    int curr_line = 0;

    // Filling heap with the first lines
    int a_index = 0, b_index = 0, temp = 0;
    while (
        curr_line < m &&
        // Read the current line on the input file and put it on the last
        // position of the heap
        fscanf(ptr_input_file, "%[^\n]%*c", buffer[0]) != EOF
    ) {
        strcpy(buffer[curr_line + 1], buffer[0]);
        heap[curr_line] = curr_line + 1;

        // Promote the current line to it's place on the heap
        a_index = curr_line; // "son"
        b_index = (a_index - 1) / 2; // "father"
        while (
            // "son" is not the heap root
            a_index > 0 &&
            // "son" is lower than "father"
            a_menor_que_b(
                buffer[heap[a_index]],
                buffer[heap[b_index]],
                line_size
            )
        ) {
            // Swap "son" and "father"
            temp = heap[a_index];
            heap[a_index] = heap[b_index];
            heap[b_index] = temp;

            // Jump to the previous heap level
            a_index = b_index; // "son" becomes "father"
            b_index = (b_index - 1) / 2; // "father" becomes it's "father"
        }

        curr_line++; // jump to the next line on the input file
    }

    int heap_size = curr_line;

    printf("heap: %d\n", heap_size);
    for(i = 0; i < heap_size; i++) {
        printf("%d - %s\n", heap[i], buffer[heap[i]]);
    }

    // Creating initial blocks
    int removed_index = 0;
    int curr_block_type = 1; // (1 | -1)
    while (heap_size > 0) {
        // Remove the root of the heap
        removed_index = heap[0];

        // If a element of another block is being removed, switch to this block
        if (
            (removed_index < 0 && curr_block_type > 0) ||
            (removed_index > 0 && curr_block_type < 0)
        ) {
            curr_block_type = curr_block_type * (-1);
            printf("\nChanging blocks! %d\n\n", curr_block_type);
            fprintf(ptr_output_file, "%s\n", "\n");
        }

        // Promote the right most leaf
        heap[0] = heap[heap_size - 1];
        printf("\nRemoving %d %s and promoting %d %s\n", removed_index, buffer[abs(removed_index)], heap[heap_size - 1], buffer[abs(heap[heap_size - 1])]);
        heap_size--;

        // Demote until it is on the right place
        a_index = 0; // "father"
        b_index = 1; // "son"
        while (
            // "father" is not a leaf
            b_index < heap_size
        ) {
            // The "father" has two "sons"
            if (b_index < heap_size - 1) {
                if (
                    // "son" and the other "son" aren't of the same block
                    (
                        (heap[b_index + 1] > 0 && heap[b_index] < 0) ||
                        (heap[b_index + 1] < 0 && heap[b_index] > 0)
                    ) ||
                    // The other "son" is lower than this one
                    a_menor_que_b(
                        buffer[abs(heap[b_index + 1])],
                        buffer[abs(heap[b_index])],
                        line_size
                    )
                ) {
                    printf("R is lower than L %d %d\n", heap[b_index + 1], heap[b_index]);
                    b_index++;
                }
            }

            if (
                // "father" and "son" are of the same block
                (
                    (heap[a_index] > 0 && heap[b_index] > 0) ||
                    (heap[a_index] < 0 && heap[b_index] < 0)
                ) &&
                // The "father" is lower than the "son"
                a_menor_que_b(
                    buffer[abs(heap[a_index])],
                    buffer[abs(heap[b_index])],
                    line_size
                )
            ) {
                printf("%s is on the right place! %d %d\n", buffer[abs(heap[a_index])], heap[a_index], heap[b_index]);
                break;
            }

            // Swap "father" and "son"
            temp = heap[a_index];
            heap[a_index] = heap[b_index];
            heap[b_index] = temp;

            // Jump to the next heap level
            a_index = b_index; // "father" becomes "son"
            b_index = 2 * a_index + 1; // "son" becomes it's "son"
        }

        // Put the removed line on a block
        fprintf(ptr_output_file, "%s\n", buffer[abs(removed_index)]);

        strcpy(buffer[0], buffer[abs(removed_index)]);

        if (fscanf(ptr_input_file, "%[^\n]%*c", buffer[abs(removed_index)]) == EOF) {
            continue;
        } else {
            printf("New line added and ");
        }

        // Put the new line on the heap
        heap[heap_size] = heap_size;

        if (a_menor_que_b(
            buffer[abs(removed_index)],
            buffer[0],
            line_size
        )) {
            // Mark the new line to be on the next block and leave it as the
            // greater line
            heap[heap_size] = -heap_size;
            printf("its not on the same block! :)\n");
        } else {
            printf("its on the same block! :(\n");
            // Promote the new line to it's place on the heap
            a_index = heap_size; // "son"
            b_index = (heap_size - 1) / 2; // "father"
            while (
                // "son" is not the heap root
                a_index > 0 &&
                // "son" and "father" are of the same block
                (
                    (heap[a_index] < 0 && heap[b_index] < 0) ||
                    (heap[a_index] > 0 && heap[b_index] > 0)
                ) &&
                // "son" is lower than "father"
                a_menor_que_b(
                    buffer[abs(heap[a_index])],
                    buffer[abs(heap[b_index])],
                    line_size
                )
            ) {
                // Swap "son" and "father"
                temp = heap[a_index];
                heap[a_index] = heap[b_index];
                heap[b_index] = temp;

                // Jump to the previous heap level
                a_index = b_index; // "son" becomes "father"
                b_index = (b_index - 1) / 2; // "father" becomes it's "father"
            }
        }

        heap_size++;
    }

    printf("heap: %d\n", heap_size);
    for(i = 0; i < heap_size; i++) {
        printf("%d - %s\n", heap[i], buffer[heap[i]]);
    }

    fclose(ptr_input_file);
    fclose(ptr_output_file);
    mathias_free(heap);
    for(i = 0; i < (m + 1); i++) {
        mathias_free(buffer[i]);
    }
    mathias_free(buffer);
}
