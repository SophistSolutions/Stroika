

; ---------------------------------------------------------------------
;
; QTMACROS.INC - QuickTime for Windows Helper Macros
;
;                Version 1.1
;
;                (c) Copyright 1988-1994 Apple Computer, Inc. All Rights Reserved.
;
; ---------------------------------------------------------------------


COMMENT @
Video dispatch codes
@
VDSP_SETBANK     EQU  1
VDSP_SLIDEWINDOW EQU  2
VDSP_SAVECONTEXT EQU  3
VDSP_RESTCONTEXT EQU  4
VDSP_SETTARGET   EQU  5
VDSP_IDENTIFY    EQU 21
VDSP_VERSION     EQU 22
VDSP_BANKTABLE   EQU 23
VDSP_BITBLTTYPE  EQU 24
VDSP_SCANWIDTH   EQU 25
VDSP_LINEAR_BUF  EQU 26
VDSP_TERMINATE   EQU 86

COMMENT @
VHDW selectors
@
kSniffVideoHardware     EQU 0
kVideoHardwareSetBounds EQU 1

COMMENT @
BitBlt types for hardware support
@
BBL_NONE         EQU  0           ; unknown BitBlt type
BBL_MOVSD        EQU  1           ; use MOVSD
BBL_DRVR         EQU  2           ; use the driver's BitBlt
BBL_MOVPL16      EQU  3           ; 16-bit planar

COMMENT @
BMP types
@
BMP_NONE         EQU  0           ; unknown type
BMP_DIB          EQU  1           ; DIB
BMP_MONO         EQU  2           ; monochrome
BMP_PACKED_4     EQU  3           ; packed 4 bit, e.g., Fahrenheit
BMP_PLANAR_4     EQU  4           ; VGA or SVGA
BMP_INDEX_8      EQU  5           ; palettized driver
BMP_5_5_5        EQU  6           ; 32,768 colors
BMP_5_6_5        EQU  7           ; XGA, Intel order
BMP_PLANAR_16    EQU  8           ; two planes of one byte each
BMP_8_8_8_RGB    EQU  9           ; true color RGB
BMP_MEMERR       EQU 10           ; insufficient memory for buffers
BMP_8_8_8_BGR    EQU 11           ; true color BGR
BMP_5_6_5_M      EQU 12           ; XGA, Motorola order
BMP_aRGB32       EQU 13           ; 32-bit with alpha channel
BMP_RGBa32       EQU 14           ; 32-bit with alpha channel
BMP_aBGR32       EQU 15           ; 32-bit with alpha channel
BMP_BGRa32       EQU 16           ; 32-bit with alpha channel

; note below-16:32 selectors are not the base of a
; selectorArray, therefore huge ptr arithmetic is not valid

BMP_IF09         EQU 17            ; YVU9 with difference bits (16:32 selector,linear frame buffer)
BMP_YVU9_PLANAR  EQU 18            ; YVU9 (16:32 selector,linear frame buffer)
BMP_YVU12_PLANAR EQU 19            ; YVU12 (16:32 selector,linear frame buffer)
BMP_YVU9_PACKED  EQU 20            ; YVU9 (16:32 selector,linear frame buffer)
BMP_YVU12_PACKED EQU 21            ; YVU12 (16:32 selector,linear frame buffer)
BMP_YUV_422      EQU 22            ; YUV_422 (16:32 selector,linear frame buffer)
BMP_YUV_411      EQU 23            ; YUV_411 (16:32 selector,linear frame buffer)

COMMENT @
Macros AlignBP and AlignBPRet:
Align BP on a DWORD boundary

On 386DX and 486 CPUs, applications get better performance by accessing
DWORD operands on DWORD boundaries.  Unaligned operands require an extra
bus cycle.  Our tests on various QuickTime assembler functions shows
unaligned operands typically add a 10% peformance penalty to the entire
function.  By being careful, we can ensure that WORD arguments and WORD
local variables occur in pairs, but we cannot guarantee the alignment of
BP, which is used to access these variables.  The alignment of BP is
determined at run time.  MSC 7.0 aligns the stack only on WORD
boundaries.

This macro ensures that BP is aligned on a DWORD boundary.  It is
intended only for use with NEAR PROCs that use the PASCAL calling
sequence.

For a NEAR PROC the function arguments and local variables are separated
by two WORDs, the return address and the caller's BP.  Thus, an aligned
BP optimizes accesses to both.  For a FAR PROC the function arguments
and local variables are separated by three WORDs, since the return
address is two WORDs.  In that case, we must define a dummy WORD as the
first local variable and misalign BP, so that the memory references will
be aligned.

When BP is not aligned, the macro aligns it, then moves the function
arguments, return address and saved BP accordingly.  The PASCAL
restriction allows the macro to find the byte size of the arguments in
the RET instruction.  The application must insert label "AlignBPRet"
immediately after one of its RET instructions.  The application must
also define a dummy WORD as the last local variable.

The macro assumes the direction indicator is 0 (forward), that the
application already saved SI and DI (via USES SI DI on the PROC
statement), and that ES and CX can be destroyed.

Upon exit from the function, we must adjust the stack pointer if BP was
changed at entry.  We accomplish this by storing in the extra word made
available at the top of the stack the caller's return address.  At the
location in the stack that normally holds the caller's return address,
we place an address in this program.  At the doctored return address, we
execute a near return.

Macro AlignBPExit uses a machine instruction for the near RET to prevent
the assembler from generating epilog code.
@
ifndef _QTW32_
AlignBP  MACRO
         LOCAL   AlreadyAligned
         TEST    BP, 2            ;; Already on DWORD boundary?
         JE      AlreadyAligned   ;; Skip if already aligned
         CMP     BYTE PTR AlignBPRet - 3, 0C2H ;; Expected RET instruction?
         JNE     AlreadyAligned   ;; Don't adjust if environment unknown
         MOV     CX, WORD PTR [AlignBPRet - 2];; Get byte size of parameter list
         SHR     CX, 1            ;; Convert byte count to word count
         ADD     CX, 2            ;; Account for return address and saved BP
         SUB     BP, 2            ;; Perform the alignment ourselves
                                  ;; Requires a word of slop after all local
                                  ;; variables
         PUSH    SS               ;; Copy SS to ES
         POP     ES               ;;
         MOV     DI, BP           ;; Set target pointer
         LEA     SI, [DI+2]       ;; Set source pointer
         CLD
         REP     MOVSW [DI],SS:[SI] ;; Move the arguments down
         MOV     AX, [BP+2]       ;; Get caller's return address
         MOV     ES:[DI], AX      ;; Store at top of stack
         MOV     [BP+2], OFFSET AlignFixSP ;; Substitute our return address
AlreadyAligned:
         ENDM
else
AlignBP  MACRO
         ENDM
endif

ifndef _QTW32_
AlignFarBP  MACRO
         LOCAL   AlreadyAligned
         TEST    BP, 2            ;; Already on DWORD boundary?
         JE      AlreadyAligned   ;; Skip if already aligned
         CMP     BYTE PTR AlignFarBPRet - 3, 0CAH ;; Expected RET instruction?
         JNE     AlreadyAligned   ;; Don't adjust if environment unknown
         MOV     CX, WORD PTR [AlignFarBPRet - 2];; Get byte size of parameter list
         SHR     CX, 1            ;; Convert byte count to word count
         ADD     CX, 3            ;; Account for return address:selector and saved BP
         SUB     BP, 6            ;; Perform the alignment ourselves
                                  ;; Requires 6 bytes of slop after all local
                                  ;; variables
         PUSH    SS               ;; Copy SS to ES
         POP     ES               ;;
         MOV     DI, BP           ;; Set target pointer
         LEA     SI, [DI+6]       ;; Set source pointer
         CLD
         REP     MOVSW [DI],SS:[SI] ;; Move the arguments down
         MOV     AX, [BP+2]       ;; Get caller's return address offset
         MOV     ES:[DI], AX      ;; Store at top of stack
         mov     ax,[bp+4]        ;; get caller's return address selector
         mov     es:[di+2], ax    ;; store caller's code selector
         MOV     [BP+2], OFFSET AlignFixSP ;; Substitute our return address
AlreadyAligned:
         ENDM
else
AlignFarBP MACRO
         ENDM
endif

AlignBPExit MACRO
ifndef _QTW32_
AlignBPRet:                       ;; Immediately follow the RETN immed
         ALIGN   16               ;; Align for better performance
AlignFixSP:
         RETN                     ;; Return to caller without epilog
endif
         ENDM

AlignFarBPExit MACRO
ifndef _QTW32_
AlignFarBPRet:                       ;; Immediately follow the RETF immed
         ALIGN   16               ;; Align for better performance
AlignFixSP:
         RETF                     ;; Return to caller without epilog
endif
         ENDM

COMMENT @
Macro BSWAPX:
Exchange the bytes in a DWORD register.

On a 486, the BSWAP instruction uses 1 clock instead of the 6 or 7
clocks required for this macro. Unfortunately, there is no cheap
inline way at run time to distinguish between a 386 and 486.
@
BSWAPX   MACRO   reg:REQ
Root     SUBSTR  <reg>, 2, 1
Xchg8L    CATSTR  Root, <L>
Xchg8H    CATSTR  Root, <H>
         xchg    Xchg8L,Xchg8H
         ROL     reg, 16
         xchg    Xchg8L,Xchg8H
         ENDM

COMMENT @
Macro to initialize dirty list, initialize bank bounds table, set GS:0
to bank bounds table, set ES:EDI to initial hardware target address,
set the bank for banked adapters.
@
ifndef _QTW32_
SetHdwTarget MACRO
         LGS     DI, BankBds      ;; Point to bank bounds table
         MOV     DI, GS:[DI]      ;; Get residual block count of first bad row
         MOV     NextBadBlock, DI ;; Save the residual block count to match

         MOV     AX, WORD PTR OffScreen ;; Point to dirty list
         REPEAT  BPPT * Mag
         ADD     AX, BWIDTH       ;; Allocate room for offscreen pixels
         ENDM                     ;; of REPEAT  BPPT * Mag
         IF      Mag EQ 2
         ADD     AX, WdBytes      ;; Allocate room for second scan line in pair
         ENDIF
         INC     AX               ;; Round up to even offset for efficiency
         AND     AL, 0FEH         ;;
         ADD     AX, 4            ;; Leave room for post-processing
         MOV     DirtyListFirst, AX ;;
         MOV     AX, DESTY        ;; Get starting row
         MUL     WdBytes          ;; Multiply by width of adapter scan line
         REPEAT  BPPT             ;; Bytes per target pixel
         ADD     AX, DESTX        ;; Add in destination column
         ADC     DX, 0            ;; Bump bank number if carry
         ENDM                     ;; of REPEAT BPPT
         PUSH    DX               ;; Push high order word
         PUSH    AX               ;; Push low order word
         PUSH    VDSP_SETTARGET   ;; Dispatch code for SetTarget
         CALL    [Hardware]       ;; Set the initial bank, also ES:DI
         ADD     SP, 6            ;; Remove arguments from the stack
         MOV     Bank, AX         ;; Save the returned bank
         MOV     SelVRAM, ES      ;; Save VRAM selector
         ENDM                     ;; of SetHdwTarget MACRO
else
SetHdwTarget MACRO
         MOV     EDI, BankBds     ;; Point to bank bounds table
         MOVZX   EDI, WORD PTR [EDI] ;; Get residual block count of first bad row
         MOV     NextBadBlock, EDI;; Save the residual block count to match

         MOV     EAX, DWORD PTR OffScreen ;; Point to dirty list
         MOVZX   EDX, BWIDTH
         REPEAT  BPPT * Mag
         ADD     EAX, EDX         ;; Allocate room for offscreen pixels
         ENDM                     ;; of REPEAT  BPPT * Mag
         IF      Mag EQ 2
         ADD     EAX, WdBytes     ;; Allocate room for second scan line in pair
         ENDIF
         INC     EAX              ;; Round up to even offset for efficiency
         AND     AL, 0FEH         ;;
         ADD     EAX, 4           ;; Leave room for post-processing
         MOV     DirtyListFirst, EAX ;;
         MOVZX   EAX, DESTY        ;; Get starting row
         MUL     WdBytes           ;; Multiply by width of adapter scan line
         MOVZX   EDX, DESTX
         REPEAT  BPPT             ;; Bytes per target pixel
         ADD     EAX, EDX        ;; Add in destination column
         ENDM                     ;; of REPEAT BPPT
         PUSH    EAX               ;; Push target address
         PUSH    VDSP_SETTARGET   ;; Dispatch code for SetTarget
         CALL    [Hardware]       ;; Set the initial bank, also EDI
         ADD     ESP, 8            ;; Remove arguments from the stack
         MOV     Bank, EAX
         ENDM                     ;; of SetHdwTarget MACRO
endif

COMMENT @
Bank change macros:
Used by functions that write directly to video adapter hardware.

The bank change macros assume that all bank changes are of size 1.  This
is so, since the program moves at most 4 rows at a time and each bank holds
at least 40 rows.
@
DoBankUp MACRO
         INC     Bank             ;; Go to next bank
         PUSH    Bank             ;; Pass argument to function
         PUSH    VDSP_SETBANK     ;; Dispatch code for SetBank
         CALL    [Hardware]       ;; Set the new bank
ifndef _QTW32_
         ADD     SP, 4            ;; Remove arguments from the stack
else
         ADD     ESP, 8           ;; Remove arguments from the stack
endif
         ENDM

CheckBankUp MACRO  nbr:REQ        ;; Check bank, jumping only when the bank
         JC      NewBankUp&nbr&   ;;  changes
NewBankUp&nbr&Ret:
         ENDM

SetBankUp MACRO  nbr:REQ          ;; Effect a bank change
         ALIGN   16
NewBankUp&nbr&:
         DoBankUp
         JMP     NewBankUp&nbr&Ret;; Return to main code
         ENDM

DoBankDn MACRO
         DEC     Bank             ;; Go to previous bank
         PUSH    Bank             ;; Pass argument to function
         PUSH    VDSP_SETBANK     ;; Dispatch code for SetBank
         CALL    [Hardware]       ;; Set the new bank
ifndef _QTW32_
         ADD     SP, 4            ;; Remove arguments from the stack
else
         ADD     ESP, 8           ;; Remove arguments from the stack
endif
         ENDM

CheckBankDn MACRO  nbr:REQ        ;; Check bank, jumping only when the bank
         JC      NewBankDn&nbr&   ;;  changes
NewBankDn&nbr&Ret:
         ENDM

SetBankDn MACRO  nbr:REQ          ;; Effect a bank change
         ALIGN   16
NewBankDn&nbr&:
         DoBankDn
         JMP     NewBankDn&nbr&Ret;; Return to main code
         ENDM

SlideWindow MACRO
         PUSH    Bank             ;; SlideWindow may adjust this
         PUSH    VDSP_SLIDEWINDOW ;; Dispatch code for SlideWindow
         CALL    [Hardware]       ;; Call the function
ifndef _QTW32_
         ADD     SP, 4            ;; Remove arguments from the stack
                                  ;; AX = bank, ES:DI may be changed
         MOV     Bank, AX         ;; Bank number may have changed
else
         ADD     ESP, 8           ;; Remove arguments from the stack
                                  ;; AX = bank, ES:DI may be changed
         MOV     Bank, EAX        ;; Bank number may have changed
endif
         ENDM

COMMENT @
Macros used with downward dithers from 24 bit sources

Get554 is used with 8 bit targets.  If the 2nd parameter (tableBase) is
not present, then the resulting 5/5/4 pixel is placed in EBX, where it
can then be used as an index into a dither table.  If tableBase *is*
present, the resulting 5/5/4 pixel is placed in EBX, and tableBase is
added to it.  This is particularly useful in flat-model 32-bit code,
where the base of the table is not in a segment register.

Get555 is used with 15 bit (5-5-5) targets.

Get565 and Get565M are used with 16 bit (5-6-5) targets.
@
Get554   MACRO   offst:REQ, tableBase             ;;
         XOR     EBX, EBX              ;; some 16-bit code uses EBX as index
         MOV     BH, [ESI+offst]       ;; Get R
         SHR     BH, 3                 ;; Save 5 bits of R
         MOV     BL, [ESI+offst+1]     ;; Get G
         SHL     EBX, 5                ;; Save 5 bits of G
         MOV     BL, [ESI+offst+2]     ;; Get B
         SHR     EBX, 4                ;; Get 5-5-4
         IFNB <tableBase>
             ADD     EBX, tableBase
         ENDIF
         ENDM

Get555   MACRO   offst:REQ
         MOV     AH, [ESI+offst]       ;; Get R
         SHR     AH, 3                 ;; Save 5 bits of R
         MOV     AL, [ESI+offst+1]     ;; Get G
         SHL     EAX, 5                ;; Save 5 bits of G
         MOV     AL, [ESI+offst+2]     ;; Get B
         SHR     EAX, 3                ;; Get 5-5-5
         ENDM

Get565   MACRO   offst:REQ
         MOV     AH, [ESI+offst]       ;; Get R
         SHR     AH, 3                 ;; Save 5 bits of R
         MOV     AL, [ESI+offst+1]     ;; Get G
         SHL     EAX, 6                ;; Save 6 bits of G
         MOV     AL, [ESI+offst+2]     ;; Get B
         SHR     EAX, 3                ;; Get 5-6-5
         ENDM

Get565M  MACRO   offst:REQ
         Get565  offst                 ;; Get 5-6-5 in Intel order
         XCHG    AH,AL                 ;; Put bytes in Motorola order
         ENDM

COMMENT @
Utility macros

The macro assumes CX bytes will be moved from DS:SI to ES:DI, and that
DX is available.
@
@Equals  MACRO   p1:REQ, p2:REQ
         EXITM   %( @InStr(,p1,p2) * @InStr(,p2,p1))
         ENDM                     ;; of @Equals MACRO

CopyBytes MACRO
         CLD
         MOV     DX, CX           ;; Save byte count
         SHR     CX, 2            ;; Byte count to DWORD count
         REP     MOVSD            ;; Copy most of the bytes
         MOV     CX, DX           ;; Restore byte count
         AND     CX, 3            ;; 0-3 bytes left
         SHR     CX, 1            ;; Check for 2 or more bytes left
         REP     MOVSW            ;; Copy stray WORD, if any
         ADC     CX, CX           ;; 0-1 bytes left
         REP     MOVSB            ;; Copy stray byte, if any
         ENDM                     ;; of CopyBytes MACRO

COMMENT @
Macros used with decompressors that write directly to video adapter
hardware.
@
GoOnScreenX MACRO
         ALIGN   16
GoOnScreen:
ifndef _QTW32_
         PUSH    CX               ;; Save register
         MOV     CX, DirtyListNext     ;; Offset of next dirty list entry
         SUB     CX, DirtyListFirst    ;; 4 * number of dirty list entries
         JLE     NotDirty         ;; Skip if dirty list is empty

         PUSH    BX               ;; Save registers
         PUSH    DX               ;;
         PUSH    SI               ;;
         PUSH    DI               ;;
         PUSH    DS               ;;
         PUSH    ES               ;;

         SHR     CX, 2            ;; Number of dirty list entries
         MOV     DX, BadStart     ;; Onscreen offset of line start
         NEG     DX               ;; Offset in offscreen buffer of bank split
                                  ;; Merge adjacent ranges
                                  ;; Split a range that splits a bank
         MOV     DS, WORD PTR OffScreen +2  ;; Offscreen buffer is the source
         MOV     SI, DirtyListFirst         ;; Point to first dirty list entry
         LEA     DI, [SI-4]                 ;; Allow room for one split entry
LoopNewRange:
         MOV     AX, [SI]         ;; Get start of range
         MOV     [DI], AX         ;; Save start of range
LoopSameRange:
         MOV     BX, [SI+2]       ;; Get end of range
         ADD     SI, 4            ;; Point to next source slot
         DEC     CX               ;; One fewer source slot
         JLE     RangeDone        ;; Skip if no more source slots
         CMP     BX, [SI]         ;; Is the next range adjacent to this?
         JB      RangeDone        ;; Skip if ranges are not adjacent
         JMP     LoopSameRange    ;; Keep combining adjacent ranges
RangeDone:
         CMP     AX, DX           ;; Might split occur in this range?
         JGE     NoSplit          ;; Skip if split does not occur in this range
         CMP     BX, DX           ;; Does split occur in this range?
         JLE     NoSplit          ;; Skip if split does not occur in this range
         MOV     [DI+2], DX       ;;
         ADD     DI, 4            ;; Point to next target slot
         MOV     [DI], DX         ;; Start of new range
NoSplit:
         MOV     [DI+2], BX       ;; Set right edge of range
         ADD     DI, 4            ;; Point to next target slot
         TEST    CX, CX           ;; Any source slots left?
         JG      LoopNewRange     ;; Loop while source slots remain
         OR      WORD PTR [DI], -1;; Terminate the list
                                  ;; Make offsets relative to prior entries
                                  ;; Convert ending offsets to byte sizes
         LEA     CX, [DI+4]       ;; Compute number of list entries
         SUB     CX, DirtyListFirst    ;; 4 * number of WORDs in the list
         LEA     BX, [DI-2]       ;; Point to last WORD in list
         SHR     CX, 1            ;; 2 * number of WORDs in list
         DEC     CX               ;; Don't process first WORD
LoopDiff:
         MOV     AX, [BX-2]       ;; Get previous WORD in list
         SUB     [BX], AX         ;; Compute difference from previous WORD
         SUB     BX, 2            ;; Back up one WORD
         DEC     CX               ;; One fewer WORD
         JG      LoopDiff         ;; Loop once for each WORD in list
                                  ;; Copy the first line of the pair
         MOV     ES, SelVRAM      ;; VRAM is the target
         MOV     DI, BadStart     ;;
         MOV     SI, WORD PTR OffScreen     ;; Offscreen buffer is the source
LoopCopy:
         MOV     AX, [BX]         ;; Get offset of range start
         TEST    AX, AX           ;; End of the list?
         JL      DirtyDone        ;; Skip if end of the list
         MOV     CX, [BX+2]       ;; Get number of bytes in the range
         ADD     BX, 4            ;; Point to next list entry
         ADD     SI, AX           ;; Locate data in offscreen buffer
         ADD     DI, AX           ;; Position pointer in onscreen buffer
         CheckBankUp 01E          ;; Change bank if necessary
         CopyBytes                ;; Copy the bytes
         TEST    DI, DI           ;; Did range end on a bank boundary?
         JZ      NewBankUp02E     ;; Skip if range ended on a bank boundary
NewBankUp02ERet:
         JMP     LoopCopy         ;; Loop once for each range

         ALIGN   16
DirtyDone:
         TEST    DI, DI           ;; Did we cross the bank boundary?
         JGE     DirtyCrossed     ;; Skip if we already crossed
         DoBankUp                 ;; Cross the bank boundary
DirtyCrossed:
         POP     ES               ;; Restore registers
         POP     DS               ;;
         POP     DI               ;;
         POP     SI               ;;
         POP     DX               ;;
         POP     BX               ;;
         POP     CX               ;;
         RETN                     ;; Return to caller without epilog

         ALIGN   16
NotDirty:
         POP     CX               ;; Restore register
         DoBankUp                 ;; Go to next bank
         RETN                     ;; Return to caller without epilog
else
         PUSH    ECX              ;; Save register
         MOV     ECX, DirtyListNext    ;; Offset of next dirty list entry
         SUB     ECX, DirtyListFirst   ;; 4 * number of dirty list entries
         JLE     NotDirty         ;; Skip if dirty list is empty

         PUSH    EBX              ;; Save registers
         PUSH    EDX              ;;
         PUSH    ESI              ;;
         PUSH    EDI              ;;
         XOR     EAX, EAX
         XOR     EBX, EBX

         SHR     ECX, 2           ;; Number of dirty list entries
         MOV     EDX, BadStart    ;; Onscreen offset of line start
         NEG     EDX              ;; Offset in offscreen buffer of bank split
                                  ;; Merge adjacent ranges
                                  ;; Split a range that splits a bank
;         MOV     DS, WORD PTR OffScreen +2 ;; Offscreen buffer is the source
         MOV     ESI, DirtyListFirst        ;; Point to first dirty list entry
         LEA     EDI, [ESI-4]               ;; Allow room for one split entry
LoopNewRange:
         MOV     AX, [ESI]        ;; Get start of range
         MOV     [EDI], AX        ;; Save start of range
LoopSameRange:
         MOV     BX, [ESI+2]      ;; Get end of range
         ADD     ESI, 4           ;; Point to next source slot
         DEC     CX               ;; One fewer source slot
         JLE     RangeDone        ;; Skip if no more source slots
         CMP     BX, [ESI]        ;; Is the next range adjacent to this?
         JB      RangeDone        ;; Skip if ranges are not adjacent
         JMP     LoopSameRange    ;; Keep combining adjacent ranges
RangeDone:
         CMP     AX, DX           ;; Might split occur in this range?
         JGE     NoSplit          ;; Skip if split does not occur in this range
         CMP     BX, DX           ;; Does split occur in this range?
         JLE     NoSplit          ;; Skip if split does not occur in this range
         MOV     [EDI+2], DX      ;;
         ADD     EDI, 4           ;; Point to next target slot
         MOV     [EDI], DX        ;; Start of new range
NoSplit:
         MOV     [EDI+2], BX      ;; Set right edge of range
         ADD     EDI, 4           ;; Point to next target slot
         TEST    CX, CX           ;; Any source slots left?
         JG      LoopNewRange     ;; Loop while source slots remain
         OR      WORD PTR [EDI], -1;; Terminate the list
                                  ;; Make offsets relative to prior entries
                                  ;; Convert ending offsets to byte sizes
         LEA     ECX, [EDI+4]     ;; Compute number of list entries
         SUB     ECX, DirtyListFirst   ;; 4 * number of WORDs in the list
         LEA     EBX, [EDI-2]     ;; Point to last WORD in list
         SHR     ECX, 1           ;; 2 * number of WORDs in list
         DEC     ECX              ;; Don't process first WORD
LoopDiff:
         MOV     AX, [EBX-2]      ;; Get previous WORD in list
         SUB     [EBX], AX        ;; Compute difference from previous WORD
         SUB     EBX, 2           ;; Back up one WORD
         DEC     CX               ;; One fewer WORD
         JG      LoopDiff         ;; Loop once for each WORD in list
                                  ;; Copy the first line of the pair
;         MOV     ES, SelVRAM     ;; VRAM is the target
         MOV     EDI, BadStart    ;;
         MOV     ESI, DWORD PTR OffScreen     ;; Offscreen buffer is the source
LoopCopy:
         MOV     AX, [EBX]        ;; Get offset of range start
         TEST    AX, AX           ;; End of the list?
         JL      DirtyDone        ;; Skip if end of the list
         MOV     CX, [EBX+2]      ;; Get number of bytes in the range
         ADD     EBX, 4           ;; Point to next list entry
         MOVZX   EAX, AX
         ADD     ESI, EAX         ;; Locate data in offscreen buffer
         ADD     EDI, EAX         ;; Position pointer in onscreen buffer
         CheckBankUp 01E          ;; Change bank if necessary
         CopyBytes                ;; Copy the bytes
         TEST    EDI, EDI         ;; Did range end on a bank boundary?
         JZ      NewBankUp02E     ;; Skip if range ended on a bank boundary
NewBankUp02ERet:
         JMP     LoopCopy         ;; Loop once for each range

         ALIGN   16
DirtyDone:
         TEST    EDI, EDI         ;; Did we cross the bank boundary?
         JGE     DirtyCrossed     ;; Skip if we already crossed
         DoBankUp                 ;; Cross the bank boundary
DirtyCrossed:
         POP     EDI              ;;
         POP     ESI              ;;
         POP     EDX              ;;
         POP     EBX              ;;
         POP     ECX              ;;
         RETN                     ;; Return to caller without epilog

         ALIGN   16
NotDirty:
         POP     ECX              ;; Restore register
         DoBankUp                 ;; Go to next bank
         RETN                     ;; Return to caller without epilog
endif
         ENDM                     ;; of GoOnScreenX

GoOnScreen2x MACRO
         ALIGN   16
GoOnScreen:
ifndef _QTW32_
         PUSH    CX               ;; Save register
         MOV     CX, DirtyListNext     ;; Offset of next dirty list entry
         SUB     CX, DirtyListFirst    ;; 4 * number of dirty list entries
         JLE     NotDirty         ;; Skip if dirty list is empty

         PUSH    BX               ;; Save registers
         PUSH    DX               ;;
         PUSH    SI               ;;
         PUSH    DI               ;;
         PUSH    DS               ;;
         PUSH    ES               ;;

         SHR     CX, 2            ;; Number of dirty list entries
         MOV     BX, BadStart     ;; Onscreen offset of line start
         MOV     DX, BX           ;; Copy onscreen offset of line start
         NEG     DX               ;; Offset in offscreen buffer of bank split?
         ADD     BX, WdBytes      ;; Onscreen offset of second line in pair
         JC      FirstLineSplit   ;; Skip if first line in pair has bank split
         MOV     DX, BX           ;; Copy onscreen offset of line start
         NEG     DX               ;; Offset in offscreen buffer of bank split
FirstLineSplit:
                                  ;; Merge adjacent ranges
                                  ;; Split a range that splits a bank
         MOV     DS, WORD PTR OffScreen +2  ;; Offscreen buffer is the source
         MOV     SI, DirtyListFirst         ;; Point to first dirty list entry
         LEA     DI, [SI-4]                 ;; Allow room for one split entry
LoopNewRange:
         MOV     AX, [SI]         ;; Get start of range
         MOV     [DI], AX         ;; Save start of range
LoopSameRange:
         MOV     BX, [SI+2]       ;; Get end of range
         ADD     SI, 4            ;; Point to next source slot
         DEC     CX               ;; One fewer source slot
         JLE     RangeDone        ;; Skip if no more source slots
         CMP     BX, [SI]         ;; Is the next range adjacent to this?
         JB      RangeDone        ;; Skip if ranges are not adjacent
         JMP     LoopSameRange    ;; Keep combining adjacent ranges
RangeDone:
         CMP     AX, DX           ;; Might split occur in this range?
         JGE     NoSplit          ;; Skip if split does not occur in this range
         CMP     BX, DX           ;; Does split occur in this range?
         JLE     NoSplit          ;; Skip if split does not occur in this range
         MOV     [DI+2], DX       ;;
         ADD     DI, 4            ;; Point to next target slot
         MOV     [DI], DX         ;; Start of new range
NoSplit:
         MOV     [DI+2], BX       ;; Set right edge of range
         ADD     DI, 4            ;; Point to next target slot
         TEST    CX, CX           ;; Any source slots left?
         JG      LoopNewRange     ;; Loop while source slots remain
                                  ;;
         OR      WORD PTR [DI], -1;; Terminate the list
                                  ;; Make offsets relative to prior entries
                                  ;; Convert ending offsets to byte sizes
         LEA     CX, [DI+4]       ;; Compute number of list entries
         SUB     CX, DirtyListFirst    ;; 4 * number of WORDs in the list
         LEA     BX, [DI-2]       ;; Point to last WORD in list
         SHR     CX, 1            ;; 2 * number of WORDs in list
         DEC     CX               ;; Don't process first WORD
LoopDiff:
         MOV     AX, [BX-2]       ;; Get previous WORD in list
         SUB     [BX], AX         ;; Compute difference from previous WORD
         SUB     BX, 2            ;; Back up one WORD
         DEC     CX               ;; One fewer WORD
         JG      LoopDiff         ;; Loop once for each WORD in list

                                  ;; Copy the first line of the pair
         MOV     ES, SelVRAM      ;; VRAM is the target
         MOV     DI, BadStart     ;;
         MOV     SI, WORD PTR OffScreen     ;; Offscreen buffer is the source
LoopCopy1:
         MOV     AX, [BX]         ;; Get offset of range start
         TEST    AX, AX           ;; End of the list?
         JL      DirtyDone1       ;; Skip if end of the list
         MOV     CX, [BX+2]       ;; Get number of bytes in the range
         ADD     BX, 4            ;; Point to next list entry
         ADD     SI, AX           ;; Locate data in offscreen buffer
         ADD     DI, AX           ;; Position pointer in onscreen buffer
         CheckBankUp 01E          ;; Change bank if necessary
         CopyBytes                ;; Copy the bytes
         TEST    DI, DI           ;; Did range end on a bank boundary?
         JZ      NewBankUp02E     ;; Skip if range ended on a bank boundary
NewBankUp02ERet:
         JMP     LoopCopy1        ;; Loop once for each range

         ALIGN   16
                                  ;; Copy the second line of the pair
DirtyDone1:
         MOV     AX, BadStart     ;; Onscreen offset of first line in pair
         ADD     AX, WdBytes      ;; Onscreen offset of second line in pair
         SUB     AX, DI           ;; Offset from current target pointer
         ADD     DI, AX           ;; Add the offset back in
         CheckBankUp 03E          ;; Change bank if necessary
         MOV     SI, WORD PTR OffScreen     ;; Offscreen buffer is the source
         MOV     BX, DirtyListFirst         ;; Point to first dirty list entry
         SUB     BX, 4                      ;; Post-processed first entry
LoopCopy2:
         MOV     AX, [BX]         ;; Get offset of range start
         TEST    AX, AX           ;; End of the list?
         JL      DirtyDone2       ;; Skip if end of the list
         MOV     CX, [BX+2]       ;; Get number of bytes in the range
         ADD     BX, 4            ;; Point to next list entry
         ADD     SI, AX           ;; Locate data in offscreen buffer
         ADD     DI, AX           ;; Position pointer in onscreen buffer
         CheckBankUp 04E          ;; Change bank if necessary
         CopyBytes                ;; Copy the bytes
         TEST    DI, DI           ;; Did range end on a bank boundary?
         JZ      NewBankUp05E     ;; Skip if range ended on a bank boundary
NewBankUp05ERet:
         JMP     LoopCopy2        ;; Loop once for each range

         ALIGN   16
DirtyDone2:
         TEST    DI, DI           ;; Did we cross the bank boundary?
         JGE     DirtyCrossed2    ;; Skip if we already crossed
         DoBankUp                 ;; Cross the bank boundary
DirtyCrossed2:
         POP     ES               ;; Restore registers
         POP     DS               ;;
         POP     DI               ;;
         POP     SI               ;;
         POP     DX               ;;
         POP     BX               ;;
         POP     CX               ;;
         RETN                     ;; Return to caller without epilog

         ALIGN   16
NotDirty:
         POP     CX               ;; Restore register
else
         PUSH    ECX              ;; Save register
         MOV     ECX, DirtyListNext    ;; Offset of next dirty list entry
         SUB     ECX, DirtyListFirst   ;; 4 * number of dirty list entries
         JLE     NotDirty         ;; Skip if dirty list is empty

         PUSH    EBX              ;; Save registers
         PUSH    EDX              ;;
         PUSH    ESI              ;;
         PUSH    EDI              ;;

         SHR     ECX, 2           ;; Number of dirty list entries
         MOV     EBX, BadStart    ;; Onscreen offset of line start
         MOV     EDX, EBX         ;; Copy onscreen offset of line start
         NEG     EDX              ;; Offset in offscreen buffer of bank split?
         ADD     EBX, WdBytes     ;; Onscreen offset of second line in pair
         JC      FirstLineSplit   ;; Skip if first line in pair has bank split
         MOV     EDX, EBX         ;; Copy onscreen offset of line start
         NEG     EDX              ;; Offset in offscreen buffer of bank split
FirstLineSplit:
                                  ;; Merge adjacent ranges
                                  ;; Split a range that splits a bank
;         MOV     DS, WORD PTR OffScreen +2  ;; Offscreen buffer is the source
         MOV     ESI, DirtyListFirst        ;; Point to first dirty list entry
         LEA     EDI, [ESI-4]               ;; Allow room for one split entry
LoopNewRange:
         MOV     AX, [ESI]        ;; Get start of range
         MOV     [EDI], AX        ;; Save start of range
LoopSameRange:
         MOV     EBX, [ESI+2]     ;; Get end of range
         ADD     ESI, 4           ;; Point to next source slot
         DEC     ECX              ;; One fewer source slot
         JLE     RangeDone        ;; Skip if no more source slots
         CMP     EBX, [ESI]       ;; Is the next range adjacent to this?
         JB      RangeDone        ;; Skip if ranges are not adjacent
         JMP     LoopSameRange    ;; Keep combining adjacent ranges
RangeDone:
         CMP     AX, DX           ;; Might split occur in this range?
         JGE     NoSplit          ;; Skip if split does not occur in this range
         CMP     BX, DX           ;; Does split occur in this range?
         JLE     NoSplit          ;; Skip if split does not occur in this range
         MOV     [EDI+2], DX      ;;
         ADD     EDI, 4           ;; Point to next target slot
         MOV     [EDI], DX        ;; Start of new range
NoSplit:
         MOV     [EDI+2], BX      ;; Set right edge of range
         ADD     EDI, 4           ;; Point to next target slot
         TEST    CX, CX           ;; Any source slots left?
         JG      LoopNewRange     ;; Loop while source slots remain
                                  ;;
         OR      WORD PTR [EDI], -1;; Terminate the list
                                  ;; Make offsets relative to prior entries
                                  ;; Convert ending offsets to byte sizes
         LEA     ECX, [EDI+4]     ;; Compute number of list entries
         SUB     ECX, DirtyListFirst   ;; 4 * number of WORDs in the list
         LEA     EBX, [EDI-2]     ;; Point to last WORD in list
         SHR     ECX, 1           ;; 2 * number of WORDs in list
         DEC     ECX              ;; Don't process first WORD
LoopDiff:
         MOV     AX, [EBX-2]      ;; Get previous WORD in list
         SUB     [EBX], AX        ;; Compute difference from previous WORD
         SUB     EBX, 2           ;; Back up one WORD
         DEC     CX               ;; One fewer WORD
         JG      LoopDiff         ;; Loop once for each WORD in list

                                  ;; Copy the first line of the pair
;         MOV     ES, SelVRAM      ;; VRAM is the target
         MOV     EDI, BadStart    ;;
         MOV     ESI, DWORD PTR OffScreen   ;; Offscreen buffer is the source
LoopCopy1:
         MOV     AX, [EBX]        ;; Get offset of range start
         TEST    AX, AX           ;; End of the list?
         JL      DirtyDone1       ;; Skip if end of the list
         MOV     CX, [EBX+2]      ;; Get number of bytes in the range
         ADD     EBX, 4           ;; Point to next list entry
         ADD     ESI, EAX         ;; Locate data in offscreen buffer
         ADD     EDI, EAX         ;; Position pointer in onscreen buffer
         CheckBankUp 01E          ;; Change bank if necessary
         CopyBytes                ;; Copy the bytes
         TEST    EDI, EDI         ;; Did range end on a bank boundary?
         JZ      NewBankUp02E     ;; Skip if range ended on a bank boundary
NewBankUp02ERet:
         JMP     LoopCopy1        ;; Loop once for each range

         ALIGN   16
                                  ;; Copy the second line of the pair
DirtyDone1:
         MOV     EAX, BadStart    ;; Onscreen offset of first line in pair
         ADD     EAX, WdBytes     ;; Onscreen offset of second line in pair
         SUB     EAX, EDI         ;; Offset from current target pointer
         ADD     EDI, EAX         ;; Add the offset back in
         CheckBankUp 03E          ;; Change bank if necessary
         MOV     ESI, DWORD PTR OffScreen   ;; Offscreen buffer is the source
         MOV     EBX, DirtyListFirst        ;; Point to first dirty list entry
         SUB     EBX, 4                     ;; Post-processed first entry
LoopCopy2:
         MOV     AX, [EBX]        ;; Get offset of range start
         MOVSX   EAX, AX
         TEST    EAX, EAX         ;; End of the list?
         JL      DirtyDone2       ;; Skip if end of the list
         MOV     CX, [EBX+2]      ;; Get number of bytes in the range
         ADD     EBX, 4           ;; Point to next list entry
         ADD     ESI, EAX         ;; Locate data in offscreen buffer
         ADD     EDI, EAX         ;; Position pointer in onscreen buffer
         CheckBankUp 04E          ;; Change bank if necessary
         CopyBytes                ;; Copy the bytes
         TEST    EDI, EDI         ;; Did range end on a bank boundary?
         JZ      NewBankUp05E     ;; Skip if range ended on a bank boundary
NewBankUp05ERet:
         JMP     LoopCopy2        ;; Loop once for each range

         ALIGN   16
DirtyDone2:
         TEST    EDI, EDI         ;; Did we cross the bank boundary?
         JGE     DirtyCrossed2    ;; Skip if we already crossed
         DoBankUp                 ;; Cross the bank boundary
DirtyCrossed2:
         POP     EDI              ;; Restore registers
         POP     ESI              ;;
         POP     EDX              ;;
         POP     EBX              ;;
         POP     ECX              ;;
         RETN                     ;; Return to caller without epilog

         ALIGN   16
NotDirty:
         POP     ECX              ;; Restore register
endif
         DoBankUp                 ;; Go to next bank
         RETN                     ;; Return to caller without epilog
         ENDM                     ;; of GoOnScreen2x

COMMENT @
Macro used to move pixels from onscreen buffer to offscreen buffer.
@
ifndef _QTW32_
GoOffScreenX MACRO
         LOCAL   NewBank
         LOCAL   AllFits
         LOCAL   ExitPath
         ALIGN   16
GoOffScreen:
         PUSH    DS               ;; Save register
         PUSH    CX               ;; Save register
         PUSH    DX               ;; Save register
         PUSH    SI               ;; Save register
         PUSH    DI               ;; Save register
         MOV     CX, AX           ;; Get byte count
         MOV     DS, SelVRAM      ;; Point to onscreen buffer
         MOV     SI, DI           ;; Copy target offset
         SUB     SI, WORD PTR OffScreen ;; Compute offset from start of buffer
         ADD     SI, BadStart     ;; Index into onscreen buffer
         JC      NewBank          ;; Skip if we crossed a bank boundary
         ADD     SI, CX           ;; Check ending source offset
         JNC     AllFits          ;; Skip if no bank change required
         JZ      AllFits          ;; Skip if no bank change required
         PUSH    SI               ;; Save byte count for new bank
         SUB     SI, CX           ;; Restore source pointer
         MOV     CX, SI           ;; Compute byte count for old bank
         NEG     CX               ;;
         CopyBytes                ;; Copy the bytes
         POP     CX               ;; Get byte count for new bank
NewBank:
         DoBankUp                 ;; Go to new bank
         CopyBytes                ;; Copy the bytes
         DoBankDn                 ;; Back to old bank
ExitPath:
         POP     DI               ;; Restore register
         POP     SI               ;; Restore register
         POP     DX               ;; Restore register
         POP     CX               ;; Restore register
         POP     DS               ;; Restore register
         RETN                     ;; Return to caller without epilog

         ALIGN   16
AllFits:                          ;; Source does not cross a bank boundary
         SUB     SI, CX           ;; Restore source pointer
         CopyBytes                ;; Copy the bytes
         JMP     ExitPath         ;; Go to common exit
         ENDM
else
GoOffScreenX MACRO
         LOCAL   NewBank
         LOCAL   AllFits
         LOCAL   ExitPath
         ALIGN   16
GoOffScreen:
         PUSH    ECX              ;; Save register
         PUSH    EDX              ;; Save register
         PUSH    ESI              ;; Save register
         PUSH    EDI              ;; Save register
         MOV     ECX, EAX         ;; Get byte count
         MOV     ESI, EDI         ;; Copy target offset
         SUB     ESI, DWORD PTR OffScreen ;; Compute offset from start of buffer
         ADD     ESI, BadStart    ;; Index into onscreen buffer
         JC      NewBank          ;; Skip if we crossed a bank boundary
         ADD     ESI, ECX         ;; Check ending source offset
         JNC     AllFits          ;; Skip if no bank change required
         JZ      AllFits          ;; Skip if no bank change required
         PUSH    ESI              ;; Save byte count for new bank
         SUB     ESI, ECX         ;; Restore source pointer
         MOV     ECX, ESI         ;; Compute byte count for old bank
         NEG     ECX              ;;
         CopyBytes                ;; Copy the bytes
         POP     ECX              ;; Get byte count for new bank
NewBank:
         DoBankUp                 ;; Go to new bank
         CopyBytes                ;; Copy the bytes
         DoBankDn                 ;; Back to old bank
ExitPath:
         POP     EDI              ;; Restore register
         POP     ESI              ;; Restore register
         POP     EDX              ;; Restore register
         POP     ECX              ;; Restore register
         RETN                     ;; Return to caller without epilog

         ALIGN   16
AllFits:                          ;; Source does not cross a bank boundary
         SUB     ESI, ECX         ;; Restore source pointer
         CopyBytes                ;; Copy the bytes
         JMP     ExitPath         ;; Go to common exit
         ENDM
endif

COMMENT @
Macro to compute number of source bytes processed, used for BMP banding
@
BytesUsed MACRO
ifndef _QTW32_
         XOR     EAX, EAX           ;; Zero out high order word
         MOV     AX, WORD PTR Inbuf ;; Get offset of source pointer
         SUB     ESI, EAX           ;; Compute number of source bytes processed
         SHLD    EDX, ESI, 16       ;; Put high order word in DX
         MOV     AX, SI             ;; Put low order word in AX
else
        MOV     EAX, DWORD PTR Inbuf ;; Where did we start
        SUB     ESI, EAX             ;; How far did we get
        MOV     EAX, ESI             ;; Return it
endif
         ENDM

COMMENT @
Macros for moving pixels to and from overscan area

Note that the 2X versions of the macros should not be used with 8-bit
targets since dithering will produce different target values for the
same source.

We generate error messages only in the SavePixels macro, figuring they
would be superfluous in RestPixels.
@
SaveOneRow MACRO
j        =       0
         REPEAT  Limit
ifndef _QTW32_
         MOV     EAX, ES:[DI+j]   ;; Get 4 bytes
else
         MOV     EAX, [EDI+j]   ;; Get 4 bytes
endif
         MOV     OverScan[i], EAX ;; Save the bytes
i        =       i + 4
j        =       j + 4
         ENDM                     ;; of REPEAT Limit
         ENDM

RestOneRow MACRO Width:REQ
j        =       0
         WHILE   j  LT  Width - Width MOD 4
         MOV     EAX, OverScan[i] ;; Get 4 bytes
ifndef _QTW32_
         MOV     ES:[DI+j], EAX   ;; Restore the bytes
else
         MOV     [EDI+j], EAX   ;; Restore the bytes
endif
i        =       i + 4
j        =       j + 4
         ENDM                     ;; of WHILE  j  LT  Width - Width MOD 4
         IF      j  LT  Width - Width MOD 2
         MOV     AX, WORD PTR OverScan[i]   ;; Get 2 bytes
ifndef _QTW32_
         MOV     ES:[DI+j], AX              ;; Restore the bytes
else
         MOV     [EDI+j], AX              ;; Restore the bytes
endif
i        =       i + 2
j        =       j + 2
         ENDIF                    ;; of IF      j  LT  Width - Width MOD 2
         IF      j  LT  Width
         MOV     AL, BYTE PTR OverScan[i]   ;; Get 1 byte
ifndef _QTW32_
         MOV     ES:[DI+j], AL              ;; Restore the byte
else
         MOV     [EDI+j], AL              ;; Restore the byte
endif
i        =       i + 1
j        =       j + 1
         ENDIF                    ;; of IF      j  LT  Width
i        =       (i + 3) AND 0FFFCH    ;; Round up to multiple of 4
         ENDM                     ;; of RestOneRow MACRO

SavePixels MACRO Width:REQ, Height:REQ, Increment:REQ
i        =       0
Limit    =       (((Width) + 3) AND 0FFFCH) / 4
ReqdSize TEXTEQU %(Limit * Height)
         IFNDEF  OverScan
%       .ERR     <Define OverScan[ReqdSize]:DWORD>
         EXITM
         ENDIF
         IF      ( TYPE OverScan  NE  4)  OR  (ReqdSize  NE  LENGTHOF OverScan)
%       .ERR     <Define OverScan[ReqdSize]:DWORD>
         EXITM
         ENDIF
         REPEAT  Height
ifndef _QTW32_
         ADD     DI, Increment
else
         ADD     EDI, Increment
endif
         SaveOneRow
         ENDM                     ;; of REPEAT Height
         ENDM                     ;; of MACRO

RestPixels MACRO Width:REQ, Height:REQ, Increment:REQ
i        =       0
Limit    =       (((Width) + 3) AND 0FFFCH) / 4
ReqdSize TEXTEQU %(Limit * Height)
         IFNDEF  OverScan         ;; If variable not defined
         EXITM                    ;; Avoid further error messages
         ENDIF
         IF      ( TYPE OverScan  NE  4)  OR  (ReqdSize  NE  LENGTHOF OverScan)
         EXITM
         ENDIF
         REPEAT  Height
ifndef _QTW32_
         ADD     DI, Increment
else
         ADD     EDI, Increment
endif
         RestOneRow (Width)
         ENDM                     ;; of REPEAT Height
         ENDM                     ;; of MACRO

SavePix2X MACRO Width:REQ, Height:REQ, Increment:REQ
i        =       0
Limit    =       (((Width) * 2 + 3) AND 0FFFCH) / 4
ReqdSize TEXTEQU %(Limit * Height)
         IFNDEF  OverScan
%       .ERR     <Define OverScan[ReqdSize]:DWORD>
         EXITM
         ENDIF
         IF      ( TYPE OverScan  NE  4)  OR  (ReqdSize  NE  LENGTHOF OverScan)
%       .ERR     <Define OverScan[ReqdSize]:DWORD>
         EXITM
         ENDIF
         REPEAT  Height
ifndef _QTW32_
         ADD     DI, Increment
         ADD     DI, Increment
else
         ADD     EDI, Increment
         ADD     EDI, Increment
endif
         SaveOneRow
         ENDM                     ;; of REPEAT Height
         ENDM                     ;; of MACRO

RestPix2X MACRO Width:REQ, Height:REQ, Increment:REQ
i        =       0
Limit    =       (((Width) * 2 + 3) AND 0FFFCH) / 4
ReqdSize TEXTEQU %(Limit * Height)
         IFNDEF  OverScan         ;; If variable not defined
         EXITM                    ;; Avoid further error messages
         ENDIF
         IF      ( TYPE OverScan  NE  4)  OR  (ReqdSize  NE  LENGTHOF OverScan)
         EXITM
         ENDIF
ifndef _QTW32_
         PUSH    BX               ;; Save register
         MOV     BX, Increment    ;; Initialize index register
j        =       0
         REPEAT  Limit            ;; Restore row #2 from row #1
         MOV     EAX, ES:[DI+j]   ;; Get 4 uncorrupted bytes
         MOV     ES:[DI+BX+j], EAX;; Restore the bytes
j        =       j + 4
         ENDM                     ;; of REPEAT Limit
         REPEAT  Height - 1
         ADD     DI, BX
         ADD     DI, BX
j        =       0
         REPEAT  Limit
         MOV     EAX, OverScan[i] ;; Get 4 saved bytes
         MOV     ES:[DI+j], EAX   ;; Restore the bytes to row #1
         MOV     ES:[DI+BX+j], EAX;; Restore the bytes to row #2
i        =       i + 4
j        =       j + 4
         ENDM                     ;; of REPEAT Limit
         ENDM                     ;; of REPEAT Height
         ADD     DI, BX           ;; Point to last row
         ADD     DI, BX
         RestOneRow ((Width) * 2)
         POP     BX               ;; Restore register
else
         PUSH    EBX              ;; Save register
         MOV     EBX, Increment   ;; Initialize index register
j        =       0
         REPEAT  Limit            ;; Restore row #2 from row #1
         MOV     EAX, [EDI+j]     ;; Get 4 uncorrupted bytes
         MOV     [EDI+EBX+j], EAX ;; Restore the bytes
j        =       j + 4
         ENDM                     ;; of REPEAT Limit
         REPEAT  Height - 1
         ADD     EDI, EBX
         ADD     EDI, EBX
j        =       0
         REPEAT  Limit
         MOV     EAX, OverScan[i] ;; Get 4 saved bytes
         MOV     [EDI+j], EAX   ;; Restore the bytes to row #1
         MOV     [EDI+EBX+j], EAX;; Restore the bytes to row #2
i        =       i + 4
j        =       j + 4
         ENDM                     ;; of REPEAT Limit
         ENDM                     ;; of REPEAT Height
         ADD     EDI, EBX         ;; Point to last row
         ADD     EDI, EBX
         RestOneRow ((Width) * 2)
         POP     EBX              ;; Restore register
endif
         ENDM                     ;; of MACRO
