/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
#include "val.h"
using std::vector;
using std::map;

// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE };





// a "forward declaration" for a class to hold values
class Value;



class ParseTree {
    public:
	int			linenum;
    ParseTree	*left;
    ParseTree	*right;

    //map<string,Val> symbols;



public:
	ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
		: linenum(linenum), left(l), right(r) {}

	virtual ~ParseTree() {
		delete left;
		delete right;
	}
	//virtual Val Eval(map<string,Val>& symbols) const = 0;

    //map<string,Val> symbols;
	int GetLineNumber() const { return linenum; }
    /*
    ParseTree getright()
    {
        return *right;
    }
    ParseTree getleft()
    {
        return *left;
    }
    */

	int Traverse(int input, int (ParseTree::*func)(void)) {
		if( left ) input += left->Traverse(input, func);
		if( right ) input += right->Traverse(input, func);
		return Visit(input, func);
	}

	virtual int Visit(int input, int (ParseTree::*func)(void)) {
		return input;
	}

	int MaxDepth() const {
        int lc = 0, rc = 0;
		if( left ) lc = left->MaxDepth();
		if( right ) rc += right->MaxDepth();
		if( lc > rc ) return lc + 1;
        return rc + 1;
	}

	void runtime_err(int line, string msg) const //uhh const here works (question mark)
    {
        string throwit = to_string(line)+": "+msg; //itoa doesnt work but to_string does
        throw(throwit);
        exit(1); //looked up, 0 means good 1 means bad so uh runtime error... bad
    }

	virtual bool IsIdent() const { return false; }
	virtual bool IsVar() const { return false; }
	virtual string GetId() const { return ""; }
    virtual int IsBang() const { return 0; }
    virtual bool IsLet() const { return false; }
    virtual Val Eval(map<string,Val>& symbols) const = 0;

	int BangCount() const {
		int cnt = 0;
		if( left ) cnt += left->BangCount();
		if( right ) cnt += right->BangCount();
		cnt += IsBang();
		return cnt;
	}

	int CheckLetBeforeUse(map<string,bool>& var) {
        int cnt = 0;
		if( left ) cnt += left->CheckLetBeforeUse(var);
		if( right ) cnt += right->CheckLetBeforeUse(var);
		if( IsLet() )
			var[ this->GetId() ] = true;
        if( IsIdent() && var.find(GetId()) == var.end() ) {
            cout << "UNDECLARED VARIABLE " << GetId() << endl;
            cnt++;
        }
        return cnt;

	}

	//virtual Value Eval();
};
class StmtList : public ParseTree {

public:
	StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}

    //eval
    //Val Eval(map<string,Val>)& symbols){
    //virtual Val Eval(map<string,Val>& symbols) const override {
     Val Eval(map<string,Val>& symbols) const override{ //DOES THIS WORK??

    left->Eval(symbols);
    if(right)
    {
        right->Eval(symbols);
    }
    return Val();
    }

	//this was here before uhhh idk what
//	int Visit(int input, int (ParseTree::*func)(void)) {
//		for( auto s : statements ) {
//			input += s->Visit(input, func);
//		}
//		return input;
//	}
};

class Let : public ParseTree {
	string id;

public:
	Let(Lex& t, ParseTree *e) : ParseTree(t.GetLinenum(), e), id(t.GetLexeme()) {}

	string GetId() const { return id; }
	bool IsLet() const { return true; }
	Val Eval(map<string,Val>& symbols) const override{

        //LEFT->VALUE THIS->ID : |
        //cout<<"IM IN LET"<<endl;
        auto vals =left->Eval(symbols);
        string id = this->GetId();
        if(symbols.find(id)==symbols.end()){
            //was not in map, put in map
            symbols.insert({id,vals});
        }
        else
        {
            //oh it exists, replace the value
            symbols[id]=vals;
        }


         return Val();



    }
};

class Print : public ParseTree {
public:
	Print(int line, ParseTree *e) : ParseTree(line, e) {}
	Val Eval(map<string,Val>& symbols) const override{
	    //alrighty print
        auto answer =left->Eval(symbols);
        //will there be a right? i dont think there will be but just do this incase
        if(right)
        {
            //cout<<"Theres a right"<<endl;  this if stm is useless
            auto vals =right->Eval(symbols);
        }
        //hold up check if its a id
        if(answer.isStr())
        {
            //ok its string check to see if its in the map
            string id = answer.ValString();
             if(symbols.find(id)==symbols.end()){
                //was not in the map side note shouldnt
                //i just do != instead ???
             }
             else
             {

                 answer=symbols[id];
                 //cout<<"ANSWER IS "<<answer<<endl;
             }

        }


        cout<<answer;
        return answer; //should I return val or answer? uhh ive been returning val previously
        //changed to answer since other expr have it, change back if problem erupts here
    }
};

class Loop : public ParseTree {

public:
	Loop(int line, ParseTree *e, ParseTree *s) : ParseTree(line, e, s) {}
	Val Eval(map<string,Val>& symbols) const override{
        //uh ok
        //Loop is expr must evulate to an integer
        //IF THE integer is non zero, then Slist is executed until it is zero
        //ok i think LEFT is expression and RIGHT is Slist
        auto exp =left->Eval(symbols); //EXPRESSION MUST evulate an int
        if(!exp.isInt()){ //RUN TIME ERROR EXPR MUST EVALUATE INT
            runtime_err(linenum, "Expression is not an integer");
        }
        int expi= exp.ValInt(); //should be a int
        while(expi!=0) //made it a while
        {
            //evulate the expression
            right->Eval(symbols);
            auto renew_exp = left->Eval(symbols);
            expi= renew_exp.ValInt(); //ok this should 'renew' the expi value so it keeps on checking
        }
        //what should I return? i guess Val() im not really doing anything else like no answer
        //should be returned... i think : |
        return Val();
    }
};

class If : public ParseTree {

public:
	If(int line, ParseTree *e, ParseTree *s) : ParseTree(line, e, s) {}
	Val Eval(map<string,Val>& symbols) const override{
        //okie so an IF is an
        //EXPR followed by a begin than a Slist and then a END
        //uhhhh I guess I'll 'ignore' the begin and end here and focus on expr
        //unlike loop if will occur only once
        //run time here for non ints do like throw("not int"); or whatever
        auto exp =left->Eval(symbols);
        if(!exp.isInt()){ //RUN TIME ERROR EXPR MUST EVALUATE INT
            runtime_err(linenum, "Expression is not an integer");
        }
        //auto sls = right->Eval(symbols); //Should I call this here?
        int expi= exp.ValInt(); //should be a int
        if(expi!=0) //if its a non-zero
        {
            right->Eval(symbols); //evalute an expr
        }

        return Val(); //uh do i return value?
    }
};


class PlusExpr : public ParseTree {
public:
	PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
	//GIVEN CODE FROM EVALNOTES
Val Eval(map<string,Val>& symbols) const override {
        auto L = left->Eval(symbols);
        if( L.isErr() ) {
            runtime_err(linenum, L.GetErrMsg());
            }
        auto R = right->Eval(symbols);
        if( R.isErr() ) {
            runtime_err(linenum, R.GetErrMsg());
            //R.GetErrMsg();
            }


        auto answer = L + R;
        if( answer.isErr() ) {
            runtime_err(linenum, answer.GetErrMsg());
        }
        return answer;
    }
};

class MinusExpr : public ParseTree {
public:
	MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

	Val Eval(map<string,Val>& symbols) const override {
        auto L = left->Eval(symbols);
        if( L.isErr() ) {
            runtime_err(linenum, L.GetErrMsg());
        }
        auto R = right->Eval(symbols);
        if( R.isErr() ) {
            runtime_err(linenum, R.GetErrMsg());
        }
        auto answer = L - R;
        if( answer.isErr() ) {
            runtime_err(linenum, answer.GetErrMsg());
        }
        return answer;
    }
};

class TimesExpr : public ParseTree {
public:
	TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

	Val Eval(map<string,Val>& symbols) const override {
        auto L = left->Eval(symbols);
        if( L.isErr() ) {
            runtime_err(linenum, L.GetErrMsg());
        }
        auto R = right->Eval(symbols);
        if( R.isErr() ) {
            runtime_err(linenum, R.GetErrMsg());
        }

        if(R.isStr() || L.isStr()) //is one of a them a string?
        {
            //make sure one is an int
            if(R.isInt())
            {
                if(R.ValInt()<0) //cant multi a negative
                {
                     runtime_err(linenum, "Cannot multiply string by negative int");
                }

            }
            else if(L.isInt())
            {
               if(L.ValInt()<0)
               {
                   runtime_err(linenum, "Cannot multiply string by negative int");
               }
            }
            else{
                //cout<<"Something went very wrong 2 strs multi"<<endl;
            }
        }
        auto answer = L * R;
        if( answer.isErr() ) {
            runtime_err(linenum, answer.GetErrMsg());
        }
        return answer;
    }
};

class DivideExpr : public ParseTree {
public:
	DivideExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

	Val Eval(map<string,Val>& symbols) const override {
        auto L = left->Eval(symbols);
        if( L.isErr() ) {
            runtime_err(linenum, L.GetErrMsg());
        }
        auto R = right->Eval(symbols);
        if( R.isErr() ) {
            runtime_err(linenum, R.GetErrMsg());
        }
        if(R.ValInt()==0)
        {
            runtime_err(linenum, "Divide by zero error");
        }
        auto answer = L / R;
        if( answer.isErr() ) {}
            //runtime_err(linenum, answer.GetErrMsg());

        return answer;
    }
};

class BangExpr : public ParseTree {
public:
	BangExpr(int line, ParseTree *l) : ParseTree(line,l) {}
    virtual int IsBang() const { return 1; }
    Val Eval(map<string,Val>& symbols) const override {
        auto L = left->Eval(symbols);
        if( L.isErr() ) {}
            //runtime_err(linenum, L.GetErrMsg());

        if(right)
        {
            auto R = right->Eval(symbols);
            // should not occur
        }
        /*
        auto R = right->Eval(symbols);
        if( R.isErr() ) {}
            //runtime_err(linenum, R.GetErrMsg());
        */

        auto answer = !L; //this will work, right?
        if( answer.isErr() ) {}
            //runtime_err(linenum, answer.GetErrMsg());

        return answer;
    }
};

class IConst : public ParseTree {
	int val;

public:
	IConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}
	Val Eval(map<string,Val>& symbols) const override { //const override somehow fixes it
        return Val(val);
    }
};

class SConst : public ParseTree {
	string val;

public:
	SConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = t.GetLexeme();
	}
	Val Eval(map<string,Val>& symbols) const override { //const override somehow fixes it
        return Val(val);
    }
};

class Ident : public ParseTree {
	string id;

public:
	Ident(Lex& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {}

	bool IsIdent() const { return true; }
	string GetId() const { return id; }
	Val Eval(map<string,Val>& symbols) const override { //const override somehow fixes it

        //should I check if it exists? ...
        if(symbols.find(id)==symbols.end()){
            cout<<"couldnt find it"<<endl;
        }
        else{ //found it
            //ok so its something else
            //return val but with ew id
            //cout<<"id is "<<id<<endl;
            //cout<<"value is "<<symbols[id]<<endl;
        }
        //^ this is useless
        //return Val(id); //ok in its current state it just returns the id like
        return Val(symbols[id]);
    }
};

#endif /* PARSETREE_H_ */
