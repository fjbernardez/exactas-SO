#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


/*
PADRE	MANJEA SIGINT
HIJO	MANEJA SIGURG
	
	El padre manda la señal en un loop, con un sleep.
	Al recibir la señal SIGINT rompe el loop y queda a la espera del hijo para finalizar.

	El hijo con cada señal SIGUR imprime un mensaje de respuesta y aumenta un contador hasta llegar a N.

	El hijo al alcanzar el valor N envia una señal al padre, lo que permite que este salga de su loop y quede a la espera de finalizacion del hijo.

	El hijo ejecuta el programa solicitado y termina.

	El padre recibe la señal de fin del hijo y termina tambien.

*/

#include <stdbool.h>

#define TOTAL_ROUNDS 4

void sigint_handler();
void sigurg_handler();

int rounds = 0;

int main(int argc, char* argv[]) {
	
	pid_t pid;

	if (argc <= 1) {
		fprintf(stderr, "Uso: %s comando [argumentos ...]\n", argv[0]);
		exit(1);
	}

	if (pid = fork()) {
		// padre

		signal(SIGINT, sigint_handler);

		while (1) {
			kill(pid, SIGURG);
			sleep(1);
			write(1, "sup!\n", 5);
		}
		
	} else {
		// hijo

		signal(SIGURG, sigurg_handler);

		while (rounds < TOTAL_ROUNDS) {
		}

		kill(getppid(), SIGINT);
		
		execvp(argv[1], argv+1);
		perror("ERROR child exec(...)"); exit(1);
	}
}

void sigint_handler(int* a) {
	int status;
	
	if(wait(&status) < 0){
		perror("wait");
	}

	exit(EXIT_SUCCESS);
}

void sigurg_handler() {
	rounds++;
	write(1, "ya va!\n", 7);
}