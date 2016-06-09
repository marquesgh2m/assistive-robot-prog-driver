#include "logoLexer.h"
#include "logoParser.h"
#include <cassert>
#include <cstring>
#include <map>
#include <vector>
#include <stack>
#include <string>
#include <iostream>

using std::map;
using std::vector;
using std::stack;
using std::string;
using std::cout;
using std::endl;

struct mem
{
   map<string,int> memory;
};

struct element
{
  int typeInt;
  float typeFloat;
};

struct procDec
{
  pANTLR3_BASE_TREE node;
  int argNum;
  vector<string> args;
};

class ExprTreeEvaluator {
    map<string,int> memory;         // Dicionário de variáveis globais
    map<string,procDec> proc;       // Dicionário de procedimentos
    stack<mem> localMem;            // Stack de dicionários de variáveis locais
    map<string,int>::iterator it;   // Iterados dos dicionários
    int memFlag;                    // Flag de indicação variáves globais/locais
public:
    ExprTreeEvaluator();
    int run(pANTLR3_BASE_TREE);
};

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE, unsigned);
const char* getText(pANTLR3_BASE_TREE tree);
bool compare (int a, int b, string comp);

int main(int argc, char* argv[])
{
  pANTLR3_INPUT_STREAM input;
  plogoLexer lex;
  pANTLR3_COMMON_TOKEN_STREAM tokens;
  plogoParser parser;

  //char* text = (char*)" FOR[criar a = 1; a < 8; criar a = a + 1] [PF 2 + 4 PD 90]";       //  Utilizado para modo terminal 
                                                                                            //  #
  //uint8_t* bufferData;                                                                    //  #
                                                                                            //  #
  //uint32_t bufferSize;                                                                    //  #
                                                                                            //  #
  //pANTLR3_UINT8 bufferName;                                                               //  #
                                                                                            //  #
  //bufferData = (uint8_t*)text;                                                            //  #
                                                                                            //  #
  //bufferSize = strlen(text);                                                              //  #
                                                                                            //  #
  //cout << "size: " << bufferSize << endl;                                                 //  #
                                                                                            //  #
  //input = antlr3NewAsciiStringInPlaceStream( bufferData, bufferSize, bufferName);         //  #

  assert(argc > 1);
  input = antlr3AsciiFileStreamNew((pANTLR3_UINT8)argv[1]);                                 //  Utilizar para modo script
  assert(input != NULL);
  lex = logoLexerNew(input);
  assert(lex != NULL);
  tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
  assert(tokens != NULL);

  parser = logoParserNew(tokens);
  assert(parser != NULL);

  logoParser_prog_return r = parser->prog(parser);

  pANTLR3_BASE_TREE tree = r.tree;

  if (tree == NULL)
  {
    cout << "error" << endl;
    return -1;
  }

  ExprTreeEvaluator eval;
  int rr = eval.run(tree);
  //cout << "Evaluator result: " << rr << '\n';

  parser->free(parser);
  tokens->free(tokens);
  lex->free(lex);
  input->close(input);

  return 0;
}

ExprTreeEvaluator::ExprTreeEvaluator()
{
  memFlag = 0;
}


int ExprTreeEvaluator::run(pANTLR3_BASE_TREE tree)
{
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    if(tok) {
        switch(tok->type) {

          case INT:
          {
            return atoi(getText(tree));
          }

          case ID:
          {            
            if (!memFlag)         // Se flag for zero retorna o valor da variável global caso contrario o valor da variável local  
            {
              if(memory.find(getText(tree)) != memory.end())
              {
                return memory[getText(tree)];
              }
              else
                cout << "Variavel " << getText(tree) << " global não existe" << endl;
            }
            else
            {
              if(localMem.top().memory.find(getText(tree)) != localMem.top().memory.end())
              {
                return localMem.top().memory[getText(tree)]; // Variável local do primeiro item da stack
              }
              else
                cout << "Variavel " << getText(tree) << " local não existe" << endl;
            }
            break;              
          }
  
          case STRINGE:
          {
            break;
          }

          case PLUS:
          {

            if (tree->getChildCount(tree) < 2)
              return run(getChild(tree,0));
            else
              return run(getChild(tree,0)) + run(getChild(tree,1));
          }

          case MINUS:
          {

            if (tree->getChildCount(tree) < 2)
             return -1*(run(getChild(tree,0)));
            else
             return run(getChild(tree,0)) - run(getChild(tree,1));
          }

          case MULT:
          {

            //int fi = tree->getChildCount(tree);
            //cout << "MULT: " << fi << std::endl;
            return run(getChild(tree,0)) * run(getChild(tree,1));
          }

          case DIV:
          {

            //int fi = tree->getChildCount(tree);
            //cout << "DIV: " << fi << std::endl;
            return run(getChild(tree,0)) / run(getChild(tree,1));
          }

          case PF:
          {

            cout << "PF: " << run(getChild(tree,0)) << endl;
            break;
          }

          case PT:
          {

            cout << "PT: " << run(getChild(tree,0)) << endl;
            break;
          }

          case PD:
          {

            cout << "PD: " << run(getChild(tree,0)) << endl;
            break;
          }

          case PE:
          {

            cout << "PE: " << run(getChild(tree,0)) << endl;
            break;
          }

          case SCAN:
          {

            cout << "SCAN"<< endl;
            break;
          }

          case STATUS:
          {
            cout << "STATUS"<< endl;
            break;
          }

          case RANGER:
          {
            const char* cmdTxt= getText(tree);

            cout << "RANGER: " << getText(tree) << endl;
            return 2;
          }

          case POS:
          {
            cout << "POS: " << getText(tree) << endl;
            return 3;
          }

          case COMENT:
          {
            cout << "COMMENT: " << endl; 
            cout << getText(tree) << endl;
            break;
          }

          case PRINTE:
          {
            cout << "PRINTE: " << endl; 
            //cout << "TIPE: " << tree->getToken(getChild(tree,0))->type << endl;
            if(tree->getToken(getChild(tree,0))->type == STRINGE)         // Caso seja string informa texto do filho caso contrario executa o filho
              cout << getText(getChild(tree,0)) << endl;
            else
              cout << run(getChild(tree,0)) << endl;
            break;
          }



          case FORE:
          {
            //cout << "filhos: " << tree->getChildCount(tree);
            run(getChild(tree,0));                              // Executa condição inicial
            int a = run(getChild(tree,1));                      // Retorna o valor das variáveis na condição
            int b = run(getChild(tree,3));                      // #
            string c = (string)getText(getChild(tree,2));       // Retorna operador de condição

            bool ok = compare(a,b,c);       // Compara variaveis

            while (ok == true)  
            { 
              run(getChild(tree,5));                                        //Executa bloco
              run(getChild(tree,4));                                        //Executa operação de incremento
              ok = compare(run(getChild(tree,1)),run(getChild(tree,3)),c);  //Realiza comparação novamente
            }
            break;
          }

          case REPEAT:
          {
            for (int r = 0; r < run(getChild(tree,0)); r++)
            {
              run(getChild(tree,1));
            }
            break;
          }

          case IFE:
          {
            int a = run(getChild(tree,0));                  // Retorna o valor das variáveis na condição
            int b = run(getChild(tree,2));                  // #
            string c = (string)getText(getChild(tree,1));   // Retorna operador de condição

            bool ok = compare(a,b,c);           // Compara variaveis

            if (ok == true)
            {
              run(getChild(tree,3));            // Caso verdadeiro executa bloco THEN
              break;
            }
            if (tree->getChildCount(tree) == 5) // Caso exista e condição for falsa executa bloco ELSE
            {
              if (ok == false)
              {
                run(getChild(tree,4));
              }
            }
            break;
          }

          //case THEN:
          //case ELSEE:
          case BLOCK:
          {
            for (int f = 0; f < tree->getChildCount(tree); f++)
              {
                run(getChild(tree,f));
              }
            break;
          }
          
          case MAKE:
          {
            
            
            char* var = (char*)getText(getChild(tree,0));
            int val;
            if (tree->getChildCount(tree) < 2)
            {
              val = 0;                           // Se não for atribuido valor, variável começa com zero 
            }
            else 
              val = run(getChild(tree,1));

            cout << "MAKE: " << var << " = " << val << endl;

            if (!memFlag)
            {
              if(memory.find(var) != memory.end())
              {
                cout << "Variavel " << var << " global já foi declarada" << endl;        // Se flag for zero e variável ainda não foi declarada cria-se uma variavel global se flag for diferente de zero cria-se uma local
              }
              else
              {
                memory[var] = val;
                return val;
              }
                
            }
              
            else
            {
              if(localMem.top().memory.find(var) != localMem.top().memory.end())
              {
                cout << "Variavel " << var << " local já foi declarada" << endl;
              }
              else
              {
                localMem.top().memory[var] = val;
                return val;
              }
                
            }
            break;
            }

          case PROCINV:
          {
            char* name = (char*)getText(getChild(tree,0));

            mem local;                                      // Inicia dicionário local

            for (int f = 1; f <= proc[name].argNum ; f++)
              local.memory[proc[name].args[f-1]] = run(getChild(tree,f));   // Atribui as variaveis de argumento no dicionário

            localMem.push(local);                      // Empilha dicionário 
            memFlag++;                                 // Incrementa flag de memória

            run(proc[name].node);                      // Executa procedimento 

            localMem.pop();                            // Desempilha dicionário
            memFlag--;                                 // Decrementa flag de memória

            break;
          }

          case PROCDEC:
          {

            char* name = (char*)getText(getChild(tree,0));

            int childNum = tree->getChildCount(tree);

            if(childNum < 3)
            {
              proc[name].argNum = 0;                    // Caso procedimento não apresente parametros salva bloco do procedimento
              proc[name].node = getChild(tree,1);
            }
            else
            {
              for(int f = 1; f < childNum - 1; f++)
                proc[name].args.push_back(getText(getChild(tree,f)));   // Caso tenha parametros salva numa lista 

              proc[name].argNum = childNum - 2;
              proc[name].node = getChild(tree,childNum - 1);      // Salva bloco do procedimento
            }

            break;
          }

          case SEMICOLON:
          {
  
              //int fi = tree->getChildCount(tree);
              //cout << "COMMENT: " << fi << std::endl;
             break;
          }

          case WS:
          {
  
              //int fi = tree->getChildCount(tree);
              //cout << "WS: " << fi << std::endl;
              break;
          }

          case COMP:
          {
  
              //return getText(tree);
              break;
          }

          case INFO:
          {
            break;
          }

          case EQ:
          {
  
            char* var = (char*)getText(getChild(tree,0));
            int val = run(getChild(tree,1));

            if (!memFlag)
            {
              if(memory.find(var) != memory.end())
              {
                memory[var] = val;
                return val;
              }
              else
                cout << "Variavel " << var << " global não existe" << endl;
                
            }
            else
            {
              if(localMem.top().memory.find(var) != localMem.top().memory.end())
              {
                localMem.top().memory[var] = val;
                return val;
              }
              else
                cout << "Variavel " << var << " local não existe" << endl;
            }
            break;            
          }
  
          default:
              cout << "Unhandled token: #" << tok->type << '\n';
               break;
        }
    }
    else {
        int k = tree->getChildCount(tree);
        int r = 0;
        for(int i = 0; i < k; i++) {
            r = run(getChild(tree, i));
        }
        return r;
    }
}

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i)
{
    assert(i < tree->getChildCount(tree));
    return (pANTLR3_BASE_TREE) tree->getChild(tree, i);
}

const char* getText(pANTLR3_BASE_TREE tree)
{
    return (const char*) tree->getText(tree)->chars;
}


bool compare (int a, int b, string comp)
{
  if (comp == "<")
  {
    return (a < b);
  }
  else if (comp == "<=")
  {
    return (a <= b);
  }
  else if (comp == ">")
  {
    return (a > b);
  }
  else if (comp == ">=")
  {
    return (a >= b);
  }
  else if (comp == "==")
  {
    return (a == b);
  }
  else if (comp == "<>")
  {
    return (a != b);
  }
  else
  {
    cout << "null comp" << endl;
    return false;
  }
    
}