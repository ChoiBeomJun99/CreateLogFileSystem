#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <fstream>
#include <sstream>
#include <cassert>

using namespace std;

class log_file {

public:
	log_file() {// ������
	}

	int log_file_write(char *in_data, int in_data_len);
	void log_file_read();
	void log_file_remove(char tmp1, char tmp2); //��¥�� �ٸ����� (��¥ ����)�� ���� ��¥�� �ٽ� �������ֱ� ���� ���� ���� ������ ���� �޼ҵ� 

};

int log_file::log_file_write(char *in_data, int in_data_len) {
	

	try {
		
		
		ofstream outStrm;
		outStrm.open("Log.txt", ios::out | ios::app);
		
		if (!outStrm.is_open()) {
			cout << "File cannot be opened!";
			assert(false);
		}

		outStrm << in_data;

		outStrm.close();
		
		/*
		ofstream fd(in_path, ios_base::out | ios_base::app);
		fd.write(in_data, in_data_len);
		fd.close();
		*/
		
		

		return in_data_len;
	}
	catch(exception e){ //�����ϴٰ� ������ �߻��Ǵ� ���
		return -1;
	}
}

void log_file::log_file_read() {

	ifstream inStrm;
	inStrm.open("Log.txt", ios::in);

	if (!inStrm.is_open()) {
		cout << "File cannot be opened!";
		assert(false);
	}
	char ch;

	while (inStrm.get(ch)) {
		cout.put(ch);
	}

	inStrm.close();
	
	return;

}

void log_file::log_file_remove(char tmp1, char tmp2) {

	ifstream inStrm;
	inStrm.open("Log.txt", ios::in);

	if (!inStrm.is_open()) {
		cout << "File cannot be opened!";
		assert(false);
	}

	char date[30] = { 0, };

	for (int i = 0; i <30; i++) {
		inStrm >> date[i];
	}

	if ((date[10] != tmp1) || (date[11] != tmp2)) {

		int result = remove("Log.txt");


		inStrm.close();
		return;
		
	}

	inStrm.close();
	return;
}

class lib_log : public log_file { //log Ŭ������ log_file Ŭ������ ��� �޴´�
private:
	int g_log_option;
	int g_log_type_len;
	char g_log_type[10];

public:

	lib_log() {
		log_init_type();
		log_init_option();
	}

	void control();
	void log_init_type();
	void log_set_type(char *in_type, int in_type_len);
	int log_get_type(char *out_type);
	void log_init_option();
	void log_set_option(int in_option);
	int log_get_option();
	void log_get_dt(char *out_dt); //�ð��� �޾ƿ��� �Լ�
	void log_print(char in_type, const char* in_log_data, const char* in_src_file, const char* in_func, const int in_line_no); // �Ķ���� �������� �о�� ���
	void log_readToFile();
	char select_logType();

};

void lib_log::log_init_type() { //������ �ʱ�ȭ
	memset(g_log_type, 0x00, sizeof(g_log_type));
	g_log_type_len = 0;
}

void lib_log::log_set_type(char *in_type, int in_type_len) {
	memcpy(g_log_type, in_type, in_type_len);
	g_log_type_len = in_type_len;

}

int lib_log::log_get_type(char *out_type) { //���� �������� ������ �־��ش�.
	memcpy(out_type, g_log_type, g_log_type_len);
	return g_log_type_len;

}

void lib_log::log_init_option() { //option �ʱ�ȭ
	g_log_option = 0;

}

void lib_log::log_set_option(int in_option) { //option return 
	g_log_option = in_option;
}

int lib_log::log_get_option() { //������ option return
	return g_log_option;
}

void lib_log::log_get_dt(char * out_dt) {
	char buf[128] = { 0, };
	char dt[300] = { 0, };
	struct timeb tb;
	struct tm tstruct;

	ftime(&tb); //���� �ð��� �о�´�.
	localtime_s(&tstruct, &tb.time);
	sprintf_s(dt, "%04d%02d%02d%02d%02d%02d%d", tstruct.tm_year, tstruct.tm_mon + 1, tstruct.tm_mday, tstruct.tm_hour, tstruct.tm_min, tstruct.tm_sec, tb.millitm);
	memcpy(out_dt, dt, strlen(dt));

}


void lib_log::log_print(char in_type, const char* in_log_data, const char* in_src_file, const char* in_func, const int in_line_no) { //Ÿ�� , �α׷� ���� ������, ���� �̸�, �Լ� �̸�, ���� �ѹ�
	int cnt = 0, flag = 0, log_type_flag = 0, log_len =0 , log_type_len = 0;
	char log_type[30] = { 0, }; //0���� �� �ʱ�ȭ
	char log_data[2048] = { 0, }; //0���� �� �ʱ�ȭ
	char dt[30] = { 0, }; //0���� �� �ʱ�ȭ
	//string file_name = NULL;
	//char file_name[5] = { 'l', 'o', 'g', '\n' };

	//time ���ϱ�
	log_get_dt(dt);

	//optionȮ��
	flag = log_get_option();

	//type Ȯ��
	log_type_len = log_get_type(log_type);
	for (cnt = 0; cnt < log_type_len; cnt++) {
		
		if (log_type[cnt] == in_type) {
			log_type_flag = 1; //������ �̰� �Է¹޾ƾ���
			break;
		}
		else {
			log_type_flag = 0; //������
		}
	}

	if (log_type_flag == 0) {
		flag = 0;
	}


	//��� �� ���� ����

	//file_name = "Log.txt";

	sprintf_s(log_data, "[%c]:%s:FILENAME : %s, FUNNAME : %s, LINENO : %d -> %s \r\n", in_type, dt, in_src_file, in_func, in_line_no, in_log_data);
	log_len = strlen(log_data);

	if ((flag == 1) | (flag == 3)) {
		printf("%s", log_data);
	}

	if ((flag == 3) | (flag == 2)) {
	
		log_file_remove(dt[6], dt[7]); // ���Ͽ� ������ �Ҷ� ��¥�� �ٸ��ٸ� �����ϰ� �ٽ� �����ϱ� ���� �޼ҵ��̴�.
		log_file_write(log_data, log_len);
	}

}

void lib_log::log_readToFile() {
	
	log_file_read();
}

char lib_log::select_logType() {

	int select;
	char tmp;
	cout << "� Ÿ���� �α׸� ���� �Ͻðڽ��ϱ� ? " << endl;
	cout << " 1.Error(E) 2.Warning(W) 3.Event(e) 4.Debug(D) : ";
	cin >> select;
	cout << "\n" << endl;

	switch (select) {
	
	case 1:
		tmp = 'E';
		return tmp;
		break;
	case 2:
		tmp = 'W';
		return tmp;
		break;
	case 3:
		tmp = 'e';
		return tmp;
		break;
	case 4:
		tmp = 'D';
		return tmp;
		break;
	default:
		cout << "�߸� �Է� �ϼ̽��ϴ�." << endl;
		return 'f';
	}

}

void lib_log::control() {

	int tmp = 0;
	char *input;
	char buffer[50];
	
	
	while(tmp != 4){

		// ��Ʈ���� ����ϴ� �޼ҵ�
		cout << "1. ȭ�鿡 ��� 2. ���Ͽ� ���� 3. �� �� 4. ���� ���� ����ϱ� 5. ���� : ";
		cin >> tmp;

		if (tmp == 1) {
			//ȭ�鿡 ����ϴ� ��� ����
			char log_type[30] = { 0, };
			lib_log *log = new lib_log();
			log->log_set_option(1);

			log_type[0] = select_logType();
		
			if (log_type[0] == 'f') {
				return;
			}

			cout << "�α� ������ �Է��Ͻÿ� : ";
			input = buffer;
			cin >> input;

			log->log_set_type(log_type, 1); //�ϳ��� ���������Ƿ�
			log->log_print(log_type[0] , input, __FILE__, __FUNCTION__, __LINE__);

		}
		else if (tmp == 2) {
			//���Ͽ� �����ϴ� ��� ����
			char log_type[30] = { 0, };
			lib_log *log = new lib_log();
			log->log_set_option(2);

			log_type[0] = select_logType();

			if (log_type[0] == 'f') {
				return;
			}

			cout << "�α� ������ �Է��Ͻÿ� : ";
			input = buffer;
			cin >> input;

			log->log_set_type(log_type, 1); //�ϳ��� ���������Ƿ�
			log->log_print(log_type[0], input, __FILE__, __FUNCTION__, __LINE__);

		}
		else if (tmp == 3) {
			char log_type[30] = { 0, };
			lib_log *log = new lib_log();
			log->log_set_option(3);

			log_type[0] = select_logType();

			if (log_type[0] == 'f') {
				return;
			}

			cout << "�α� ������ �Է��Ͻÿ� : ";
			input = buffer;
			cin >> input;

			log->log_set_type(log_type, 1); //�ϳ��� ���������Ƿ�
			log->log_print(log_type[0], input, __FILE__, __FUNCTION__, __LINE__);

		}
		else if (tmp == 4) {
			
			log_readToFile();

		}
		else{
			cout << "�ùٸ� ���ڰ� �ƴմϴ�. "<< endl;
			return ;
		}

	}

	cout << "���α׷��� ���� �մϴ�." << endl;
	
}


int main() {

	int tmp = 0;
	char *input;
	char buffer[50];

	char log_type[30] = { 0, };
	lib_log *log = new lib_log();


	while (tmp != 5) {

		// ��Ʈ���� ����ϴ� �޼ҵ�
		cout << "1. ȭ�鿡 ��� 2. ���Ͽ� ���� 3. �� �� 4. ���� ���� ����ϱ� 5. ���� : ";
		cin >> tmp;
		cout << "\n" << endl;

		if (tmp == 1) {
			//ȭ�鿡 ����ϴ� ��� ����
			
			log->log_set_option(1);

			log_type[0] = log->select_logType();

			if (log_type[0] == 'f') {
				return 0;
			}

			cout << "�α� ������ �Է��Ͻÿ� : ";
			input = buffer;
			cin >> input;
			cout << "\n" << endl;

			log->log_set_type(log_type, 1); //�ϳ��� ���������Ƿ�
			log->log_print(log_type[0], input, __FILE__, __FUNCTION__, __LINE__);

		}
		else if (tmp == 2) {
			//���Ͽ� �����ϴ� ��� ����
			char log_type[30] = { 0, };
			lib_log *log = new lib_log();
			log->log_set_option(2);

			log_type[0] = log->select_logType();

			if (log_type[0] == 'f') {
				return 0;
			}

			cout << "�α� ������ �Է��Ͻÿ� : ";
			input = buffer;
			cin >> input;
			cout << "\n" << endl;

			log->log_set_type(log_type, 1); //�ϳ��� ���������Ƿ�
			log->log_print(log_type[0], input, __FILE__, __FUNCTION__, __LINE__);

		}
		else if (tmp == 3) {
			char log_type[30] = { 0, };
			lib_log *log = new lib_log();
			log->log_set_option(3);

			log_type[0] = log->select_logType();

			if (log_type[0] == 'f') {
				return 0;
			}

			cout << "�α� ������ �Է��Ͻÿ� : ";
			input = buffer;
			cin >> input;
			cout << "\n" << endl;

			log->log_set_type(log_type, 1); //�ϳ��� ���������Ƿ�
			log->log_print(log_type[0], input, __FILE__, __FUNCTION__, __LINE__);

		}
		else if (tmp == 4) {
			log->log_readToFile();
		}
		else {
			cout << "���α׷��� ���� �մϴ�." << endl;
			return 0;
		}

	}

	cout << "���α׷��� ���� �մϴ�." << endl;
}






