#include <cstring>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

struct spiparams {
	const char* device;
	unsigned long speed;
	unsigned long data;
};

bool istxrxdataok(unsigned char* out,
	unsigned char* in, size_t size)
{
	return (0 == memcmp(out, in, size)) ?
				true:false;
}


void spitest(const spiparams& params)
{
	static const size_t sizemax = 4;

	unsigned char outdata[sizemax] = {
		(unsigned char)(params.data>>24),
		(unsigned char)(params.data>>16),
		(unsigned char)(params.data>>8 ),
		(unsigned char)(params.data>>0 ),
	};
	unsigned char indata[sizemax] = {0};
	double speedmhz = params.speed/1000000.0;

	std::cout<<">> send/receive SPI data with params:"<<std::endl;
	std::cout<<"device: \e[4m"<<params.device<<"\e[0m"<<std::endl;
	std::cout<<"speed: \e[4m"<<params.speed<<" Hz ("
			<<speedmhz<<" MHz)\e[0m"<<std::endl;
	std::cout.setf(std::ios::hex, std::ios::basefield); 
	std::cout.setf(std::ios::showbase); 
	std::cout<<"data: "<<params.data<<std::endl;
	std::cout.unsetf(std::ios::hex); 
	std::cout<<"size: "<<sizemax<<std::endl<<std::endl;
	       		
        struct spi_ioc_transfer spi[] = { 
		{
			.tx_buf = (unsigned long)outdata,
			.rx_buf = (unsigned long)indata,
			.len = sizemax,
			.speed_hz = params.speed,
			.delay_usecs = 0,
			.bits_per_word = 8
		},
	};

	int fd = open(params.device, O_RDWR);
        int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);
	close(fd);

	std::cout.setf(std::ios::hex, std::ios::basefield); 
	std::cout.setf(std::ios::showbase); 

	std::cout<<">> sent data:"<<std::endl;
	for(int i = 0; i < sizemax; i++) {
		std::cout<<"["<<i<<"] "<<(int)outdata[i]<<", ";
	}
	std::cout<<std::endl;

	std::cout<<">> received data:"<<std::endl;
	for(int i = 0; i < sizemax; i++) {
		std::cout<<"["<<i<<"] "<<(int)indata[i]<<", ";
	}
	std::cout<<std::endl<<std::endl;
	std::cout.unsetf(std::ios::hex); 

	std::cout<<"[Test result] ";
	if(true == istxrxdataok(outdata, indata, sizemax)) {
		std::cout<<"\e[1;32mOk\e[0m"<<std::endl;
	}
	else {
		std::cout<<"\e[1;31mFAIL\e[0m"<<std::endl;
	}
}

#define DEFAULT_SPIDEV	"/dev/spidev0."
#define CHIPSEL "cs"
#define DEFAULT_CHIPSEL "0"
#define SPEED "freq"
#define DEFAULT_SPEED 500000
#define DATA "hexdata"
#define DEFAULT_DATA 0xF12A3301

int main(int argc, char** argv)
{
	std::string device = DEFAULT_SPIDEV DEFAULT_CHIPSEL;
	spiparams params = {
		device.c_str(),
		DEFAULT_SPEED,
		DEFAULT_DATA,
	};

	for(int i = 0; i < argc; i++) { 
		if(std::string("--" CHIPSEL) == std::string(argv[i]) ||
		   std::string("/" CHIPSEL) == std::string(argv[i])) {
			device = DEFAULT_SPIDEV + std::string(argv[++i]);
			params.device = device.c_str();
		}	
		if(std::string("--" SPEED) == std::string(argv[i]) ||
		   std::string("/" SPEED) == std::string(argv[i])) {
			params.speed = atol(argv[++i]);
		}	
		if(std::string("--" DATA) == std::string(argv[i]) ||
		   std::string("/" DATA) == std::string(argv[i])) {
			params.data = (int)strtol(argv[++i], nullptr, 16);
		}	
	}

	spitest(params);

	return 0;
}

