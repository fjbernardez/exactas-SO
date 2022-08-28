#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>



int main(int argc, char* argv[]) {
	int status;
	pid_t child;

    long orig_rax;
    long rax;

	if (argc <= 1) {
		fprintf(stderr, "Uso: %s comando [argumentos ...]\n", argv[0]);
		exit(1);
	}

	/* Fork en dos procesos */
	child = fork();
	if (child == -1) { perror("ERROR fork"); return 1; }
	if (child == 0) {
		/* Solo se ejecuta en el Hijo */

		// indica al padre que debe monitorearlo
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);

		execvp(argv[1], argv+1);
		/* Si vuelve de exec() hubo un error */
		perror("ERROR child exec(...)"); exit(1);

	} else {
		/* Solo se ejecuta en el Padre */

		// En que caso seria util este modificador?
		// ptrace(PTRACE_ATTACH, child); 

		while(1) {
			if (wait(&status) < 0) { perror("waitpid"); break; }
			if (WIFEXITED(status)) break; /* Proceso terminado */

			orig_rax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);
			rax = ptrace(PTRACE_PEEKUSER, child, 8 * RAX, NULL);

			if (rax == -ENOSYS) {
				if (orig_rax == 62) { // KILL = 62
					if(ptrace(PTRACE_PEEKUSER, child, 8 * RSI, NULL) == 9) {
						ptrace(PTRACE_KILL, child);
						printf("El proceso %d ha intentado enviar la señal SIGKILL y por eso ha sido detenido.\n", child);
						break;
					}
				}
			}

			// Entiendo que el primer wait se rompe por alguna razon (wait(&status) < 0) y luego queda configurado para detenerse en 
			// cada SYSCALL. Pero este linea que esta comentada: ptrace(PTRACE_ATTACH, child); no parece tener ningun efecto.
			ptrace(PTRACE_SYSCALL, child);
			
		}

		ptrace(PTRACE_DETACH, child);
	}
	return 0;
}
