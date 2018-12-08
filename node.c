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

static int sequenceNumber = -1;
static int hopFromSink = -1;

static linkaddr_t parent;

struct message{
    int sequenceNumber;
    int hopCount;

};

PROCESS(broadcast_process, "Node process");

AUTOSTART_PROCESSES(&broadcast_process);

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
    struct message *message_pointer;

    message_pointer = packetbuf_dataptr();

    if(message_pointer->sequenceNumber > sequenceNumber){
        printf("Hop Count: %d\n", message_pointer->hopCount);

        sequenceNumber  = message_pointer->sequenceNumber;
        hopFromSink     = message_pointer->hopCount;

        parent.u8[0] = from->u8[0];
        parent.u8[1] = from->u8[1];

        message_pointer->hopCount++;

        packetbuf_copyfrom(&message_pointer, sizeof(struct message));
        broadcast_send(&broadcast);
    }
    

    
    
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