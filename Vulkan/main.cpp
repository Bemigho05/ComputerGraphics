#include "app.h"
int main() { auto app = std::make_unique<App>(640 * 2, 480 * 2, true);  app->run(); }
