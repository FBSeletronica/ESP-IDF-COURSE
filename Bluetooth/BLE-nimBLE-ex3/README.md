##  Bluetooth LE GATT Server Example - revised
This example creates a GATT server with: 
 - GAP (name, flags, advertising params/fields)
 - GATT (services, characteristics, descriptors)
 - Heart Rate Service (0x180D)
 - Battery Service (0x180F)
 - LED Control Service (custom UUID)
 - Uses NimBLE stack on ESP32
 - Simulates heart rate and battery level
 - Controls an LED via a writable characteristic
 
  I revised from the original example to better structure and comments. 
  Now the code is more modular and easier to understand and add new services.

This code is part of the course "Academia ESP32 Profissional" by Fábio Souza
The course is available on https://cursos.embarcados.com.br
This example code Creative Commons Attribution 4.0 International License.
When using the code, you must keep the above copyright notice,
this list of conditions and the following disclaimer in the source code.
(http://creativecommons.org/licenses/by/4.0/)

- Author: Fábio Souza
- This code is for teaching purposes only.
- No warranty of any kind is provided.
