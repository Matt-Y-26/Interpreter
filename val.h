#ifndef VAL_H
#define VAL_H

#include <string>
using namespace std;

class Val {
    int        i;
    string    s;
    enum ValType { ISINT, ISSTR, ISERR } vt;

public:
    Val() : i(0), vt(ISERR) {}
    Val(int i) : i(i), vt(ISINT) {}
    Val(string s) : i(0), s(s), vt(ISSTR) {}
    Val(ValType vt, string errmsg) : i(0), s(errmsg), vt(ISERR) {}

    ValType getVt() const { return vt; }

    bool isErr() const { return vt == ISERR; }
    bool isInt() const { return vt == ISINT; }
    bool isStr() const { return vt == ISSTR; }

    int ValInt() const {
        if( isInt() ) return i;
        throw "This Val is not an Int";
    }
    string ValString() const {
        if( isStr() ) return s;
        throw "This Val is not a Str";
    }

    friend ostream& operator<<(ostream& out, const Val& v) {
        // IMPLEMENT
        //gee this looks familiar
        //hmm prob will have to add error checks in future here...
        //out<<v.i;

        if(v.isInt())
        {
            out<<v.i; //is it .i or .v?
            //maybe its ValInt()
            //out<<v.ValInt(); is it this?
            //
            //out<<ValInt(); maybe this?

        }
        else if(v.isStr())
        {
            out<<v.s;
        }

        return out;
    }

    string GetErrMsg() const {
        if( isErr() ) return s;
        throw "This Val is not an Error";
    }

    Val operator+(const Val& op) const {
        if( isInt() && op.isInt() )
            return ValInt() + op.ValInt();
        if( isStr() && op.isStr() )
            return ValString() + op.ValString();
        return Val(ISERR, "Type mismatch on operands of +");
    }

    Val operator-(const Val& op) const {
        // IMPLEMENT
        //subtraction is the result of two integers ONLY INTEGERS!
        if( isInt() && op.isInt() )
        {
            return ValInt() - op.ValInt();
        }
        return Val(ISERR, "Type mismatch on operands of -");
    }

    Val operator*(const Val& op) const {
        // IMPLEMENT
        //multiplication is two ints OR
        if( isInt() && op.isInt() )
        {
            return ValInt() * op.ValInt();
        }
        //for string many times just do a for loop
        if( isInt() && op.isStr() )
        {
            if(ValInt()<0) //had <= before
            {
                return Val(ISERR, "Cannot multiply by a negative");
            }
            string out ="";
            int i = ValInt();
            while(i>0)
            {
                out +=  op.ValString(); //append it to out

                //do i need space?
                i--;
            }
            //return ValString() + op.ValString();

            return out;
        }
        if( isStr() && op.isInt() )
        {
            if(op.ValInt()<0)
            {
                return Val(ISERR, "Cannot multiply by a negative");
            }
            string out ="";
            int i = op.ValInt();
            while(i>0)
            {
                out +=  ValString(); //append it to out

                //do i need space?
                i--;
            }

            return out;
        }
        return Val(ISERR, "Type mismatch on operands of *");
    }

    Val operator/(const Val& op) const {
        // IMPLEMENT
        //HANDLE DIVIDE BY ZERO IN RUN TIME
        //so do I handle it here??

        if( isInt() && op.isInt() )
        {
            if(op.ValInt()==0)
            {
                return Val(ISERR, "You can't divide by 0");
            }
            return ValInt() / op.ValInt();
        }
        return Val(ISERR, "Type mismatch on operands of /");
    }

    Val operator!() const {
         // IMPLEMENT
         if(isInt())   //is this->isInt() valid
         {
            string out="";
            int temp=ValInt();
            while(temp!=0)
            {
                out += to_string((temp%10));//append last num
                temp=temp/10;//remove last num
                if(temp<0)
                {
                    temp=temp*-1;
                }
            }
            return stoi(out);
         }
         else if(isStr())
         {
            string out="";
            string temp=ValString();
            for(int i=temp.length()-1;i>=0;i--)  //>=-1
            {
                out+=temp[i];
            }
            //cout<<"out is currently: "<<out<<"END"<<endl;
            return out;
         }
         return Val(ISERR, "Type mismatch on operands of !");
    }

};

#endif