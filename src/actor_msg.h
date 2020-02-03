typedef long actor_msg_type_t;

enum ACTOR_MSG {
    ACTOR_MSG_QUIT,
    ACTOR_MSG_MAX_SYSTEM = 0x1000,
};

typedef struct _opaque_actor_t *actor_t;

typedef struct _opaque_actor_msg_t {
    actor_msg_type_t msg_type;
    void* payload;
    actor_t sender;
} *actor_msg_t;