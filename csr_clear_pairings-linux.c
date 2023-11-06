/************************************************************************/
/*                  CSR CLEAR PAIRINGS CMD LINE TOOL                    */
/************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <libusb-1.0/libusb.h>

/************************************************************************/
/*                             Defines                                  */
/************************************************************************/




/************************************************************************/
/*                         Type definitions                             */
/************************************************************************/

typedef unsigned char			uchar;
typedef unsigned int			uint;

/************************************************************************/
/*                          Global variables                            */
/************************************************************************/

libusb_device **dev_list;
libusb_device *dev = NULL;
libusb_device_handle *handle = NULL;
struct libusb_device_descriptor dev_descr;

int verbose;

uchar ctrl_buf[9];

/************************************************************************/
/*                          Global constants                            */
/************************************************************************/

const int vendor_hid = 0x0a12, product_hid = 0x100b; // CSR 8510A10 USB dongle HID proxy mode
const int vendor_hci = 0x0a12, product_hci = 0x0001; // CSR 8510A10 USB dongle HCI mode

/************************************************************************/
/*                          Function prototypes                         */
/************************************************************************/

void print_usage(uchar **argv);

void print_usage(uchar **argv)
{
	printf("Usage: %s [-option]", argv[0]);
	printf("\n\n");
	printf("Options:");
	printf("\n\n");
	printf("-hid2hci (switch from HID proxy to HCI mode)\n");
	printf("-clear_pairings\n");
	printf("-version\n");
}

int main(int argc, uchar **argv)
{
	printf("\n\r+++ Bluetooth-USB-Bridge / HID Proxy Dongle Command Line Interface +++\n\n");

	if (argc > 1 && strcmp(argv[1], "-version") == 0)
	{
		printf("BTUSBCMD for Linux, version 0.1, November 6, 2023\n");
		return 0;
	}

	int valid_arg_cnt = 0;

	if (argc > 1 && strcmp(argv[1], "-hid2hci") == 0) valid_arg_cnt++;
	if (argc > 1 && strcmp(argv[1], "-clear_pairings") == 0) valid_arg_cnt++;

	if ((argc < 2) || valid_arg_cnt == 0)
	{
		print_usage(argv);
		return 0;
	}

	int err = 0;

	libusb_init(NULL);

	int cnt = libusb_get_device_list(NULL, &dev_list);
	unsigned char Manufacturer_String[255];
	unsigned char Product_String[255];

	//loop through device list and find device
	for (int i = 0; i < cnt; i++)
	{
		dev = dev_list[i];
		libusb_get_device_descriptor(dev, &dev_descr);
		
		if ((vendor_hid == dev_descr.idVendor) && (product_hid == dev_descr.idProduct))
		{
			printf("\nDevice found. Trying to get handle...\n\n");

			err = libusb_open(dev, &handle);
			if (err)
			{
				printf("LIBUSB error code: %s", libusb_error_name(err));
				printf("\n\n");
			}
			else
			{
				libusb_get_string_descriptor_ascii(handle, dev_descr.iManufacturer, Manufacturer_String, 255);
				libusb_get_string_descriptor_ascii(handle, dev_descr.iProduct, Product_String, 255);
				printf("Device info: No. %d   Vendor ID: %x   Manufacturer: %s   Product ID: %x   Device name: %s   Bus: %d   Address: %d\n\n",
					i, dev_descr.idVendor, Manufacturer_String, dev_descr.idProduct, Product_String,
					libusb_get_bus_number(dev), libusb_get_device_address(dev));
			}

			libusb_free_device_list(dev_list, 1);
			break;
		}
	}

	if (handle == NULL)
	{
		fprintf(stderr, "Can't get handle for specified device!\n");
		libusb_exit(NULL);
		exit(1);
	}

	
	if (handle)
	
	if(libusb_detach_kernel_driver(handle, 0) != 0) printf("Cannot detach device from kernel driver!\n");
	if(libusb_claim_interface(handle, 0) != 0) printf("Cannot claim interface!\n");

	{
		
		if (argc > 1 && strcmp(argv[1], "-hid2hci") == 0)
		{
		
		    // in case the dongle is configured as a composite device    
		    
			ctrl_buf[0] = 1;
			ctrl_buf[1] = 5;
			libusb_control_transfer(handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS |
				LIBUSB_RECIPIENT_INTERFACE, LIBUSB_REQUEST_SET_CONFIGURATION, 0x301, 0, ctrl_buf, 9, 1000);
						
			// in case the dongle is configured as a single interface device
			
			ctrl_buf[0] = 5;
			ctrl_buf[1] = 0;
			libusb_control_transfer(handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS |
				LIBUSB_RECIPIENT_INTERFACE, LIBUSB_REQUEST_SET_CONFIGURATION, 0x300, 0, ctrl_buf, 8, 1000);
						
			printf("Switching from HID proxy to HCI mode.\n");
		}
		else
			if (argc > 1 && strcmp(argv[1], "-clear_pairings") == 0)
			{
			
			    // in case the dongle is configured as a composite device
			    
				ctrl_buf[0] = 1;
				ctrl_buf[1] = 6;
				libusb_control_transfer(handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS |
					LIBUSB_RECIPIENT_INTERFACE, LIBUSB_REQUEST_SET_CONFIGURATION,
					0x301, 0, ctrl_buf, 9, 1000);
					
				// in case the dongle is configured as a single interface device
				
				ctrl_buf[0] = 6;
				ctrl_buf[1] = 0;
				libusb_control_transfer(handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS |
					LIBUSB_RECIPIENT_INTERFACE, LIBUSB_REQUEST_SET_CONFIGURATION,
					0x300, 0, ctrl_buf, 8, 1000);
					
				printf("Pairings cleared.\n");
			}
	}
    
    libusb_release_interface(handle, 0);
    libusb_attach_kernel_driver(handle, 0);
	libusb_close(handle);
	libusb_exit(NULL);

	return 0;	// shouldn't get here
}
