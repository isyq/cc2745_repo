#include "hal_os_api.h"
#include "util_tool.h"
#include "msg_bus.h"
#include "fwk_event.h"

#define FWK_CHANN_EVENT 0

DEF_WEAK_TOPIC_HANDLER(HANDLER_NAME(FWK_EVENT_INIT_STACK))
DEF_WEAK_TOPIC_HANDLER(HANDLER_NAME(FWK_EVENT_STACK_READY))
DEF_WEAK_TOPIC_HANDLER(HANDLER_NAME(FWK_EVENT_ENTER_SLEEP))
DEF_WEAK_TOPIC_HANDLER(HANDLER_NAME(FWK_EVENT_EXIT_SLEEP))

DEF_STATIC_TOPIC_LIST(m_event_topics, FWK_EVENT_COUNT)

static mbus_chann_t m_topic_channels[] =
{
    {FWK_CHANN_EVENT, "FWK Event", &m_event_topics},
};

static mbus_t m_mbus;

void fwk_event_init(void)
{
    mbus_init(&m_mbus, m_topic_channels, ARRAY_SIZE(m_topic_channels));

    mbus_register_topic(&m_mbus, FWK_CHANN_EVENT, &(mbus_topic_t){
        FWK_EVENT_INIT_STACK, "Init Stack", HANDLER_NAME(FWK_EVENT_INIT_STACK)
    });

    mbus_register_topic(&m_mbus, FWK_CHANN_EVENT, &(mbus_topic_t){
        FWK_EVENT_STACK_READY, "Stack Ready", HANDLER_NAME(FWK_EVENT_STACK_READY)
    });

    mbus_register_topic(&m_mbus, FWK_CHANN_EVENT, &(mbus_topic_t){
        FWK_EVENT_ENTER_SLEEP, "Enter Sleep", HANDLER_NAME(FWK_EVENT_ENTER_SLEEP)
    });

    mbus_register_topic(&m_mbus, FWK_CHANN_EVENT, &(mbus_topic_t){
        FWK_EVENT_EXIT_SLEEP, "Exit Sleep", HANDLER_NAME(FWK_EVENT_EXIT_SLEEP)
    });

    mbus_sort_topic(&m_mbus);
}

void fwk_event_post(uint8_t event)
{
    mbus_produce_message(&m_mbus, FWK_CHANN_EVENT, event, NULL, 0);
}

void fwk_event_pend(void)
{
    mbus_consume_all(&m_mbus);
}
