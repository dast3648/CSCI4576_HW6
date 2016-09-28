# Daniel Strawn
# CSCI-4576
# Makefile

CC			=	mpicc
CXX			=	mpicxx

IFLAGS		=	./include ./csvparser.c
CFLAGS		=	-g -Wall -std=c99 
CXXFLAGS	=	-DMPICH_IGNORE_CXX_SEEK

all:			StrawnDaniel_HW6-1 StrawnDaniel_HW6-2 StrawnDaniel_HW6-3

C_FILES		=	StrawnDaniel_HW6-1.c 
O1_FILES	=	StrawnDaniel_HW6-1.o 
StrawnDaniel_HW6-1: $(O1_FILES)
	$(CC) -I $(IFLAGS) -o StrawnDaniel_HW6-1 $(O1_FILES) -lm

C_FILES		=	StrawnDaniel_HW6-2.c 
O2_FILES	=	StrawnDaniel_HW6-2.o 
StrawnDaniel_HW6-2: $(O2_FILES)
	$(CC) -I $(IFLAGS) -o StrawnDaniel_HW6-2 $(O2_FILES) -lm

C_FILES		=	StrawnDaniel_HW6-3.c 
O3_FILES	=	StrawnDaniel_HW6-3.o 
StrawnDaniel_HW6-3: $(O3_FILES)
	$(CC) -I $(IFLAGS) -o StrawnDaniel_HW6-3 $(O3_FILES) -lm

.PHONY:	clean
clean:
	/bin/rm -f core $(O1_FILES) StrawnDaniel_HW6-1
	/bin/rm -f core $(O2_FILES) StrawnDaniel_HW6-2
	/bin/rm -f core $(O3_FILES) StrawnDaniel_HW6-3
