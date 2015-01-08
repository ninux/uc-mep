;--------------------------------------------------------------------
;          HSLU T&A Hochschule Luzern Technik+Architektur            
;--------------------------------------------------------------------
;
; \brief         Template for HCS08 Assembler
; \file
; \author        J.Wassner/C.Jost/L.Kressebuch
; \date          10.10.2012
;
; \b Language:   Assembler \n\n
; \b Target:     MC-CAR \n
;
; \par Description:
;
; $Id: main.asm 910 2014-09-22 07:41:25Z chj-hslu $
;--------------------------------------------------------------------
                include 'derivative.inc'
                

; Global definierte Symbole sind von aussen sichtbar.
;--------------------------------------------------------------------
                GLOBAL      _Startup    ; Start Hauptprogramm
                GLOBAL      main
                GLOBAL      dummy       ; Dummy Interrupt Service Routine

; EQUATIONS
;--------------------------------------------------------------------
StackSize:      EQU     $60             ; Equation for the stack size
pi:             EQU     31416           ; Example for a constant value

; STACK, VARIABLEN (Datenspeicher) und KONSTANTEN (Festwertspeicher) 
;--------------------------------------------------------------------
DATEN_STACK:    SECTION
TofStack:       DS      StackSize-1     ; definiton of "Top of Stack"
BofStack:       DS      1               ; definition of "Bottom of Stack"

DATEN:          SECTION
Variable1:      DS      1               ; Example of a 1 Byte Variable
Array1:         DS      $20             ; Example of an Array of $20 Bytes

KONSTANTEN:     SECTION
Maske1:         DC.B    %00000001
Parameter1:     DC.B    $3A             ; DC mit Punkt-> Coloring geht nicht!
Parameter2:     DC.W    57100
Reserve_Par:    DS      16              ; nur leerer Bereich reservieren!
STRING1:        DC.B    10,"Hello",$0D

; Programmstart (Initialisierungen)
;--------------------------------------------------------------------
PROGRAMM:       SECTION                 ; Code Segment
_Startup:                               ; Resetvektor zeigt hierhin
Stackinit:      LDHX    #(BofStack+1)
                TXS                     ; TXS dekrementiert, deshalb oben +1     
                LDA     #$00
                STA     SOPT1           ; Disable Watchdog

; Start des Benutzer-Codes
;--------------------------------------------------------------------       
main:		; configure the LED FL pins
		LDA	PTFDD		; read Data-Direction F
		ORA	#$01		; set pin 0 = OUT (LED B FL)
		ORA	#$02		; set pin 1 = OUT (LED R FL)
		STA	PTFDD		; write Data-Direction F
		
		LDA	PTCDD		; read Data-Direction C
		ORA	#$0F		; set pin 4 = OUT (LED G FL)
		STA	PTCDD		; write Data-Direction C
		
		; set the pin states
		LDA	PTFD		; read Data-Register F
		AND	#$FE		; set pin 0 = LOW (B FL = ON)
		ORA	#$02		; set pin 1 = HIGH (R FL = OFF)
		STA	PTFD		; write Data-Register F
		
		LDA	PTCD		; read Data-Register C
		ORA	#$0F		; set pin 4 = HIGH (R FL = OFF)
		STA	PTCD		; write Data-Register C
		

EndLoop:        BRA     *               ; Endlos-Loop
                




;--------------------------------------------------------------------
; dummy - Alle nicht benutzten Interruptvektoren werden hierher
;         geleitet damit kein Absturz passiert, wenn ein unerwarteter
;         Interrupt auftritt. (z.B. loss of lock vom ICG)
;--------------------------------------------------------------------
dummy:          BGND
                BRA     dummy
