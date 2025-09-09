/*
 * UART BLE service - NUS (Nordic UART Service)
 * SPDX-FileCopyrightText: 2025 Fabio Souza
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "uart.h"

#include <string.h>
#include <ctype.h>

#include "host/ble_hs.h"
#include "os/os_mbuf.h"

/* Exported Handle values (filled by ble_gatts_add_svcs) */
uint16_t uart_rx_val_handle = 0;
uint16_t uart_tx_val_handle = 0;

/* UART Sevice state */
static uint16_t s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
static bool     s_tx_notify_enabled = false;

/* Relay component instance (for command execution) */
static Relay   *s_relay = NULL;

/* ---------- utils ---------- */

// Trim whitespace (in place)
static inline void str_trim(char *s) {
    size_t n = strlen(s);
    while (n && (s[n-1] == '\r' || s[n-1] == '\n' || s[n-1] == ' ' || s[n-1] == '\t'))
        s[--n] = '\0';
    size_t i = 0; while (s[i] == ' ' || s[i] == '\t') i++;
    if (i) memmove(s, s+i, strlen(s+i)+1);
}

// Convert string to uppercase (in place)
static inline void str_up(char *s) { for (; *s; ++s) *s = (char)toupper((unsigned char)*s); }

/* ---------- BLE Implementation ---------- */
/* Send data via notification (TX characteristic) */
int uart_notify(const void *data, uint16_t len) {
    if (!s_tx_notify_enabled || s_conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        return BLE_HS_EBUSY; /* sem assinante */
    }
    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, len);
    if (!om) return BLE_HS_ENOMEM;
    return ble_gatts_notify_custom(s_conn_handle, uart_tx_val_handle, om);
}

/* Execute a command received via UART RX */
/* Supported commands:
 *   RELE ON   - turn relay on
 *   RELE OFF  - turn relay off
 * Responds via UART TX with:
 *   OK RELE ON
 *   OK RELE OFF
 *   ERR UNKNOWN CMD
 */
static void uart_exec_cmd(const char *cmd_in) {
    if (!s_relay) return;

    /* normnalize input (trim + UPPER CASE)*/
    char buf[48];                                   //local buffer
    size_t n = strlen(cmd_in);                      //input length
    if (n >= sizeof(buf)) n = sizeof(buf) - 1;      //limit to buffer size
    memcpy(buf, cmd_in, n); buf[n] = '\0';          //copy + null terminate
    str_trim(buf);                                  //trim whitespace    
    str_up(buf);                                    //to upper case

    /* compare and execute */
    if (strcmp(buf, "RELE ON") == 0) {              // if command is "RELE ON"
        relay_turn_on(s_relay);                     // turn relay on
        (void)uart_notify("OK RELE ON\n", 11);      // respond OK

    } else if (strcmp(buf, "RELE OFF") == 0) {      // if command is "RELE OFF"
        relay_turn_off(s_relay);                    // turn relay off
        (void)uart_notify("OK RELE OFF\n", 12);     // respond OK

    } else {                                         // unknown command   
        (void)uart_notify("ERR UNKNOWN CMD\n", 17);  // respond error
    }
}

/* ---------- API ---------- */
/*
    * Initialize UART service state
    * relay: pointer to a valid Relay instance (already initialized)
*/
void uart_init(Relay *relay) {
    s_conn_handle = BLE_HS_CONN_HANDLE_NONE;     // no connection
    s_tx_notify_enabled = false;                 // notifications disabled        
    s_relay = relay;                             // store relay instance
}

/* 
    * callback: uart_rx_chr_access
    * handle: uart_rx_val_handle (filled by ble_gatts_add_svcs)

*/
int uart_rx_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                       struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)arg;                                          
    if (attr_handle != uart_rx_val_handle) {            // if not RX handle
        return BLE_ATT_ERR_UNLIKELY;                    // return error
    }

    // copy payload to local buffer (limit 256)
    uint8_t buf[256];
    uint16_t len = OS_MBUF_PKTLEN(ctxt->om);            // get length
    if (len >= sizeof(buf)) len = sizeof(buf)-1;        // limit to buffer size

    int rc = os_mbuf_copydata(ctxt->om, 0, len, buf);   // copy data
    if (rc != 0) return BLE_ATT_ERR_UNLIKELY;           // error on copy

    buf[len] = '\0';                                    // null terminate                   
    s_conn_handle = conn_handle;                        // store connection handle    

    uart_exec_cmd((const char*)buf);                    // execute command received via RX
    return 0;
}


/*
    TX: READ optional (just to have something readable in the app)
    print "UART READY\n" when read
*/
int uart_tx_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                       struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle; (void)arg;
    if (attr_handle != uart_tx_val_handle) return BLE_ATT_ERR_UNLIKELY;

    const char *hello = "UART READY\n";
    return os_mbuf_append(ctxt->om, hello, strlen(hello)) == 0
           ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}


/* 
    Notifications subscribed on TX characteristic
*/
void uart_on_subscribe(uint16_t conn_handle, uint16_t attr_handle, uint8_t cur_notify)
{
    if (attr_handle != uart_tx_val_handle) return;

    s_conn_handle = conn_handle;
    s_tx_notify_enabled = (cur_notify != 0);

    if (s_tx_notify_enabled) {
        const char *hello = "UART READY\n";
        (void)uart_notify(hello, (uint16_t)strlen(hello));
    }
}
