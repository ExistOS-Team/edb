#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include "CComHelper.h"
#include "WinReg.h"
#include <time.h>

#include "EDBInterface.h"

using namespace std;


vector<flashImg> imglist;

EDBInterface edb;




void showUsage()
{
	cout << "Usage:" << endl;
	cout << "\t-f <bin file> <page>" << endl;
	cout << "\t-p <serial port> ,if not provided an automatic select will be used." << endl;
	cout << "\t-s use USB MSC to communicate (High Speed)." << endl;
	cout << "\t-r Reboot if all operations done." << endl;
}


int main(int argc, char *argv[])
{
	bool hs = false;
	bool reboot = false;
	bool mscmode = false;

	if (argc < 2) 
	{
		showUsage();
		return -1;
	}

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-f") == 0)
		{
			if (i + 2 > argc) {
				showUsage();
				return -1;
			}
			flashImg item;
			errno_t ret;
			memset(&item, 0, sizeof(item));
			//INFO("Open: %s\n", argv[i + 1]);
			ret = fopen_s(&item.f, argv[i + 1], "rb");
			if (ret != 0)
			{
				INFO("Open: %s Failed.\n", argv[i + 1]);
				return -1;
			}
			item.filename = argv[i + 1];
			item.toPage = atoi(argv[i + 2]);
			if (i + 3 < argc)
			{
				if(strcmp(argv[i + 3], "b") == 0)
				{
					INFO("Set boot img.\n");
					item.bootImg = true;
				}
			}
			INFO("Flash to page:%d\n", item.toPage);
			imglist.push_back(item);
		}

		if (strcmp(argv[i], "-s") == 0)
		{
			hs = true;
			INFO("Use USB MSC.\n");
		}

		if (strcmp(argv[i], "-r") == 0)
		{
			reboot = true;
		}
		if (strcmp(argv[i], "-m") == 0)
		{
			mscmode = true;
		}


	}

	if (edb.open(hs))
	{

		//return -1;
		hs = false;
		if (edb.open(false))
		{
			return -1;
		}
	}




	edb.reset(EDB_MODE_TEXT);
	if (edb.ping() == false)
	{
		cout << "Device No Responses." << endl;
		return 10;
	}

	if (mscmode)
	{
		edb.vm_suspend();
		edb.mscmode();
	}

	edb.vm_suspend();
	for (flashImg& item : imglist)
	{
		edb.flash(item);
		cout << fclose(item.f) << endl;
	}
	//edb.vm_reset();
	//edb.vm_resume();

	if (reboot)
	{
		edb.reboot();
	}
	

	edb.close();

	return 0;
}