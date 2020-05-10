#include "Source.h"

Outrospection oinstance;

Outrospection* getOutrospection()
{
	return &oinstance;
}

int main()
{
	oinstance.run();
	return 0;
}