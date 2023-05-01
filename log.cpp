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
	log_file() {// 생성자
	}

	int log_file_write(char *in_data, int in_data_len);
	void log_file_read();
	void log_file_remove(char tmp1, char tmp2); //날짜가 다른날에 (날짜 변경)시 현재 날짜로 다시 생성해주기 위해 이전 파일 삭제를 위한 메소드 

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
	catch(exception e){ //오픈하다가 에러가 발생되는 경우
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

class lib_log : public log_file { //log 클래스는 log_file 클래스를 상속 받는다
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
	void log_get_dt(char *out_dt); //시간을 받아오는 함수
	void log_print(char in_type, const char* in_log_data, const char* in_src_file, const char* in_func, const int in_line_no); // 파라미터 정보들을 읽어와 출력
	void log_readToFile();
	char select_logType();

};

void lib_log::log_init_type() { //변수를 초기화
	memset(g_log_type, 0x00, sizeof(g_log_type));
	g_log_type_len = 0;
}

void lib_log::log_set_type(char *in_type, int in_type_len) {
	memcpy(g_log_type, in_type, in_type_len);
	g_log_type_len = in_type_len;

}

int lib_log::log_get_type(char *out_type) { //실제 데이터의 내용을 넣어준다.
	memcpy(out_type, g_log_type, g_log_type_len);
	return g_log_type_len;

}

void lib_log::log_init_option() { //option 초기화
	g_log_option = 0;

}

void lib_log::log_set_option(int in_option) { //option return 
	g_log_option = in_option;
}

int lib_log::log_get_option() { //저장한 option return
	return g_log_option;
}

void lib_log::log_get_dt(char * out_dt) {
	char buf[128] = { 0, };
	char dt[300] = { 0, };
	struct timeb tb;
	struct tm tstruct;

	ftime(&tb); //현재 시간을 읽어온다.
	localtime_s(&tstruct, &tb.time);
	sprintf_s(dt, "%04d%02d%02d%02d%02d%02d%d", tstruct.tm_year, tstruct.tm_mon + 1, tstruct.tm_mday, tstruct.tm_hour, tstruct.tm_min, tstruct.tm_sec, tb.millitm);
	memcpy(out_dt, dt, strlen(dt));

}


void lib_log::log_print(char in_type, const char* in_log_data, const char* in_src_file, const char* in_func, const int in_line_no) { //타입 , 로그로 넣을 데이터, 파일 이름, 함수 이름, 라인 넘버
	int cnt = 0, flag = 0, log_type_flag = 0, log_len =0 , log_type_len = 0;
	char log_type[30] = { 0, }; //0으로 다 초기화
	char log_data[2048] = { 0, }; //0으로 다 초기화
	char dt[30] = { 0, }; //0으로 다 초기화
	//string file_name = NULL;
	//char file_name[5] = { 'l', 'o', 'g', '\n' };

	//time 구하기
	log_get_dt(dt);

	//option확인
	flag = log_get_option();

	//type 확인
	log_type_len = log_get_type(log_type);
	for (cnt = 0; cnt < log_type_len; cnt++) {
		
		if (log_type[cnt] == in_type) {
			log_type_flag = 1; //있으면 이거 입력받아야함
			break;
		}
		else {
			log_type_flag = 0; //없으면
		}
	}

	if (log_type_flag == 0) {
		flag = 0;
	}


	//출력 및 파일 저장

	//file_name = "Log.txt";

	sprintf_s(log_data, "[%c]:%s:FILENAME : %s, FUNNAME : %s, LINENO : %d -> %s \r\n", in_type, dt, in_src_file, in_func, in_line_no, in_log_data);
	log_len = strlen(log_data);

	if ((flag == 1) | (flag == 3)) {
		printf("%s", log_data);
	}

	if ((flag == 3) | (flag == 2)) {
	
		log_file_remove(dt[6], dt[7]); // 파일에 쓰려고 할때 날짜가 다르다면 삭제하고 다시 생성하기 위한 메소드이다.
		log_file_write(log_data, log_len);
	}

}

void lib_log::log_readToFile() {
	
	log_file_read();
}

char lib_log::select_logType() {

	int select;
	char tmp;
	cout << "어떤 타입의 로그를 선택 하시겠습니까 ? " << endl;
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
		cout << "잘못 입력 하셨습니다." << endl;
		return 'f';
	}

}

void lib_log::control() {

	int tmp = 0;
	char *input;
	char buffer[50];
	
	
	while(tmp != 4){

		// 컨트롤을 담당하는 메소드
		cout << "1. 화면에 출력 2. 파일에 저장 3. 둘 다 4. 파일 내용 출력하기 5. 종료 : ";
		cin >> tmp;

		if (tmp == 1) {
			//화면에 출력하는 기능 구현
			char log_type[30] = { 0, };
			lib_log *log = new lib_log();
			log->log_set_option(1);

			log_type[0] = select_logType();
		
			if (log_type[0] == 'f') {
				return;
			}

			cout << "로그 내용을 입력하시오 : ";
			input = buffer;
			cin >> input;

			log->log_set_type(log_type, 1); //하나만 선택했으므로
			log->log_print(log_type[0] , input, __FILE__, __FUNCTION__, __LINE__);

		}
		else if (tmp == 2) {
			//파일에 저장하는 기능 구현
			char log_type[30] = { 0, };
			lib_log *log = new lib_log();
			log->log_set_option(2);

			log_type[0] = select_logType();

			if (log_type[0] == 'f') {
				return;
			}

			cout << "로그 내용을 입력하시오 : ";
			input = buffer;
			cin >> input;

			log->log_set_type(log_type, 1); //하나만 선택했으므로
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

			cout << "로그 내용을 입력하시오 : ";
			input = buffer;
			cin >> input;

			log->log_set_type(log_type, 1); //하나만 선택했으므로
			log->log_print(log_type[0], input, __FILE__, __FUNCTION__, __LINE__);

		}
		else if (tmp == 4) {
			
			log_readToFile();

		}
		else{
			cout << "올바른 숫자가 아닙니다. "<< endl;
			return ;
		}

	}

	cout << "프로그램을 종료 합니다." << endl;
	
}


int main() {

	int tmp = 0;
	char *input;
	char buffer[50];

	char log_type[30] = { 0, };
	lib_log *log = new lib_log();


	while (tmp != 5) {

		// 컨트롤을 담당하는 메소드
		cout << "1. 화면에 출력 2. 파일에 저장 3. 둘 다 4. 파일 내용 출력하기 5. 종료 : ";
		cin >> tmp;
		cout << "\n" << endl;

		if (tmp == 1) {
			//화면에 출력하는 기능 구현
			
			log->log_set_option(1);

			log_type[0] = log->select_logType();

			if (log_type[0] == 'f') {
				return 0;
			}

			cout << "로그 내용을 입력하시오 : ";
			input = buffer;
			cin >> input;
			cout << "\n" << endl;

			log->log_set_type(log_type, 1); //하나만 선택했으므로
			log->log_print(log_type[0], input, __FILE__, __FUNCTION__, __LINE__);

		}
		else if (tmp == 2) {
			//파일에 저장하는 기능 구현
			char log_type[30] = { 0, };
			lib_log *log = new lib_log();
			log->log_set_option(2);

			log_type[0] = log->select_logType();

			if (log_type[0] == 'f') {
				return 0;
			}

			cout << "로그 내용을 입력하시오 : ";
			input = buffer;
			cin >> input;
			cout << "\n" << endl;

			log->log_set_type(log_type, 1); //하나만 선택했으므로
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

			cout << "로그 내용을 입력하시오 : ";
			input = buffer;
			cin >> input;
			cout << "\n" << endl;

			log->log_set_type(log_type, 1); //하나만 선택했으므로
			log->log_print(log_type[0], input, __FILE__, __FUNCTION__, __LINE__);

		}
		else if (tmp == 4) {
			log->log_readToFile();
		}
		else {
			cout << "프로그램을 종료 합니다." << endl;
			return 0;
		}

	}

	cout << "프로그램을 종료 합니다." << endl;
}






