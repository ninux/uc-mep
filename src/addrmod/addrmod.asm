E1:	EQU	02
E2:	EQU	$CC

Sec1:	SECTION
C1:	    DC	    1,2,3,4
C2:	    DC.W	2500
C3:	    DC.B	$AA

Sec2:	SECTION
S1:	    DS	    $1A
S2:	    DS.W	2
S3:	    DS.B	1

Sec3:	SECTION
	                    ; Addr-Mod	    Operand		    Resultat
	    LDA	#E1	        ; Immediate	    02		        A = 0x02
	    STA	E1+E2	    ; Direct	    02 + $CC = $CE	{$CE} = 0x02
	    LDA	S3	        ; Extended	    $CE		        A = 0x02
	    INCA		    ; Inherent	    -		        A = 0x02 + 1 = 0x03
	    STA	S3+1	    ; Extended	    $CE + 1 = $CF	{$CF} = 0x03
	    LDHX		    ; Extended	    $CE		        {H:X} = 0x02:0x03
	    STA	E1,X	    ; Indexed IX1   02		        {$05} = 0x03
EndPrg:	BRA	EndPrg	    ; Relative	    -2 = 0xFE	    PC = PC - 2
