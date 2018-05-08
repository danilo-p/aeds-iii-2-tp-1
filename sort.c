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

int is_equal_type(int a, int b) {
    return (
        (a < 0 && b < 0) ||
        (a > 0 && b > 0)
    );
}

void demote_heap_node(
    int *heap,
    int heap_size,
    int node_index,
    char **buffer,
    int line_size,
    int curr_type
) {
    int temp;
    int son_index = 2 * node_index + 1; // node's left son
    while (
        // son is not a leaf
        son_index < heap_size
    ) {
        // node has two sons
        if (son_index < heap_size - 1) {
            if (
                (
                    // "son" and the other "son" aren't of the same block
                    is_equal_type(heap[son_index], heap[son_index + 1]) &&
                    // The other "son" is lower than this one
                    a_menor_que_b(
                        buffer[abs(heap[son_index + 1])],
                        buffer[abs(heap[son_index])],
                        line_size
                    )
                ) ||
                (
                    // "son" and the other "son" aren't of the same block
                    !is_equal_type(heap[son_index], heap[son_index + 1]) &&
                    // the other son is from current type
                    is_equal_type(curr_type, heap[son_index + 1])
                )
            ) {
                // Pick the right son
                son_index = son_index + 1;
            }
        }

        if (
            // node and son are of the same block
            (
                is_equal_type(heap[node_index], heap[son_index])
            ) &&
            // The node is lower than the son
            a_menor_que_b(
                buffer[abs(heap[node_index])],
                buffer[abs(heap[son_index])],
                line_size
            )
        ) {
            // node is on the right place
            break;
        }

        // Swap node and son
        temp = heap[node_index];
        heap[node_index] = heap[son_index];
        heap[son_index] = temp;

        // Jump to the next heap level
        node_index = son_index; // node becomes son
        son_index = 2 * node_index + 1;
    }
}

void promote_heap_node(
    int *heap,
    int heap_size,
    int node_index,
    char **buffer,
    int line_size,
    int curr_type
) {
    int temp;
    int father_index = (node_index - 1) / 2;
    while (
        // node is not the heap root
        node_index > 0 && (
            (
                // father and node are from different types
                !is_equal_type(heap[node_index], heap[father_index]) &&
                // node is from current type
                is_equal_type(curr_type, heap[node_index])
            ) ||
            // node is lower than father
            a_menor_que_b(
                buffer[abs(heap[node_index])],
                buffer[abs(heap[father_index])],
                line_size
            )
        )
    ) {
        // Swap node and father
        temp = heap[node_index];
        heap[node_index] = heap[father_index];
        heap[father_index] = temp;

        // Jump to the previous heap level
        node_index = father_index; // node becomes father
        father_index = (node_index - 1) / 2;
    }
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

    unsigned int line_size = 0; // Tamanho das linhas do arquivo de entrada
    unsigned int memory_max_lines = 0; // Máximo de linhas na memória dada

    char **buffer = NULL; // Guarda as linhas trazidas da memória secundária
    int *heap = NULL; // Heap que guardará os indices das strings no buffer
    int heap_size = 0; // Tamanho atual do heap
    char *aux_line = NULL; // String auxiliar utilizada ao longo do programa


    // ============= Inicialização do algoritmo =============

    // Abertura dos arquivos de I/O

    FILE *ptr_input_file = fopen(input_file, "r");
    FILE *ptr_output_file = fopen(output_file, "w+");

    // Definição dos tamanhos

    fscanf(ptr_input_file, "%u%*c", &line_size);
    memory_max_lines = (memory * 1024) / ((line_size + 1) * sizeof(char));
    printf("memory_max_lines %d\n", memory_max_lines);;

    // Criação estruturas

    buffer = (char **) malloc(memory_max_lines * sizeof(char *));
    for (i = 0; i < memory_max_lines; i++) {
        buffer[i] = (char *) mathias_malloc((line_size + 1) * sizeof(char));
    }
    buffer = &buffer[-1]; // Transforma o vetor para "1-indexed"
    heap = (int *) calloc(memory_max_lines, sizeof(int));
    aux_line = (char *) malloc((line_size + 1) * sizeof(char));



    // =========== Divisão do arquivo de entrada em blocos ordenados ===========


    // Construção inicial do heap

    int curr_type = 1; // Tipo do bloco que está sendo construído. (1 | -1)
    for (
        i = 1;
        (
            // Há espaço na memória
            i <= memory_max_lines &&
            // O arquivo de entrada não está vazio
            fscanf(ptr_input_file, "%[^\n]%*c", aux_line) != EOF
        );
        i++
    ) {
        // Adiciona a linha lida no buffer
        strcpy(buffer[i], aux_line);

        // Adiciona o índice da linha no buffer no final do heap
        heap[heap_size] = i;
        heap_size++;

        // Promove a linha lida no heap
        promote_heap_node(heap, heap_size, heap_size - 1,
            buffer, line_size, curr_type); // Marca o índice com o tipo 1
    }


    // Divisão em blocos ordenados escritos no arquivo de saída

    fprintf(ptr_output_file, "%d\n", line_size);
    int block_count = 1; // Quantidade de blocos no arquivo de saída
    int index = 0; // Guarda o índices de linhas no buffer
    while (heap_size > 0) { // Enquanto o heap não estiver vazio
        // Recupera linha que está no topo do heap
        index = heap[0];

        // Passa para o próximo bloco caso o tipo da linha seja diferente
        if (!is_equal_type(index, curr_type)) {
            curr_type = curr_type * (-1);
            fprintf(ptr_output_file, "\n"); // Separa os blocos
            block_count++;
        }

        // Coloca a linha do topo no arquivo de saída
        index = abs(index);
        fprintf(ptr_output_file, "%s\n", buffer[index]);
        strcpy(aux_line, buffer[index]);

        // Lê a próxima linha do arquivo de entrada se não estiver vazio
        if (
            fscanf(ptr_input_file, "%[^\n]%*c", buffer[index]) != EOF &&
            !feof(ptr_input_file)
        ) {
            // Coloca a linha lida no topo do heap
            if (a_menor_que_b(
                buffer[index],
                aux_line,
                line_size
            )) {
                // Marca com o outro tipo para colocar a linha em outro bloco
                heap[0] = index * curr_type * (-1);
            } else {
                // Marca a linha com o tipo desse bloco
                heap[0] = index * curr_type;
            }
        }
        // Se não houver mais linhas
        else {
            // Coloca uma folha do heap no topo
            heap[0] = heap[heap_size - 1];
            heap[heap_size - 1] = 0;
            heap_size--;
        }

        // Rebaixa o topo do heap
        demote_heap_node(heap, heap_size, 0, buffer, line_size, curr_type);
    }


    // ============= Intercalação dos blocos do arquivo de saída =============

    char *aux_file_name = NULL; // Guarda nomes dos arquivos auxiliares
    aux_file_name = (char *) malloc(20 * sizeof(char));

    unsigned int aux_files_count;
    FILE *ptr_aux_file;
    int aux_file_max_block_count;
    int aux_file_curr_block_count;
    FILE **aux_files;
    int *aux_files_empty;
    int aux_files_empty_count;
    int *aux_files_locked;

    // Enquanto estiver mais de 1 bloco no arquivo de saída
    while (block_count > 1) {
        aux_files_count = 1; // Quantidade de arquivos auxiliares
        ptr_aux_file = NULL; // Ponteiro do arquivo auxiliar atual


        // Divisão dos blocos do arquivo de saída em arquivos auxiliares

        // Quantidade máxima de blocos nos arquivos auxiliares.
        aux_file_max_block_count = (block_count / memory_max_lines) + 1;
        // Quantidade atual de blocos no ptr_aux_file. Começa com 1.
        aux_file_curr_block_count = 1;

        // Volta para o início do arquivo de saída
        rewind(ptr_output_file);
        // Pula a primeira linha
        fgets(aux_line, line_size + 1, ptr_output_file);

        // Abre o primeiro arquivo auxiliar
        sprintf(aux_file_name, "aux_file_%d.txt", aux_files_count);
        ptr_aux_file = fopen(aux_file_name, "w");

        // Lê linhas do arquivo de saída enquanto não chegar ao fim
        while (
            fgets(aux_line, line_size + 1, ptr_output_file) != NULL &&
            !feof(ptr_output_file)
        ) {
            // Caso seja uma divisão entre blocos
            if (strcmp(aux_line, "\n") == 0) {
                // Caso o arquivo auxiliar esteja com a quantidade de blocos
                // máxima
                if (aux_file_curr_block_count == aux_file_max_block_count) {
                    // Abre outro arquivo auxiliar
                    fclose(ptr_aux_file);
                    aux_files_count++;
                    sprintf(aux_file_name, "aux_file_%d.txt", aux_files_count);
                    ptr_aux_file = fopen(aux_file_name, "w");
                    // Reinicia a contagem de blocos
                    aux_file_curr_block_count = 1;
                }
                // Caso ainda caiba mais blocos no arquivo auxiliar
                else {
                    fprintf(ptr_aux_file, "\n"); // Separa os blocos
                    aux_file_curr_block_count++; // Conta mais 1 bloco
                }
            }
            // Caso seja uma linha normal
            else {
                // Pula a quebra de linha
                fscanf(ptr_output_file, "%*c");
                // Adiciona a linha no arquivo de saída
                fprintf(ptr_aux_file, "%s\n", aux_line);
            }
        }
        fclose(ptr_aux_file); // Fecha o último arquivo auxiliar aberto


        // Intercalação dos blocos dos arquivos auxiliares no arquivo de saída

        // Limpa o arquivo de saída
        fclose(ptr_output_file);
        ptr_output_file = fopen(output_file, "w+");
        fprintf(ptr_output_file, "%d", line_size);
        block_count = 0;

        // Ponteiros para os arquivos auxiliares
        aux_files = (FILE **) malloc(sizeof(FILE *) * aux_files_count);
        aux_files = &aux_files[-1]; // Transforma o vetor para "1-indexed"

        // Vetor que trava os arquivos cujos blocos que eram lidos terminaram
        aux_files_locked = (int *) malloc(sizeof(int) * aux_files_count);
        aux_files_locked = &aux_files_locked[-1]; // Transforma para "1-indexed"

        // Vetor que marca os arquivos já vazios
        aux_files_empty = (int *) malloc(sizeof(int) * aux_files_count);
        aux_files_empty = &aux_files_empty[-1]; // Transforma para "1-indexed"
        aux_files_empty_count = 0; // Quantidade de arquivos já totalmente lidos

        for (i = 1; i <= aux_files_count; i++) {
            aux_files[i] = NULL;
            aux_files_locked[i] = 0;
            aux_files_empty[i] = 0;
        }

        // Enquanto houver arquivos auxiliares com linhas para serem lidas
        while(aux_files_empty_count < aux_files_count) {
            // Construção inicial do heap
            heap_size = 0; // Limpa o heap
            for (i = 1; i <= aux_files_count; i++) {
                // Destrava o arquivo para iniciar a leitura de um novo bloco
                aux_files_locked[i] = 0;

                // Caso o arquivo auxiliar não estiver aberto
                if (aux_files[i] == NULL) {
                    // Abre o arquivo auxiliar e coloca no vetor
                    sprintf(aux_file_name, "aux_file_%d.txt", i);
                    aux_files[i] = fopen(aux_file_name, "r");
                }

                // Caso haja linhas no arquivo auxiliar
                if (
                    !aux_files_empty[i] &&
                    fscanf(aux_files[i], "%[^\n]%*c", aux_line) != EOF &&
                    !feof(aux_files[i])
                ) {
                    // Adiciona a linha no buffer
                    strcpy(buffer[i], aux_line);

                    // Adiciona o índice da linha no buffer no final do heap
                    heap[heap_size] = i;
                    heap_size++;

                    // Promove a linha lida no heap
                    promote_heap_node(heap, heap_size, heap_size - 1,
                        buffer, line_size, 1);
                }
                // Caso não haja linhas no arquivo auxiliar
                else {
                    // Trava o arquivo
                    aux_files_locked[i] = 1;
                    // Marca o arquivo como vazio
                    aux_files_empty[i] = 1;
                    aux_files_empty_count++;
                }
            }

            // Caso haja mais blocos
            if (heap_size > 0) {
                fprintf(ptr_output_file, "\n"); // Divide os blocos
                block_count++;
            }

            // Enquanto o heap não estiver vazio
            while (heap_size > 0) {
                // Recupera linha que está no topo do heap
                index = heap[0];

                // Coloca a linha do topo no arquivo de saída
                fprintf(ptr_output_file, "%s\n", buffer[index]);

                // Se o arquivo não está vazio ou travado, lê uma nova linha
                // e a coloca como o novo topo
                if (
                    // O arquivo auxiliar não está travado
                    !aux_files_locked[index] &&
                    // O arquivo auxiliar não está no final
                    fgets(aux_line, line_size + 1, aux_files[index]) != NULL &&
                    !feof(aux_files[index])
                ) {
                    // Caso a linha lida seja uma separação de blocos
                    if (strcmp(aux_line, "\n") == 0) {
                        // Trava o arquivo
                        aux_files_locked[index] = 1;
                        // Coloca uma folha do heap no topo
                        heap[0] = heap[heap_size - 1];
                        heap[heap_size - 1] = 0;
                        heap_size--;
                    }
                    // Caso seja uma linha normal
                    else {
                        // Pula a quebra de linha
                        fscanf(aux_files[index], "%*c");
                        // Adiciona a linha no topo do heap
                        strcpy(buffer[index], aux_line);
                    }
                }
                // Se o arquivo está travado ou vazio, coloca uma folha do heap
                // no topo
                else {
                    // Coloca uma folha do heap no topo
                    heap[0] = heap[heap_size - 1];
                    heap[heap_size - 1] = 0;
                    heap_size--;
                }

                // Rebaixa o topo do heap até seu lugar
                demote_heap_node(heap, heap_size, 0, buffer, line_size, 1);
            }
        }

        // Transforma de volta para "0-indexed"
        aux_files = &aux_files[1];
        aux_files_empty = &aux_files_empty[1];
        aux_files_locked = &aux_files_locked[1];

        for (i = 0; i < aux_files_count; i++) {
            if (aux_files[i] != NULL) {
                fclose(aux_files[i]);
                sprintf(aux_file_name, "aux_file_%d.txt", i + 1);
                remove(aux_file_name);
            }
        }
        free(aux_files);
        free(aux_files_empty);
        free(aux_files_locked);
    }

    buffer = &buffer[1]; // Gambiarra pro vetor voltar a ser "0-indexed"
    for (i = 0; i < memory_max_lines; i++) {
        mathias_free(buffer[i]);
    }
    free(buffer);
    free(heap);
    free(aux_line);
    free(aux_file_name);
    fclose(ptr_input_file);
    fclose(ptr_output_file);
}