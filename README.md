# ActorFramework

A simple C implementation of Actor Concurrency Model

## Install

Download and put files into your project

## Usage

Include the header

```c
#include "actor.h"
```

Define your actor

```c
void my_actor_func(actor_t self, void* args)
{
    actor_msg_t msg = actor_receive(self); // Receive msg from mailbox
    
    printf("Recv: %ld", msg->msg_type);
    fflush(stdout);
    // Handle msg
    // ...

    actor_dispose_msg(msg); // delete msg

    return;
}
```
Spawn your actor

```c
    actor_t my_actor = spawn_actor(my_actor_func, NULL);
```

Send msg to actor

```c
    actor_msg_t msg = actor_make_msg(MY_MSG_TYPE);

    // copy data to msg->payload;
    // set reply actor as msg->sender

    actor_send(actor_foo, &msg);  // send msg
```