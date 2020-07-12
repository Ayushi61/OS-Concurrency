/* test_and_set.S - ctxsw (for x86) */

		.text
		.intel_syntax noprefix
		.globl	test_and_set

/*------------------------------------------------------------------------
 * test_and_set -  X86 context switch; the call is test_and_set(&ptr,new_value)
 *------------------------------------------------------------------------
 */

test_and_set:
		PUSH EBP      //push the base register to stack before doing anything, save this base pointer, so that the previous functions base register is saved
		MOV EBP,ESP   //save stack pointer to the current base pointer, to reference the values passed to the functuion test and set
		MOV EAX,[EBP+12] //access the new_value
		MOV ECX,[EBP+8]  //access the ptr 
		XCHG EAX,[ECX]  //atomic exchange of new value and value at ptr
		MOV ESP,EBP  //restore the stack pointer
		POP EBP  // restore the base pointer
		ret  //exit the function
