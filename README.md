# Maksym Sadovenko
#Variant 11

The program takes one or more parametesr as input - path to the files.
For example:
```./a.out tests/test1/test1.sig tests/test2/test2.sig ``
If there are lexical errors in the test program, parsing will not be performed.

## Grammar
```
1. <signal-program> --> <program>
2. <program> --> PROGRAM <procedure-identifier> ;
            <block>.
3. <block> --> <declarations> BEGIN <statements-list> END
4. <declarations> --> <label-declarations>
5. <label-declarations> --> LABEL <unsigned-integer><labels-list>; |
    <empty>
6. <labels-list> --> , <unsigned-integer><label-list> |
    <empty>
7. <statements-list> --> <statement> <statements-list> |
            <empty>
8. <statement> --> <unsigned-integer> :
        <statement> |
        GOTO <unsigned-integer>; |
        LINK <variable-identifier>, <unsigned-integer>; |
        IN <unsigned-integer>; |
        OUT <unsigned-integer>;
9. <variable-identifier> --> <identifier>
10. <procedure-identifier> --> <identifier>
11. <identifier> --> <letter><string>
12. <string> --> <letter><string> |
            <digit><string> |
            <empty>
13. <unsigned-integer> --> <digit><digits-string>
16. <digits-string> --> <digit><digits-string> |
            <empty>
17. <digit> --> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
18. <letter> --> A | B | C | D | ... | Z
```
### Knuth Machine

|   | command address       | command code              |AT     |AF     |
|---| :-------------:       | :-----------------------: | :---: | :---: | 
|1  |\<signal-program\>     |\<program\>                |       |F      |
|2  |\<program\>            | PROGRAM                   |       |F      |
|3  |                       |\<procedure-identifier\>   |       |F      |
|4  |                       |;                          |       |F      |
|5  |                       |\<block\>                  |       |F      |
|6  |                       |.                          |T      |F      |
|7  |\<block\>              |\<declarations\>           |       |F      |
|8  |                       |BEGIN                      |       |F      |
|9  |                       |\<statements-list\>        |       |F      |
|10 |                       |END                        |T      |F      |
|11 |\<declarations\>       |\<label-declarations>      |if (lexem == "LABEL") goto 12 else return T       |return F if returned  from below F      |
|12 |\<label-declarations\> |LABEL                      |       |F       |
|13 |                       |\<unsigned-integer\>       |       |F      |
|14 |                       |\<labels-list\>            |       |F      |
|15 |                       |;                          |T      |F      |
|16 |\<labels-list\>        |,                          |       |F      |
|17 |                       |\<unsigned-integer\>       |       |F      |
|18 |                       |\<labels-list\>            |16     |F      |
|19 |\<statements-list\>    |\<statement\>               |if (lexem == "GOTO"\|\| lexem == "LINK" \|\| lexem == "IN" \|\| lexem == "OUT" \|\| (lexem is \<unsigned-integer\> && lexem + 1 == ":" ) ) goto 21 else return T   |return F <br >if returned <br> from below F|      
|20 |                       |\<statements-list\>        |19     |F      |
|21 |\<statement\>          |\<unsigned-integer\>       |       |F      |
|22 |                       |:                          |       |F      |
|23 |                       |\<statement\>              |21     |F      |
|24 |                       |GOTO                       |       |F      |
|25 |                       |\<unsigned-integer\>       |       |F      |
|26 |                       |;                          |T      |F      |
|27 |                       |LINK                       |       |F      |
|28 |                       |\<varialbe-identifier\>    |       |F      |
|29 |                       |,                          |       |F      |
|30 |                       |\<unsigned-integer\>       |       |F      |
|31 |                       |;                          |T      |F      |
|32 |                       |IN                         |       |F      |
|33 |                       |\<unsigned-integer\>       |       |F      |
|34 |                       |;                          |T      |F      |
|35 |                       |OUT                        |       |F      |
|36 |                       |\<unsigned-integer\>       |       |F      |
|37 |                       |;                          |T      |F      |
|38 |\<variable-identifier\>|\<identifier\>             |T      |F      |
|39 |\<procedure-identifier\>|\<identifier\>            |T      |F      |

