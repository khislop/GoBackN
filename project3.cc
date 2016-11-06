#include "project3.h"

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

simulator *simulation;

int timer = 500;

char data[50000][20];

int aNum = 0;
//int bAckNum = 0;
int aAckedNum = 0;
int bRNum = 0;
int aReAckNum = 0;
int timerNum = 0;

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
    std::cout << "A side has recieved a message from the application that should be sent to side B: " << message.data << std::endl;
    std::cout << "Sequence num = " << aNum << std::endl;
    
    //Add message to the buffer
    snprintf(data[aNum], 20, "%s", message.data);
    
    //Create checksum
    int csum = 0;
    for(int i = 0; i < 20; i++){
        csum += data[aNum][i];
        //std::cout << "Checksum = " << csum << std::endl;
    }
    
    //Create the packet
    pkt packet;
    packet.seqnum = aNum;
    packet.acknum = 0;
    packet.checksum = csum;
    snprintf(packet.payload, 20, "%s", message.data);
    //snprintf(packet.payload, 20, "%s", data[aNum]);

    //Pass the packet down to the network
    simulation->tolayer3(A, packet);

    //If there are no packages in flight, there should be no timer and we need to start one
    if(aAckedNum == aNum){
        simulation->starttimer(A, timer);
    }

    //Increment sequence number
    aNum += 1;

  


}

void B_output(struct msg message)  /* need be completed only for extra credit */
{
  std::cout << "B side has recieved a message from the application that should be sent to sideA: " << message.data << std::endl;
  

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
    std::cout << "A side has recieved a packet sent over the network from side B: " << packet.payload << std::endl;
    int reAck;
    //If the incoming ack is greater than the max acked num then update the acked number. 
    if(packet.acknum > aAckedNum){
	    aAckedNum = packet.acknum;
	    aReAckNum = 0;
	    std::cout << "Acked new message, ";
	    
	    //Stop timer and restart it if there are still messages in flight
	    simulation->stoptimer(A);
	    if(aAckedNum < aNum){
	        simulation->starttimer(A, timer);
	    }
    }else{
        aReAckNum += 1;
        reAck = packet.acknum;
        std::cout << "ReAcked old message, ";
    }
    std::cout << "Current Acked package: " << aAckedNum << std::endl;
    std::cout << "Current Number of ReAcked packages: " << aReAckNum << std::endl;
    
    if(aReAckNum > 3){
        //Go back N and resend all the data up to that point
        int oldNum = aNum;
        aNum = reAck;
        for(int i = reAck; i < oldNum; i++){
            msg message;
            snprintf(message.data, 20, "%s", data[i]);
            A_output(message);
        }
        
        aReAckNum = 0;
    }
}


/* called when A's timer goes off */
void A_timerinterrupt()
{
    std::cout << "Side A's timer has gone off " << std::endl;
    
    //Go back N and resend all the data up to that point
    int oldNum = aNum;
    aNum = aAckedNum;
    for(int i = aAckedNum; i < oldNum; i++){
        msg message;
        snprintf(message.data, 20, "%s", data[i]);
        A_output(message);
    }
    
    if(aAckedNum < aNum){
	        simulation->starttimer(A, timer);
    }
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
}


/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    std::cout << "B side has recieved a packet sent over the network from side A: " << packet.payload << std::endl;
    pkt packet2;
    
    //Create checksum
    int csum = 0;
    for(int i = 0; i < 20; i++){
        csum += packet.payload[i];
        //std::cout << "Checksum = " << csum << std::endl;
    }
    
    //Check if packet checksum matches. If not return a NACK (prentend it was lost) and return.
    if(csum != packet.checksum){
        std::cout << "OH SHIIIIIIIIIIIIIIIIIIIIIIIITTTTTTTTTTTTTTTTTTTTTTTT" << std::endl;
        packet2.seqnum = 0;
        packet2.acknum = bRNum;
        packet2.checksum = 0;
        snprintf(packet2.payload, 5, "%s", "NACK");
        simulation->tolayer3(B, packet2);
        return;
    }
    
    //If new message in order and not duplicate, ACK and pass up. Otherwise, NACK and throw away.
    if(packet.seqnum == bRNum){
        packet2.seqnum = 0;
        packet2.acknum = packet.seqnum + 1;
        packet2.checksum = 0;
        snprintf(packet2.payload, 4, "%s", "ACK");
        simulation->tolayer3(B, packet2);
        bRNum += 1;
        //Pass data up
        simulation->tolayer5(B, packet.payload);
    }else{
        packet2.seqnum = 0;
        packet2.acknum = bRNum;
        packet2.checksum = 0;
        snprintf(packet2.payload, 5, "%s", "REACK");
        simulation->tolayer3(B, packet2);

    }
    

}

/* called when B's timer goes off */
void B_timerinterrupt()
{
    std::cout << "Side B's timer has gone off " << std::endl;

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
}

int main(int argc, char **argv) {

  simulation = new simulator;

  A_init();
  B_init();
  simulation->go();
}
