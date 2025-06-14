#include "app.h"
int main() { auto app = std::make_unique<App>(640, 480, true);  app->run(); }
