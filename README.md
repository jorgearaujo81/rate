# Escalonamento em tempo real

Nesse projeto o algoritmo rate-monotonic (rate) para simulação de sistemas de tempo real deve ser implementado, em que o tempo total de simulação e as tarefas periódicas a serem escalonadas serão descritas a partir de um arquivo de entrada com a seguinte formatação:

[TOTAL TIME]

[TASK NAME] [PERIOD] [CPU BURST]

O arquivo de saída deverá ser nomeado por rate_login.out, em que login corresponde as iniciais do e-mail em minúsculas. Algumas especificações

1. O programa deve ser implementado em C e ser executável em sistemas Linux, Unix ou macOS, com a compilação feita por Makefile, através simplesmente do comando make via terminal, e retornar o arquivo com nome rate executável;

2. A execução deve ser realizada utilizando comando rate [namefile], em que o namefile corresponde ao nome do arquivo com a lista das tarefas.
Segue um exemplo.
Dado que um arquivo de entrada hipotético de nome input.txt apresente o seguinte conteúdo:
165
T1 50 25
T2 80 35
, indicando que a execução terá 165 unidades de tempo (u.t.) e duas tarefas, que chegam simultaneamente ao sistema para serem executadas: a primeira de nome T1, a ser chamada a cada 50 u.t. e com o tempo de execução de 25 u.t., e uma segunda com nome T2, a ser chamada a cada 80 u.t. e com o tempo de execução de 35 u.t.. O deadline para cada tarefa requer que a mesma seja completada antes de uma nova instância da mesma. A execução

rate input.txt

deverá cria o arquivo rate_login.out com o seguinte conteúdo:

EXECUTION BY RATE
[T1] for 25 units - F
[T2] for 25 units - H
[T1] for 25 units - F
[T2] for 5 units - L
[T2] for 20 units - H
[T1] for 25 units - F
[T2] for 15 units - F
idle for 10 units
[T1] for 15 units - K

LOST DEADLINES
[T1] 0
[T2] 1

COMPLETE EXECUTION
[T1] 3
[T2] 1

KILLED
[T1] 1
[T2] 1

Segue a descrição de cada linha:
EXECUTION BY RATE - o algoritmo utilizado
[T1] for 25 units - F - T1 executou por 25 u.t. e finalizou (F)
[T2] for 25 units - H - T2 executou por 25 u.t. e ficou em espera (H)
[T1] for 25 units - F - T1 executou por 25 u.t. e finalizou (F)
[T2] for 5 units - L - T2 executou por 5 u.t. mas atingiu o deadline e foi perdido
[T2] for 20 units - H - T2 executou por 20 u.t. e ficou em espera (H)
[T1] for 25 units - F - T1 executou por 25 u.t. e finalizou (F)
[T2] for 15 units - F - T2 executou por 15 u.t. que restava e finalizou (F)
idle for 10 units - não há nenhuma processo a ser executado
[T1] for 15 units - K - T1 executou por 15 u.t. e foi removido (K) pois o tempo expirou

LOST DEADLINES - indicar para cada tarefa, quantas atingiram o deadline
[T1] 0 - nenhuma tarefa T1 atingiu o deadline
[T2] 1 - uma tarefa T2 atingiu o deadline

COMPLETE EXECUTION - quantas tarefas de cada tipo finalizaram durante o tempo total
[T1] 3 - três tarefas T1 finalizaram
[T2] 1 - uma tarefa T2 finalizou

KILLED - quantos processos foram removidos pois o tempo expirou
[T1] 1 - uma tarefa T1 foi removida
[T2] 1 - uma tarefa T2 foi removida

O programa não deve falhar se encontrar um erro, precisando checar todos os parâmetros antes
de aceita-los. Assim, seu programa deve apresentar um mensagem de erro coerente e encerrar
a execução quando: (1) um número incorreto de argumento para o comando rate for passado,
(2) se o arquivo de entrada não existir, não puder ser aberto ou encontrar-se numa formatação
diferente daquela indicada nesse documento.