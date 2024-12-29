#include <stdio.h>
#include <event.h>

void timer_callback(int fd, short event, void *arg) {
    printf("Timer expired!\n");
    // 重新调度定时器事件，或者在这里退出事件循环
    struct event *ev = (struct event *)arg;
    struct timeval tv = {1, 0}; // 1 秒后再次触发
    event_add(ev, &tv);
}

int main() {
    struct event_base *base = event_base_new();
    if (!base) {
        fprintf(stderr, "Could not initialize libevent!\n");
        return 1;
    }

    // 创建一个定时器事件
    struct event *timer_event = event_new(base, -1, EV_PERSIST, timer_callback, event_self_cbarg());

    // 设置定时器事件在 1 秒后触发
    struct timeval tv = {1, 0};
    event_add(timer_event, &tv);

    // 进入事件循环
    event_base_dispatch(base);

    // 清理资源
    event_free(timer_event);
    event_base_free(base);

    return 0;
}
