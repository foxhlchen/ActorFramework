#include "actor.h"
#include <time.h>

static void* actor_run(void* param) 
{
    if (! param)
        return;

    actor_ctl_blk* ctlblk = (actor_ctl_blk*) param;
    ctlblk->actor_hdl.func(&ctlblk->actor_hdl, ctlblk->actor_hdl.func_args);

    return NULL;
}

actor_t spawn_actor(actor_func actor, void* args) 
{
    actor_ctl_blk* new_actor_ctl = (actor_ctl_blk*) ACTOR_MM_ALOC(sizeof(actor_ctl_blk));
    new_actor_ctl->status = ACTOR_S_INVALID;
    new_actor_ctl->actor_hdl.actor_id = (long) new_actor_ctl;
    new_actor_ctl->actor_hdl.func = actor;
    new_actor_ctl->actor_hdl.func_args = args;

    pthread_condattr_init(&new_actor_ctl->cv);
    int err = pthread_create(& new_actor_ctl->actor_thread, NULL, actor_run, (void*) new_actor_ctl);
    if (err != 0) {
        ACTOR_MM_FREE(new_actor_ctl);
        pthread_cond_destroy(&new_actor_ctl->cv);

        return NULL;
    }

    err = pthread_detach(new_actor_ctl->actor_thread);
    if (err != 0) {
        //TODO log err
    }
    
    return &new_actor_ctl->actor_hdl;
}

actor_msg_t actor_receive(actor_t actor) 
{
    actor_ctl_blk* ctlblk = (actor_ctl_blk*) actor;

    pthread_mutex_lock(&ctlblk->mtx);
    pthread_cond_wait(&ctlblk->cv, &ctlblk->mtx);

    // TODO heck Msg

    pthread_mutex_unlock(&ctlblk->mtx);
}

actor_msg_t actor_receive_timewait(actor_t actor, unsigned sec) 
{
    actor_ctl_blk* ctlblk = (actor_ctl_blk*) actor;

    pthread_mutex_lock(&ctlblk->mtx);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += sec;
    pthread_cond_timewait(&ctlblk->cv, &ctlblk->mtx, &ts);

    // TODO Check Msg

    pthread_mutex_unlock(&ctlblk->mtx);
}


int actor_send(actor_t actor, actor_msg_t* msg) 
{
    actor_ctl_blk* ctlblk = (actor_ctl_blk*) actor;
    if (! ctlblk || ctlblk->status != ACTOR_S_RUN) {
        return;
    }

    pthread_mutex_lock(&ctlblk->mtx);

    // TODO Send Msg

    pthread_cond_signal(&ctlblk->cv);
    pthread_mutex_unlock(&ctlblk->mtx);

}

void actor_destory(actor_t actor)
{

}