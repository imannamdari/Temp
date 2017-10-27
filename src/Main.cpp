#include "Handler.h"

int main() {
    Handler *handler = new Handler("../res/flows.txt", "../res/res.txt", 4, 50);
    handler->handle();
    return 0;
}