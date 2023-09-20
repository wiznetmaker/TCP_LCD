#include <stdio.h>
#include "pico/multicore.h"
#include <string.h>

#include "pico/stdlib.h"
#include "mode0/mode0.h"
#include "port_common.h"
#include "wizchip_conf.h"
#include "w5x00_spi.h"
#include "socket.h"


#define SERVER_IP {192, 168, 11, 197} // 서버 IP 주소
#define SERVER_PORT 5000 // 서버 포트 번호
#define CLIENT_SOCKET 1 // 클라이언트 소켓 번호
#define PLL_SYS_KHZ (133 * 1000)
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)
#define MAX_TEXT 100  // 최대 문자 수

static wiz_NetInfo g_net_info = {
    .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x57},
    .ip = {192, 168, 11, 3},
    .sn = {255, 255, 255, 0},
    .gw = {192, 168, 11, 1},
    .dns = {8, 8, 8, 8},
    .dhcp = NETINFO_STATIC
};

static void set_clock_khz(void) {
    set_sys_clock_khz(PLL_SYS_KHZ, true);
    clock_configure(
        clk_peri,
        0,
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
        PLL_SYS_KHZ * 1000,
        PLL_SYS_KHZ * 1000
    );
}


static uint8_t g_buf[1024] = {0,};

// core1에서 실행될 함수
static void display_received_data_core1() {
    ili9341_init(); // Initialize the mode0 library
    mode0_set_cursor(0, 0);
    mode0_set_foreground(MODE0_WHITE);  // Set default text color to white
    mode0_set_background(MODE0_BLACK);  // Set default background color to black

    while (true) {
        char* received_data = (char*)multicore_fifo_pop_blocking();
        
        // Check for foreground color change commands
        if (strncmp(received_data, "**set_fg_", 9) == 0) {
            if (strcmp(received_data, "**set_fg_BLACK**") == 0) mode0_set_foreground(MODE0_BLACK);
            else if (strcmp(received_data, "**set_fg_BROWN**") == 0) mode0_set_foreground(MODE0_BROWN);
            else if (strcmp(received_data, "**set_fg_RED**") == 0) mode0_set_foreground(MODE0_RED);
            else if (strcmp(received_data, "**set_fg_BLUSH**") == 0) mode0_set_foreground(MODE0_BLUSH);
            else if (strcmp(received_data, "**set_fg_GRAY**") == 0) mode0_set_foreground(MODE0_GRAY);
            else if (strcmp(received_data, "**set_fg_DESERT**") == 0) mode0_set_foreground(MODE0_DESERT);
            else if (strcmp(received_data, "**set_fg_ORANGE**") == 0) mode0_set_foreground(MODE0_ORANGE);
            else if (strcmp(received_data, "**set_fg_YELLOW**") == 0) mode0_set_foreground(MODE0_YELLOW);
            else if (strcmp(received_data, "**set_fg_WHITE**") == 0) mode0_set_foreground(MODE0_WHITE);
            else if (strcmp(received_data, "**set_fg_IDNIGHT**") == 0) mode0_set_foreground(MODE0_MIDNIGHT);
            else if (strcmp(received_data, "**set_fg_DARK_SLATE_GRAY**") == 0) mode0_set_foreground(MODE0_DARK_SLATE_GRAY);
            else if (strcmp(received_data, "**set_fg_GREEN**") == 0) mode0_set_foreground(MODE0_GREEN);
            else if (strcmp(received_data, "**set_fg_YELLOW_GREEN**") == 0) mode0_set_foreground(MODE0_YELLOW_GREEN);
            else if (strcmp(received_data, "**set_fg_BLUE**") == 0) mode0_set_foreground(MODE0_BLUE);
            else if (strcmp(received_data, "**set_fg_PICTON_BLUE**") == 0) mode0_set_foreground(MODE0_PICTON_BLUE);
            else if (strcmp(received_data, "**set_fg_PALE_BLUE**") == 0) mode0_set_foreground(MODE0_PALE_BLUE);
            // ... (include other foreground color checks here)
            mode0_print("Foreground color changed!\n");
        } 
        // Check for background color change commands
        else if (strncmp(received_data, "**set_bg_", 9) == 0) {
            if (strcmp(received_data, "**set_bg_BLACK**") == 0) mode0_set_background(MODE0_BLACK);
            else if (strcmp(received_data, "**set_bg_BROWN**") == 0) mode0_set_background(MODE0_BROWN);
            else if (strcmp(received_data, "**set_bg_RED**") == 0) mode0_set_background(MODE0_RED);
            else if (strcmp(received_data, "**set_bg_BLUSH**") == 0) mode0_set_background(MODE0_BLUSH);
            else if (strcmp(received_data, "**set_bg_GRAY**") == 0) mode0_set_background(MODE0_GRAY);
            else if (strcmp(received_data, "**set_bg_DESERT**") == 0) mode0_set_background(MODE0_DESERT);
            else if (strcmp(received_data, "**set_bg_ORANGE**") == 0) mode0_set_background(MODE0_ORANGE);
            else if (strcmp(received_data, "**set_bg_YELLOW**") == 0) mode0_set_background(MODE0_YELLOW);
            else if (strcmp(received_data, "**set_bg_WHITE**") == 0) mode0_set_background(MODE0_WHITE);
            else if (strcmp(received_data, "**set_bg_IDNIGHT**") == 0) mode0_set_background(MODE0_MIDNIGHT);
            else if (strcmp(received_data, "**set_bg_DARK_SLATE_GRAY**") == 0) mode0_set_background(MODE0_DARK_SLATE_GRAY);
            else if (strcmp(received_data, "**set_bg_GREEN**") == 0) mode0_set_background(MODE0_GREEN);
            else if (strcmp(received_data, "**set_bg_YELLOW_GREEN**") == 0) mode0_set_background(MODE0_YELLOW_GREEN);
            else if (strcmp(received_data, "**set_bg_BLUE**") == 0) mode0_set_background(MODE0_BLUE);
            else if (strcmp(received_data, "**set_bg_PICTON_BLUE**") == 0) mode0_set_background(MODE0_PICTON_BLUE);
            else if (strcmp(received_data, "**set_bg_PALE_BLUE**") == 0) mode0_set_background(MODE0_PALE_BLUE);
            // ... (include other background color checks here)
            mode0_print("Background color changed!\n");
        } 
        // If it's not a color command, display the received data normally
        else {
            if (strcmp(received_data, "help") == 0) { // "help" 문자열을 확인
            mode0_print("Usage:\n");
            mode0_print("Foreground: **set_fg_COLORNAME**\n");
            mode0_print("Background: **set_bg_COLORNAME**\n");
            mode0_print("Example: **set_fg_RED**\n");
            mode0_print("Available colors: BLACK, BROWN, RED, BLUSH, GRAY, DESERT, ORANGE, YELLOW, WHITE, MIDNIGHT, DARK_SLATE_GRAY, GREEN, YELLOW_GREEN, BLUE, PICTON_BLUE, PALE_BLUE");
            } 
            else {
                mode0_print(received_data);
                mode0_print("\n");
            }
        }
    }
}

static void send_message_to_core1(const char* message) {
    multicore_fifo_push_blocking((uint32_t)message);
}

static void connect_to_server(uint8_t sn, uint8_t* buf, uint8_t* server_ip, uint16_t port) {
    int32_t ret;
    static bool isConnected = false; // 연결 상태를 추적하기 위한 변수 추가


    switch(getSn_SR(sn)) {
        case SOCK_INIT:
            if ((ret = connect(sn, server_ip, port)) != SOCK_OK) {
                // 연결 오류 처리
                if (!isConnected) {
                //    printf("IP: %d.%d.%d.%d - No connected\n", server_ip[0], server_ip[1], server_ip[2], server_ip[3]);
                    send_message_to_core1("No connection\n"); // Core1로 연결 안됨 메시지 전송
                    isConnected = false;
                }
                return;
            }
            break;
        case SOCK_ESTABLISHED:
            if (!isConnected) {
            //    printf("Server connected\n");
                send_message_to_core1("Server Connected\n"); // Core1로 연결됨 메시지 전송
                isConnected = true;
            }
            memset(g_buf, 0, sizeof(g_buf)); // g_buf 초기화
            if ((ret = recv(sn, buf, sizeof(g_buf))) > 0) {
                // 이 부분은 메시지 전송 방식을 변경하거나 다른 메커니즘을 사용해야 할 수도 있습니다.
                // 현재는 간단하게 동일한 방식을 사용하였습니다.
                multicore_fifo_push_blocking((uint32_t)buf);
            }
            break;
        case SOCK_CLOSED:
            if (isConnected) {  // 만약 이전에 연결되었었다면 연결 끊김 메시지를 전송
                send_message_to_core1("Lost connection\n");
                isConnected = false;
            }
            if ((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) {
                // 소켓 생성 오류 처리
                return;
            }
        break;
    }
}

int main() {
    set_clock_khz();
    stdio_init_all();
    // Core1에서 display_received_data_core1 함수 실행
    multicore_launch_core1(display_received_data_core1); 
    send_message_to_core1("IP : 192.168.11.3\n");
    sleep_ms(10);
    memset(g_buf, 0, sizeof(g_buf)); // g_buf 초기화
    send_message_to_core1("PORT : 5000\n");
    sleep_ms(10);
    memset(g_buf, 0, sizeof(g_buf)); // g_buf 초기화

    wizchip_spi_initialize();
    wizchip_cris_initialize();
    wizchip_reset();
    wizchip_initialize();
    wizchip_check();

    network_initialize(g_net_info);


    
    uint8_t server_ip[] = SERVER_IP;

    while (1) {

        connect_to_server(CLIENT_SOCKET, g_buf, server_ip, SERVER_PORT);
        sleep_ms(100);
    }
}