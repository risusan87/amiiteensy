#include "imxrt.h"
#include "utils.h"

struct {
        uint32_t next;
        volatile uint32_t status;
        uint32_t pointer0;
        uint32_t pointer1;
        uint32_t pointer2;
        uint32_t pointer3;
        uint32_t pointer4;
        uint32_t callback_param;
} transfer_t;

typedef struct {
	uint32_t config;
	uint32_t current;
	uint32_t next;
	uint32_t status;
	uint32_t pointer0;
	uint32_t pointer1;
	uint32_t pointer2;
	uint32_t pointer3;
	uint32_t pointer4;
	uint32_t reserved;
	uint32_t setup0;
	uint32_t setup1;
	struct transfer_t *first_transfer;
	struct transfer_t *last_transfer;
	void (*callback_function)(struct transfer_t *completed_transfer);
	uint32_t unused1;
} endpoint_t;

union {
    struct {
     union {
      struct {
           uint8_t bmRequestType;
           uint8_t bRequest;
      };
           uint16_t wRequestAndType;
     };
           uint16_t wValue;
           uint16_t wIndex;
           uint16_t wLength;
    };
    struct {
           uint32_t word1;
           uint32_t word2;
    };
       uint64_t bothwords;
} setup_t;

// endpoint queue head must be aligned to 4096 bytes
__attribute__((aligned(4096))) 
static endpoint_t queue_head_list[16];

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
    queue_head_list[0].config = (64 << 16) | (1 << 15);
	queue_head_list[1].config = (64 << 16);
    USB1_ENDPOINTLISTADDR = (uint32_t)&queue_head_list;

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
}
