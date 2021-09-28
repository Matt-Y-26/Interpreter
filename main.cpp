/*
 * main.cpp
 */

#include "lex.h"
#include "parse.h"
#include <fstream>
#include "val.h"
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;

int
main(int argc, char *argv[])
{
    //declaring here didnt work
    map<string,Val> symbols;
    //virtual Val Eval(map<string,Val>& symbols) const = 0;
	ifstream file;
	istream *in;
	int linenum = 0;

	if( argc == 1 ) {
		in = &cin;
	}

	else if( argc == 2 ) {

	    //for future errors might have to do something ' - '
		file.open(argv[1]);
		if( file.is_open() == false ) {
			cout << "COULD NOT OPEN " << argv[1] << endl;
			return 1;
		}
		in = &file;
	}

	else {
		cout << "TOO MANY FILENAMES" << endl;
		return 1;
	}

	ParseTree *prog = Prog(*in, linenum);

	if( prog == 0 )
		return 0;

    map<string,bool> declaredId;
    int cnt = prog->CheckLetBeforeUse(declaredId);
    //^ if cnt gets returned theres an undeclared id
	//cout << "BANG COUNT: " << prog->BangCount() << endl;
	//cout << "MAX DEPTH: " << prog->MaxDepth() << endl;

	//do try catch
	if(cnt==0){ //if cnt is 0, it means no undeclaredID so you're good to go

	try{
        prog->Eval(symbols);
	}
	catch (std::string& e)
	{
	    cout << "RUNTIME ERROR at " << e <<endl;
	}

    }
	return 0;
}
