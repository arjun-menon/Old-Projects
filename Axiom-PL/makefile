axiom: main.o lexer.o exec.o userobj.o datatypes.o
	g++ -o axiom main.o lexer.o exec.o userobj.o datatypes.o
main.o: main.cpp common.h base.hpp datatypes.hpp userobj.hpp lexer.hpp
	g++ -c main.cpp
lexer.o: lexer.cpp common.h base.hpp datatypes.hpp userobj.hpp lexer.hpp
	g++ -c lexer.cpp
exec.o: exec.cpp common.h base.hpp datatypes.hpp userobj.hpp lexer.hpp
	g++ -c exec.cpp
userobj.o: userobj.cpp common.h base.hpp datatypes.hpp userobj.hpp lexer.hpp
	g++ -c userobj.cpp
datatypes.o: datatypes.cpp common.h base.hpp datatypes.hpp userobj.hpp lexer.hpp
	g++ -c datatypes.cpp