#include <iostream>
#include <fstream>
#include "lexical_analyzer.h"
#include "utils.h"

using namespace std;

int main(int argc,char* argv[]) {

	//std::cout <<argc << std::endl;//��������ĸ���
	//for (int i = 0; i < argc; i++) {
	//	std::cout <<i<< argv[i] << std::endl;//����������
	//}

	/*���Ե�ʱ��ע�͵�
    string code_filename=argv[1];
	cout << code_filename << endl;
	*/
	char c = '1';
	string code_filename = "test.cpp";
    LexicalAnalyzer lexcial_analyzer;
    if (!lexcial_analyzer.IsReadyToAnalyze(true, code_filename))
        return false;
	lexcial_analyzer.packBuffer();
    while (true) {
        WordInfo get_word = lexcial_analyzer.GetWord();
		if (get_word.type == LEOF)	break;
    }
	system("pause");
	return 0;
}