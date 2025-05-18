#include "imxrt.h"
#include "utils.h"

// Device transfer descriptor (42.5.5.2 page 2396)
// must be aligned to 32 bytes
typedef struct __attribute__((aligned(32)))
{
    uint32_t next_link_pointer; // next dTD
    uint32_t status;
    uint32_t buffer0;
    uint32_t buffer1;
    uint32_t buffer2; // reserved
    uint32_t buffer3; // reserved
    uint32_t buffer4; // reserved
    uint32_t reserved; // reserved
} dtd_t;

// typedef struct {
//         uint32_t next;
//         volatile uint32_t status;
//         uint32_t pointer0;
//         uint32_t pointer1;
//         uint32_t pointer2;
//         uint32_t pointer3;
//         uint32_t pointer4;
//         uint32_t callback_param;
// } transfer_t;

// 42.5.5.1 page 2394
// TODO: union this for better complehension
typedef struct __attribute__((aligned(64))) {
	uint32_t config;
	dtd_t *current_dtd;
	dtd_t *next_dtd;
	uint32_t status;
	uint32_t buffer0;
	uint32_t buffer1;
	uint32_t buffer2;
	uint32_t buffer3;
	uint32_t buffer4;
	uint32_t reserved;
	uint32_t setup0; 
    uint32_t setup1; 
} endpoint_qh_t;

// typedef struct {
// 	uint32_t config;
// 	uint32_t current;
// 	uint32_t next;
// 	uint32_t status;
// 	uint32_t pointer0;
// 	uint32_t pointer1;
// 	uint32_t pointer2;
// 	uint32_t pointer3;
// 	uint32_t pointer4;
// 	uint32_t reserved;
// 	uint32_t setup0;
// 	uint32_t setup1;
// 	transfer_t *first_transfer;
// 	transfer_t *last_transfer;
// 	void (*callback_function)(transfer_t *completed_transfer);
// 	uint32_t unused1;
// } endpoint_qh_t;

typedef union 
{
    struct 
    {
        union 
        {
            struct 
            {
                uint8_t bmRequestType;
                uint8_t bRequest;
            };
            uint16_t wRequestAndType;
        };
        uint16_t wValue;
        uint16_t wIndex;
        uint16_t wLength;
    };
    struct 
    {
           uint32_t word1;
           uint32_t word2;
    };
       uint64_t bothwords;
} setup_t;

// endpoint queue head must be aligned to 2048 bytes (42.5.5 page 2394)
// num of endponts = 8, each endpoint has 2 queue heads (IN/OUT) (42.5.6.3)
__attribute__((aligned(2048))) 
static endpoint_qh_t queue_head_list[8][2];

__attribute__((section(".startup"), used))
void usb1_init(void)
{
    // enable PLL3
    CCM_ANALOG_PLL_USB1_SET = (1 << 13) | (1 << 12);
    while (!(CCM_ANALOG_PLL_USB1 & (1 << 31)));
    CCM_ANALOG_PLL_USB1_CLR = (1 << 16); 
    CCM_ANALOG_PLL_USB1_SET = (1 << 6);

    // clock gate USB1
    CCM_CCGR6 |= CCM_CCGR6_USBOH3(CCM_CCGR_ON);
    USB1_BURSTSIZE = 0x0404;

    if ((USBPHY1_PWD & (USBPHY_PWD_RXPWDRX | USBPHY_PWD_RXPWDDIFF | USBPHY_PWD_RXPWD1PT1
        | USBPHY_PWD_RXPWDENV | USBPHY_PWD_TXPWDV2I | USBPHY_PWD_TXPWDIBIAS
        | USBPHY_PWD_TXPWDFS)) || (USB1_USBMODE & USB_USBMODE_CM_MASK)) 
    {
        // USB reset
        USBPHY1_CTRL_SET = USBPHY_CTRL_SFTRST;
        USB1_USBCMD |= USB_USBCMD_RST;
        int count=0; 
        while (USB1_USBCMD & USB_USBCMD_RST) count++;
        NVIC_CLEAR_PENDING(IRQ_USB1);
        USBPHY1_CTRL_CLR = USBPHY_CTRL_SFTRST; // reset PHY
        // delay(25);
    }
    
    // USB1_USBCMD |= USB_USBCMD_RST;  // Reset controller before mode change
    // while (USB1_USBCMD & USB_USBCMD_RST);  // Wait for reset complete

    // power USBPHY
    USBPHY1_CTRL_CLR = USBPHY_CTRL_CLKGATE;
	USBPHY1_PWD = 0;

    // set device mode
    USB1_USBMODE = USB_USBMODE_CM(2) | USB_USBMODE_SLOM;

    // initialize endpoint queue head list
    memset(queue_head_list, 0, sizeof(queue_head_list));
    queue_head_list[0][0].config = (64 << 16) | (1 << 15); // RX
	queue_head_list[0][1].config = (64 << 16); // TX
    USB1_ENDPOINTLISTADDR = (uint32_t) &queue_head_list;

    USB1_USBINTR = USB_USBINTR_UE | USB_USBINTR_UEE | /* USB_USBINTR_PCE | */
		USB_USBINTR_URE | USB_USBINTR_SLE;
    // enable interrupts
    NVIC_ENABLE_IRQ(IRQ_USB1);
    
    // enable USB controller
    USB1_USBCMD |= USB_USBCMD_RS;

}

__attribute__((section(".isr"), used))
void usb1_isr(void)
{
    // isr must acknowledge by writing bits back to the status register
    uint32_t status = USB1_USBSTS;
    USB1_USBSTS = status;

    if (status & USB_USBSTS_URI) 
    {
        // Reset interrupt acknowledgement
        USB1_USBSTS = USB_USBSTS_URI;

        USB1_ENDPTFLUSH = 0xFFFFFFFF;  // Cancel all endpoint primed status
        while (USB1_ENDPTFLUSH); 

        // from core/teensy4
        USB1_ENDPTSETUPSTAT = USB1_ENDPTSETUPSTAT; // Clear all setup token semaphores
		USB1_ENDPTCOMPLETE = USB1_ENDPTCOMPLETE; // Clear all the endpoint complete status
		while (USB1_ENDPTPRIME != 0) ; // Wait for any endpoint priming

        queue_head_list[0][0].config = (64 << 16);
        queue_head_list[0][1].config = (64 << 16);
        USB1_DEVICEADDR = 0;

        // usb_serial_reset();
    }
    
    if (status & USB_USBSTS_UI) 
    {
        // dTD complete interrupt acknowledgement
        USB1_USBSTS = USB_USBSTS_UI;

        uint32_t setupstatus = USB1_ENDPTSETUPSTAT;
        while (setupstatus) {
			USB1_ENDPTSETUPSTAT = setupstatus;
			setup_t s;
			do {
				USB1_USBCMD |= USB_USBCMD_SUTW;
				s.word1 = endpoint_queue_head[0][0].setup0;
				s.word2 = endpoint_queue_head[0][0].setup1;
			} while (!(USB1_USBCMD & USB_USBCMD_SUTW));
			USB1_USBCMD &= ~USB_USBCMD_SUTW;
			//printf("setup %08lX %08lX\n", s.word1, s.word2);
			USB1_ENDPTFLUSH = (1<<16) | (1<<0); // page 3174
			while (USB1_ENDPTFLUSH & ((1<<16) | (1<<0))) ;
			// endpoint0_notify_mask = 0;
            // setup ep0 given s.bothwords
            setup_t setup;
            setup.bothwords = s.bothwords;
            switch (setup.wRequestAndType) {
                case 0x0680: // GET_DESCRIPTOR (device)
                case 0x0681: // interface
                
                return;

            }

			setupstatus = USB1_ENDPTSETUPSTAT; // page 3175
		}

        uint32_t complete = USB1_ENDPTCOMPLETE;
        USB1_ENDPTCOMPLETE = complete;  // Acknowledge per-endpoint completions

        if (USB1_ENDPTSETUPSTAT & 0x01) 
        {
            memcpy(&usb_setup_packet, queue_head_list[0][0].setup, 8);
        }

    }
}
