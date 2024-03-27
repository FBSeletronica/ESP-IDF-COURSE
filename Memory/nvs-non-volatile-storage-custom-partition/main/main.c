/******************************************************************************
* NVS - Non-Volatile Storage - Custom Partition
* This example shows how to use the NVS (Non-Volatile Storage) API to read and
* write values to the NVS in a custom partition.

* This code is part of the course "Programe o ESP32 com ESP-IDF 5" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
* This example code Creative Commons Attribution 4.0 International License.
* When using the code, you must keep the above copyright notice,
* this list of conditions and the following disclaimer in the source code.
* (http://creativecommons.org/licenses/by/4.0/)

* Author: Fábio Souza
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_random.h"

const char *TAG = "NVS Test";                                                 //define the tag for the log

#define DEFAULT_SETPOINT 25                                                   //define the default setpoint value

uint8_t setpoint;                                                             //variable to store the setpoint value

typedef struct {                                                              //create a structure to store the pid values
    float kp;
    float ki;
    float kd;
} pid_contrtoler_t;                                                          //define the type of the structure

char ssid[32];                                                               //variable to store the ssid
char password[32];                                                          //variable to store the password

void app_main(void)
{
    // Initialize NVS
    ESP_LOGI(TAG, "\n\nInitializing NVS");                                                   //log the action
    esp_err_t err = nvs_flash_init_partition("nvs_ext");                                     //initialize the default NVS partition        
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)           //if the partition is truncated, erase it and retry
    {
        // NVS partition was truncated and needs to be erased
        ESP_ERROR_CHECK(nvs_flash_erase_partition("nvs_ext"));                              //erase the partition
        err = nvs_flash_init_partition("nvs_ext");  ;                                       //initialize the default NVS partition
    }
    ESP_ERROR_CHECK( err );                                                                 //check if the initialization was successful
/*************************************************************************************/

    //get the NVS statistics
    nvs_stats_t nvsStats;
    nvs_get_stats("nvs_ext", &nvsStats);
    ESP_LOGI(TAG, "used: %d, free: %d, total: %d, namespace count: %d", nvsStats.used_entries,
           nvsStats.free_entries, nvsStats.total_entries, nvsStats.namespace_count);

    //read setpoint from NVS
    nvs_handle_t nvs_handle;                                                                //create a handle to the nvs
    err = nvs_open_from_partition("nvs_ext", "storage", NVS_READWRITE, &nvs_handle);        //open the nvs partition
    if (err != ESP_OK)                                                                      //check if the partition was opened successfully
    {
        //log the error
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));            //log the error
    } 
    else 
    {
        ESP_LOGI(TAG, "Reading setpoint from NVS ... ");                                    //log the action
        err = nvs_get_u8(nvs_handle, "setpoint", &setpoint);                                //read the setpoint from the nvs
        switch (err)                                                                        //check the return of the function
        {
            case ESP_OK:                                                                    //if the function returns ESP_OK
                ESP_LOGI(TAG, "Done");                                                    //log the action
                ESP_LOGI(TAG, "Setpoint = %" PRIu8, setpoint);                              //log the value read
                break;
            case ESP_ERR_NVS_NOT_FOUND:                                                     //if the value was not found
                ESP_LOGI(TAG, "The value is not initialized yet!");                        //log the action
                ESP_LOGI(TAG, "Initializing setpoint to %d", DEFAULT_SETPOINT);            //log the action
                setpoint = DEFAULT_SETPOINT;                                                //initialize the setpoint
                //Write setpoint to NVS
                ESP_LOGI(TAG, "Writing setpoint to NVS ... ");                              //log the action
                err = nvs_set_u8(nvs_handle, "setpoint", setpoint);                         //write the setpoint to the nvs
                ESP_LOGI(TAG, "%s",(err != ESP_OK) ? "Failed!" : "Done");                    //log the action
                err = nvs_commit(nvs_handle);                                               //commit the changes to the nvs
                ESP_LOGI(TAG, "%s",(err != ESP_OK) ? "Failed!" : "Done");                   //log the action
                break;
            default :
                ESP_LOGI(TAG, "Error (%s) reading!", esp_err_to_name(err));               //log the error
        }
        nvs_close(nvs_handle);                                                              //close the nvs handle    
    }
/*************************************************************************************/

    pid_contrtoler_t pid;                                                               //create a variable to store the pid values
    //read PID values from NVS using blob
    err = nvs_open_from_partition("nvs_ext", "storage", NVS_READWRITE, &nvs_handle);      //open the nvs partition
    if (err != ESP_OK)                                                                  //check if the partition was opened successfully
    {
        //log the error
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));        //log the error
    } 
    else 
    {
        ESP_LOGI(TAG, "Reading PID values from NVS ... ");                              //log the action
        size_t required_size = sizeof(pid);                                             //get the size of the pid structure
        err = nvs_get_blob(nvs_handle, "pid", &pid, &required_size);                    //read the pid values from the nvs
        switch (err) 
        {
            case ESP_OK:                                                                        //if the function returns ESP_OK
                ESP_LOGI(TAG, "Done");                                                          //log the action
                ESP_LOGI(TAG, "PID values: Kp = %f, Ki = %f, Kd = %f", pid.kp, pid.ki, pid.kd); //log the values read
                break;
            case ESP_ERR_NVS_NOT_FOUND:                                                         //if the value was not found
                ESP_LOGI(TAG, "The value is not initialized yet!");                             //log the action
                ESP_LOGI(TAG, "Initializing PID values");                                       //log the action
                pid.kp = 1.0;
                pid.ki = 0.8;
                pid.kd = 0.2;
                //Write PID values to NVS
                ESP_LOGI(TAG, "Writing PID values to NVS ... ");                                 //log the action
                err = nvs_set_blob(nvs_handle, "pid", &pid, sizeof(pid));                        //write the pid values to the nvs
                ESP_LOGI(TAG, "%s",(err != ESP_OK) ? "Failed!" : "Done");                        //log the action
                err = nvs_commit(nvs_handle);                                           //commit the changes to the nvs
                ESP_LOGI(TAG, "%s",(err != ESP_OK) ? "Failed!" : "Done");               //log the action
                break;
            default :
                ESP_LOGI(TAG, "Error (%s) reading!", esp_err_to_name(err));                     //log the error
        }   
        nvs_close(nvs_handle);                                                                  //close the nvs handle
    }

/*************************************************************************************/

    //read ssid from NVS using string
    //err = nvs_open("storage", NVS_READWRITE, &nvs_handle);                                      //open the nvs partition
    err = nvs_open_from_partition("nvs_ext", "storage", NVS_READWRITE, &nvs_handle);            //open the nvs partition
    if (err != ESP_OK)                                                                          //check if the partition was opened successfully
    {
        //log the error
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));                //log the error
    } 
    else 
    {

        ESP_LOGI(TAG, "Reading SSID from NVS ... ");                                            //log the action
        size_t required_size = sizeof(ssid);                                                    //get the size of the ssid string
        err = nvs_get_str(nvs_handle, "ssid", ssid, &required_size);                            //read the ssid from the nvs
        switch (err)                                                                            //check the return of the function
        {
            case ESP_OK:                                                                        //if the function returns ESP_OK
                ESP_LOGI(TAG, "Done");                                                          //log the action
                ESP_LOGI(TAG, "SSID = %s", ssid);                                               //log the value read
                break;
            case ESP_ERR_NVS_NOT_FOUND:                                                         //if the value was not found
                ESP_LOGI(TAG, "The value is not initialized yet!");                             //log the action
                ESP_LOGI(TAG, "Initializing SSID");                                             //log the action
                strcpy(ssid, "MySSID");
                //Write SSID to NVS
                ESP_LOGI(TAG, "Writing SSID to NVS ... ");                                      //log the action
                err = nvs_set_str(nvs_handle, "ssid", ssid);                                    //write the ssid to the nvs
                ESP_LOGI(TAG, "%s",(err != ESP_OK) ? "Failed!" : "Done");                        //log the action   
                err = nvs_commit(nvs_handle);                                           //commit the changes to the nvs
                ESP_LOGI(TAG, "%s",(err != ESP_OK) ? "Failed!" : "Done");               //log the action                         
                break;
            default :
                ESP_LOGI(TAG, "Error (%s) reading!", esp_err_to_name(err));                     //log the error
        }
        nvs_close(nvs_handle);
    }

    


    while(1)
    {
        vTaskDelay(5000 / portTICK_PERIOD_MS);                                      //wait 5 seconds

        //random a new setpoint value between 25 and 50
        setpoint = (esp_random() % 25) + 25;                                        
        ESP_LOGI(TAG, "New setpoint = %" PRIu8, setpoint);                          //log the new setpoint value
        //Write setpoint to NVS
        //err = nvs_open("storage", NVS_READWRITE, &nvs_handle);                      //open the nvs partition
        err = nvs_open_from_partition("nvs_ext", "storage", NVS_READWRITE, &nvs_handle);    //open the nvs partition
        if (err != ESP_OK)                                                          //check if the partition was opened successfully
        {
            //log the error
            ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err)); //log the error
        } 
        else 
        {
            ESP_LOGI(TAG, "Writing setpoint to NVS ... ");                          //log the action
            err = nvs_set_u8(nvs_handle, "setpoint", setpoint);                     //write the setpoint to the nvs
            ESP_LOGI(TAG, "%s",(err != ESP_OK) ? "Failed!" : "Done");               //log the action
            err = nvs_commit(nvs_handle);                                           //commit the changes to the nvs
            ESP_LOGI(TAG, "%s",(err != ESP_OK) ? "Failed!" : "Done");               //log the action
            nvs_close(nvs_handle);                                                  //close the nvs handle
        }
        
        esp_restart();                                                            //restart the ESP32
    }

}

