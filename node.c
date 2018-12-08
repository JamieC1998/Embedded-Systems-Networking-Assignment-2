/*
Node
Author      : Jamie Cotter
Student ID  : R00154256
Date        : 6/12/2018
*/

#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include <stdio.h>

/* These hold the broadcast and unicast structures, respectively. */
static struct broadcast_conn broadcast;

struct message{
    int sequenceNumber;
    int hopCount;

};

PROCESS(broadcast_process, "Node process");

AUTOSTART_PROCESSES(&broadcast_process);

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
    struct message message_pointer;

    struct message *contents = packetbuf_dataptr();

    message_pointer = &contents;

    printf("Hop Count: %d\n", message_pointer->hopCount);
    
}

static const struct broadcast_callbacks broadcast_call = {broadcast_recv};

PROCESS_THREAD(broadcast_process, ev, data){

    //Creating our ETimer
    static struct etimer et;

    PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

    PROCESS_BEGIN();

    /*
    Here we open a broadcast connection on band 146
    and open a unicast on band 140
    */
    broadcast_open(&broadcast, 146, &broadcast_call);

    PROCESS_END();
}