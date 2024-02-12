#include "ADXL345.h"
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

int fd;						// Memory Map I/O Driver
volatile int *I2C0_ptr;    	// virtual address pointer to I2C0	
volatile int *SYSMGR_ptr;   // virtual address pointer to SYSMGR	

double findPeakGroundAcceleration(int16_t xyz[3]);

int main(void){

    uint8_t devid;
    int16_t mg_per_lsb = 4;
    int16_t XYZ[3];
	
	// Open Memory Map I/O driver and map virtual address for I2C0 and SYSMGR
    openPhysical();
	// if( (fd = open( "/dev/mem", (O_RDWR | O_SYNC))) == -1) 
	// {
	// 	printf( "ERROR: could not open \"/dev/mem\"...\n" );
	// 	return( 1 );
	// }
    I2C0_ptr = (volatile unsigned int *)mmap(NULL, 0x00000100, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0xFFC04000);
	SYSMGR_ptr = (volatile unsigned int *)mmap(NULL, 0x00000800, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0xFFD08000);
	
    // Configure Pin Muxing
    Pinmux_Config();
    
    // Initialize I2C0 Controller
    I2C0_Init();
    
    // 0xE5 is read from DEVID(0x00) if I2C is functioning correctly
    ADXL345_REG_READ(0x00, &devid);
    
    // Correct Device ID
    if (devid == 0xE5)
    {
        // Initialize accelerometer chip
        ADXL345_Init();
        
        while(1)
        {
			if (ADXL345_WasActivityUpdated())
			{
                ADXL345_XYZ_Read(XYZ);
                printf("X=%d mg, Y=%d mg, Z=%d mg\n", XYZ[0]*mg_per_lsb, XYZ[1]*mg_per_lsb, XYZ[2]*mg_per_lsb);
                printf("PGA=%f m/s^2\n", findPeakGroundAcceleration(XYZ));
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
    	if( (fd = open( "/dev/mem", (O_RDWR | O_SYNC))) == -1) 
	{
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}
}

//Find the Peak Ground Acceleration (PGA)
double findPeakGroundAcceleration(int16_t xyz[3]){
    double PGA;

    // Convert from mg to m/s^2
    double x = xyz[0]*(9.8/1000);
    double y = xyz[1]* (9.8/1000);
    double z = xyz[2]/9800;
    
    //Find the Peak Ground Acceleration (PGA) 
    //by taking the square root of the sum of the squares of the X, Y, and Z values.
    PGA = sqrt(x*x + y*y + z*z);

    return PGA;
}
