#include "Handler.h"

int main() {
    Handler *handler = new Handler("../res/flows.txt", "../res/res.txt", 5, 10);
    handler->handle();
    return 0;
}