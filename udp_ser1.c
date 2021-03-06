/**************************************
udp_ser.c: the source file of the server in udp transmission
**************************************/
#include "headsock.h"

// transmitting and receiving function
void transmit_and_receive_packets(int sockfd,int datagram_count);                                                           

const char* output_file_name = "outputFile.txt";

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in my_address;
	int datagram_count = 0;		
	//create socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {			
		printf("error in socket");
		exit(EXIT_FAILURE);
	}

	//Initialize Socket Address
	my_address.sin_family = AF_INET;
	my_address.sin_port = htons(MYUDP_PORT);
	my_address.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_address.sin_zero), 8);

	//Bind Socket
	if (bind(sockfd, (struct sockaddr *) &my_address, sizeof(struct sockaddr)) == -1) {           
		printf("error in binding");
		exit(EXIT_FAILURE);
	}
	printf("start receiving\n");
	transmit_and_receive_packets(sockfd,datagram_count);                        
	close(sockfd);
	exit(EXIT_SUCCESS);
}

void transmit_and_receive_packets(int sockfd,int datagram_count)
{
	char buf[BUFSIZE];
	long lseek = 0;
	char received_message[DATALEN];
	int  n = 0, client_address_length = 0, end = 0;
	struct sockaddr_in client_address;
	client_address_length = sizeof (struct sockaddr_in);
	//Initializing acknowledgement
	struct ack_so acknowledgement;
	acknowledgement.num = ACK_CODE;
	acknowledgement.len = 0;
	struct ack_so NACK;
	NACK.num = NACK_CODE;
	NACK.len = 0;
	
	while(!end){
	//receive the packet
	if ((n=recvfrom(sockfd, &received_message, DATALEN, 0, (struct sockaddr *)&client_address, &client_address_length)) > 0) {      
		datagram_count++;
		printf("Received Datagram %d \n",datagram_count);
		if(received_message[n-1] == '\0')
		{
			end = 1;
			n--;
		}
		memcpy((buf+lseek), received_message, n);
		lseek += n;
	
		if ((sendto(sockfd, &acknowledgement, sizeof(acknowledgement), 0, (struct sockaddr *) &client_address, client_address_length)) <= 0 )
		{	
			printf("Error in sending acknowledgement, n = %d\n",n);								
			exit(EXIT_FAILURE);
		}
		else
			printf("Acknowledgement sent\n");
	}
	else{
		if(n==0){
			datagram_count++;
			sendto(sockfd, &NACK, sizeof(NACK), 0, (struct sockaddr *) &client_address, client_address_length);
			printf("Sending NACK\n");
		}
		else{	
			printf("Error receiving Datagram,n=%d\n",n);
			exit(EXIT_FAILURE);
		}
	}
	}
	FILE *file_pointer = fopen(output_file_name, "a+w");
	fwrite (buf , 1 , lseek , file_pointer);
	printf("Total number of bytes received is : %d\n",(int)lseek+1);
	close(sockfd);
	exit(EXIT_SUCCESS);	
}
