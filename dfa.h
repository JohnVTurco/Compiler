#include <string>


std::string DFAstring = R"(
.STATES
start
ID!
ZERO!
NUM!
LPAREN!
RPAREN!
LBRACE!
RBRACE!
LBRACK!
RBRACK!
BECOMES!
EQ!
PLUS!
MINUS!
STAR!
SLASH!
?COMMENT!
PCT!
AMP!
COMMA!
SEMI!
LT!
LE!
GT!
GE!
NOT
NE!
?WHITESPACE!
.TRANSITIONS
start       a-z A-Z         ID
ID          a-z A-Z 0-9     ID
start       0               ZERO
start       1-9             NUM
NUM         0-9             NUM
start       (               LPAREN
start       )               RPAREN
start       {               LBRACE
start       }               RBRACE
start       [               LBRACK
start       ]               RBRACK
start       =               BECOMES
BECOMES     =               EQ
start       +               PLUS
start       -               MINUS
start       *               STAR
start       /               SLASH
SLASH       /               ?COMMENT
?COMMENT     \x00-\x09 \x0B \x0C \x0E-\x7F   ?COMMENT
start       %               PCT
start       &               AMP
start       ,               COMMA
start       ;               SEMI
start       <               LT
LT          =               LE
start       >               GT
GT          =               GE
start       !               NOT
NOT         =               NE
start       \s \t \r \n     ?WHITESPACE
)";