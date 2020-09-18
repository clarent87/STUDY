//
// Created by alien on 2020-09-17.
//

// 일단 man paged의 기본으로 모니터링 진행
// 1) thread로 빼긴 해야 함. ( c 형태 )
// 2) signal은 일단은 무시
// 3) dump는 dumpper가 일단은 없으니.. 그냥 read로 진행
// 4) event 확인시 kill 진행 필요
// 5) x86에뮬에 adb 가능한지 check
// 6) dump 준비 ( syscall 하는걸로 진행 )

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include "monitor.h"

// 종료 조건은 두지 않는다.
// wd는 단순 error 처리만
// task 내용 전부 읽어서 => maps / mem 따로 할당해야 할듯.. 일단은
// path는 self까진 하드코딩이니까 받을 필요 없음.

/**
 * TODO wd도 받아서 읽힌 이벤트 확인하는 로직도 고려 but 일단은 kill을 진행.
 * 일단 event 발생 사실 print 하고 바로 kill 진행.
 * @param fd
 */
static void handle_event(int fd ){
    char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    int i;
    ssize_t len;
    char *ptr;


    kill(getpid(),SIGKILL); // process id 받아서 강제 kill ( 아 fork가 resource 문제 있댔지.. kill 말고. )
    return;
}

/**
 * - 무한 루프를 도는 api
 * - thread에서 사용해야 하며
 * - todo : signal handler 등록해 줘야함 ( 근데 해당 시그널을 다른데서 처리하면 어쪄지? thread zombie 처리는 어떻게 하지? 일단 test용이니까 걍진행?)
 */
void mem_monitor(){
    int fd, i, poll_num;
    nfds_t nfds = 1;      // fd 하나만 볼것.
    struct pollfd fds[1]; // fd 하나만 볼것.


    // 1) inotify fd 생성
    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1)
    {
        perror("[*] inotify_init1");
        exit(EXIT_FAILURE);
    }

    // 2) make path
        // TODO 일단 단순 self만 진행

    // 3) watch
    for (i = 0; i < PATH_LEN; i++)
    {
        int wd = inotify_add_watch(fd, g_path[i],IN_OPEN | IN_CLOSE);
        if (wd == -1) {
            fprintf(stderr, "[*] Cannot watch '%s': %s\n", g_path[i], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // 4) prepare poll
    fds[0].fd = fd;
    fds[0].events = POLLIN;
    printf("[*] Listening for events.\n");

    // 5) loop poll
    while (1)
    {
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1)
        {
            if (errno == EINTR) continue;
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (poll_num > 0)
        {
            if (fds[1].revents & POLLIN) handle_event(fd);

        }
    }

    printf("Listening for events stopped.\n");
    return;
}

// TODO 시그널 핸들러에서 아래 내용 진행해야 함
///* Close inotify file descriptor */
// close(fd);
// path malloc 내용 free
//exit(EXIT_SUCCESS);