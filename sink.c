/*
Sink
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

PROCESS(broadcast_process, "Broadcast process");

AUTOSTART_PROCESSES(&broadcast_process);

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
    
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

    //struct message *message_pointer = malloc(sizeof(struct message) * 1);

    struct message *message_pointer;
    
    message_pointer->hopCount = 0;

    while(1) {

        message_pointer->sequenceNumber += 1;

        /* Delay 4 seconds */
        etimer_set(&et, CLOCK_SECOND * 4);

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        /*
        Every 2 - 4 seconds we copy a string into the packetbuffer
        and then send out the packet in a broadcast
        */
        packetbuf_copyfrom(&message_pointer, sizeof(struct message));
        broadcast_send(&broadcast);

    }

    PROCESS_END();
}