#include "vedparse.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

typedef enum
{
    IDLE = 0,
    RECORD_BEGIN,
    RECORD_NAME,
    RECORD_VALUE,
    CHECKSUM,
    RECORD_HEX,
} VEDPARSE_state_e;

typedef struct 
{
    uint8_t prop_char_index;
    vedframe_t frame;
    vedprop_t prop;
    VEDPARSE_state_e state;
    uint8_t checksum;
} VEDPARSE_data_t;

const char* checksumTagName = "CHECKSUM";
static VEDPARSE_data_t _internal;

void doAddProperty(vedframe_t*, vedprop_t*);
void doEndFrame(vedframe_t*, uint8_t, uint8_t);
bool is_hex_char(char c);

bool is_hex_char(char c) {
    return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'));
}

void doAddProperty(vedframe_t* pFrame, vedprop_t* pProp)
{
    //printf("Key: %s, Value: %s\n", pProp->key, pProp->value);

    uint8_t index = pFrame->property_count;

    vedprop_t* pDest = &pFrame->properties[index];
    vedprop_t* pSrc = pProp;

    memcpy(pDest, pSrc, sizeof(vedprop_t));

    pFrame->property_count++;
}

void doEndFrame(vedframe_t* pFrame, uint8_t checksumReceived, uint8_t checksumCalc)
{
    pFrame->checksum_valid = (checksumReceived == checksumCalc);
}

void VEDPARSE_init()
{
    memset(&_internal, 0, sizeof(VEDPARSE_data_t)); // clear any history
}


bool VEDPARSE_process(uint8_t inbyte)
{
    bool frame_ready = false;
    //printf("State: %d\n", _internal.state);

    // check for hex records, we will ignore these
    if ( (inbyte == ':') && (_internal.state != CHECKSUM) ) 
    {
        _internal.state = RECORD_HEX;
    }

    // compute the checksum on the fly
    if (_internal.state != RECORD_HEX) 
    {
        _internal.checksum += inbyte;
    }
    
    // convert the char to upper case if we can
    inbyte = toupper(inbyte);

    // run the sate machine
    switch(_internal.state) 
    {
        case IDLE:  /* wait for \n of the start of an record */

            switch(inbyte) 
            {
                case '\n':
                    _internal.state = RECORD_BEGIN;
                break;
                case '\r': /* Skip */
                default:
                    // do nothing
                break;
            }
        break;

        case RECORD_BEGIN:
            _internal.prop.key[_internal.prop_char_index++] = inbyte; // first we will be writing to the key
            _internal.state = RECORD_NAME; 
        break;

        case RECORD_NAME:
        switch(inbyte) // The record name is being received, terminated by a \t
        {
            case '\t':
                // the Checksum record indicates a EOR
                if ( _internal.prop_char_index < VEDPARSE_KEY_LENGTH_MAX ) 
                {
                    _internal.prop.key[_internal.prop_char_index++] = 0; /* Zero terminate */
                    if (strcmp(_internal.prop.key, checksumTagName) == 0)  // see if this is the checksum property
                    {
                        _internal.state = CHECKSUM;
                        break;
                    }
                }
                _internal.prop_char_index = 0; // reset the indexer
                _internal.state = RECORD_VALUE;
            break;
            default:
                // add byte to name, but do no overflow
                if ( _internal.prop_char_index < VEDPARSE_KEY_LENGTH_MAX  )
                    _internal.prop.key[_internal.prop_char_index++] = inbyte; 
            break;
        }
        break;

        case RECORD_VALUE:
            // The record value is being received. The \r indicates a new record.
            switch(inbyte) 
            {
                case '\n':
                    // forward record, only if it could be stored completely
                    if ( _internal.prop_char_index < VEDPARSE_VALUE_LENGTH_MAX ) 
                    {
                        _internal.prop.value[_internal.prop_char_index++] = 0; /* Zero terminate */
                        doAddProperty(&_internal.frame, &_internal.prop); // add the completed property to the frame struct
                    }
                    _internal.prop_char_index = 0; // reset the indexer
                    _internal.state = RECORD_BEGIN;
                break;
            case '\r': /* Skip */
            break;
            default:
                // add byte to value, but do no overflow
                if ( _internal.prop_char_index < VEDPARSE_VALUE_LENGTH_MAX)
                    _internal.prop.value[_internal.prop_char_index++] = inbyte; 
            break;
            }
        break;

        case CHECKSUM:
        {
            //printf("CSUM is %02X\n", _internal.checksum);
            bool valid = (_internal.checksum == 0);
            _internal.frame.checksum_valid = valid;
            _internal.frame.checksum = _internal.checksum;
            #if 0
            if (!valid)
                printf("[CHECKSUM] Invalid frame\n");
            #endif
            _internal.checksum = 0;
            _internal.state = IDLE;
            frame_ready = true;
            // frameEndEvent(valid); // TODO: add callback
            break;
        }

        case RECORD_HEX:
        if (!is_hex_char(inbyte)) // TODO: check me
        {
            _internal.checksum = 0;
            _internal.state = IDLE;
        }
        break;
    }

    return frame_ready;
}


int32_t VEDPARSE_get_frame(vedframe_t* pFrame)
{
    memcpy(pFrame, &_internal.frame, sizeof(vedframe_t));
    return 0;
}