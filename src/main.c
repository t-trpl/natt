#include "main.h"

int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);
    natt_data* nd = create_natt_info();
    display_data* display = nd->dd;
    display->dpy = XOpenDisplay(NULL);
    if (!display->dpy) {
        perror("XOpenDisplay(NULL) failed");
        fprintf(stderr, "ERROR! Ensure DISPLAY is set correctly!\n");
        return EXIT_FAILURE;
    }
    display->screens = XScreenCount(display->dpy);
    build_app(nd);
    if (argc > 1 && (strncmp(argv[1], "-h", 2) == 0 || 
                     strncmp(argv[1], "--hidden", 8) == 0)) {
        gtk_widget_hide(nd->wp->window);
    }
    gtk_main();
    return 0;
}
