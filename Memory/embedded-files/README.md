## Embedding Binary Data in ESP32 Flash Memory

This example shows how to embed binary data in the flash memory of the ESP32.
First we will configure the CMakeLists.txt file to include the binary data in the build process.
Then we will use the asm() function to get the address of the start and end of the binary data.
We will also use the address to calculate the size of the binary data.
Finally, we will use the address to access the binary data and print it to the console.

This code is part of the course "Programe o ESP32 com ESP-IDF 5" by Fábio Souza
The course is available on https://cursos.embarcados.com.br

This example code Creative Commons Attribution 4.0 International License.
When using the code, you must keep the above copyright notice,
this list of conditions and the following disclaimer in the source code.
(http://creativecommons.org/licenses/by/4.0/)

Author: Fábio Souza
This code is for teaching purposes only.
No warranty of any kind is provided.
