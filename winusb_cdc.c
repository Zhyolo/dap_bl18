/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "usbd_core.h"
#include "usbd_cdc_acm.h"

#define WINUSB_IN_EP        0x81
#define WINUSB_OUT_EP       0x02

#define CDC_IN_EP           0x83
#define CDC_OUT_EP          0x04
#define CDC_INT_EP          0x85

#define USBD_VID            0x0D28
#define USBD_PID            0x0204
#define USBD_MAX_POWER      500
#define USBD_LANGID_STRING  1033

#define USB_CONFIG_SIZE     (9 + 9 + 7 + 7 + CDC_ACM_DESCRIPTOR_LEN)
#define INTF_NUM            3

#ifdef CONFIG_USB_HS
#define WINUSB_EP_MPS       512
#else
#define WINUSB_EP_MPS       64
#endif

#define USBD_WINUSB_VENDOR_CODE 0x20

#define USBD_WEBUSB_ENABLE      0
#define USBD_BULK_ENABLE        1
#define USBD_WINUSB_ENABLE      1

/* WinUSB Microsoft OS 2.0 descriptor sizes */
#define WINUSB_DESCRIPTOR_SET_HEADER_SIZE  10
#define WINUSB_FUNCTION_SUBSET_HEADER_SIZE 8
#define WINUSB_FEATURE_COMPATIBLE_ID_SIZE  20

#define FUNCTION_SUBSET_LEN                160
#define DEVICE_INTERFACE_GUIDS_FEATURE_LEN 132

#define USBD_WINUSB_DESC_SET_LEN (WINUSB_DESCRIPTOR_SET_HEADER_SIZE + USBD_WEBUSB_ENABLE * FUNCTION_SUBSET_LEN + USBD_BULK_ENABLE * FUNCTION_SUBSET_LEN)

__ALIGN_BEGIN const uint8_t USBD_WinUSBDescriptorSetDescriptor[] = {
    WBVAL(WINUSB_DESCRIPTOR_SET_HEADER_SIZE), /* wLength */
    WBVAL(WINUSB_SET_HEADER_DESCRIPTOR_TYPE), /* wDescriptorType */
    0x00, 0x00, 0x03, 0x06, /* >= Win 8.1 */  /* dwWindowsVersion*/
    WBVAL(USBD_WINUSB_DESC_SET_LEN),          /* wDescriptorSetTotalLength */
#if (USBD_WEBUSB_ENABLE)
    WBVAL(WINUSB_FUNCTION_SUBSET_HEADER_SIZE), // wLength
    WBVAL(WINUSB_SUBSET_HEADER_FUNCTION_TYPE), // wDescriptorType
    0,                                         // bFirstInterface USBD_WINUSB_IF_NUM
    0,                                         // bReserved
    WBVAL(FUNCTION_SUBSET_LEN),                // wSubsetLength
    WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_SIZE),  // wLength
    WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_TYPE),  // wDescriptorType
    'W', 'I', 'N', 'U', 'S', 'B', 0, 0,        // CompatibleId
    0, 0, 0, 0, 0, 0, 0, 0,                    // SubCompatibleId
    WBVAL(DEVICE_INTERFACE_GUIDS_FEATURE_LEN), // wLength
    WBVAL(WINUSB_FEATURE_REG_PROPERTY_TYPE),   // wDescriptorType
    WBVAL(WINUSB_PROP_DATA_TYPE_REG_MULTI_SZ), // wPropertyDataType
    WBVAL(42),                                 // wPropertyNameLength
    'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0,
    'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0,
    'G', 0, 'U', 0, 'I', 0, 'D', 0, 's', 0, 0, 0,
    WBVAL(80), // wPropertyDataLength
    '{', 0,
    '9', 0, '2', 0, 'C', 0, 'E', 0, '6', 0, '4', 0, '6', 0, '2', 0, '-', 0,
    '9', 0, 'C', 0, '7', 0, '7', 0, '-', 0,
    '4', 0, '6', 0, 'F', 0, 'E', 0, '-', 0,
    '9', 0, '3', 0, '3', 0, 'B', 0, '-',
    0, '3', 0, '1', 0, 'C', 0, 'B', 0, '9', 0, 'C', 0, '5', 0, 'A', 0, 'A', 0, '3', 0, 'B', 0, '9', 0,
    '}', 0, 0, 0, 0, 0,
#endif
#if USBD_BULK_ENABLE
    WBVAL(WINUSB_FUNCTION_SUBSET_HEADER_SIZE), /* wLength */
    WBVAL(WINUSB_SUBSET_HEADER_FUNCTION_TYPE), /* wDescriptorType */
    0,                                         /* bFirstInterface USBD_BULK_IF_NUM*/
    0,                                         /* bReserved */
    WBVAL(FUNCTION_SUBSET_LEN),                /* wSubsetLength */
    WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_SIZE),  /* wLength */
    WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_TYPE),  /* wDescriptorType */
    'W', 'I', 'N', 'U', 'S', 'B', 0, 0,        /* CompatibleId*/
    0, 0, 0, 0, 0, 0, 0, 0,                    /* SubCompatibleId*/
    WBVAL(DEVICE_INTERFACE_GUIDS_FEATURE_LEN), /* wLength */
    WBVAL(WINUSB_FEATURE_REG_PROPERTY_TYPE),   /* wDescriptorType */
    WBVAL(WINUSB_PROP_DATA_TYPE_REG_MULTI_SZ), /* wPropertyDataType */
    WBVAL(42),                                 /* wPropertyNameLength */
    'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0,
    'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0,
    'G', 0, 'U', 0, 'I', 0, 'D', 0, 's', 0, 0, 0,
    WBVAL(80), /* wPropertyDataLength */
    '{', 0,
    'C', 0, 'D', 0, 'B', 0, '3', 0, 'B', 0, '5', 0, 'A', 0, 'D', 0, '-', 0,
    '2', 0, '9', 0, '3', 0, 'B', 0, '-', 0,
    '4', 0, '6', 0, '6', 0, '3', 0, '-', 0,
    'A', 0, 'A', 0, '3', 0, '6', 0, '-',
    0, '1', 0, 'A', 0, 'A', 0, 'E', 0, '4', 0, '6', 0, '4', 0, '6', 0, '3', 0, '7', 0, '7', 0, '6', 0,
    '}', 0, 0, 0, 0, 0
#endif
};

#define USBD_NUM_DEV_CAPABILITIES (USBD_WEBUSB_ENABLE + USBD_WINUSB_ENABLE)

#define USBD_WEBUSB_DESC_LEN 24
#define USBD_WINUSB_DESC_LEN 28

#define USBD_BOS_WTOTALLENGTH (0x05 +                                      \
                               USBD_WEBUSB_DESC_LEN * USBD_WEBUSB_ENABLE + \
                               USBD_WINUSB_DESC_LEN * USBD_WINUSB_ENABLE)

__ALIGN_BEGIN const uint8_t USBD_BinaryObjectStoreDescriptor[] = {
    0x05,                         /* bLength */
    0x0f,                         /* bDescriptorType */
    WBVAL(USBD_BOS_WTOTALLENGTH), /* wTotalLength */
    USBD_NUM_DEV_CAPABILITIES,    /* bNumDeviceCaps */
#if (USBD_WEBUSB_ENABLE)
    USBD_WEBUSB_DESC_LEN,           /* bLength */
    0x10,                           /* bDescriptorType */
    USB_DEVICE_CAPABILITY_PLATFORM, /* bDevCapabilityType */
    0x00,                           /* bReserved */
    0x38, 0xB6, 0x08, 0x34,         /* PlatformCapabilityUUID */
    0xA9, 0x09, 0xA0, 0x47,
    0x8B, 0xFD, 0xA0, 0x76,
    0x88, 0x15, 0xB6, 0x65,
    WBVAL(0x0100), /* 1.00 */ /* bcdVersion */
    USBD_WINUSB_VENDOR_CODE,  /* bVendorCode */
    0,                        /* iLandingPage */
#endif
#if (USBD_WINUSB_ENABLE)
    USBD_WINUSB_DESC_LEN,           /* bLength */
    0x10,                           /* bDescriptorType */
    USB_DEVICE_CAPABILITY_PLATFORM, /* bDevCapabilityType */
    0x00,                           /* bReserved */
    0xDF, 0x60, 0xDD, 0xD8,         /* PlatformCapabilityUUID */
    0x89, 0x45, 0xC7, 0x4C,
    0x9C, 0xD2, 0x65, 0x9D,
    0x9E, 0x64, 0x8A, 0x9F,
    0x00, 0x00, 0x03, 0x06, /* >= Win 8.1 */ /* dwWindowsVersion*/
    WBVAL(USBD_WINUSB_DESC_SET_LEN),         /* wDescriptorSetTotalLength */
    USBD_WINUSB_VENDOR_CODE,                 /* bVendorCode */
    0,                                       /* bAltEnumCode */
#endif
};

const uint8_t winusbv2_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_1, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    /* Configuration 0 */
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, INTF_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    /* Interface 0 */
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x02, 0xFF, 0x00, 0x00, 0x02),
    /* Endpoint OUT 2 */
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_OUT_EP, USB_ENDPOINT_TYPE_BULK, WINUSB_EP_MPS, 0x00),
    /* Endpoint IN 1 */
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_IN_EP, USB_ENDPOINT_TYPE_BULK, WINUSB_EP_MPS, 0x00),
    CDC_ACM_DESCRIPTOR_INIT(0x01, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, WINUSB_EP_MPS, 0x00),
    /* String 0 (LANGID) */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /* String 1 (Manufacturer) */
    0x0C,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'Z', 0x00,                  /* wcChar0 */
    't', 0x00,                  /* wcChar1 */
    'o', 0x00,                  /* wcChar2 */
    'o', 0x00,                  /* wcChar3 */
    'l', 0x00,                  /* wcChar4 */
    /* String 2 (Product) */
    0x20,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'Z', 0x00,                  /* wcChar0 */
    't', 0x00,                  /* wcChar1 */
    'o', 0x00,                  /* wcChar2 */
    'o', 0x00,                  /* wcChar3 */
    'l', 0x00,                  /* wcChar4 */
    ' ', 0x00,                  /* wcChar5 */
    'C', 0x00,                  /* wcChar6 */
    'M', 0x00,                  /* wcChar7 */
    'S', 0x00,                  /* wcChar8 */
    'I', 0x00,                  /* wcChar9 */
    'S', 0x00,                  /* wcChar10 */
    '-', 0x00,                  /* wcChar11 */
    'D', 0x00,                  /* wcChar12 */
    'A', 0x00,                  /* wcChar13 */
    'P', 0x00,                  /* wcChar14 */
    /* String 3 (Serial Number) */
    0x1A,                       // bLength
    USB_DESCRIPTOR_TYPE_STRING, // bDescriptorType
    '0', 0,                     // wcChar0
    '1', 0,                     // wcChar1
    '2', 0,                     // wcChar2
    '3', 0,                     // wcChar3
    '4', 0,                     // wcChar4
    '5', 0,                     // wcChar5
    'A', 0,                     // wcChar6
    'B', 0,                     // wcChar7
    'C', 0,                     // wcChar8
    'D', 0,                     // wcChar9
    'E', 0,                     // wcChar10
    'F', 0,                     // wcChar11
#ifdef CONFIG_USB_HS
    /* Device Qualifier */
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x10,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    /* End */
    0x00
};

static void (*winusb_rx_cb)(uint32_t) = NULL;
static void (*winusb_tx_done)(void) = NULL;
static void (*winusb_cfg_done)(void) = NULL;

static void (*cdc_rx_cb)(uint32_t) = NULL;
static void (*cdc_tx_done)(void) = NULL;
static void (*cdc_cfg_done)(void) = NULL;

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            break;
        case USBD_EVENT_SUSPEND:
            break;
        case USBD_EVENT_CONFIGURED:
            /* setup first out ep read transfer */
            if(NULL != winusb_cfg_done)
            {
                winusb_cfg_done();
            }

            if(NULL != cdc_cfg_done)
            {
                cdc_cfg_done();
            }

            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

static void usbd_winusb_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if(NULL != winusb_rx_cb)
    {
        winusb_rx_cb(nbytes);
    }
}

static void usbd_winusb_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if ((nbytes % usbd_get_ep_mps(busid, ep)) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(busid, WINUSB_IN_EP, NULL, 0);
    } else {
        if(NULL != winusb_tx_done)
        {
            winusb_tx_done();
        }
    }
}

static void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if(NULL != cdc_rx_cb)
    {
        cdc_rx_cb(nbytes);
    }
}

static void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if ((nbytes % usbd_get_ep_mps(busid, ep)) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(0, CDC_IN_EP, NULL, 0);
    } else {
        if(NULL != cdc_tx_done)
        {
            cdc_tx_done();
        }
    }
}

struct usbd_endpoint winusb_out_ep1 = {
    .ep_addr = WINUSB_OUT_EP,
    .ep_cb = usbd_winusb_out
};

struct usbd_endpoint winusb_in_ep1 = {
    .ep_addr = WINUSB_IN_EP,
    .ep_cb = usbd_winusb_in
};

static struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

static struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

struct usbd_interface winusb_intf;
struct usbd_interface intf1;
struct usbd_interface intf2;

struct usb_msosv2_descriptor msosv2_desc = {
    .vendor_code = USBD_WINUSB_VENDOR_CODE,
    .compat_id = USBD_WinUSBDescriptorSetDescriptor,
    .compat_id_len = USBD_WINUSB_DESC_SET_LEN,
};

struct usb_bos_descriptor bos_desc = {
    .string = USBD_BinaryObjectStoreDescriptor,
    .string_len = USBD_BOS_WTOTALLENGTH
};

void winusb_cdc_init(uint8_t busid, uintptr_t reg_base)
{
    usbd_desc_register(busid, winusbv2_descriptor);
    usbd_bos_desc_register(busid, &bos_desc);
    usbd_msosv2_desc_register(busid, &msosv2_desc);

    /*!< winusb */
    usbd_add_interface(busid, &winusb_intf);
    usbd_add_endpoint(busid, &winusb_out_ep1);
    usbd_add_endpoint(busid, &winusb_in_ep1);

    /*!< cdc acm */
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf1));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf2));
    usbd_add_endpoint(busid, &cdc_out_ep);
    usbd_add_endpoint(busid, &cdc_in_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

void winusb_register_cb(void (*rx_cb)(uint8_t *data, uint32_t len), void (*tx_done)(void), void (*cfg_done)(void))
{
    winusb_rx_cb = rx_cb;
    winusb_tx_done = tx_done;
    winusb_cfg_done = cfg_done;
}

void winusb_send(uint8_t *buf, uint32_t len)
{
    usbd_ep_start_write(0, WINUSB_IN_EP, buf, len);
}

void winusb_recv(uint8_t *buf, uint32_t len)
{
    usbd_ep_start_read(0, WINUSB_OUT_EP, buf, len);
}

void cdc_register_cb(void (*rx_cb)(uint8_t *data, uint32_t len), void (*tx_done)(void), void (*cfg_done)(void))
{
    cdc_rx_cb = rx_cb;
    cdc_tx_done = tx_done;
    cdc_cfg_done = cfg_done;
}

void cdc_send(uint8_t *buf, uint32_t len)
{
    usbd_ep_start_write(0, CDC_IN_EP, buf, len);
}

void cdc_recv(uint8_t *buf, uint32_t len)
{
    usbd_ep_start_read(0, CDC_OUT_EP, buf, len);
}
