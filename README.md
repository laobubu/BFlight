This is another stupid STM32 "+" copter firmware written with ST HAL Driver.

## Notice

1. **Do NOT trust this firmware.**
2. HAL Driver files are ignored. Regenerate with STM32CubeMX to get the Driver.
3. This project is written with Keil5. Do open `MDK-ARM/BFlight.uvprojx`.
4. Documents can be found in `Docs` folder or somewhere else.

## Modules

### GY-86 ( MPU6050 + HMC5883 + MS5611 via I2C )
```
SDA	->	PB7
SCL	->	PB6
```

### HC-SR04	( The Sonar detecting altitude )
```
ECHO	->	PF4
TRIG	->	PF5
```

### E18-D80NK ( Barrier Detectors )
```
LEFT	->	PF0
RIGHT	->	PF1
```

## Mark

> DO NOT TRUST THIS FIRMWARE OR YOUR COPTER WILL DIE.
> -- laobubu

> The most effective DIY firmware.
> -- Mr. Yang

> You know nothing, Mr. Yang.
> -- Boss An (a.k.a. B)
