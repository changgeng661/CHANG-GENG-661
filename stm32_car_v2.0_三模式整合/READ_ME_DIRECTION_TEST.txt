STM32F103C8T6 + TB6612 direction-test version

Pin mapping used:
PA1 -> PWMA  (TIM2_CH2)
PA2 -> PWMB  (TIM2_CH3)
PA4 -> AIN2
PA5 -> AIN1
PA6 -> BIN1
PA7 -> BIN2

Important fixes vs. the original project:
1. Original PWM code only output PWM on PA2/TIM2_CH3.
   But your wiring uses PA1 as PWMA and PA2 as PWMB, so two PWM channels are required.
2. Left motor speed now uses TIM2_CH2 / PA1 / PWMA.
3. Right motor speed uses TIM2_CH3 / PA2 / PWMB.
4. main.c now calls Motor_Init() before driving the motors.
5. main.c runs a repeating direction test sequence.

TB6612 output wiring reminder:
A01 and A02 are the TWO terminals of channel A, not two independent motor channels.
Both left motors should normally be connected in parallel across A01 and A02.
Both right motors should normally be connected in parallel across B01 and B02.

Also make sure TB6612 STBY is HIGH. If STBY is not pulled high on your module,
connect STBY to 3.3V or control it with a GPIO and drive it high.
