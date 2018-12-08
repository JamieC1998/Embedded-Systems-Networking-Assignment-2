/*
Author      : Jamie Cotter
Student ID  : R00154256
Date        : 6/12/2018
*/

#include "contiki.h"
#include "net/rime.h"
#include "random.h"
#include <stdio.h>

/* These hold the broadcast and unicast structures, respectively. */
static struct broadcast_conn broadcast;

struct message{
    int sequenceNumber = 0;
    int hopCount = 0;

};

PROCESS(broadcast_process, "Node process");

AUTOSTART_PROCESSES(&broadcast_process);

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
    struct message *message_pointer;

    message_pointer = packetbuf_dataptr();

    printf("Broadcast: %d.%d\nHop Count: %d\nSequence Number: %d", from->u8[0], from->u8[1], message_pointer->hopCount, message_pointer->sequenceNumber);
    
}

static const struct broadcast_callbacks broadcast_call = {broadcast_recv};

PROCESS_THREAD(example_broadcast_process, ev, data){

    //Creating our ETimer
    static struct etimer et;

    PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

    PROCESS_BEGIN();

    /*
    Here we open a broadcast connection on band 146
    and open a unicast on band 140
    */
    broadcast_open(&broadcast, 146, &broadcast_call);

    struct message *message_pointer = calloc(sizeof (struct message), 1);

    PROCESS_END();
}