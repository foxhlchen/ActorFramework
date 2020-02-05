//
// Copyright 2019 Fox Chen <foxhlchen@gmail.com>
//

#include "actor_msg.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define ACTOR_MM_ALOC malloc
#define ACTOR_MM_FREE free

enum ACTOR_STATUS {
	ACTOR_S_INVALID = 0x0,
	ACTOR_S_CREATED = 0x11,
	ACTOR_S_RUN = 0x12,
	ACTOR_S_SUSPEND = 0x13,
	ACTOR_S_STOP = 0x14,
	ACTOR_S_DESTORYED = 0x14,
};

typedef void (*actor_func) (actor_t self, void* args);

typedef struct _opaque_actor_t {
	long actor_id;

	actor_func func;
	void* func_args;

	// automatically destroy actor itself when actor function exits.
	int self_destroy;

} *actor_t;

// actor control block
typedef struct {
	struct _opaque_actor_t actor_hdl;

	char status;

	pthread_t actor_thread;

	pthread_cond_t cv;
	pthread_mutex_t mtx;
	actor_msg_t headmsg;

} actor_ctl_blk;

// return NULL when actor creation fails
extern actor_t spawn_actor(actor_func actor, void* args);

extern actor_msg_t actor_make_msg(actor_msg_type_t msgtype);

extern void actor_dispose_msg(actor_msg_t msg);

// Note: after being called, msg will set to NULL to prevent reuse
extern int actor_send(actor_t actor, actor_msg_t* msg);

extern actor_msg_t actor_receive(actor_t actor);

extern actor_msg_t actor_receive_timewait(actor_t actor, unsigned sec);

extern void actor_destory(actor_t actor);