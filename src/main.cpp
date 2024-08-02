#include "Application.h"

int main() {
    Application app(1920, 1080);
    app.Run();
    getc(stdin);
    return 0;
}