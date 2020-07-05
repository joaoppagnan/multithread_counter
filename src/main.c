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

/* definicao das travas do espaco de memoria em que os numeros sao armazenados e seu estado */
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
  int thread_id = (*id_ptr); /* o argumento recebido na funcao eh o identificador da thread, so utilizado para debugg */
  int num_index = 0; /* variavel para representar o indice dos numeros na memoria */
  unsigned long long num_test = 0; /* definicao da variavel do numero a ser testado */
  while (1){ /* vai ficar em loop ate terminar de varrer a lista de estados do threadpool */
    pthread_mutex_lock(&stat_lock); /* pede a trava para leitura dos estados */
    while ((num_stat[num_index] != 0) && (num_index < num_count)){ /* enquanto n estiver terminado de varrer a lista, vai procurar um numero que ainda nao foi checado */
      ++num_index;
    }
    if (num_index >= num_count){ /* se ele terminou a checagem, ja pode terminar a execucao da thread */
      pthread_mutex_unlock(&stat_lock); /* libera a trava de estados */
      break;
    }
    num_stat[num_index] = 1; /* atualiza o estado do indice correspondente para 'em trabalho' */
    pthread_mutex_unlock(&stat_lock); /* libera a trava de estados */
    num_test = num_mem[num_index]; /* o numero a ser testado eh o relacionado ao indice que esta em trabalho */
    int prime = prime_check(num_test); /* chama a funcao de teste de primo */
    pthread_mutex_lock(&num_lock); /* pede a trava do numero */
    num_mem[num_index] = prime; /* substitui o numero q foi testado pelo resultado da funcao de primo */
    pthread_mutex_unlock(&num_lock); /* libera a trava do numero */
    pthread_mutex_lock(&stat_lock); /* pede a trava de estado */
    num_stat[num_index] = 2; /* atualiza o estado do indice do threadpool para 'finalizado' */
    pthread_mutex_unlock(&stat_lock); /* libera a trava de estado */
  }
  return NULL; /* finaliza a thread */
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
    num_mem = realloc(num_mem, (num_count + 1)*sizeof(unsigned long long)); /* aumenta o espaco de memoria para caber mais um numero */
    num_stat = realloc(num_stat, (num_count + 1)*sizeof(unsigned int)); /* e tambem mais um estado */
    num_mem[num_count] = atoll(num_ptr);
    num_stat[num_count] = NOT_ASSIGNED; /* atualiza o estado correspondente para 'nao testado' */
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

  /* terminados os threads em execucao, nos calculamos quantos primos temos*/
  int prime_count = 0;
  for (int i = 0; i < num_count; i++){ /* varre a lista de numeros, que agora sao o resultado da operacao de checagem de primo */
    if (num_mem[i] == PRIME){ /* se na lista ele encontrar o valor correspondente o primo, incrementa o o contador */
      ++prime_count;
    }
  }

  /* exibe o resultado final */
  printf("%d\n", prime_count);

  /* libera o espaco de memoria utilizado */
  free(num_mem);
  free(num_stat);
  return 0;
}
