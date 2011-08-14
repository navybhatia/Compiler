#include <stdio.h>
#include <iostream>
#include "RegTable.h"
#include "Ast.h"
#include "Value.h"
#include "MemManage.h"
#include "IntermediateCode.h"
#include "STEClasses.h"
#include "CommonSubExpr.h"
#include "Misc.h"

using namespace std;
#define MAX_EVENTS 128
#define MAX_STATES 5

typedef struct {

    int arr[MAX_EVENTS];

} array;

class RulePattern
{
    vector<string> pattern_;
    vector<array*> matrix;
    string reg_;
    string curStateReg_;
    string finalStateReg_;
    string ruleLabel_;
    string outLabel_;
    int patternLength_;
    int baseAddress_;
    int relBaseAddress_;
    int finalState_;

    public:
    void insertPat(int index, string pat) { 
      if(index > pattern_.size()) {
          int i = index - pattern_.size();
          while(i) {
              pattern_.push_back ("$");
              i--;
            }
         }
      pattern_[index-1] = pat;
    }
    vector<string>& getPattern() { return pattern_;}
    void setCurStateRegister(string reg) {curStateReg_ = reg;}
    string getCurStateRegister() { return curStateReg_;}
    void setFinalStateRegister(string reg) {finalStateReg_ = reg;}
    string getFinalStateRegister() { return finalStateReg_;}
    void setRegister(string reg) {reg_ = reg;}
    string getRegister() { return reg_;}
    void setRuleLabel(string lbl) {ruleLabel_ = lbl;}
    string getRuleLabel() { return ruleLabel_;}
    void setOutLabel(string lbl) {outLabel_ = lbl;}
    string getOutLabel() { return outLabel_;}
    void setPatLength(int len) {patternLength_ = len;}
    int getPatLength() { return patternLength_;}
    int getPatSize() { return pattern_.size();}
    void setBaseAddress(int addr) {baseAddress_ = addr;}
    int getBaseAddress() { return baseAddress_;}
    void setRelBaseAddress(int addr) {relBaseAddress_ = addr;}
    int getRelBaseAddress() { return relBaseAddress_;}
    void  setFinalState(int finalState) {finalState_ = finalState;}
    int  getFinalState() {return finalState_;}
    vector<array*>&  getMatrix() { return matrix;}
    void setMatrix(int *data, int& state) {
        for(int i = 0; i <= state; i++)
        { 
            array *event_data = new array;
            int *temp_data = data + i*MAX_EVENTS;
            for(int j = 0; j < MAX_EVENTS; j++)
                event_data->arr[j] = temp_data[j];
            matrix.push_back(event_data);
        }
/*       cout<<endl;
       cout<<"=================================================";
       cout<<endl;
     for(int i = 0; i <= state; i++)
        cout<<"\t"<<i;
    for(int j = 0; j < MAX_EVENTS; j++) {
       cout<<endl;
       cout<<j;
       for(int i = 0; i<= state; i++)
           cout<<"\t"<<(matrix[i])->arr[j];
    }*/
    }
};
