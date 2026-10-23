#include "nub.h"
#include <dolphin/base/PPCArch.h>

/*
 * Save and restore everything the debugger cannot get back from the CPU on
 * its own: the sixteen segment registers, the time base, the BATs, the
 * SPRGs, and the rest of extended block 1.  The groups the save side stages
 * are the groups the restore side takes; the restore side also consumes the
 * two restore flags the register-access side sets when the debugger writes
 * the time base or the decrementer into the context.  It restores a written
 * time base, but not a written decrementer: its restore block is bypassed by
 * an unconditional branch, so the decrementer flag is only read and cleared.
 */
asm void TRKSaveExtended1Block(void) {
	nofralloc
	lis r2,gTRKCPUState@h
	ori r2,r2,gTRKCPUState@l
	mfsr r16,0
	mfsr r17,1
	mfsr r18,2
	mfsr r19,3
	mfsr r20,4
	mfsr r21,5
	mfsr r22,6
	mfsr r23,7
	mfsr r24,8
	mfsr r25,9
	mfsr r26,10
	mfsr r27,11
	mfsr r28,12
	mfsr r29,13
	mfsr r30,14
	mfsr r31,15
	stmw r16,424(r2)
	mftb r10
	mftbu r11
	mfspr r12,HID0
	mfspr r13,HID1
	mfsrr1 r14
	mfpvr r15
	mfibatu r16,0
	mfibatl r17,0
	mfibatu r18,1
	mfibatl r19,1
	mfibatu r20,2
	mfibatl r21,2
	mfibatu r22,3
	mfibatl r23,3
	mfdbatu r24,0
	mfdbatl r25,0
	mfdbatu r26,1
	mfdbatl r27,1
	mfdbatu r28,2
	mfdbatl r29,2
	mfdbatu r30,3
	mfdbatl r31,3
	stmw r10,488(r2)
	mfsdr1 r22
	mfdar r23
	mfdsisr r24
	mfsprg r25,0
	mfsprg r26,1
	mfsprg r27,2
	mfsprg r28,3
	li r29,0
	mfspr r30,IABR
	mfear r31
	stmw r22,604(r2)
	mfspr r20,GQR0
	mfspr r21,GQR1
	mfspr r22,GQR2
	mfspr r23,GQR3
	mfspr r24,GQR4
	mfspr r25,GQR5
	mfspr r26,GQR6
	mfspr r27,GQR7
	mfspr r28,HID2
	mfspr r29,WPAR
	mfspr r30,DMA_U
	mfspr r31,DMA_L
	stmw r20,764(r2)
	b save_block_sprs_2
	mfspr r16,928
	mfspr r17,935
	mfspr r18,UMMCR0
	mfspr r19,UPMC1
	mfspr r20,UPMC2
	mfspr r21,USIA
	mfspr r22,UMMCR1
	mfspr r23,UPMC3
	mfspr r24,UPMC4
	mfspr r25,943
	mfspr r26,944
	mfspr r27,951
	mfspr r28,959
	mfspr r29,1014
	mfspr r30,1015
	mfspr r31,1023
	stmw r16,696(r2)
save_block_sprs_2:
	mfspr r19,DABR
	mfspr r20,PMC1
	mfspr r21,PMC2
	mfspr r22,PMC3
	mfspr r23,PMC4
	mfspr r24,SIA
	mfspr r25,MMCR0
	mfspr r26,MMCR1
	mfspr r27,THRM1
	mfspr r28,THRM2
	mfspr r29,THRM3
	mfspr r30,ICTC
	mfspr r31,L2CR
	stmw r19,644(r2)
	blr
	mfspr r25,976
	mfspr r26,977
	mfspr r27,978
	mfspr r28,979
	mfspr r29,980
	mfspr r30,981
	mfspr r31,982
	stmw r25,576(r2)
	mfdec r31
	stw r31,632(r2)
	blr
}

asm void TRKRestoreExtended1Block(void) {
	nofralloc
	lis r2,gTRKCPUState@h
	ori r2,r2,gTRKCPUState@l
	lis r5,gTRKRestoreFlags@h
	ori r5,r5,gTRKRestoreFlags@l
	lbz r3,0(r5)
	lbz r6,1(r5)
	li r0,0
	stb r0,0(r5)
	stb r0,1(r5)
	cmpwi r3,0
	beq restore_block_sprs
	lwz r24,488(r2)
	lwz r25,492(r2)
	mttbl r24
	mttbu r25
restore_block_sprs:
	lmw r20,764(r2)
	mtspr GQR0,r20
	mtspr GQR1,r21
	mtspr GQR2,r22
	mtspr GQR3,r23
	mtspr GQR4,r24
	mtspr GQR5,r25
	mtspr GQR6,r26
	mtspr GQR7,r27
	mtspr HID2,r28
	mtspr DMA_U,r30
	mtspr DMA_L,r31
	b restore_block_sprs_2
	lmw r26,736(r2)
	mtspr 944,r26
	mtspr 951,r27
	mtspr 1014,r29
	mtspr 1015,r30
	mtspr 1023,r31
restore_block_sprs_2:
	lmw r19,644(r2)
	mtspr DABR,r19
	mtspr PMC1,r20
	mtspr PMC2,r21
	mtspr PMC3,r22
	mtspr PMC4,r23
	mtspr SIA,r24
	mtspr MMCR0,r25
	mtspr MMCR1,r26
	mtspr THRM1,r27
	mtspr THRM2,r28
	mtspr THRM3,r29
	mtspr ICTC,r30
	mtspr L2CR,r31
	b restore_block_gprs
	cmpwi r6,0
	beq restore_block_decr
	lwz r26,632(r2)
	mtdec r26
restore_block_decr:
	lmw r25,576(r2)
	mtspr 976,r25
	mtspr 977,r26
	mtspr 978,r27
	mtspr 979,r28
	mtspr 980,r29
	mtspr 981,r30
	mtspr 982,r31
restore_block_gprs:
	lmw r16,424(r2)
	mtsr 0,r16
	mtsr 1,r17
	mtsr 2,r18
	mtsr 3,r19
	mtsr 4,r20
	mtsr 5,r21
	mtsr 6,r22
	mtsr 7,r23
	mtsr 8,r24
	mtsr 9,r25
	mtsr 10,r26
	mtsr 11,r27
	mtsr 12,r28
	mtsr 13,r29
	mtsr 14,r30
	mtsr 15,r31
	lmw r12,496(r2)
	mtspr HID0,r12
	mtspr HID1,r13
	mtsrr1 r14
	mtspr PVR,r15
	mtibatu 0,r16
	mtibatl 0,r17
	mtibatu 1,r18
	mtibatl 1,r19
	mtibatu 2,r20
	mtibatl 2,r21
	mtibatu 3,r22
	mtibatl 3,r23
	mtdbatu 0,r24
	mtdbatl 0,r25
	mtdbatu 1,r26
	mtdbatl 1,r27
	mtdbatu 2,r28
	mtdbatl 2,r29
	mtdbatu 3,r30
	mtdbatl 3,r31
	lmw r22,604(r2)
	mtsdr1 r22
	mtdar r23
	mtdsisr r24
	mtsprg 0,r25
	mtsprg 1,r26
	mtsprg 2,r27
	mtsprg 3,r28
	mtspr IABR,r30
	mtear r31
	blr
}
