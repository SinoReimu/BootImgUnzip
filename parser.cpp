#include <iostream>
#include <fstream>

using namespace std;

/*define header struct*/
struct boot_img_header {
	unsigned char magic[8];
	unsigned kernel_size;
	unsigned kernel_addr;
	unsigned ramdisk_size;
	unsigned ramdisk_addr;
	unsigned second_size;
	unsigned second_addr;
	unsigned tags_addr;
	unsigned page_size;
	unsigned unused[2];
	unsigned char name[16];
	unsigned char cmdline[512];
	unsigned id[8];
};

int main (int argc, char *argv[]){

	/*check if this file avaliable*/
	if(argc <= 1) {
		cout << "Paramter error, paramter 1 should be file name" << endl;
		return 0;
	}
	
	ifstream imgin(argv[1], ios::in|ios::binary);
	if(!imgin) {
		cout << "File " << argv[1] << " open error, please check permission or if this file exist!" << endl;
		return 0;
	}
	
	boot_img_header *header = new boot_img_header;
	imgin.read((char*)header, sizeof(boot_img_header));
	
	unsigned char *temp = header->magic;
	
	if(temp[0]!='A'||temp[1]!='N'||temp[2]!='D'||temp[3]!='R'||temp[4]!='O'||temp[5]!='I'||temp[6]!='D'||temp[7]!='!') {
		cout << "magic Code check error, this may not a android boot/recovery image file" << endl;
		return 0;
	}
	
	/*print details of this image file*/
	cout << "PageSize: " << header->page_size << endl;
	cout << "KernelSize: " << dec << header->kernel_size << "(0x" << hex << header->kernel_size << ")" << endl;
	cout << "RamdiskSize: " << dec << header->ramdisk_size << "(0x" << hex << header->ramdisk_size << ")" << endl;
	cout << "SecondSize: " << dec << header->second_size << "(0x" << hex << header->second_size << ")" << dec << endl;
	cout << "Name: " << header->name <<endl;
	cout << "CommandLine: " << header->cmdline << endl;

	/*calculate offset of each file*/
	int n = int((header->kernel_size + header->page_size - 1) / header->page_size);
	int m = int((header->ramdisk_size + header->page_size - 1) / header->page_size);
	int o = int((header->second_size + header->page_size - 1) / header->page_size);
	int k_offset = header->page_size;
	int r_offset = k_offset + (n * header->page_size);
	int s_offset = r_offset + (m * header->page_size);
	
	/* start write kernel*/
	cout << "Writing Kernel File...";
	ofstream outKernel("kernel", ios::out|ios::binary);
	
	int count = header->kernel_size;
	char buff[1024];
	imgin.seekg(k_offset, ios::beg);
	while(count > 1024) {
		imgin.read(buff, 1024);
		outKernel.write(buff, 1024);
		count -= 1024;
	}
	imgin.read(buff, count);
	outKernel.write(buff, count);
	outKernel.close();
	cout << " Completed!" << endl;
	
	/* start write ramdisk*/
	cout << "Writing Ramdisk File...";
	ofstream outRamdisk("ramdisk.gz", ios::out|ios::binary);
	count = header->ramdisk_size;
	imgin.seekg(r_offset, ios::beg);
	while(count > 1024) {
		imgin.read(buff, 1024);
		outRamdisk.write(buff, 1024);
		count -= 1024;
	}
	imgin.read(buff, count);
	outRamdisk.write(buff, count);
	outRamdisk.close();
	cout << " Completed!" << endl;
	
	imgin.close();
	
	return 1;
}
