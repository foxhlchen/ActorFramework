#include "actor.h"
#include "stdio.h"
#include <unistd.h>

void actor_foo_func(actor_t self, void* args)
{
    actor_msg_t msg = actor_receive(self);
    
    printf("Recv: %ld", msg->msg_type);
    
    actor_dispose_msg(msg);

    return;
}

int main(int argc, char const *argv[])
{
    actor_t actor_foo = spawn_actor(actor_foo_func, NULL);

    if (actor_foo == NULL)
        return -1;

    actor_msg_t msg = actor_make_msg(100);

    actor_send(actor_foo, &msg);

    sleep(5);

    return 0;
}
