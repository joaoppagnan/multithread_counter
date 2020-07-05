/* Lab 5 - Multithread Prime Counter - Joao P. O. Pagnan - 199727 */

/* bibliotecas que serao utilizadas no codigo */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/* define o numero maximo de threads em paralelo */
#define N_MAX_THREADS 4

/* definicao dos estados da threadpool */
#define NOT_ASSIGNED 0
#define ASSIGNED 1
#define FINISHED 2

/* definicao dos estados da condicao de primo */
#define PRIME 1
#define NOT_PRIME 0

/* define o tamanho do buffer de entrada */
const int tam_buffer = 100;

/* variaveis globais de ponteiros do heap e contagem de numero */
int *num_mem;
int *num_stat;
int num_count;

/* definicao das travas de acesso ao espaco de memoria em que os numeros sao armazenados, seu estado e se eh primo ou nao */
pthread_mutex_t num_lock;
pthread_mutex_t stat_lock;

/* definicao de uma funcao para checar se um numero eh primo */
int prime_check(unsigned long long num){
  if ((num == 0) || (num == 1)){ /* definicao dos casos base */
    return NOT_PRIME;
  }
  unsigned long long divisor;
  for (divisor = 2; divisor <= num - 1; divisor++){
    if (num%divisor == 0){ /* checa se o numero eh um multiplo do divisor e retorna 0 para o nao-primo*/
      return NOT_PRIME;
    }
  }
  if (divisor == num){ /* se o numero passou por todos os testes de divisor, entao ele eh primo*/
    return PRIME;
  }
}

/* funcao da atividade da thread */
void* threadwork(void* arg){
  int* id_ptr = (int*)(arg);
  int thread_id = (*id_ptr);
  int num_index = 0;
  unsigned long long num_test = 0;
  while (1){
    pthread_mutex_lock(&stat_lock);
    while ((num_stat[num_index] != 0) && (num_index < num_count)){
      ++num_index;
    }
    if (num_index >= num_count){
      pthread_mutex_unlock(&stat_lock);
      break;
    }
    num_stat[num_index] = 1;
    pthread_mutex_unlock(&stat_lock);
    num_test = num_mem[num_index];
    int prime = prime_check(num_test);
    pthread_mutex_lock(&num_lock);
    num_mem[num_index] = prime;
    pthread_mutex_unlock(&num_lock);
    pthread_mutex_lock(&stat_lock);
    num_stat[num_index] = 2;
    pthread_mutex_unlock(&stat_lock);
  }
  return NULL;
}

/* funcao principal do codigo */
int main(){
  num_count = 0; /* variavel para contar o numero de numeros a serem checados */

  /* aloca um espaco de memoria para guardar os numeros */
  num_mem = (int*) malloc(sizeof(unsigned long long));

  /* aloca um espaco de memoria para guardar uma lista do status dos numeros */
  num_stat = (int*) malloc(sizeof(unsigned int));

  /* instrucoes para receber uma string de entrada dividir nos espacos, e armazenar na memoria */
  char num_str[tam_buffer];
  fgets(num_str, tam_buffer, stdin);
  char* num_ptr = strtok(num_str, " ");
  while (num_ptr != NULL){
    num_mem = realloc(num_mem, (num_count + 1)*sizeof(unsigned long long));
    num_stat = realloc(num_stat, (num_count + 1)*sizeof(unsigned int));
    num_mem[num_count] = atoll(num_ptr);
    num_stat[num_count] = NOT_ASSIGNED;
    num_ptr = strtok(NULL, " ");
    num_count++;
  }

  /* definicao das threads */
  pthread_t threads[N_MAX_THREADS];
  int thread_id[N_MAX_THREADS];

  /* disparando threads */
  for (int i = 0; i < N_MAX_THREADS; i++) {
    thread_id[i] = i;
    pthread_create(&(threads[i]), NULL, threadwork, (void*) (&thread_id[i]));
  }

  /* esperando as threads terminarem as checadas */
  for (int i = 0; i < N_MAX_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  /* terminados os threads em execucao, nos exibimos o resultado final*/
  /* printf("%d\n", ); */
  printf("Aqui estao os numeros da strings e o seus respectivos status:\n");
  for (int i = 0; i < num_count; i++){
    printf("numero %d: %d, status: %d\n", i, num_mem[i], num_stat[i]);
  }
  printf("agora vou liberar o espaco de memoria alocado\n");
  free(num_mem);
  free(num_stat);
  return 0;
}
