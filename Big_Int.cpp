#include<bits/stdc++.h>
using namespace std;
class BIG_INT{
    string digits;
public:

    // Constructors
    BIG_INT(){
        digits = "0"; // Default Value;
    }
    BIG_INT(string d){
        for(char c: d){
            if(isdigit(c)) digits.push_back(c);
            else{
                cout<< "INVALID NUMBER"<< endl;
                break;
            }
        }
    }
    BIG_INT(const char *d){
        int ss = sizeof(d) - 1;
        for(int i = 0; i<=ss; i++){
            if(isdigit(d[i])) digits.push_back(d[i]);
            else{
                cout<< "INVALID NUMBER"<< endl;
                break;
            }
        }
    }
    BIG_INT(unsigned long long int k){
        if(k==0) digits.push_back('0');
        while(k!=0){
            char c = (k%10 + '0');
            digits = c + digits;
            k = k/10;
        }   
    }
    BIG_INT(BIG_INT &b){
        this->digits = b.digits;
    }

    // Print Function
    void prt(){
        cout<<digits<<endl;
    }
    
    // Length Function
    int Length(){
        return digits.size();
    }

    // Is_Zero Function
    bool Is_Zero(){
        return digits=="0";
    }

    friend bool operator==(BIG_INT &a, BIG_INT &b);
    friend bool operator>(BIG_INT &a, BIG_INT &b);
    friend BIG_INT operator+(BIG_INT &a, BIG_INT &b);
    friend BIG_INT operator-(BIG_INT &a, BIG_INT &b);
    friend BIG_INT operator*(BIG_INT &a, BIG_INT &b);
    friend void div_by_2(BIG_INT &a);
    friend BIG_INT operator/(BIG_INT &a, BIG_INT &b);

};

// Relational Operators
bool operator==(BIG_INT &a, BIG_INT &b){
    return a.digits == b.digits;
}
bool operator>(BIG_INT &a, BIG_INT &b){
    if(a.Length() > b.Length()) return true;
    else if(a.Length() == b.Length()){
        for(int i=0; i<a.Length(); i++){
            if(a.digits[i] > b.digits[i]) return true;
            else if(a.digits[i] < b.digits[i]) return false;
        }
    }
}
bool operator<(BIG_INT &a, BIG_INT &b){
    return !(a==b || a>b);
}
bool operator<=(BIG_INT &a, BIG_INT &b){
    return !(a>b);
}
bool operator>=(BIG_INT &a, BIG_INT &b){
    return (a==b || a>b);
}

// Arithmatic Operators
BIG_INT operator+(BIG_INT &a, BIG_INT &b){
    int m = a.Length() - 1;
    int n = b.Length() - 1;
    int size = max(m,n) + 2;
    int pos = size - 1;
    vector<int> res(size,0);
    int carry = 0;
    while(m>=0 && n>=0){
        int r = (a.digits[m] - '0') + (b.digits[n] - '0') + carry;
        carry = r/10;
        res[pos] = r%10;
        m--;
        n--;
        pos--;
    }
    while(m>=0){
        int r = (a.digits[m] - '0') + carry;
        carry = r/10;
        res[pos] = r%10;
        m--;
        pos--;
    }
    while(n>=0){
        int r = (b.digits[n] - '0') + carry;
        carry = r/10;
        res[pos] = r%10;
        n--;
        pos--;
    }
    if(carry == 1) res[pos] = 1;
    string temp;
    bool k = false;
    for(int i: res){
        if(i!=0) k = true;
        if(k) temp.push_back(i+'0');
    }
    return BIG_INT(temp);
}
void operator++(BIG_INT &a, int b){
    BIG_INT c(1);
    a = a + c;
    return ;
}
void operator+=(BIG_INT &a, BIG_INT &b){
    a = a + b;
    return ;
}
BIG_INT operator-(BIG_INT &a, BIG_INT &b){
    if(a == b){
        return BIG_INT();
    }
    if(a > b){
        string temp = a.digits;
        int index = a.Length() - b.Length();
        for(int i=0; i< b.Length(); i++){
            if(b.digits[i] > temp[index]){
                temp[index] = temp[index] - b.digits[i] + '0' + 10;
                int ia = index - 1;
                while(temp[ia] == '0'){
                    temp[ia] = '9';
                    ia --;
                }
                temp[ia]--;
            }
            else{
                temp[index] = temp[index] - b.digits[i] + '0';
            }
            index++;
        }
        int er = 0;
        for(char c: temp){
            if(c != '0') break;
            er++;
        }
        temp.erase(0,er);
        return BIG_INT(temp);
    }
    cout<<"Out of Bounds(Value Set to Zero)"<<endl;
    return BIG_INT();
}
void operator--(BIG_INT &a, int b){
    BIG_INT c(1);
    a = a - c;
    return ;
}
void operator-=(BIG_INT &a, BIG_INT &b){
    a = a - b;
    return ;
}
BIG_INT operator*(BIG_INT &a, BIG_INT &b){
    if(a.digits=="0" || b.digits=="0"){
        return BIG_INT();
    }
    int n= a.Length() - 1;
    int m= b.Length() - 1;
    vector<int> temp(n+m+2,0);
    for(int i=n; i>=0; i--){
        int x = a.digits[i] - '0';
        if(x==0) continue;
        for(int j=m; j>=0; j--){
            int y = b.digits[j] - '0';
            int index = i+j+1;
            temp[index] += x*y;
            temp[index-1] += (temp[index] / 10);
            temp[index] = temp[index] % 10;
        }
    }
    string k;
    bool ge = false;
    for(int i: temp){
        if(i != 0) ge = true;
        if(ge) k.push_back(i+'0');
    }
    return BIG_INT(k);
}
void operator*=(BIG_INT &a, BIG_INT &b){
    a = a*b;
    return ;
}
void mul_by_2(BIG_INT &a){
    BIG_INT b(2);
    a = a*b;
    return ;
}
void div_by_2(BIG_INT &b){
    int carry = 0;
    for(int i=0; i <b.Length(); i++){
        int d = (b.digits[i]-'0') + (10*carry);
        carry = d&1;
        d = d/2;
        b.digits[i] = d + '0';
    }
    int er = 0;
    for(char c: b.digits){
        if(c != '0') break;
        er++;
    }
    b.digits.erase(0,er);
}
BIG_INT operator/(BIG_INT &a, BIG_INT &b){
    if(b.Is_Zero()){
        cout<<"Divide by zero error"<<endl;
        return BIG_INT();
    }
    BIG_INT t(b);
    BIG_INT num(a);
    BIG_INT i(1);
    BIG_INT q;
    while(num >= b){
        while(num >= t){
            mul_by_2(t);
            mul_by_2(i);
        }
        div_by_2(t);
        div_by_2(i);
        num -= t;
        q += i;
        i = 1;
        t = b;
    }
    return q;
}
void operator/=(BIG_INT &a, BIG_INT &b){
    a = a/b;
    return ;
}
BIG_INT operator%(BIG_INT &a, BIG_INT &b){
    if(b.Is_Zero()){
        cout<<"Divide by zero error"<<endl;
        return BIG_INT();
    }
    BIG_INT t(b);
    BIG_INT num(a);
    while(num >= b){
        while(num >= t){
            mul_by_2(t);
        }
        div_by_2(t);
        num -= t;
        t = b;
    }
    return num;
}
void operator%=(BIG_INT &a, BIG_INT &b){
    a = a%b;
    return ;
}

// Fibonacci Function
BIG_INT NthFibonacci(int n){
	BIG_INT a, b(1), c;
    while(n>0){
        c = a + b;
        a = b;
        b = c;
        n--;
    }
	return a;
}

// Factorial Function
BIG_INT Factorial(int k){
    BIG_INT f(1);
    for(int i = 2; i<=k; i++){
        BIG_INT m(i);
        f *= m;
    }
    return f;
}

int main(){
    BIG_INT t("13476868");
    BIG_INT k;
    k = "12445661";
    k++;
    //t += k;
    k.prt();
    mul_by_2(k);
    k.prt();
    div_by_2(k);
    k.prt();
    t.prt();
    cout<< (k<=t) << endl;
    BIG_INT ad = t+k;
    ad.prt();
    BIG_INT sb = t-k;
    sb.prt();
    BIG_INT ml = t*k;
    ml.prt();
    BIG_INT dv = t/k;
    dv.prt();
    BIG_INT a(42);
    BIG_INT b(4);
    a %= b;
    a.prt();
    BIG_INT fb = NthFibonacci(12);
    fb.prt();
    BIG_INT ff;
    ff = Factorial(100);
    ff.prt();
    BIG_INT asf(100000);
    BIG_INT asb(1);
    BIG_INT tst;
    tst = asf % asb;
    tst.prt();
    return 0;
}