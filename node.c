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


/*
Initialise my nodes
sequence number and
hopsFromSink
*/
static int sequenceNumber = -1;
static int hopFromSink = -1;

//This is used to store parent
static linkaddr_t parent;

//This is the struct
//That will be received in
//The broadcast
struct message{
    int sequenceNumber;
    int hopCount;
    int wipe_value;

};

//Declaring my struct
struct message status;

PROCESS(broadcast_process, "Node process");

AUTOSTART_PROCESSES(&broadcast_process);

/*
This function is called when a broadcast
is received. It reads in the struct from
the parent.
If it's parent isn't set then it sets its
parent to whoever sent the packet.
It it's parent is already set and it
receives a packet, it'll only read it 
if it's from it's parent and the sequence
number is greater than its own.
Otherwise it'll only read if the wipe packet
is set to 1, at which point it removes its
set parent and erases its last recorded seq
number
*/
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
    struct message *message_pointer;

    //Copying the struct in from the broadcast received
    message_pointer = packetbuf_dataptr();
    /*
    If the message pointer has its
    wipe packet set to 1
    we remove the current parent
    we set the hopCount to 0 
    and we set the Sequence number to 0
    then we forward the packet 
    */
    if(message_pointer -> wipe_value == 1){
        parent.u8[0] = NULL;
        parent.u8[1] = NULL;

        status.hopCount = 0;
        status.sequenceNumber = 0;

        packetbuf_copyfrom(&status, sizeof(struct message));
        broadcast_send(&broadcast);


    }
    
    //Otherwise we fall in here
    else{

        /*
        If the hop count is 0 we fall in here
        And set our parent and then forward the packet on
        */
        if(message_pointer -> hopCount == 0){
            printf("Hop Count: %d\nSequence Number: %d\n", message_pointer->hopCount, message_pointer->sequenceNumber);
            status.sequenceNumber  = message_pointer->sequenceNumber;
            parent.u8[0] = from->u8[0];
            parent.u8[1] = from->u8[1];
            status.hopCount = message_pointer->hopCount + 1;

            packetbuf_copyfrom(&status, sizeof(struct message));
            broadcast_send(&broadcast);
        
        }

        /*
        If the received sequence number
        is greater than our own then we 
        read in the packet
        Set our parent to the packet received
        And then we increase hop count and forward it
        */
        else if(message_pointer->sequenceNumber > status.sequenceNumber || (parent.u8[0] == from->u8[0] &&  parent.u8[1] == from->u8[1])){
            printf("Hop Count: %d\nSequence Number: %d\n", message_pointer->hopCount, message_pointer->sequenceNumber);

            status.sequenceNumber  = message_pointer->sequenceNumber;
            parent.u8[0] = from->u8[0];
            parent.u8[1] = from->u8[1];
            status.hopCount = message_pointer->hopCount + 1;

            packetbuf_copyfrom(&status, sizeof(struct message));
            broadcast_send(&broadcast);

        }
    }
    
    //packetbuf_copyfrom(&status, sizeof(struct message));
    //broadcast_send(&broadcast);
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