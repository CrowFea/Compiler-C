#pragma once
#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include<iostream>
#include<fstream>
#include<string>
#include<set>
#include<vector>
#include "utils.h"

using namespace std;

class LexicalAnalyzer {
private:
    //���������
    set<string> OPERATORS = { "+", "-", "*", "/", "=", "==", ">", ">=", "<", "<=", "!="};

    //�ؼ���
    set<string> KEYWORDS = { "if", "else", "return", "while"};

    //��������
    set<string> TYPES = {"int", "void", "char", "float"};

    //ǰ�������
    set<string> PRE_OPERATORS = { "+", "-"};

    //���
    set<char> BORDERS = { '(', ')', '{', '}', ',', ';'};

    ifstream code_reader_;
    ofstream lexical_analyser_printer_;

    unsigned int line_counter_;//���ڴʷ�������������ʱ��������λ
    bool print_detail_;//ѡ���Ƿ񽫴ʷ�������������txt��
    unsigned int step_counter_;//�ʷ������Ʋ���
    bool IsDot(const unsigned char ch);					  //�Ƿ�ΪС����
    bool IsLetter(const unsigned char ch);//�Ƿ���ĸ
    bool IsDigital(const unsigned char ch);//�Ƿ�����
    bool IsSingleCharOperator(const unsigned char ch);//�Ƿ񵥷��������
    bool IsDoubleCharOperatorPre(const unsigned char ch);//�Ƿ�˫���������
    bool IsBlank(const unsigned char ch);//�Ƿ��ǿհ׷�
    unsigned char GetNextChar();//��ȡ�ַ����е���һ���ַ���ͬʱ����������
    vector<unsigned char> vec1;//˫������
    vector<unsigned char> vec2;
    vector<unsigned char> now;
    int nextBuffer=1;//��һ��Ӧ��ʹ�õĻ�����
    int bufferPlace = 0;
    int bufferSum = 0;//��ʾ��ǰ�����������ַ���
    bool wetherBack = false;
    bool fileEnd = false;//��ʾ�����ļ�ĩβ
    void PrintDetail(WordInfo word);//��ӡ�ʷ�������Ϣ
    WordInfo GetBasicWord();//��ȡ�ķ�����,�����޷����д�ӡ

public:
    LexicalAnalyzer();
    ~LexicalAnalyzer();
    bool IsReadyToAnalyze(bool show_detail, const string code_filename);//�ļ��Ƿ����
    WordInfo GetWord();//��ȡ�ķ����ţ��ɽ��д�ӡ��Ϣ��
    void packBuffer();
    unsigned char getBufferChar();
    void returnBufferPlace();
};


#endif