#include<iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;
class Loader
{
private:
	string input_line;
	int MEM_start_addr;
	int prog_start_addr;
	int prog_length;
	int Transfer_address;
	int TMErecord__start_addr;
	int TMrecord__length;
	bool is_relocation; 
	int reloca_offset;
	string result;
	//output
	string name;
	string MEM_start_addr_str;
	string prog_length_str;
	string Transfer_address_str;
	vector <char> Irecord;
	vector <char> output;

public:
	void read_input();
	void TMErecord_process();
	void Hrecord_process();
	int stringHEX_to_DEC(string hex);
	string int_to_stringHEX(int dec);
	void output_file();
};

int Loader::stringHEX_to_DEC(string hex)
{
	//int length = hex.size();
	const char* str = hex.c_str();
	int num = (int)strtol(str, NULL, 16);
	return num;
}

string Loader::int_to_stringHEX(int dec)
{
	std::ostringstream ss;
	ss << std::hex << dec;
	result = ss.str();
	return result;
}

void Loader::read_input()
{
	ifstream  ifile;
	ifile.open("SIC.txt");
	if (false == ifile.is_open())
	{
		cerr << "open file error" << endl;
	}
	// H record
	getline(ifile, input_line);
	Hrecord_process();
	//initialize
	Irecord.resize(25);
	output.resize(prog_length*2);	//half byte
	for (int i = 0; i < output.size(); i++)
	{
		output[i] = 'F';
	}
	//T M E record
	while (getline(ifile, input_line))
	{
		TMErecord_process();
	}

	ifile.close();
}

void Loader::Hrecord_process()
{
	if (input_line[0]=='H')
	{
		string substr;
		//name
		name = input_line.substr(1, 6);
		//start addr
		substr = input_line.substr(7, 6);
		prog_start_addr = stringHEX_to_DEC(substr);
		if (prog_start_addr!=0)    //absolute
		{
			is_relocation = false;
			reloca_offset = 0;
			MEM_start_addr = prog_start_addr;
		}
		else   //relocate
		{
			is_relocation = true;
			reloca_offset = 32*(rand()%30);
			MEM_start_addr = prog_start_addr + reloca_offset;
		}
		//prog length
		prog_length_str = input_line.substr(13, 6);
		prog_length = stringHEX_to_DEC(prog_length_str);
	}
	else
	{
		cout << "error H reocrd!!" << endl;
	}
}
void Loader::TMErecord_process()
{
	string substr;
	string temp;
	string result;
	int jump_addr;
	int sub;
	switch (input_line[0])
	{	
	case 'T':
		substr = input_line.substr(1, 6);
		TMErecord__start_addr = (stringHEX_to_DEC(substr)*2);
		for (int i = 0; i < input_line.length()-9; i++)
		{
			output[(TMErecord__start_addr- prog_start_addr*2)+i]=input_line[i+9];
		}
		break;
	case 'M':
		substr = input_line.substr(1, 6);
		TMErecord__start_addr = stringHEX_to_DEC(substr);//byte
		TMErecord__start_addr = TMErecord__start_addr * 2; //half byte    if odd==>+1 is index   if even==> is index
		substr = input_line.substr(7, 2);
		TMrecord__length = stringHEX_to_DEC(substr);//half byte
		temp.resize(TMrecord__length);
		result.resize(TMrecord__length);
		if (TMrecord__length%2==0)//even
		{
			for (int i = 0; i < TMrecord__length; i++)
			{
				temp[i] = output[TMErecord__start_addr + i];
			}
			jump_addr = stringHEX_to_DEC(temp);
			jump_addr = jump_addr + reloca_offset;
			result = int_to_stringHEX(jump_addr);
			sub = TMrecord__length - result.length();
			for (int i = 0; i < sub; i++)
			{
				result.insert(0, "0");
			}
			for (int i = 0; i < TMrecord__length; i++)
			{
				output[TMErecord__start_addr + i] = result[i];
			}
		}
		else  //odd
		{
			for (int i = 0; i < TMrecord__length; i++)
			{
				temp[i] = output[(TMErecord__start_addr+1) + i];
			}
			jump_addr = stringHEX_to_DEC(temp);
			jump_addr = jump_addr + reloca_offset;
			result = int_to_stringHEX(jump_addr);
			sub = TMrecord__length - result.length();
			for (int i = 0; i < sub; i++)
			{
				result.insert(0, "0");
			}
			for (int i = 0; i < TMrecord__length; i++)
			{
				output[(TMErecord__start_addr+1) + i] = result[i];
			}
		}
		break;
	case 'E':
		substr = input_line.substr(1, 6);
		TMErecord__start_addr = stringHEX_to_DEC(substr);
		Transfer_address = TMErecord__start_addr + reloca_offset;
		if (TMErecord__start_addr==prog_start_addr)
		{
			output_file();
		}
		else
		{
			cout << "transfer address error";
		}
		break;
	default:
		cout << "error record!" << endl;
		break;
	}
}

void Loader::output_file()
{
	ofstream ofile;
	ofile.open("DEVF2");
	int counter = 0;
	string buff1;
	int buff1_len;
	string buff2;
	int buff2_len;
	//I
	MEM_start_addr_str = int_to_stringHEX(MEM_start_addr);
	buff1_len = 6 - MEM_start_addr_str.length();	
	buff1.resize(buff1_len);
	for ( int i = 0; i < buff1_len; i++)
	{
		buff1[i] = '0';
	}
	Transfer_address_str = int_to_stringHEX(Transfer_address);
	buff2_len = 6 - Transfer_address_str.length();
	buff2.resize(buff2_len);
	for (int i = 0; i < buff2_len; i++)
	{
		buff2[i] = '0';
	}
	ofile << "I" << name <<buff1<< MEM_start_addr_str << prog_length_str <<buff2<< Transfer_address_str <<endl;
	//others
	for (int i = 0; i < output.size(); i++)
	{
		ofile << output[i];
		if (counter==63)
		{
			counter = (counter + 1) % 64;
			ofile << endl;
		}
		else
		{
			counter = counter + 1;
		}			
	}
	ofile.close();
}

int main() 
{
	srand(time(0));
	Loader loader;
	loader.read_input();
	return(0);
}
