ENABLE_ASSERTIONS=	1
INCLUDE_SYMBOLS	=	1

# Can be blank, or -O2, etc..
COPTIMIZE_FLAGS	=	

#C++-Compiler
CPlusPlus=		g++
#CPlusPlus=		gcc
#CPlusPlus=		g++-4.6
#CPlusPlus=		g++ -V4.5
#CPlusPlus=		g++ -V4.6

#Linker-Driver
Linker	=	$(CPlusPlus)