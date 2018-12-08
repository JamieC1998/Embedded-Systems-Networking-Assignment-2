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
 struct broadcast_conn broadcast;



static int sequenceNumber = -1;
static int hopFromSink = -1;

static linkaddr_t parent;

struct message{
    int sequenceNumber;
    int hopCount;

};

struct message status;

PROCESS(broadcast_process, "Node process");

AUTOSTART_PROCESSES(&broadcast_process);

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
    struct message *message_pointer;

    message_pointer = packetbuf_dataptr();
    if(message_pointer -> hopCount == 0){
        status.sequenceNumber  = message_pointer->sequenceNumber;
        parent.u8[0] = from->u8[0];
        parent.u8[1] = from->u8[1];
        status.hopCount = message_pointer->hopCount + 1;
    }

    else if(message_pointer->sequenceNumber > status.sequenceNumber || (parent.u8[0] == from->u8[0] &&  parent.u8[1] == from->u8[1])){
        printf("Hop Count: %d\nSequence Number: %d\n", message_pointer->hopCount, message_pointer->sequenceNumber);

        status.sequenceNumber  = message_pointer->sequenceNumber;
        parent.u8[0] = from->u8[0];
        parent.u8[1] = from->u8[1];
        status.hopCount = message_pointer->hopCount + 1;

     }
    else{

    }
    packetbuf_copyfrom(&status, sizeof(struct message));
    broadcast_send(&broadcast);
    //printf("Broadcast message sent from Node\n");
}

static const struct broadcast_callbacks broadcast_call = {broadcast_recv};

PROCESS_THREAD(broadcast_process, ev, data){

    PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

    PROCESS_BEGIN();

    /*
    Here we open a broadcast connection on band 146
    and open a unicast on band 140
    */
    broadcast_open(&broadcast, 146, &broadcast_call);


    PROCESS_END();
}