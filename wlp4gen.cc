#include "wlp4type.h"

// For mutual recursion
void generateHelper(Node *root, Procedure proc);

// MIPS PRINTING HELPERS
void Add(int d, int s, int t)
{
  std::cout << "add $" << d << ", $" << s << ", $" << t << "\n";
}

void Sub(int d, int s, int t)
{
  cout << "sub $" << d << ", $" << s << ", $" << t << endl;
}

void Mult(int d, int s)
{
  cout << "mult $" << d << ", $" << s << endl;
}

void Mflo(int d)
{
  cout << "mflo $" << d << endl;
}

void Mfhi(int d)
{
  cout << "mfhi $" << d << endl;
}

void Div(int d, int s)
{
  cout << "div $" << d << ", $" << s << endl;
}

void Beq(int s, int t, std::string label)
{
  std::cout << "beq $" << s << ", $" << t << ", " + label + "\n";
}

void Bne(int s, int t, std::string label)
{
  std::cout << "bne $" << s << ", $" << t << ", " + label + "\n";
}

void Slt(int d, int s, int t)
{
  cout << "slt $" << d << ", $" << s << ", $" << t << endl;
}

void Sltu(int d, int s, int t)
{
  cout << "sltu $" << d << ", $" << s << ", $" << t << endl;
}

void lw(int d, int s, int t)
{
  cout << "lw $" << d << ", " << s << "($" << t << ")" << endl;
}

void sw(int d, int s, int t)
{
  cout << "sw $" << d << ", " << s << "($" << t << ")" << endl;
}

void Jr(int s)
{
  std::cout << "jr $" << s << "\n";
}

void Jalr(int s)
{
  cout << "jalr $" << s << endl;
}

void lis(int i)
{
  cout << "lis $" << i << endl;
}

void Word(int i)
{
  std::cout << ".word " << i << "\n";
}

void Word(std::string label)
{
  std::cout << ".word " + label + "\n";
}

void Label(std::string name)
{
  std::cout << name + ":\n";
}

void push(int s)
{
  cout << "sw $" << s << ", -4($30)" << endl;
  cout << "sub $30, $30, $4" << endl;
}

void pop(int d)
{
  cout << "add $30, $30, $4" << endl;
  cout << "lw $" << d << ", -4($30)" << endl;
}

void pop()
{
  cout << "add $30, $30, $4" << endl;
}

int labelCounter = 0;

Node *lvalueHelper(Node *root)
{

  if (root->rule.RHS.size() == 3)
  {
    return lvalueHelper(root->getChild("lvalue"));
  }

  return root;
}

void testHelper(Node *root, string label, Procedure proc)
{

  generateHelper(root->getChild("expr"), proc);
  push(3);
  generateHelper(root->getChild("expr", 2), proc);
  pop(5);

  if (root->rule.RHS.at(1) == "EQ")
  {
    Bne(5, 3, label);
  }
  else if (root->rule.RHS.at(1) == "NE")
  {
    Beq(5, 3, label);
  }
  else if (root->rule.RHS.at(1) == "LT")
  {

    if (root->getChild("expr")->type == "int*")
    {
      Sltu(3, 5, 3);
      Bne(3, 11, label);
    }
    else
    {
      Slt(3, 5, 3);
      Bne(3, 11, label);
    }
  }
  else if (root->rule.RHS.at(1) == "LE")
  {

    if (root->getChild("expr")->type == "int*")
    {
      Add(3, 3, 11);
      Sltu(3, 5, 3);
      Bne(3, 11, label);
    }
    else
    {
      Add(3, 3, 11);
      Slt(3, 5, 3);
      Bne(3, 11, label);
    }
  }
  else if (root->rule.RHS.at(1) == "GE")
  {

    if (root->getChild("expr")->type == "int*")
    {
      Sub(3, 3, 11);
      Sltu(3, 3, 5);
      Bne(3, 11, label);
    }
    else
    {
      Sub(3, 3, 11);
      Slt(3, 3, 5);
      Bne(3, 11, label);
    }
  }
  else
  {

    if (root->getChild("expr")->type == "int*")
    {
      Sltu(3, 5, 3);
      Beq(3, 11, label);
    }
    else
    {
      Slt(3, 3, 5);
      Bne(3, 11, label);
    }

  }
}

void generateHelper(Node *root, Procedure proc)
{

  if (root->rule.LHS == "expr")
  {

    if (root->rule.RHS.size() == 1)
    {
      generateHelper(root->getChild("term"), proc);
    }
    else if (root->rule.RHS.size() == 3)
    {

      if (root->getChild("expr")->type == "int*" && root->getChild("term")->type == "int")
      {

        generateHelper(root->getChild("expr"), proc);
        push(3);
        generateHelper(root->getChild("term"), proc);
        Mult(3, 4);
        Mflo(3);
        pop(5);

        if (root->rule.RHS.at(1) == "PLUS")
        {
          Add(3, 5, 3);
        }
        else
        {
          Sub(3, 5, 3);
        }
      }
      else if (root->getChild("expr")->type == "int" && root->getChild("term")->type == "int*")
      {

        generateHelper(root->getChild("expr"), proc);
        push(3);
        generateHelper(root->getChild("term"), proc);
        pop(5);

        Mult(5, 4);
        Mflo(5);

        if (root->rule.RHS.at(1) == "PLUS")
        {
          Add(3, 5, 3);
        }
      }
      else
      {
        generateHelper(root->getChild("expr"), proc);
        push(3);
        generateHelper(root->getChild("term"), proc);
        pop(5);

        if (root->rule.RHS.at(1) == "PLUS")
        {
          Add(3, 3, 5);
        }
        else
        {
          Sub(3, 5, 3);

          if (root->getChild("expr")->type == "int*" && root->getChild("term")->type == "int*")
          {
            Div(3, 4);
            Mflo(3);
          }
        }
      }
    }
  }
  else if (root->rule.LHS == "term")
  {

    if (root->rule.RHS.size() == 1)
    {
      generateHelper(root->getChild("factor"), proc);
    }
    else if (root->rule.RHS.size() == 3)
    {

      generateHelper(root->getChild("factor"), proc);
      push(3);
      generateHelper(root->getChild("term"), proc);
      pop(5);

      if (root->rule.RHS.at(1) == "STAR")
      {
        Mult(3, 5);
        Mflo(3);
      }
      else if (root->rule.RHS.at(1) == "SLASH")
      {
        Div(3, 5);
        Mflo(3);
      }
      else
      {
        Div(3, 5);
        Mfhi(3);
      }
    }
  }
  else if (root->rule.LHS == "factor")
  {

    if (root->rule.RHS.size() == 1)
    {

      if (root->rule.RHS.at(0) == "ID")
      {
        Node *id = root->getChild("ID");
        string name = id->token.lexeme;
        auto it = proc.offsetTable.find(name);
        int offset = it->second;

        lw(3, offset, 29);
      }
      else if (root->rule.RHS.at(0) == "NUM")
      {
        int num = stoi(root->getChild("NUM")->token.lexeme);
        lis(3);
        Word(num);
      }
      else
      {
        lis(3);
        Word(1);
      }
    }
    else if (root->rule.RHS.size() == 3)
    {

      if (root->rule.RHS.at(0) == "LPAREN")
      {
        generateHelper(root->getChild("expr"), proc);
      }
      else if (root->rule.RHS.at(0) == "ID")
      {

        string name = root->getChild("ID")->token.lexeme;
        push(31);
        push(29);
        lis(5);
        Word("ProcedureNamed" + name);
        Jalr(5);
        pop(29);
        pop(31);
      }
    }
    else if (root->rule.RHS.size() == 2)
    {

      if (root->rule.RHS.at(0) == "AMP")
      {

        Node *lvalue = lvalueHelper(root->getChild("lvalue"));

        // TWO DIFFERENT POSSIBILITIES
        if (lvalue->rule.RHS.size() == 1)
        {
          string name = lvalue->getChild("ID")->token.lexeme;

          auto it = proc.offsetTable.find(name);
          int offset = it->second;

          lis(3);
          Word(offset);
          Add(3, 29, 3);
        }
        else
        {
          generateHelper(lvalue->getChild("factor"), proc);
        }
      }
      else
      {
        generateHelper(root->getChild("factor"), proc);
        lw(3, 0, 3);
      }
    }
    else if (root->rule.RHS.size() == 5)
    {
      generateHelper(root->getChild("expr"), proc);

      Add(1, 0, 3);

      lis(5);
      Word("new");

      push(31);
      Jalr(5);
      pop(31);

      Bne(3, 0, "SKIPPING" + to_string(labelCounter));
      Add(3, 3, 11);
      Label("SKIPPING" + to_string(labelCounter));
      labelCounter++;
    }
    else if (root->rule.RHS.size() == 4)
    {

      string name = root->getChild("ID")->token.lexeme;
      push(31);
      push(29);

      Node *args = root->getChild("arglist");

      int counter = 0;

      while (args->rule.RHS.size() != 1)
      {
        generateHelper(args->getChild("expr"), proc);
        push(3);
        args = args->getChild("arglist");
        counter++;
      }

      generateHelper(args->getChild("expr"), proc);
      push(3);
      counter++;

      lis(5);
      Word("ProcedureNamed" + name);
      Jalr(5);

      for (int i = 0; i < counter; i++)
      {
        pop();
      }

      pop(29);
      pop(31);
    }
  }
}

void statementHelper(Node *root, Procedure proc)
{

  if (root->rule.LHS == "statements")
  {
    if (root->rule.RHS.size() == 2)
    {
      statementHelper(root->getChild("statements"), proc);
      statementHelper(root->getChild("statement"), proc);
    }
  }
  else if (root->rule.LHS == "statement")
  {

    if (root->rule.RHS.size() == 4)
    {

      Node *lvalue = root->getChild("lvalue");
      lvalue = lvalueHelper(lvalue);

      if (lvalue->rule.RHS.size() == 1)
      {

        string name = lvalue->getChild("ID")->token.lexeme;
        generateHelper(root->getChild("expr"), proc);

        auto it = proc.offsetTable.find(name);
        int offset = it->second;

        sw(3, offset, 29);
      }
      else
      {
        generateHelper(lvalue->getChild("factor"), proc);
        push(3);
        generateHelper(root->getChild("expr"), proc);
        pop(5);

        sw(3, 0, 5);
      }
    }
    else if (root->rule.RHS.size() == 5)
    {

      if (root->rule.RHS.at(0) == "PRINTLN")
      {
        generateHelper(root->getChild("expr"), proc);
        Add(1, 3, 0);
        push(31);
        lis(5);
        Word("print");
        Jalr(5);
        pop(31);
      }
      else if (root->rule.RHS.at(0) == "DELETE")
      {

        generateHelper(root->getChild("expr"), proc);
        Beq(3, 11, "DoNotDelete" + to_string(labelCounter));

        Add(1, 0, 3);
        lis(5);
        Word("delete");
        push(31);
        Jalr(5);
        pop(31);
        Label("DoNotDelete" + to_string(labelCounter));
        labelCounter++;
      }
    }
    else if (root->rule.RHS.size() == 7)
    {

      string startLabel = "RidiculousChewSTART" + to_string(labelCounter);
      string endLabel = "RidiculousChewEND" + to_string(labelCounter);
      labelCounter++;

      Label(startLabel);

      testHelper(root->getChild("test"), endLabel, proc);

      statementHelper(root->getChild("statements"), proc);

      Beq(0, 0, startLabel);

      Label(endLabel);
    }
    else if (root->rule.RHS.size() == 11)
    {
      string elseLabel = "GRUMPYLICIOUsELSE" + to_string(labelCounter);
      string endLabel = "GRUMPYLICIOUsEND" + to_string(labelCounter);
      labelCounter++;

      testHelper(root->getChild("test"), elseLabel, proc);

      statementHelper(root->getChild("statements"), proc);

      Beq(0, 0, endLabel);

      Label(elseLabel);

      statementHelper(root->getChild("statements", 2), proc);

      Label(endLabel);
    }
  }
}

void generateMain(Node *root, ProcedureTable procTable)
{

  string name = root->children.at(1)->token.lexeme;
  Procedure proc = procTable.Get(name);

  if (name == "wain")
  {

    Node *dcl = root->getChild("dcl");
    string paramName = dcl->getChild("ID")->token.lexeme;

    proc.offsetTable.insert(make_pair(paramName, 8));

    push(1);

    dcl = root->getChild("dcl", 2);
    paramName = dcl->getChild("ID")->token.lexeme;

    proc.offsetTable.insert(make_pair(paramName, 4));

    push(2);

    Sub(29, 30, 4);

    lis(5);
    Word("init");
    if (root->getChild("dcl")->getChild("type")->children.size() == 1)
    {
      lis(2);
      Word(0);
    }
    push(31);
    Jalr(5);
    pop(31);

    Node *dcls = root->getChild("dcls");
    int counter = 0;

    while (dcls->children.size() != 0)
    {
      int value;

      if (dcls->children.at(3)->token.kind == "NUM")
      {
        value = stoi(dcls->children.at(3)->token.lexeme);
      }
      else
      {
        value = 1;
      }

      string name = dcls->getChild("dcl")->getChild("ID")->token.lexeme;
      lis(3);
      Word(value);
      push(3);
      proc.offsetTable.insert(make_pair(name, -4 * counter));

      dcls = dcls->getChild("dcls");
      counter++;
    }

    statementHelper(root->getChild("statements"), proc);

    generateHelper(root->getChild("expr"), proc);

    for (int i = 0; i < counter + 2; i++)
    {
      pop();
    }

    Jr(31);
  }
  else
  {

    Label("ProcedureNames" + name);

    Sub(29, 30, 4);

    Node *params = root->getChild("params");

    if (params->rule.RHS.size() != 0)
    {

      params = params->getChild("paramlist");

      int offsetCounter = 0;

      while (params->rule.RHS.size() != 1)
      {
        string name = params->getChild("dcl")->getChild("ID")->token.lexeme;
        proc.offsetTable.insert(make_pair(name, (proc.paramTypes.size() - offsetCounter) * 4));
        offsetCounter++;
        params = params->getChild("paramlist");
      }

      string name = params->getChild("dcl")->getChild("ID")->token.lexeme;

      proc.offsetTable.insert(make_pair(name, (proc.paramTypes.size() - offsetCounter) * 4));
    }

    Node *dcls = root->getChild("dcls");
    int counter = 0;

    while (dcls->children.size() != 0)
    {
      int value;
      if (dcls->children.at(3)->token.kind == "NUM")
      {
        value = stoi(dcls->children.at(3)->token.lexeme);
      }
      else
      {
        value = 1;
      }
      string name = dcls->getChild("dcl")->getChild("ID")->token.lexeme;
      lis(3);
      Word(value);
      push(3);
      proc.offsetTable.insert(make_pair(name, -4 * counter));
      dcls = dcls->getChild("dcls");
      counter++;
    }

    statementHelper(root->getChild("statements"), proc);

    generateHelper(root->getChild("expr"), proc);

    for (int i = 0; i < counter; i++)
    {
      pop();
    }

    Jr(31);
  }
}

void recursiveProcHelper(Node *root, ProcedureTable procTable)
{

  if (root->rule.RHS.size() == 1)
  {
    generateMain(root->getChild("main"), procTable);
  }
  else
  {
    recursiveProcHelper(root->getChild("procedures"), procTable);
    generateMain(root->getChild("procedure"), procTable);
  }
}

int main()
{
  cout << ".import print" << endl;
  cout << ".import init" << endl;
  cout << ".import new" << endl;
  cout << ".import delete" << endl;

  cout << "lis $4" << endl;
  cout << ".word 4" << endl;
  cout << "lis $11" << endl;
  cout << ".word 1" << endl;

  // Get the CFG rules and store in vector
  vector<rules> *CFG = CFG_Builder();

  // Get the SLR DFA
  SLR_DFA *slrDFA = new SLR_DFA();
  slrDFA->transitions = transitionMapBuilder();
  slrDFA->reductions = recutionMapBuilder();

  // Tree stack, pointer to each tree
  vector<Node *> *treeStack = new vector<Node *>;

  try
  {
    std::stringstream s(DFAstring);

    // Read WLP4 and convert to tokens
    dfa myDFA = createDFA(s);
    vector<Token> tokens = simplifiedMaximalMunch(myDFA);

    // Convert vector of tokens to Deque
    deque<Token> myDeque;
    myDeque.push_back(Token("BOF", "BOF"));
    for (int i = 0; i < tokens.size(); i++)
    {
      myDeque.push_back(tokens[i]);
    }
    myDeque.push_back(Token("EOF", "EOF"));

    // State stack
    vector<int> stateStack;
    stateStack.push_back(0);

    int length = myDeque.size();

    while (myDeque.size() != 0)
    {
      Token currentToken = myDeque.front();

      pair<int, string> myPair(stateStack.back(), currentToken.kind);

      if (slrDFA->reductions.find(myPair) != slrDFA->reductions.end())
      {
        int rule = slrDFA->reductions[myPair];
        reduceTrees(CFG->at(rule), treeStack);
        reduceStates(CFG->at(rule), stateStack, slrDFA);
      }
      else
      {
        shift(myDeque, treeStack, stateStack, slrDFA);
      }
    }

    reduceTrees(CFG->at(0), treeStack);

    ProcedureTable procTable = collectProcedures(treeStack->at(0)->children.at(1));

    // START OF NEW STUFF

    recursiveProcHelper(treeStack->at(0)->getChild("procedures"), procTable);
    // generateMain(treeStack->at(0)->getChild("procedures"), procTable);

    // END OF NEW STUFF

    // treePrint(treeStack->at(0));

    delete CFG;
    delete slrDFA;

    int treeStackSize = treeStack->size();

    for (int i = 0; i < treeStackSize; i++)
    {
      delete treeStack->at(i);
    }

    delete treeStack;
  }
  catch (std::runtime_error &e)
  {
    std::cerr << "ERROR: " << e.what() << "\n";

    delete CFG;
    delete slrDFA;

    for (int i = 0; i < treeStack->size(); i++)
    {
      delete treeStack->at(i);
    }

    delete treeStack;

    return 1;
  }
  return 0;
}
