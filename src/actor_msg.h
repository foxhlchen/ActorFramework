typedef long actor_msg_type_t;

enum ACTOR_MSG {
    ACTOR_MSG_QUIT = -1,
    ACTOR_MSG_MAX_SYSTEM = 0x1000,
};

typedef struct _opaque_actor_t *actor_t;

typedef struct _opaque_actor_msg_t {
    actor_msg_type_t msg_type;
    void* payload;
    unsigned long payload_size;
    actor_t sender;
    struct _opaque_actor_msg_t* next;
} *actor_msg_t;