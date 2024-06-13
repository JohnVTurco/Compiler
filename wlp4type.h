#include "wlp4parse.h"

struct Variable {
  string name;
  string type;

  Variable(Node* root){
    Node* typeChild = root->children.at(0);

    if(typeChild->children.size() == 2){
      type = "int*";
    }
    else{
      type = "int";
    }

    Node* nameChild = root->children.at(1);

    name = nameChild->token.lexeme;
  }

};

struct VariableTable{
  map<string, Variable> variableInstances;

  void Add(Variable var){
    string name = var.name;

    if(variableInstances.find(name) != variableInstances.end()){
      throw runtime_error("Duplicate variable decleration");
    }

    variableInstances.insert(make_pair(name, var));
  }

  Variable Get(string varName){
    
    if(variableInstances.find(varName) == variableInstances.end()){
      throw runtime_error("Use of undeclared variable");
    }

    auto value = variableInstances.find(varName);

    return value->second;

  }

};

struct Procedure{
  string name;
  vector<string> paramTypes;
  VariableTable localVarTable;
  map<string, int> offsetTable;

  Procedure(Node* root){
    name = root->children.at(1)->token.lexeme;
    
    if(name == "wain"){

      Node* paramOne = root->getChild("dcl", 1);
      Variable varOne = Variable(paramOne);
      paramTypes.push_back(varOne.type);
      localVarTable.Add(varOne);

      Node* paramTwo = root->getChild("dcl", 2);
      Variable varTwo = Variable(paramTwo);
      paramTypes.push_back(varTwo.type);
      localVarTable.Add(varTwo);


    }
    else{

      Node* paramList = root->getChild("params");
      
      if(paramList->children.size() != 0){
        paramList = paramList->children.at(0);

        while(true){
          Variable var = Variable(paramList->getChild("dcl"));
          paramTypes.push_back(var.type);
          localVarTable.Add(var);

          if(paramList->children.size() == 1){
            break;
          }

          paramList = paramList->getChild("paramlist");
        }

      }

    }

     Node* dcls = root->getChild("dcls");

    while(dcls->children.size() != 0){

        if(dcls->children.size() == 0){
          break;
        }

        Variable var = Variable(dcls->getChild("dcl"));

        localVarTable.Add(var);
        
        dcls = dcls->getChild("dcls");
      }

  }

};
    
struct ProcedureTable {
  map<string, Procedure> proceduresList;

    void Add(Procedure proc){
      if(proceduresList.find(proc.name) != proceduresList.end()){
        throw runtime_error("duplicate procedure declaration");
      }
      
      proceduresList.insert(make_pair(proc.name, proc));
    }

    Procedure Get(string procName){
      if(proceduresList.find(procName) == proceduresList.end()){
        throw runtime_error("use of undeclared procedure");
      }
       auto it = proceduresList.find(procName);
       return it->second;
    }

};

void annotateTypes(Node* root, Procedure & currentProc, ProcedureTable & procList){

  for(int i = 0; i < root->children.size(); i++){
    annotateTypes(root->children.at(i), currentProc, procList);
  }

  if(root->rule.LHS == "expr"){
    
    if(root->rule.RHS.size() == 1){
      root->type = root->getChild("term")->type;
    }
    else{
      if(root->rule.RHS.at(1) == "PLUS"){

        if(root->getChild("expr")->type == "int" && root->getChild("term")->type == "int"){
          root->type = "int";
        }
        else if (root->getChild("expr")->type == "int*" && root->getChild("term")->type == "int"){
          root->type = "int*";
        }
        else if(root->getChild("expr")->type == "int" && root->getChild("term")->type == "int*"){
          root->type = "int*";
        }


      }
      else{

        if(root->getChild("expr")->type == "int" && root->getChild("term")->type == "int"){
          root->type = "int";
        }
        else if (root->getChild("expr")->type == "int*" && root->getChild("term")->type == "int"){
          root->type = "int*";
        }
        else if(root->getChild("expr")->type == "int*" && root->getChild("term")->type == "int*"){
          root->type = "int";
        }

      }
    }

  }
  else if(root->rule.LHS == "term"){

    if(root->rule.RHS.size() == 1){
      root->type = root->getChild("factor")->type;
    }
    else{
      root->type = "int";

      if(root->children.at(0)->type != "int" || root->children.at(2)->type != "int"){
        throw runtime_error("For term STAR factor, term and factor must be int");
      }
    }

  }
  else if(root->rule.LHS == "factor"){

    // Factor -> NUM or NULL or ID
    if(root->rule.RHS.size() == 1){

      if(root->rule.RHS.at(0) == "NUM"){
        root->type = "int";
      }
      else if(root->rule.RHS.at(0) == "NULL"){
        root->type = "int*";
      }
      else{
        Variable var = currentProc.localVarTable.Get(root->getChild("ID")->token.lexeme);
        root->type = var.type;
      }

    }
    //Factor -> LPAREN expr RPAREN  or  ID LPAREN RPAREN
    else if(root->rule.RHS.size() == 3){

      if(root->rule.RHS.at(0) == "LPAREN" && root->rule.RHS.at(1) == "expr" && root->rule.RHS.at(2) == "RPAREN"){
        root->type = root->getChild("expr")->type;
      }
      else{
        root->type = "int";

        string name = root->getChild("ID")->token.lexeme;

        if(currentProc.localVarTable.variableInstances.find(name) != currentProc.localVarTable.variableInstances.end()){
          throw runtime_error("Proc has same name as local var");
        }

        Procedure proc = procList.Get(name);

        if(proc.paramTypes.size() != 0){
          throw runtime_error("For Factor -> ID LPAREN RPAREN, proc (ID) must have no params");
        }
      }

    }
    //Factor -> AMP lvalue  or   STAR factor
    else if(root->rule.RHS.size() == 2){


      if(root->rule.RHS.at(0) == "AMP"){
        root->type = "int*";

        if(root->getChild("lvalue")->type != "int"){
          throw runtime_error("For Factor -> AMP lvalue, lvalue must be int");
        }
      }
      else if(root->rule.RHS.at(0) == "STAR"){
        root->type = "int";


        if(root->getChild("factor")->type != "int*"){
          throw runtime_error("For Factor -> STAR factor, factor must be int*");
        }
      }

    }
    //factor -> NEW INT LBRACK expr RBRACK
    else if(root->rule.RHS.size() == 5){
      root->type = "int*";

      if(root->getChild("expr")->type != "int"){
        throw runtime_error("For factor -> NEW INT LBRACK expr RBRACK, expr must be int");
      }
    }
    //factor -> ID LPAREN arglist RPAREN
    else if(root->rule.RHS.size() == 4){
      root->type = "int";

      string name = root->getChild("ID")->token.lexeme;
      Procedure proc = procList.Get(name);
      vector<string> params = proc.paramTypes;

      if(currentProc.localVarTable.variableInstances.find(name) != currentProc.localVarTable.variableInstances.end()){
        throw runtime_error("Proc has same name as local var");
      }

      if(params.size() == 0){
        throw runtime_error("Proc has no parameters");
      }

      Node* temp = root->getChild("arglist");
      int counter = 0;

      while(temp->rule.RHS.size() > 1){

        if(temp->getChild("expr")->type != params[counter]){
          throw runtime_error("Param types don't match for proc call");
        }
        counter++;

        if(counter > params.size()-1){
          throw runtime_error("Too many args passed in");
        }

        temp = temp->getChild("arglist");
      }

      
      if(temp->getChild("expr")->type != params[counter]){
        throw runtime_error("Param types don't match for proc call");
      }
      counter++;

      if(counter != params.size()){
        throw runtime_error("Number of params required and number provided do not match");
      }


    }
    
  }
  else if(root->rule.LHS == "lvalue"){

    //lvalue -> ID
    if(root->rule.RHS.size() == 1){

      if(root->rule.RHS.at(0) == "ID"){
         Variable var = currentProc.localVarTable.Get(root->getChild("ID")->token.lexeme);
         root->type = var.type;
      }

    }
    //lvalue -> LPAREN lvalue RPAREN  
    else if (root->rule.RHS.size() == 3){
      root->type = root->getChild("lvalue")->type;
    }
    //lvalue -> STAR factor
    else if(root->rule.RHS.size() == 2){
      root->type = "int";

      if(root->getChild("factor")->type != "int*"){
        throw runtime_error("For lvalue -> STAR factor, factor must be int*");
      }
    }

  }
  

}

void checkStatements(Node* root){

  for(int i = 0; i < root->children.size(); i++){
    checkStatements(root->children.at(i));
  }

  if(root->rule.LHS == "statement"){
    
    if(root->rule.RHS.size() == 4){
      string typeOne = root->getChild("lvalue")->type;
      string typeTwo = root->getChild("expr")->type;

      if(typeOne != typeTwo){
        throw runtime_error("For statement → lvalue BECOMES expr SEMI, lvalue and expr must be same type");
      }

    }
    else if(root->rule.RHS.size() == 5){

      if(root->rule.RHS.at(0) == "PRINTLN"){

        if(root->getChild("expr")->type != "int"){
          throw runtime_error("For statement → PRINTLN LPAREN expr RPAREN SEMI, expr must be int");
        }

      }
      else if(root->rule.RHS.at(0) == "DELETE"){

        if(root->getChild("expr")->type != "int*"){
          throw runtime_error("For statement → DELETE LBRACK RBRACK expr SEMI, expr must be int*");
        }
      }

    }

  }
  else if(root->rule.LHS == "test"){
    string typeOne = root->getChild("expr", 1)->type;
    string typeTwo = root->getChild("expr", 2)->type;

    if(typeOne != typeTwo){
      throw runtime_error("For tests, both expr must be same type");
    }

  }
  else if(root->rule.LHS == "dcls"){
    
    if(root->rule.RHS.size() == 5){

      if(root->rule.RHS.at(3) == "NUM"){

        if(root->getChild("dcl")->getChild("type")->children.size() != 1){
          throw runtime_error("For dcls → dcls dcl BECOMES NUM SEMI, dcl must be int");
        }

      }
      else if(root->rule.RHS.at(3) == "NULL"){

        if(root->getChild("dcl")->getChild("type")->children.size() != 2){
          throw runtime_error("For dcls → dcls dcl BECOMES NUM SEMI, dcl must be int*");
        }

      }

    }

  }


}


ProcedureTable collectProcedures(Node* root){
    ProcedureTable procTable;
    Node* temp = root;

    while(temp->children.size() > 1){
      Node* procNode = temp->getChild("procedure");
      Procedure proc = Procedure(procNode);
      procTable.Add(proc);

      annotateTypes(procNode, proc, procTable);
      checkStatements(procNode);

      if(procNode->getChild("expr")->type != "int"){
        throw runtime_error("Return type of proc must be int");
      }

      temp = temp->getChild("procedures");
    }

    Node* mainNode = temp->getChild("main");
    Procedure proc = Procedure(mainNode);
    procTable.Add(proc);
    
    if(mainNode->getChild("dcl", 2)->getChild("type")->children.size() != 1){
      throw runtime_error("Second main param must be int");
    }


    annotateTypes(mainNode, proc, procTable);
    checkStatements(mainNode);

    if(mainNode->getChild("expr")->type != "int"){
        throw runtime_error("Return type of main must be int");
    }

    return procTable;
    
}
