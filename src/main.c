/* Lab 5 - Multithread Prime Counter - Joao P. O. Pagnan - 199727 */

/* bibliotecas que serao utilizadas no codigo */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/* define o numero maximo de threads em paralelo */
/* #define N_MAX_THREADS 4 */

/* definicao dos estados da threadpool */
#define NOT_ASSIGNED 0
#define ASSIGNED 1
#define FINISHED 2

/* define o tamanho do buffer de entrada */
const int tam_buffer = 100;

/* definicao de uma funcao para checar se um numero eh primo */
int prime_check(unsigned long long num){
  if ((num == 0) || (num == 1)){ /* definicao dos casos base */
    return 0;
  }
  unsigned long long divisor;
  for (divisor = 2; divisor <= num - 1; divisor++){
    if (num%divisor == 0){ /* checa se o numero eh um multiplo do divisor e retorna 0 para o nao-primo*/
      return 0;
    }
  }
  if (divisor == num){ /* se o numero passou por todos os testes de divisor, entao ele eh primo*/
    return 1;
  }
}

/* funcao principal do codigo */
int main(){
  int num_count = 0; /* variavel para contar o numero de numeros a serem checados */

  /* aloca um espaco de memoria para guardar os numeros */
  int *num_ptr_mem;
  num_ptr_mem = (int*) malloc(sizeof(unsigned long long));

  /* aloca um espaco de memoria para guardar uma lista do status dos numeros */
  int *num_stat_mem;
  num_stat_mem = (int*) malloc(sizeof(unsigned int));

  /* instrucoes para receber uma string de entrada e dividir nos espacos */
  char num_str[tam_buffer];
  fgets(num_str, tam_buffer, stdin);
  char* num_ptr = strtok(num_str, " ");
  while (num_ptr != NULL){
    num_ptr_mem = realloc(num_ptr_mem, (num_count + 1)*sizeof(unsigned long long));
    num_stat_mem = realloc(num_stat_mem, (num_count + 1)*sizeof(unsigned int));
    num_ptr_mem[num_count] = atoll(num_ptr);
    num_stat_mem[num_count] = NOT_ASSIGNED;
    num_ptr = strtok(NULL, " ");
    num_count++;
  }

  /* definicao das threads; eh definida um thread para cada numero da string */
  /* pthread_t thread[num_count]; */
  /* int thread_id[num_count]; */

  /* terminados os threads em execucao, nos exibimos o resultado final*/
  /* printf("%d\n", ); */
  printf("Aqui estao os numeros da strings e o seus respectivos status:\n");
  for (int i = 0; i < num_count; i++){
    printf("numero %d: %d, status: %d\n", i, num_ptr_mem[i], num_stat_mem[i]);
  }
  return 0;
}
