#include <Arduino.h>

#define FALSE 0
#define TRUE 1

//PROTOCOL DEFINES
#define DATAMAX 50
#define SYNC0   0xAA
#define SYNC1   0xBB
#define BUMPERPACK  0x42
#define RANGERPACK  0x68
#define ENCODERPACK  0x81
#define LINEFOLLOWERPACK  0x93
#define DIOPACK  0x15

#define BUMPERFRQ  100
#define RANGERFRQ  200
#define ENCODERFRQ  500
#define LINEFOLLOWERFRQ  1000
#define DIOFRQ  200
//amory - implementar contador de erro de checksum e mandar de tempos em tempos pro driver


class Player{

    public:
        Player();
        ~Player();
        int readData(unsigned char *rx_data, unsigned int *rx_data_count);
        int writeData(unsigned char *tx_data, unsigned int tx_data_count);
        int writeData(unsigned char msgType, unsigned char *tx_data, unsigned int tx_data_count);
        

    private:
        unsigned char checksum(unsigned char *msg,int size);
        int printg(char *msg);//generic string print
        
        char serial_port[50];
        int fd; //The returned file handle for the device
        int state;
        unsigned char read_buff[256];
        int received_bytes;
        int expected_packet_size;
};
