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

/*
This struct is sent
out to each node 
so as to build the
routing tree
*/
struct message {
    int sequenceNumber;
    int hopCount;
    int wipe_value;

};

//Starts the main process thread
PROCESS(broadcast_process, "Broadcast process");
AUTOSTART_PROCESSES(&broadcast_process);

//This function is used for receiving the broadcast
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
    
}


static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
/* These hold the broadcast and unicast structures, respectively. */
static struct broadcast_conn broadcast;

/*
This is the main program loop
of the program, it sends out
a broadcast every 4 - 8 seconds
Every 20 broadcasts it sends out
a wipe packet which wipes the 
entire tree. After which it 
resets the the sequence number
to zero and starts again
*/
PROCESS_THREAD(broadcast_process, ev, data){

    //Creating our ETimer
    static struct etimer et;
    
    //This is the packet we will send
    struct message message_pointer;

    //Our counter for the sequence
    //number
    static int sequenceNumber = 0;


    PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

    PROCESS_BEGIN();

    /*
    Here we open a broadcast connection on band 146
    and open a unicast on band 140
    */
    broadcast_open(&broadcast, 146, &broadcast_call);

    //struct message *message_pointer = malloc(sizeof(struct message) * 1);

    while(1) {

        /* Delay 4-8 seconds */
        etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        /*
        In this block of code I set the 
        hop count, the sequence number
        and the wipe packet
        */
        message_pointer.hopCount = 0;
        message_pointer.sequenceNumber = sequenceNumber;
        message_pointer.wipe_value = 0;

        //If the sequence number
        //Has passed 20 broadcasts
        //We set the wipe packet 
        //val to 1
        if(sequenceNumber > 20){
            message_pointer.wipe_value = 1;
        }
        /*
        Every 4 - 8 seconds we copy our struct into the packetbuffer
        and then send out the packet in a broadcast
        */
        packetbuf_copyfrom(&message_pointer, sizeof(struct message));
        broadcast_send(&broadcast);

        //Then we increase the sequence number
        sequenceNumber += 1;

        //If the wipe packet has been set
        //We reset the sequence number
        if(message_pointer.wipe_value == 1){
            sequenceNumber = 0;
        }
    }

    PROCESS_END();
}