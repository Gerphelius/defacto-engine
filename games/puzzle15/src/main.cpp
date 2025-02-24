#include <iostream>
#include <string>

#include "df/core/engine.h"

void update(double deltaTime)
{
    //std::cout << "Delta time: " << deltaTime << '\n';
}

int main()
{
    DF::Core::Engine eng{};

    eng.run(update);

    return 0;
}
