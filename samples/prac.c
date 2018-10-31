#include <stdio.h>

typedef void(*func) ( const char* message );

struct prac {
	func mes;
};

int main()
{
	struct prac p;
	p.mes
}