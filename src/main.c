#include "actor.h"

void actor_foo_func(actor_t self, actor_msg_t actor_msg)
{

}

int main(int argc, char const *argv[])
{
    actor_t actor_foo = spawn_actor(actor_foo_func);

    if (actor_foo == NULL)
        return -1;

    actor_msg_t msg = actor_make_msg(100);

    actor_send(actor_foo, &msg);

    actor_destory(actor_foo);

    return 0;
}
