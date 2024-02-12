#include "ADXL345.h"
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/time.h>

#define DURATION 1000

int fd;						// Memory Map I/O Driver
volatile int *I2C0_ptr;    	// virtual address pointer to I2C0	
volatile int *SYSMGR_ptr;   // virtual address pointer to SYSMGR	
int16_t XYZ[3];
int xyz[3];
int16_t mg_per_lsb = 4;


double findPeakGroundAcceleration(int *xyz);
void openPhysical();
long getCurrTime();

int main(void){

    uint8_t devid;
	
	// Open Memory Map I/O driver and map virtual address for I2C0 and SYSMGR
    openPhysical();
	
    // Configure I2C0 pins and enable I2C0 
    Pinmux_Config();
    I2C0_Init();
    ADXL345_REG_READ(0x00, &devid);
    
    // Correct Device ID
    if (devid == 0xE5)
    {
        // Initialize accelerometer chip
        ADXL345_Init();

        // 
        long previousTime = getCurrTime();
        while(1)
        {
            // Wait 2 second before displaying
            if(getCurrTime() - previousTime > DURATION) 
            {
                readAcceleration();
                printf("PGA=%f m/s^2\n", findPeakGroundAcceleration(&xyz[0]));
                previousTime = getCurrTime();
            }
        }
    } 
    else 
    {
        printf("Incorrect device ID\n");
    }
	
	// Clean up and return OK
    close(fd);
    return 0;
}

void openPhysical(){
    if( (fd = open( "/dev/mem", (O_RDWR | O_SYNC))) == -1) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}
    I2C0_ptr = (volatile unsigned int *)mmap(NULL, 0x00000100, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0xFFC04000);
	SYSMGR_ptr = (volatile unsigned int *)mmap(NULL, 0x00000800, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0xFFD08000);
}

//Find the Peak Ground Acceleration (PGA)
//by taking the square root of the sum of the squares of the X, Y, and Z values.
double findPeakGroundAcceleration(int *xyz) {
    double PGA;

    // Convert from mg to m/s^2
    double x = (xyz[0]) * (9.81/1000);
    double y = (xyz[1]) * (9.81/1000);
    double z = (xyz[2]-981) * (9.81/1000);
    
    //Find the Peak Ground Acceleration (PGA) 
    //by taking the square root of the sum of the squares of the X, Y, and Z values.
    PGA = sqrt((x*x) + (y*y) + (z*z));

    return PGA;
}

// Return a Long of  Current Time
long getCurrTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

// Read Acceleration from ADXL345 
void readAcceleration(){
    ADXL345_XYZ_Read(XYZ);
    xyz[0] = XYZ[0]*mg_per_lsb;
    xyz[1] = XYZ[1]*mg_per_lsb;
    xyz[2] = XYZ[2]*mg_per_lsb;
}