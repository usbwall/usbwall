#include <stdio.h>

#define CTEST_MAIN
#define CTEST_SEGFAULT

#include "ctest.h"

/**
Compilation tests:
make -f MakefileTest clean && make -f MakefileTest all

Éxécution test:
./usbwall_test
Pas de colorisation pour l'utilisation en pipe/file.
Retourne le nombre de test KO, 0 si tout est OK

./usbwall_test parser permet de lancer les tests du module parser

Besoin de CLANG pour la compilation de ctest
*/

int main(int argc, const char *argv[])
{
	printf("\n=== Starting USBWall testing suites. ===\n\n");
	int result = ctest_main(argc, argv);
	return result;
}


