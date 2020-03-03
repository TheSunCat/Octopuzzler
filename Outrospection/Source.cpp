#include "Source.h"

// instance of the game
Outrospection instance = Outrospection();

Outrospection getOutrospection() {
	return instance;
}

int main()
{
	instance.run();
	return 0;
}