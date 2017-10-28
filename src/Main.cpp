#include <ctime>
#include "Handler.h"

int main() {
    std::srand(std::time(NULL));
    Handler *handler = new Handler("../res/flows.txt", "../res/res.txt", 6, 5);
    handler->handle();
    return 0;
}