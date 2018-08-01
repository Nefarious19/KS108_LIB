# 1. This library is developed on STM32F051R8T6 microcontroller.

This library will be developed also on other architectures like AVR,
PIC or TI MSP430. Stay tuned.

# 2. This library is not using HAL or any other low layer drivers. 

If you see this as problem, just search for another library. 

# 3. This seciton will describe API of the library.

##	3.1. How to setup example project
	
First of all, you need to include proper *.h files in your code.

* ks108.h --> this file contains basic API to initialize library.
* 

```c
	void KS108_InitDisplay (void);
    void KS108_WriteInstruction (uint8_t instruction, CS_number CS_nr );
    void KS108_WriteDsiplay (uint8_t data, CS_number CS_nr);
```



