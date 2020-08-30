#ifndef _MISCELLANEOUS_H_
#define _MISCELLANEOUS_H_

#include <FreeRTOS.h>


typedef struct
{
    xQueueHandle q_in;
    xQueueHandle q_out;
}Queue_Pipe;

Queue_Pipe* buildQueuePipe(xQueueHandle queue_in, xQueueHandle queue_out)
{
    Queue_Pipe* queue_pipe = (Queue_Pipe*)malloc(sizeof(Queue_Pipe));

    queue_pipe->q_in = queue_in;
    queue_pipe->q_out = queue_out;

    return queue_pipe;
}

int freeQueuePipe(Queue_Pipe* queue_pipe)
{
    free(queue_pipe);
}






//---------------------------------------------------------
#define MAX_NUM_OF_MESSAGE_QUEUE_ELEMENT 5

typedef struct 
{
    char* message;
    void* data;
}Message_Queue;

inline Message_Queue* creatMQ()
{
    return (Message_Queue*)malloc(sizeof(Message_Queue));
}

inline char* createMQMessage(uint32_t message_len)
{
    return (char*)malloc(message_len);
}

inline void deleteMQ(Message_Queue* mq_handle)
{
    free(mq_handle);
}

inline void deleteMQMessage(Message_Queue* mq_handle)
{
    free(mq_handle->message);
}
//---------------------------------------------------------

typedef struct 
{
    char* alias;
    uint16_t port;
}Server_Properties;

inline Server_Properties* createSerProp()
{
    return (Server_Properties*)malloc(sizeof(Server_Properties));
}

inline char* createSerPropAlias(uint32_t alias_len)
{
    return (char*)malloc(alias_len);
}

inline void deleteSerProp(Server_Properties* sp_handle)
{
    free(sp_handle);
}

inline void deleteSerPropAlias(Server_Properties* sp_handle)
{
    free(sp_handle->alias);
}

inline void deleteWholeSerProp(Server_Properties* sp_handle)
{
    deleteSerPropAlias(sp_handle);
    deleteSerProp(sp_handle);
}

//---------------------------------------------------------

// Message_Queue* createSerManMQ(char* cmd_name, char* server_alias, unt16_t port)
// {
//     Message_Queue* temp_mq_handle = creatMQ();

//     temp_mq_handle->message = createMQMessage(cmd_name.length());
//     strcpy(temp_mq_handle->message, cmd_name.c_str());

//     Server_Properties* sp_handle = createSerProp();

//     sp_handle->alias = createSerPropAlias(server_alias.length());
//     strcpy(sp_handle->alias, server_alias.c_str());
    
//     sp_handle->port = server_port;

//     temp_mq_handle->data = sp_handle;

//     return;
// }


#endif