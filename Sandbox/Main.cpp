#include "CharraEngine/Core/Application.hpp"


int main()
{
    Charra::Application application({15,15}, {400, 400}, "Charra Test");

    application.run();

}