# ESP Timer - RTC Implementation

This example demonstrates how to implement a Real-Time Clock (RTC) using the ESP32. The ESP Timer (High Resolution Timer) is utilized to ensure accurate time tracking.

## Overview

The example is part of the course **"Programe o ESP32 com ESP-IDF 5"** by Fábio Souza. The course provides a comprehensive guide to programming the ESP32 using the ESP-IDF framework.

For more information, visit: [https://cursos.embarcados.com.br](https://cursos.embarcados.com.br)

### Features

- **High Precision Timing:** Utilizes the ESP Timer to achieve precise second intervals.
- **RTC Integration:** Demonstrates setting and retrieving system time using the standard POSIX time functions.
- **Code Modularity:** Clean and reusable code structure for educational purposes.

## Code License

This example code is licensed under the **Creative Commons Attribution 4.0 International License**.  
For license details, visit: [http://creativecommons.org/licenses/by/4.0/](http://creativecommons.org/licenses/by/4.0/)

### Terms of Use

- You must keep the copyright notice, list of conditions, and disclaimer in the source code when using this example.
- This code is provided **for teaching purposes only** and comes with **no warranty** of any kind.

## Author

**Fábio Souza**  
This code is part of the educational content created for the "Programe o ESP32 com ESP-IDF 5" course.

## How to Use

1. Clone or download this repository.
2. Configure the ESP-IDF environment as per the [official ESP-IDF setup guide](https://docs.espressif.com/projects/esp-idf/en/latest/).
3. Build and flash the example to your ESP32 development board.
4. Monitor the serial output to observe the RTC functionality.

### Example Output

The serial monitor will print the current time every second. Example:

```
I (123456) RTC_UPDATE: Time set to: 2024-12-05 18:53:00 
I (223457) RTC_UPDATE: Current time: 2024-12-05 18:53:01 
I (323458) RTC_UPDATE: Current time: 2024-12-05 18:53:02
```

## Educational Purpose

This example is designed to help learners understand how to:
- Use the ESP Timer for precise timing operations.
- Implement and manage system time on an ESP32.

Feel free to use and modify the code for your learning and projects, adhering to the licensing terms.

---

**Happy Coding!**