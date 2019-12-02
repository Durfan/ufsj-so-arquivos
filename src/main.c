#include "main.h"

int main(void) {

	setlocale(LC_ALL,"");
	gFatplug = false;

	system("clear");
	printf(CYEL"%s [%s %s]"CRST"\n",
		program_invocation_short_name,VERSION,TAG);
	puts("digite 'help' para ajuda");

	shell();

	return EXIT_SUCCESS;
}