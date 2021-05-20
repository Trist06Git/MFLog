
#include <stdio.h>
#include <unistd.h>
#include <sys/reboot.h>

void self_reboot(void) {
    printf("syncing fs\n");
    sync();
    printf("rebooting in 3 seconds\n");
    sleep(3);
    reboot(RB_AUTOBOOT);
}
