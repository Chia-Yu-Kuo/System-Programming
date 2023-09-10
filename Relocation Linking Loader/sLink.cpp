#include<iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <vector>


using namespace std;
class LinkLoader
{
private:
	string input_line;
	int index;
	int total_section;
	int MEM_start_addr;
	int prog_start_addr[100];
	int load_address[100];	//[0]main  [1]sub1 [2]sub2
	int relative_start_addr[100];
	int prog_length[100];
	int total_length;
	int Transfer_address;
	int TMErecord__start_addr;
	int TMrecord__length;
	bool is_relocation; 
	int reloca_offset;
	string result;
	//output
	string name[100];
	string MEM_start_addr_str;
	string prog_length_str;
	string Transfer_address_str;
	vector <char> Irecord;
	vector <char> output;

	//new
	vector<pair<string, int>> symbol_table;		//symbol table


public:
	//pass1
	void pass1();
	void main_Hrecord_process();
	void Hrecord_process();
	void HDRrecord_process();
	void Drecord_process();
	//void Rrecord_process();

	//pass2
	void pass2();
	int total_prog_length();
	void TMErecord_process();
	void main_Trecord_process();
	void Trecord_process();
	void Mrecord_process();
	void Erecord_process();
	void print_ESCTAB();
	

	int stringHEX_to_DEC(string hex);
	string int_to_stringHEX(int dec);
	void output_file();
};

int LinkLoader::stringHEX_to_DEC(string hex)
{
	//int length = hex.size();
	const char* str = hex.c_str();
	int num = (int)strtol(str, NULL, 16);
	return num;
}

string LinkLoader::int_to_stringHEX(int dec)
{
	std::ostringstream ss;
	ss << std::hex << dec;
	result = ss.str();
	return result;
}

void::LinkLoader::pass1() 
{
	int numof_prog;
	//read file name
	cout << "Please input the number of your Control sections! (include num of main program)"<<endl;
	cin >> numof_prog;

	cout << "Please input the file name! (the main program should be the first input)"<<endl;
	string file_name[100];
	for (int i = 0; i < numof_prog; i++)
	{
		cin >> file_name[i];
	}

	//open file
	ofstream omergefile;
	omergefile.open("input_merge.txt");
	ifstream isrcfile;
	char scan;
	for (string Temp_FileName : file_name)
	{
		isrcfile.open(Temp_FileName);
		while (isrcfile.get(scan))
		{
			omergefile << scan;
		}
		isrcfile.close();
	}
	omergefile.close();

	// get all input
	ifstream ifile_pass1;
	ifile_pass1.open("input_merge.txt");  


	//process mainH generalH D R

	getline(ifile_pass1, input_line);
	index = 0;
	main_Hrecord_process();	
	


	while (getline(ifile_pass1, input_line))
	{
		HDRrecord_process();
	}
	total_section = index;
	ifile_pass1.close();
}


void LinkLoader::pass2()
{
	ifstream ifile_pass2;
	ifile_pass2.open("input_merge.txt");

	index = 0;


	//initialize
	Irecord.resize(25);
	total_length = total_prog_length();
	output.resize(total_length*2);	//half byte
	for (int i = 0; i < output.size(); i++)
	{
		output[i] = 'F';
	}

	//T M E record
	while (getline(ifile_pass2, input_line))
	{
		TMErecord_process();
	}
	ifile_pass2.close();

	output_file();

}

int LinkLoader::total_prog_length() 
{
	int total=0;
	for (int i = 0; i < total_section; i++)
	{
		total += prog_length[i];
	}
	return total;
}

void LinkLoader::main_Hrecord_process()			//for main prog
{
	if (input_line[0]=='H')
	{
		string substr;
		//name
		name[0] = input_line.substr(1, 6);
		//start addr
		substr = input_line.substr(7, 6);
		prog_start_addr[0] = stringHEX_to_DEC(substr);
		if (prog_start_addr[0]!=0)    //absolute
		{
			is_relocation = false;
			reloca_offset = 0;
			MEM_start_addr = prog_start_addr[0];
		}
		else   //relocate
		{
			is_relocation = true;
			reloca_offset = 32*(rand()%30);
			MEM_start_addr = prog_start_addr[0] + reloca_offset;
		}
		//cout << MEM_start_addr << endl;


		//prog length
		prog_length_str = input_line.substr(13, 6);
		prog_length[0] = stringHEX_to_DEC(prog_length_str);

		//load addr
		load_address[0] = MEM_start_addr;
		load_address[1] = load_address[0] + prog_length[0];

		//relative
		relative_start_addr[0] = 0;

		symbol_table.push_back(make_pair(name[0], load_address[0]));
	}
	else
	{
		cout << "error H reocrd!!" << endl;
	}
}

void LinkLoader::Hrecord_process()		//for section prog
{
	if (input_line[0] == 'H')
	{
		string substr;
		//name
		name[index] = input_line.substr(1, 6);
		//start addr
		substr = input_line.substr(7, 6);
		prog_start_addr[index] = stringHEX_to_DEC(substr);
		if (prog_start_addr[index] != 0)
		{
			cout << "error section starting address!!" << endl;
		}
			
		//prog length
		prog_length_str = input_line.substr(13, 6);
		prog_length[index] = stringHEX_to_DEC(prog_length_str);

		//calculate load addr
		load_address[index + 1] = load_address[index] + prog_length[index];

		//relative
		relative_start_addr[index] = relative_start_addr[index-1]+prog_length[index -1];

		symbol_table.push_back(make_pair(name[index], load_address[index]));

	}
	else
	{
		cout << "error H reocrd!!" << endl;
	}
}

void LinkLoader::Drecord_process() 
{
	string substr1;
	string substr2;

	int counter = 1;
	while (counter != input_line.size())
	{
		substr1 = input_line.substr(counter, 6);
		substr2 = input_line.substr( (counter+6), 6);
		if (is_relocation)
		{
			symbol_table.push_back(make_pair(substr1, stringHEX_to_DEC(substr2) + load_address[index]));
		}
		else
		{
			symbol_table.push_back(make_pair(substr1, stringHEX_to_DEC(substr2)) );
		}
		counter += 12;
	}
}
/*
void LinkLoader::Rrecord_process()
{
}
*/
void LinkLoader::HDRrecord_process() 
{
	switch (input_line[0])
	{
	case 'H':
		Hrecord_process();
		break;
	case 'D':
		Drecord_process();
		break;
	case 'R':
		//Rrecord_process();
		break;
	case 'E':
		index++;
		break;
	default:		
		break;
	}
}

void LinkLoader::main_Trecord_process()
{
	string substr;
	substr = input_line.substr(1, 6);
	TMErecord__start_addr = (stringHEX_to_DEC(substr) * 2);
	for (int i = 0; i < input_line.length() - 9; i++)
	{
		output[ (TMErecord__start_addr - prog_start_addr[0] * 2) + i] = input_line[i + 9];		//avoid abs address
	}
}

void LinkLoader::Trecord_process()
{
	string substr;
	substr = input_line.substr(1, 6);
	int temp;
	TMErecord__start_addr = (stringHEX_to_DEC(substr) * 2);
	for (int i = 0; i < input_line.length() - 9; i++)
	{	
		temp = (relative_start_addr[index] * 2) + TMErecord__start_addr + i;
		output[temp] = input_line[i + 9];		//avoid abs address
		//output[  (prog_length[index-1]*2) + (TMErecord__start_addr - prog_start_addr[index] * 2) + i] = input_line[i + 9];		//avoid abs address
	}
}



void LinkLoader::Mrecord_process()		//have name or general symbol
{
	string substr;
	char add_sub;
	string temp;
	string result;
	int jump_addr;
	int sub;

	substr = input_line.substr(1, 6);
	if ((!is_relocation)&& index==0)		//
	{
		TMErecord__start_addr = stringHEX_to_DEC(substr) - load_address[0];//byte
	}
	else
	{
		TMErecord__start_addr = stringHEX_to_DEC(substr);//byte
	}

	TMErecord__start_addr = TMErecord__start_addr * 2; //half byte    if odd==>+1 is index   if even==> is index
	substr = input_line.substr(7, 2);
	TMrecord__length = stringHEX_to_DEC(substr);//half byte
	substr = input_line.substr(9, 1);
	add_sub = substr[0];
	substr = input_line.substr(10, 6);	//symbol name
	
	for (int i = 0; i < symbol_table.size(); i++)
	{
		if (symbol_table[i].first == substr)
		{
			reloca_offset = symbol_table[i].second;
		}
	}

	temp.resize(TMrecord__length);
	result.resize(TMrecord__length);

	if (TMrecord__length % 2 == 0)//even
	{
		for (int i = 0; i < TMrecord__length; i++)
		{
			temp[i] = output[TMErecord__start_addr +(relative_start_addr[index] *2)+ i];
		}
		jump_addr = stringHEX_to_DEC(temp);

		if (add_sub=='+')
		{
			jump_addr = jump_addr + reloca_offset;
		}
		else
		{
			jump_addr = jump_addr - reloca_offset;

		}
		result = int_to_stringHEX(jump_addr);
		sub = TMrecord__length - result.length();
		for (int i = 0; i < sub; i++)
		{
			result.insert(0, "0");
		}
		//cout << TMErecord__start_addr;
		for (int i = 0; i < TMrecord__length; i++)
		{
			output[TMErecord__start_addr+(relative_start_addr[index] * 2) + i] = result[i];
			//cout << output[(TMErecord__start_addr)+(relative_start_addr[index] * 2) + i];
		}
		//cout << endl;

	}
	else  //odd
	{
		for (int i = 0; i < TMrecord__length; i++)
		{
			temp[i] = output[(TMErecord__start_addr + (relative_start_addr[index] * 2) + 1) + i];
		}
		jump_addr = stringHEX_to_DEC(temp);

		if (add_sub == '+')
		{
			jump_addr = jump_addr + reloca_offset;
		}
		else
		{
			jump_addr = jump_addr - reloca_offset;

		}
		result = int_to_stringHEX(jump_addr);
		sub = TMrecord__length - result.length();
		for (int i = 0; i < sub; i++)
		{
			result.insert(0, "0");
		}
		//cout << TMErecord__start_addr+1;
		for (int i = 0; i < TMrecord__length; i++)
		{
			output[(TMErecord__start_addr + (relative_start_addr[index] * 2) + 1) + i] = result[i];
			//cout << output[(TMErecord__start_addr + (relative_start_addr[index] * 2) + 1) + i];
		}
		//cout << endl;

	}

	
}

void LinkLoader::Erecord_process() 
{
	string substr;
	if (input_line.size()!=1)
	{
		substr = input_line.substr(1, 6);
		TMErecord__start_addr = stringHEX_to_DEC(substr);
		Transfer_address = MEM_start_addr;
		if (TMErecord__start_addr != prog_start_addr[0])
		{
			cout << "transfer address error";
		}

	}
}


void LinkLoader::TMErecord_process()
{

	switch (input_line[0])
	{	
	case 'T':
		if (index==0)
		{
			main_Trecord_process();
		}
		else
		{
			Trecord_process();
		}
		break;
	case 'M':		
		Mrecord_process();			
		break;
	case 'E':
		Erecord_process();
		index++;
		break;
	default:
		break;
	}
}

void LinkLoader::output_file()
{
	ofstream ofile;
	ofile.open("DEVF2");
	int counter = 0;
	string buff1;
	int buff1_len;
	string buff2;
	int buff2_len;
	string buff3;
	int buff3_len;

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
	prog_length_str = int_to_stringHEX(total_length);
	buff3_len = 6 - prog_length_str.length();
	buff3.resize(buff3_len);
	for (int i = 0; i < buff3_len; i++)
	{
		buff3[i] = '0';
	}

	ofile << "I"<<name[0]  <<buff1<<MEM_start_addr_str  << buff3<<prog_length_str  <<buff2<< Transfer_address_str <<endl;

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

void LinkLoader::print_ESCTAB() 
{
	cout <<endl<<endl<< "ESCTAB is below!" << endl;
	for (int i = 0; i < symbol_table.size(); i++)
	{
		cout << symbol_table[i].first << "   :  " << symbol_table[i].second << endl;
	}
}


int main() 
{
	srand(time(0));

	LinkLoader linkloader;
	linkloader.pass1();
	linkloader.pass2();

	linkloader.print_ESCTAB();

	return(0);
}
