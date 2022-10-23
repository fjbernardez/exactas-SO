#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


#include <signal.h>
#include<stdbool.h> 

int main(int argc, char **argv)
{	
	if (argc != 4){ printf("Use: anillo <n> <c> <s> \n"); exit(0);}

	// cantidad de procesos que forman el anillo
	int n = atoi(argv[1]);

	// valor iniciar del mensaje
	int valor_inicial = atoi(argv[2]);

	// valor iniciar del mensaje
	int start = atoi(argv[3]);

    printf("Se crearán %i procesos, se enviará el numero %i desde proceso %i \n", n, valor_inicial, start);
	printf("=======\n");

	// creacion de los pipes necesarios:
	// 1 pipe entre el padre y el hijo que inicia el anillo
	// 1 pipe entre cada par de procesos. Si n procesos, n pipes para cerrar el anillo.
	int pipe_padre [2];
	int pipes_anillo [n][2];

	pipe(pipe_padre);

	for (size_t i = 0; i < n; i++) {
		pipe(pipes_anillo[n]);
		// printf("valores: [%d][%d]\n", pipes_anillo[n][0],pipes_anillo[n][1]);
	}

	// creacion de los procesos hijos
	int pid_hijos[n];

	int lectura_valor_inicial = 0;
	int valor_recibido_buffer = 0;

	printf("soy el padre, mi PID es: %d\n", getpid());

	for (int i = 0; i < n; i++) {
		pid_hijos[i] = fork();
		
		if (pid_hijos[i] == 0) { // hijo numero i
			printf("soy un hijo, mi PID es: %d\n", getpid());

			// cierro lo que no necesito a nivel de hijo
			//for (size_t i = 0; i < n; i++) {
				// en funcion del valor de i se en que posicion estoy. 
				// dejo la escritura del siguiente/primero habilitada
				// dejo la lectura del anterior/ultimo habilitadas
				// PREGUNTAR
			//}

			// if soy el hijo que inicia, espero bloqueado en la lectura del pipe con mi padre.
			printf("i     vale: %d\n", i);
			printf("start vale: %d\n", start);
			
			printf("el bool vale: %d\n", i == start);
			bool b = i == start;
			printf("El bool con variable es: %d\n", b);
			if (b) {
				printf("ACA ESTOY");
				read(pipe_padre[0], &lectura_valor_inicial, sizeof(int));

				if (lectura_valor_inicial == 0) perror("lecura valor inicial");

				printf("soy el hijo elejido, el numero que recibi es: %d\n", lectura_valor_inicial);
				
				// escribo en el anillo
				//write(pipes_anillo[])
				
			} 

			exit(0);
		}
	}

	// cierro todo lo que no necesito
	for (size_t i = 0; i < n; i++) {
		close(pipes_anillo[i][0]);
		close(pipes_anillo[i][1]);
	}

	// escribo valor iniciar al proceso que comienza el anillo.
	if (write(pipe_padre[1], &valor_inicial, sizeof(int)) != sizeof(int)) 
		perror("escribiendo mensaje inicial");

	// me quedo esperando que el anillo termine, bloqueado en el read()
	if (read(pipe_padre[0], &valor_recibido_buffer, sizeof(int)) != sizeof(int)) 
		perror("lecura valor inicial");

	printf("Recibi el valor: %d\n", valor_recibido_buffer);

	// padre mata a los hijos, dado que estos no tiene nocion del fin del anillo 
	// y estan bloqueados en la lectura correspondiente.

	/*
	for (size_t i = 0; i < n; i++) {
		kill(pid_hijos[i], SIGKILL);
	}
	*/

	return 0;
}
