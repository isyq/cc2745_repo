#include "hal_os_icall.h"
#include "hal_ble_api.h"
#include "ble_service_util.h"
#include "ble_observer.h"
#include "gatt_sender.h"
#include "ble_char.h"
#include "app_util.h"
#include "logger.h"

#define SHORT_UUID_LO_INDEX 12
#define SHORT_UUID_HI_INDEX 13

// TCAM-BLE Base UUID:
// 0x0236XXXX-CF3A-11E1-EFDE-0002A5D5C51B
#define TCAM_BASE_UUID128(uuid)                                                                     \
    {                                                                                               \
        0x1B, 0xC5, 0xD5, 0xA5, 0x02, 0x00, 0xDE, 0xEF, 0xE1, 0x11, 0x3A, 0xCF, U16_LOW_BYTE(uuid), \
            U16_HIGH_BYTE(uuid), 0x36, 0x02                                                         \
    }

#define CHAR_VALUE_SIZE 255
#define GATT_PERMIT_RW  GATT_PERMIT_READ | GATT_PERMIT_WRITE

/* TCAM Service
 * UUID: 0x2AFF
 */
#define SERV_UUID16  0x2AFF
#define SERV_UUID128 TCAM_BASE_UUID128(SERV_UUID16)
static uint8_t servUuid[] = SERV_UUID128;
static gattAttrType_t servAttr = {ATT_UUID_SIZE, servUuid};

/* DK Authenticate Write Char
 * UUID: 0x2A10
 * Property: Write
 * Permission: No Auth
 * Value: uint8_t[255]
 */
#define AUTH_WRITE_UUID16  0x2A10
#define AUTH_WRITE_UUID128 TCAM_BASE_UUID128(AUTH_WRITE_UUID16)
static uint8_t authWriteCharUuid[] = AUTH_WRITE_UUID128;
static uint8_t authWriteCharProps = GATT_PROP_WRITE_NO_RSP;
static uint8_t authWriteCharLen = 0;
static uint8_t authWriteCharValue[CHAR_VALUE_SIZE];

/* DK Authenticate Notify Char
 * UUID: 0x2A11
 * Property: Notify
 * Permission: No Auth
 * Value: uint8_t[255]
 */
#define AUTH_NOTIF_UUID16  0x2A11
#define AUTH_NOTIF_UUID128 TCAM_BASE_UUID128(AUTH_NOTIF_UUID16)
static uint8_t authNotifCharUuid[] = AUTH_NOTIF_UUID128;
static uint8_t authNotifCharProps = GATT_PROP_NOTIFY;
static uint8_t authNotifCharLen = 0;
static uint8_t authNotifCharValue[CHAR_VALUE_SIZE];
static uint16_t authNotifCharHandle = 0;

static gattCharCfg_t* authNotifCharCfg;

/* DK Auxiliary Write Char
 * UUID: 0x2A12
 * Property: Write
 * Permission: No Auth
 * Value: uint8_t[255]
 */
#define AUXI_WRITE_UUID16  0x2A12
#define AUXI_WRITE_UUID128 TCAM_BASE_UUID128(AUXI_WRITE_UUID16)
static uint8_t auxiWriteCharUuid[] = AUXI_WRITE_UUID128;
static uint8_t auxiWriteCharProps = GATT_PROP_WRITE_NO_RSP;
static uint8_t auxiWriteCharLen = 0;
static uint8_t auxiWriteCharValue[CHAR_VALUE_SIZE];

/* DK Auxiliary Notify Char
 * UUID: 0x2A13
 * Property: Notify
 * Permission: No Auth
 * Value: uint8_t[255]
 */
#define AUXI_NOTIF_UUID16  0x2A13
#define AUXI_NOTIF_UUID128 TCAM_BASE_UUID128(AUXI_NOTIF_UUID16)
static uint8_t auxiNotifCharUuid[] = AUXI_NOTIF_UUID128;
static uint8_t auxiNotifCharProps = GATT_PROP_NOTIFY;
static uint8_t auxiNotifCharLen = 0;
static uint8_t auxiNotifCharValue[CHAR_VALUE_SIZE];
static uint16_t auxiNotifCharHandle = 0;

static gattCharCfg_t* auxiNotifCharCfg;

/* Charging Pile Write Char
 * UUID: 0x2A20
 * Property: Write
 * Permission: No Auth
 * Value: uint8_t[255]
 */
#define CHRG_WRITE_UUID16  0x2A20
#define CHRG_WRITE_UUID128 TCAM_BASE_UUID128(CHRG_WRITE_UUID16)
static uint8_t chrgWriteCharUuid[] = CHRG_WRITE_UUID128;
static uint8_t chrgWriteCharProps = GATT_PROP_WRITE_NO_RSP;
static uint8_t chrgWriteCharLen = 0;
static uint8_t chrgWriteCharValue[CHAR_VALUE_SIZE];

/* Charging Pile Notify Char
 * UUID: 0x2A21
 * Property: Notify
 * Permission: No Auth
 * Value: uint8_t[255]
 */
#define CHRG_NOTIF_UUID16  0x2A21
#define CHRG_NOTIF_UUID128 TCAM_BASE_UUID128(CHRG_NOTIF_UUID16)
static uint8_t chrgNotifCharUuid[] = CHRG_NOTIF_UUID128;
static uint8_t chrgNotifCharProps = GATT_PROP_NOTIFY;
static uint8_t chrgNotifCharLen = 0;
static uint8_t chrgNotifCharValue[CHAR_VALUE_SIZE];
static uint16_t chrgNotifCharHandle = 0;

static gattCharCfg_t* chrgNotifCharCfg;

/* Local event ID */
static eventId_t writeCharEventId;
static eventId_t writeCccdEventId;

static gattAttribute_t gattTable[] = {
  /* Service */
    {{ATT_BT_UUID_SIZE, primaryServiceUUID}, GATT_PERMIT_READ,  0, (uint8_t*)&servAttr        },

 // DK Auth Write Char Characteristic
    {{ATT_BT_UUID_SIZE, characterUUID},      GATT_PERMIT_READ,  0, &authWriteCharProps        },
    {{ATT_UUID_SIZE, authWriteCharUuid},     GATT_PERMIT_WRITE, 0, authWriteCharValue         },

 // DK Auth Notify Char Characteristic
    {{ATT_BT_UUID_SIZE, characterUUID},      GATT_PERMIT_READ,  0, &authNotifCharProps        },
    {{ATT_UUID_SIZE, authNotifCharUuid},     0,                 0, authNotifCharValue         },
    {{ATT_BT_UUID_SIZE, clientCharCfgUUID},  GATT_PERMIT_RW,    0, (uint8_t*)&authNotifCharCfg},

 // DK Aux Write Char Characteristic
    {{ATT_BT_UUID_SIZE, characterUUID},      GATT_PERMIT_READ,  0, &auxiWriteCharProps        },
    {{ATT_UUID_SIZE, auxiWriteCharUuid},     GATT_PERMIT_WRITE, 0, auxiWriteCharValue         },

 // DK Aux Notify Char Characteristic
    {{ATT_BT_UUID_SIZE, characterUUID},      GATT_PERMIT_READ,  0, &auxiNotifCharProps        },
    {{ATT_UUID_SIZE, auxiNotifCharUuid},     0,                 0, auxiNotifCharValue         },
    {{ATT_BT_UUID_SIZE, clientCharCfgUUID},  GATT_PERMIT_RW,    0, (uint8_t*)&auxiNotifCharCfg},

 // Charging Pile Write Char Characteristic
    {{ATT_BT_UUID_SIZE, characterUUID},      GATT_PERMIT_READ,  0, &chrgWriteCharProps        },
    {{ATT_UUID_SIZE, chrgWriteCharUuid},     GATT_PERMIT_WRITE, 0, chrgWriteCharValue         },

 // Charging Pile Notify Char Characteristic
    {{ATT_BT_UUID_SIZE, characterUUID},      GATT_PERMIT_READ,  0, &chrgNotifCharProps        },
    {{ATT_UUID_SIZE, chrgNotifCharUuid},     0,                 0, chrgNotifCharValue         },
    {{ATT_BT_UUID_SIZE, clientCharCfgUUID},  GATT_PERMIT_RW,    0, (uint8_t*)&chrgNotifCharCfg},
};
static uint8_t gattTableSize = GATT_NUM_ATTRS(gattTable);

static uint8_t linkCountMax = 0;

static bStatus_t gattWriteAttrCB(uint16_t connHandle, gattAttribute_t* pAttr, uint8_t* pValue, uint16_t len,
                                 uint16_t offset, uint8_t method)
{
    bStatus_t status = SUCCESS;

    if (connHandle == BAD_CONN_HANDLE)
    {
        return ATT_ERR_INVALID_HANDLE;
    }

    /* Write characteristic value */
    if (pAttr->type.len == UUID_SIZE_VS)
    {
        uint8_t lowByte = pAttr->type.uuid[SHORT_UUID_LO_INDEX];
        uint8_t highByte = pAttr->type.uuid[SHORT_UUID_HI_INDEX];
        uint16_t uuid = BUILD_UINT16(lowByte, highByte);

        switch (uuid)
        {
        case AUTH_WRITE_UUID16:
            memcpy(pAttr->pValue, pValue, len);
            BleService_postWriteCharEvent(writeCharEventId, connHandle, BLE_CHANN_DK_AUTH, pValue, len);
            break;

        case AUXI_WRITE_UUID16:
            memcpy(pAttr->pValue, pValue, len);
            BleService_postWriteCharEvent(writeCharEventId, connHandle, BLE_CHANN_DK_AUXI, pValue, len);
            break;

        case CHRG_WRITE_UUID16:
            memcpy(pAttr->pValue, pValue, len);
            BleService_postWriteCharEvent(writeCharEventId, connHandle, BLE_CHANN_CHARGER, pValue, len);
            break;

        default:
            status = ATT_ERR_ATTR_NOT_FOUND;
            break;
        }
    }
    /* Write CCCD config */
    else if (pAttr->type.len == UUID_SIZE_SIG)
    {
        uint16 uuid = U16_DECODE_LE(pAttr->type.uuid);

        if (uuid == GATT_CLIENT_CHAR_CFG_UUID)
        {
            if (method == ATT_WRITE_REQ)
            {
                status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len, offset,
                                                        GATT_CLIENT_CFG_NOTIFY);
                if (status == SUCCESS)
                {
                    bool cccdEnabled;
                    uint16_t cccdValue;

                    cccdValue = U16_DECODE_LE(pValue);
                    cccdEnabled = (cccdValue == GATT_CLIENT_CFG_NOTIFY) ? true : false;

                    if (pAttr->handle == authNotifCharHandle)
                    {
                        BleService_postWriteCccdEvent(writeCccdEventId, connHandle, BLE_CHANN_DK_AUTH,
                                                      cccdEnabled);
                    }
                    else if (pAttr->handle == auxiNotifCharHandle)
                    {
                        BleService_postWriteCccdEvent(writeCccdEventId, connHandle, BLE_CHANN_DK_AUXI,
                                                      cccdEnabled);
                    }
                    else if (pAttr->handle == chrgNotifCharHandle)
                    {
                        BleService_postWriteCccdEvent(writeCccdEventId, connHandle, BLE_CHANN_CHARGER,
                                                      cccdEnabled);
                    }
                    else
                    {
                        status = ATT_ERR_ATTR_NOT_FOUND;
                    }
                }
            }
            else
            {
                status = ATT_ERR_UNSUPPORTED_REQ;
            }
        }
        else
        {
            status = ATT_ERR_ATTR_NOT_FOUND;
        }
    }
    else
    {
        status = ATT_ERR_ATTR_NOT_FOUND;
    }

    if (status != SUCCESS)
    {
        Logger_info("Failed to Write Attr: 0x%x, 0x%x", status, method);
    }

    return status;
}

static bStatus_t gattReadAttrCB(uint16_t connHandle, gattAttribute_t* pAttr, uint8_t* pValue, uint16_t* pLen,
                                uint16_t offset, uint16_t maxLen, uint8_t method)
{
    bStatus_t status = SUCCESS;

    // Make sure it's not a blob operation (no attributes in the profile are long)
    if (offset > 0)
    {
        status = ATT_ERR_ATTR_NOT_LONG;
    }
    else
    {
        if (pAttr->type.len == UUID_SIZE_VS)
        {
            uint16 uuid = U16_DECODE_LE(&pAttr->type.uuid[SHORT_UUID_LO_INDEX]);
            switch (uuid)
            {
            case AUTH_NOTIF_UUID16:
                *pLen = GET_MIN(maxLen, authNotifCharLen);
                memcpy(pValue, pAttr->pValue, *pLen);
                break;

            case AUXI_NOTIF_UUID16:
                *pLen = GET_MIN(maxLen, auxiNotifCharLen);
                memcpy(pValue, pAttr->pValue, *pLen);
                break;

            case CHRG_NOTIF_UUID16:
                *pLen = GET_MIN(maxLen, chrgNotifCharLen);
                memcpy(pValue, pAttr->pValue, *pLen);
                break;

            default:    // Should never get here
                *pLen = 0;
                status = ATT_ERR_ATTR_NOT_FOUND;
                break;
            }
        }
    }

    return status;
}

const gattServiceCBs_t gattServCBs = {
    gattReadAttrCB,     // Read callback function pointer
    gattWriteAttrCB,    // Write callback function pointer
    NULL,               // Authorization callback function pointer
};

static uint16_t getCharHandle(uint8_t* pValue)
{
    return GATTServApp_FindAttr(gattTable, gattTableSize, pValue)->handle;
}

bStatus_t Tcam_addService(uint8_t linkNumMax, eventHandler_t writeCharCB, eventHandler_t writeCccdCB)
{
    bStatus_t status;

    /* Allocate memory for charCfgs and initialize */
    BleService_initCharCfg(&authNotifCharCfg, linkNumMax);
    BleService_initCharCfg(&auxiNotifCharCfg, linkNumMax);
    BleService_initCharCfg(&chrgNotifCharCfg, linkNumMax);

    linkCountMax = linkNumMax;

    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService(gattTable, gattTableSize, GATT_MAX_ENCRYPT_KEY_SIZE, &gattServCBs);
    if (status == SUCCESS)
    {
        authNotifCharHandle = getCharHandle((uint8_t*)&authNotifCharCfg);
        auxiNotifCharHandle = getCharHandle((uint8_t*)&auxiNotifCharCfg);
        chrgNotifCharHandle = getCharHandle((uint8_t*)&chrgNotifCharCfg);

        /* Subscribe events */
        BleObs_getEventId(&writeCharEventId);
        BleObs_subscribe(EVENT_GROUP_CUSTOM, writeCharEventId, "Peer Write Char", writeCharCB);

        BleObs_getEventId(&writeCccdEventId);
        BleObs_subscribe(EVENT_GROUP_CUSTOM, writeCccdEventId, "Peer Write CCCD", writeCccdCB);
    }

    return status;
}

static bool getCccdState(uint16_t connHandle, gattCharCfg_t* pCharCfg)
{
    bool result = false;

    for (uint8_t i = 0; i < linkCountMax; i++)
    {
        if (pCharCfg[i].connHandle == connHandle)
        {
            result = (pCharCfg[i].value != 0);
        }
    }

    return result;
}

static bStatus_t gattNotify(uint16_t connHandle, uint8_t* pData, uint8_t dataLen, gattCharCfg_t* pCharCfg,
                            uint8_t* pCharValue, uint8_t* pCharValueLen)
{
    bStatus_t status;

    if (pData == NULL || dataLen == 0)
    {
        status = INVALIDPARAMETER;
    }
    else if (connHandle != BAD_CONN_HANDLE)
    {
        memcpy(pCharValue, pData, dataLen);
        *pCharValueLen = dataLen;

        uint16_t charHandle = getCharHandle(pCharValue);
        if (getCccdState(connHandle, pCharCfg))
        {
            status = GattSender_send(connHandle, charHandle, pData, dataLen, GATT_OP_NOTIFY);
        }
        else
        {
            status = bleIncorrectMode;
        }
    }
    else
    {
        status = bleNotReady;
    }

    return status;
}

bStatus_t Tcam_notifyToDkAuth(uint16_t connHandle, uint8_t* pData, uint8_t dataLen)
{
    return gattNotify(connHandle, pData, dataLen, authNotifCharCfg, authNotifCharValue, &authNotifCharLen);
}

bStatus_t Tcam_notifyToDkAuxi(uint16_t connHandle, uint8_t* pData, uint8_t dataLen)
{
    return gattNotify(connHandle, pData, dataLen, auxiNotifCharCfg, auxiNotifCharValue, &auxiNotifCharLen);
}

bStatus_t Tcam_notifyToCharger(uint16_t connHandle, uint8_t* pData, uint8_t dataLen)
{
    return gattNotify(connHandle, pData, dataLen, chrgNotifCharCfg, chrgNotifCharValue, &chrgNotifCharLen);
}

bStatus_t Tcam_notify(uint16_t connHandle, bleChann_t channId, uint8_t* pData, uint8_t dataLen)
{
    bStatus_t status;

    switch (channId)
    {
    case BLE_CHANN_DK_AUTH:
        status = Tcam_notifyToDkAuth(connHandle, pData, dataLen);
        break;

    case BLE_CHANN_DK_AUXI:
        status = Tcam_notifyToDkAuxi(connHandle, pData, dataLen);
        break;

    case BLE_CHANN_CHARGER:
        status = Tcam_notifyToCharger(connHandle, pData, dataLen);
        break;

    default:
        status = RET_ERR_NOT_FOUND;
        break;
    }

    return status;
}
