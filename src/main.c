#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/die/die.h"

int main(int argc, char *argv[]) {
  char *watch_file = "./src/main.c";
  char *exec_file = "./bin/main";
  if (argc == 3) {
    watch_file = argv[1];
    exec_file = argv[2];
  }
  int fd_inotify = inotify_init();
  fd_inotify >= 0 || DIE("couldn't initialize inotify instance");

  int watch_descriptor =
      inotify_add_watch(fd_inotify, watch_file, IN_CLOSE_WRITE);
  watch_descriptor > -1 || DIE("couldn't add to watch list");

  char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
  struct inotify_event *e;
  int len;

  int pid = fork();

  if (pid != 0) {
    // parent program
    // INFO("child started with pid %d", pid);
    for (;;) {
      len = read(fd_inotify, buf, sizeof(buf));
      if (len <= 0) DIE("couldn't read inotify_event");
      e = (struct inotify_event *)buf;
      if (e->mask & IN_CLOSE_WRITE) {
        // reload program
        // kill current program
        // fork and exec child

        // int pgid = getpgid(pid);
        // if (pgid != -1) {
        kill(pid, SIGINT) == 0 ||
            ERROR("couldn't send kill signal to PID: %d", pid);
        // }

        int status;
        waitpid(pid, &status, 0);
        if (system("make") == 0) {
          // 'make' successful
          // system("make run");
          printf("\033[1;32m\nRestarting...\033[0m\n");
          pid = fork();
          if (pid == 0) {
            // child process
            execl(exec_file, "", (char *)NULL);
          }
        }
      }
    }
    close(fd_inotify);
  } else {
    // child process
    if (system("make") == 0) {
      // 'make' successful
      //  pid = fork();
      // if (pid == 0) {
      // child process
      // INFO("in child process");
      execl(exec_file, "", (char *)NULL);
      //  }
    }
  }
}