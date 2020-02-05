#include "actor.h"
#include <time.h>
#include <pthread.h>

static void* actor_run(void* param) 
{
    if (! param)
        return NULL;

    actor_ctl_blk* ctlblk = (actor_ctl_blk*) param;
    ctlblk->status = ACTOR_S_RUN;
    ctlblk->actor_hdl.func(&ctlblk->actor_hdl, ctlblk->actor_hdl.func_args);
    ctlblk->status = ACTOR_S_STOP;

    if (ctlblk->actor_hdl.self_destroy) {
        actor_destory(&ctlblk->actor_hdl);
    }

    return NULL;
}

actor_msg_t actor_make_msg(actor_msg_type_t msgtype) {
    actor_msg_t newmsg = (actor_msg_t) ACTOR_MM_ALOC(sizeof(struct _opaque_actor_msg_t));
    memset(newmsg, 0, sizeof(struct _opaque_actor_msg_t));

    newmsg->msg_type = msgtype;
    return newmsg;
}

void actor_dispose_msg(actor_msg_t msg) {
    if (msg)
        ACTOR_MM_FREE(msg);
}

actor_t spawn_actor(actor_func actor, void* args) 
{
    actor_ctl_blk* new_actor_ctl = (actor_ctl_blk*) ACTOR_MM_ALOC(sizeof(actor_ctl_blk));
    memset(new_actor_ctl, 0, sizeof(actor_ctl_blk));

    new_actor_ctl->status = ACTOR_S_CREATED;
    new_actor_ctl->headmsg = NULL;
    new_actor_ctl->actor_hdl.actor_id = (long) new_actor_ctl;
    new_actor_ctl->actor_hdl.func = actor;
    new_actor_ctl->actor_hdl.func_args = args;

    pthread_cond_init(&new_actor_ctl->cv, NULL);
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
    actor_msg_t recvmsg = NULL;

    pthread_mutex_lock(&ctlblk->mtx);

    while (!ctlblk->headmsg)
    {
        pthread_cond_wait(&ctlblk->cv, &ctlblk->mtx);
    }

    // remove from actor's mailbox
    recvmsg = ctlblk->headmsg;
    ctlblk->headmsg = recvmsg->next;
    recvmsg->next = NULL;

    pthread_mutex_unlock(&ctlblk->mtx);

    return recvmsg;
}

actor_msg_t actor_receive_timewait(actor_t actor, unsigned sec) 
{
    actor_ctl_blk* ctlblk = (actor_ctl_blk*) actor;
    actor_msg_t recvmsg = NULL;

    pthread_mutex_lock(&ctlblk->mtx);

    if (!ctlblk->headmsg) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += sec;
        pthread_cond_timedwait(&ctlblk->cv, &ctlblk->mtx, &ts);
    }

    if (ctlblk->headmsg) {
        // remove from actor's mailbox
        recvmsg = ctlblk->headmsg;
        ctlblk->headmsg = recvmsg->next;
        recvmsg->next = NULL;
    }

    pthread_mutex_unlock(&ctlblk->mtx);

    return recvmsg;
}


int actor_send(actor_t actor, actor_msg_t* msg) 
{
    actor_ctl_blk* ctlblk = (actor_ctl_blk*) actor;
    if (! ctlblk || ctlblk->status != ACTOR_S_RUN) {
        return -1;
    }

    pthread_mutex_lock(&ctlblk->mtx);

    actor_msg_t* prev = &ctlblk->headmsg;
    while (*prev) {
        actor_msg_t next = *prev;
        prev = &next->next;
    }
    *prev = *msg;
    msg = NULL; // msg has delivered, msg's ownership has been transfered to target actor's mailbox. reset src pointer to NULL.

    pthread_cond_signal(&ctlblk->cv);
    pthread_mutex_unlock(&ctlblk->mtx);

    return 0;
}

void actor_destory(actor_t actor)
{
    if (! actor)
        return;

    ACTOR_MM_FREE(actor);
}