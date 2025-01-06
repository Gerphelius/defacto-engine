#include <iostream>
#include <string>

#include "df/engine.h"

void update(double deltaTime)
{
    //std::cout << "Delta time: " << deltaTime << '\n';
}

int main()
{
    DF::Engine eng{};

    eng.run(update);

    return 0;
}
