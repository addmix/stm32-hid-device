#pragma once
#include "pin_declaration.h"
#include "pin_mapping.h"
#include "input_augmentation.h"
#include "config.h"

enum Commands{
    Reset,
    TextCommand,
    GetConfigs,
    GetPins,
    GetMappings,
    GetAugments,
    ListConfigs,
    ListPins,
    ListMappings,
    ListAugments,
    ClearConfig,
    ClearPin,
    ClearMap,
    ClearAugments,
    //adds/edits item (depending on type)
    PinDeclaration,
    InputMapping,
    Augmentation,
};

void parse_serial();
void parse_line();

u_int16_t get_pin_data_size(u_int8_t index = 255);
void get_pin_data(u_int8_t*& return_buffer, u_int8_t index = 255);
u_int16_t get_mapping_data_size(u_int8_t index = 255);
void get_mapping_data(u_int8_t*& return_buffer, u_int8_t index = 255);
u_int16_t get_augment_data_size(u_int8_t index = 255);
void get_augment_data(u_int8_t*& return_buffer, u_int8_t index = 255);

void print_pins(u_int8_t index = 255);
void print_pin(u_int8_t index);
void print_mappings(u_int8_t index = 255);
void print_mapping(u_int8_t index);
void print_augments(u_int8_t index = 255);
void print_augment(u_int8_t index);
void clear_pin(u_int8_t index = 255);
void clear_mapping(u_int8_t index = 255);
void clear_augment(u_int8_t index = 255);

//2 bytes for start code, 2 bytes for payload length, >=1 byte for payload, and 1 byte for checksum
#define MINIMUM_MESSAGE_LENGTH 5 //this also represents the formatting data of commands, which excludes the payload
void parse_serial() {
    if (Serial.available() > MINIMUM_MESSAGE_LENGTH) {
        //Serial.println("Parsing serial");
        parse_line();
    }
}

//TODO: Verify that this actually works
void create_command(u_int8_t *buffer, u_int8_t *data, size_t length) {
    *buffer++ = 0xFF;
    *buffer++ = 0x00;
    
    //2 byte payload length
    *buffer++ = length >> 8 & 0xFF; //the bitwise and here may be redundant, because the buffer can only store uint8 anyways.
    *buffer++ = length & 0xFF; //the bitwise and here may be redundant, because the buffer can only store uint8 anyways.

    //payload
    u_int8_t sum = 0;
    for (size_t i = 0; i < length; ++i) {
        *buffer++ = data[i];
        //checksum
        sum += data[i];
    }
    *buffer++ = sum;
}


void parse_line() {//receive command
    
    //TODO: this basic command parsing logic should be placed into a dedicated function
    
    //first two byes are the start code
    char first_byte = Serial.read();
    char second_byte = Serial.read();

    if (first_byte != 0xFF or second_byte != 0x00) {
        //This indicates either some sort of failure, or garbage data being sent.
        //flush the buffer
        while(Serial.available() > 0) Serial.read();

        //idk print an error here or something
        Serial.println("First bytes were not start code. Aborting.");
        return;
    }
    #define MAX_PAYLOAD_BUFFER_SIZE 512
    u_int16_t payload_size = (static_cast<u_int16_t>((u_int8_t) Serial.read()) << 8) | static_cast<u_int16_t>((u_int8_t) Serial.read());
    if (payload_size > MAX_PAYLOAD_BUFFER_SIZE) {
        //If sent message is too big, abort and clear buffer
        while(Serial.available() > 0) Serial.read();
        return;
    }
    
    u_int8_t payload_buffer[payload_size];
    u_int16_t bytes_read = Serial.readBytes(payload_buffer, payload_size);

    //TODO add better checks to the lenght of the payload buffer

    //this means the readBytes function timed out before all data was read
    if ((payload_size) != bytes_read) {
        Serial.println("Serial.readBytes() timed out before reading all data. expected bytes=" + (String) (payload_size) + " bytes read=" + (String) bytes_read);
        //flush the buffer
        while(Serial.available() > 0) Serial.read();
        return;
    }

    u_int8_t checksum_value = Serial.read();
    
    //flush the buffer
    if (Serial.available() > 0) {
        Serial.println("More serial data is available than expected. Aborting.");
        return;
    }

    u_int8_t sum = 0;
    for (u_int8_t byte : payload_buffer) {
        sum += byte;
    }

    if (checksum_value != sum) {
        Serial.println("Checksum did not match: Expected checksum=" + (String) checksum_value + " calculated checksum=" + (String) sum);
        return;
    }

    //keeps track of how far into the payload we are.
    u_int8_t* message_index = payload_buffer;
    u_int8_t command = *message_index++;

    switch (command) {
        case Reset: {
            Serial.println("Reset");
            Serial.flush();
            NVIC_SystemReset();
            break;
        }
        case GetConfigs: {
            u_int16_t data_length = get_pin_data_size() + get_mapping_data_size() + get_augment_data_size();
            u_int8_t data_buffer[data_length];

            u_int8_t* data_buffer_index = data_buffer;
            get_pin_data(data_buffer_index);
            get_mapping_data(data_buffer_index);
            get_augment_data(data_buffer_index);

            u_int16_t message_length = MINIMUM_MESSAGE_LENGTH + data_length;
            u_int8_t message_buffer[message_length];
            create_command(message_buffer, data_buffer, data_length);

            Serial.write(message_buffer, message_length);
            break;
        }
        case GetPins: {
            u_int8_t index = *message_index++;

            u_int16_t data_length = get_pin_data_size(index);
            u_int8_t data_buffer[data_length];
            
            u_int8_t* data_buffer_index = data_buffer;
            get_pin_data(data_buffer_index, index);

            u_int16_t message_length = data_length + MINIMUM_MESSAGE_LENGTH;
            u_int8_t message_buffer[message_length];
            create_command(message_buffer, data_buffer, data_length);

            //Serial.write(message_buffer, message_length);
            Serial.write(data_buffer, data_length);
            
            break;
        }
        case GetMappings: {
            int index = *message_index++;

            u_int16_t data_length = get_mapping_data_size(index);
            u_int8_t data_buffer[data_length];

            u_int8_t* data_buffer_index = data_buffer;
            get_mapping_data(data_buffer_index, index);

            u_int16_t message_length = data_length + MINIMUM_MESSAGE_LENGTH;
            u_int8_t message_buffer[message_length];
            create_command(message_buffer, data_buffer, data_length);

            Serial.write(message_buffer, message_length);
            break;
        }
        case GetAugments: {
            int index = *message_index++;
            
            u_int16_t data_length = get_augment_data_size(index);
            u_int8_t data_buffer[data_length];

            u_int8_t* data_buffer_index = data_buffer;
            get_augment_data(data_buffer_index, index);

            u_int16_t message_length = data_length + MINIMUM_MESSAGE_LENGTH;
            u_int8_t message_buffer[message_length];
            create_command(message_buffer, data_buffer, data_length);

            Serial.write(message_buffer, message_length);
            break;
            break;
        }
        case ListConfigs: {
            
            Serial.println("Pins:");
            print_pins();

            Serial.println("Mappings:");
            print_mappings();

            Serial.println("Augments:");
            print_augments();

            break;
        }
        case ListPins: {
            u_int8_t index = *message_index++;
            Serial.println("Printing pin at index=" + (String) index);
            print_pins(index);
            break;
        }
        case ListMappings: {
            int index = *message_index++;
            Serial.println("Printing mapping at index=" + (String) index);
            print_mappings(index);
            break;
        }
        case ListAugments: {
            int index = *message_index++;
            Serial.println("Printing augment at index=" + (String) index);
            print_augments(index);
            break;
        }
        case ClearConfig: {
            clear_pin();
            clear_mapping();
            clear_augment();
            Serial.println("Configs cleared");
            break;
        }
        case ClearPin: {
            int index = *message_index++;
            clear_pin(index);
            Serial.println("Pins cleared");
            break;
        }
        case ClearMap: {
            int index = *message_index++;
            clear_mapping(index);
            //not sure if I should actually deallocate memory, so i'll keep it simple for now.
            //pin_bindings.shrink_to_fit();
            Serial.println("Mapping cleared");
            break;
        }
        case ClearAugments: {
            int index = *message_index++;
            clear_augment(index);
            //not sure if I should actually deallocate memory, so i'll keep it simple for now.
            //augmentations.shrink_to_fit();
            Serial.println("Augments cleared");
            break;
        }


        case PinDeclaration: {
            //syntax: int pin_number, bool analog
            PinName pin = (PinName) *message_index++;
            bool analog = *message_index++;
            pin_map[pin] = Pin(pin, analog);
            Serial.println("Added new pin: pin number=" + (String) pin + " analog=" + (String) analog);
            break;
        }
        case InputMapping: {
            PinName pin = (PinName) *message_index++;
            int input_device = *message_index++;
            int input_id = *message_index++;
            bool invert = *message_index++;

            
            pin_bindings.push_back(PinMapping(pin, input_device, input_id, invert));
            Serial.println("Added new mapping: pin number=" + (String) pin + " input device=" + (String) input_device + " input id=" + (String) input_id + " inverted=" + (String) invert);
            break;
        }
        case Augmentation: {

            break;
        }
    }
}







//response command format:
// 2 bytes start code
// 2 bytes payload length

//payload
//command that is being responded to
//section type (PinDeclaration, InputMapping, Augmentation)
//section length (total bytes)
//section type (PinDeclaration, InputMapping, Augmentation)
//section length (total bytes)
//etc...

// 1 byte checksum


u_int16_t get_pin_data_size(u_int8_t index) {
    if (index != 255) {
        return PIN_BYTE_SIZE + 3; //+3 because each data section has the type and length
    }

    return pin_map.size() * PIN_BYTE_SIZE + 3; //+3 because each data section has the type and length
}
void get_pin_data(u_int8_t*& return_buffer, u_int8_t index) {
    *return_buffer++ = PinDeclaration;

    u_int16_t data_size = get_pin_data_size(index);
    *return_buffer++ = data_size >> 8 & 0xFF; //this bitwise and may be redundant
    *return_buffer++ = data_size & 0xFF; //this bitwise and may be redundant

    if (index != 255) {
        auto it = pin_map.find(index);
        if (it != pin_map.end()) {
            it->second.pin_to_bytes(return_buffer);
        } else {
            memset(return_buffer, 0, PIN_BYTE_SIZE); //set 0s for the expected bytes of the invalid pin
            return_buffer += PIN_BYTE_SIZE; //increment the buffer pointer for those expected bytes
        }
        return;
    }
    
    for (const auto& [pin_index, pin] : pin_map) {
        pin.pin_to_bytes(return_buffer);
    }
}



u_int16_t get_mapping_data_size(u_int8_t index) {
    if (index != 255) {
        return MAPPING_BYTE_SIZE + 3; //+3 because each data section has the type and length
    }

    return pin_bindings.size() * MAPPING_BYTE_SIZE + 3; //+3 because each data section has the type and length
}
void get_mapping_data(u_int8_t*& return_buffer, u_int8_t index) {
    *return_buffer++ = InputMapping;

    u_int16_t data_size = get_mapping_data_size(index);
    *return_buffer++ = data_size >> 8 & 0xFF; //this bitwise and may be redundant
    *return_buffer++ = data_size & 0xFF; //this bitwise and may be redundant

    if (index != 255) {
        if (index < augmentations.size()) {
            PinMapping& mapping = pin_bindings[index];
            mapping.mapping_to_bytes(return_buffer);
        } else {
            memset(return_buffer, 0, MAPPING_BYTE_SIZE); //set 0s for the expected bytes of the invalid pin
            return_buffer += MAPPING_BYTE_SIZE; //increment the buffer pointer for those expected bytes
        }
        return;
    }
    
    for (size_t mapping_index = 0; mapping_index < pin_bindings.size(); ++mapping_index) {
        PinMapping& mapping = pin_bindings[mapping_index];
        mapping.mapping_to_bytes(return_buffer);
    }
}







u_int16_t get_augment_data_size(u_int8_t index) {
    if (index != 255) {
        return AUGMENT_BYTE_SIZE + 3; //+3 because each data section has the type and length
    }

    return augmentations.size() * AUGMENT_BYTE_SIZE + 3; //+3 because each data section has the type and length
}
void get_augment_data(u_int8_t*& return_buffer, u_int8_t index) {
    *return_buffer++ = Augmentation;

    u_int16_t data_size = get_augment_data_size(index);
    *return_buffer++ = data_size >> 8 & 0xFF; //this bitwise and may be redundant
    *return_buffer++ = data_size & 0xFF; //this bitwise and may be redundant

    if (index != 255) {
        if (index >= augmentations.size()) {
            InputAugmentation& augment = augmentations[index];
            augment.augment_to_bytes(return_buffer);
        } else {
            memset(return_buffer, 0, AUGMENT_BYTE_SIZE); //set 0s for the expected bytes of the invalid pin
            return_buffer += AUGMENT_BYTE_SIZE; //increment the buffer pointer for those expected bytesd
        }
        return;
    }
    
    for (size_t augment_index = 0; augment_index < augmentations.size(); ++augment_index) {
        InputAugmentation& augment = augmentations[augment_index];
        augment.augment_to_bytes(return_buffer);
    }
}









void print_pins(u_int8_t index) {
    if (index != 255) {
        print_pin(index);
        return;
    }

    for (const auto& [pin_index, pin] : pin_map) {
        print_pin((u_int8_t) pin_index);
    }
}
void print_pin(u_int8_t index) {
    auto it = pin_map.find(index);
    if (it == pin_map.end()) {
        Serial.println("Pin not found: pin=" + (String) index);
        return;
    }

    Pin& pin = it->second;

    Serial.println((String) index + 
    ": pin number=" + (String) pin.pin_name + 
    " analog=" + (String) pin.analog 
    );
}

void print_mappings(u_int8_t index) {
    if (index != 255) {
        print_mapping(index);
        return;
    }

    for (size_t mapping_index = 0; mapping_index < pin_bindings.size(); ++mapping_index) {
        print_mapping((u_int8_t) mapping_index);
    }
}
void print_mapping(u_int8_t index) {
    if (index >= pin_bindings.size()) {
        Serial.println("mapping index out of range: index=" + (String) index);
        return;
    }
    PinMapping& mapping = pin_bindings[index];

    Serial.println((String) index + 
    ": pin number=" + (String) mapping.pin_name + 
    " input device=" + (String) mapping.input_type +
    " input id=" + (String) mapping.input_id +
    " inverted=" + (String) mapping.invert +
    " max report value=" + (String) mapping.max_report_value +
    " activation value=" + (String) mapping.activation_value +
    " scale=" + (String) mapping.scale +
    " deadzone=" + (String) mapping.deadzone +
    " change amount before update=" + (String) mapping.change_amount_before_update +
    " quick release=" + (String) mapping.quick_release +
    " counter strafe help time ms=" + (String) mapping.counter_strafe_help_time_ms
    );
}

void print_augments(u_int8_t index) {
    if (index != 255) {
        print_augment(index);
        return;
    }

    for (size_t augment_index = 0; augment_index < augmentations.size(); ++augment_index) {
        print_augment((u_int8_t) augment_index);
    }
}
void print_augment(u_int8_t index) {
    if (index >= augmentations.size()) {
        Serial.println("Pin index out of range: index=" + (String) index);
        return;
    }
    InputAugmentation& augment = augmentations[index];
    
    Serial.println((String) index + 
    ": pin number=" + (String) augment.pin +
    " secondary pin number=" + (String) augment.secondary_pin +
    " type=" + (String) augment.type +
    " rotation=" + (String) augment.control_rotation
    );
}



void clear_pin(u_int8_t index) {
    if (index == 255) {
        pin_map.clear();
        return;
    }

    auto it = pin_map.find(index);

    if (it == pin_map.end()) {
        Serial.println("Pin not found: pin=" + (String) index);
        return;
    }

    pin_map.erase(index);
}
void clear_mapping(u_int8_t index) {
    if (index == 255) {
        pin_bindings.clear();
        return;
    }

    if (index < pin_bindings.size()) {
        pin_bindings.erase(pin_bindings.begin() + index);
        //pin_bindings.shrink_to_fit();
    }
}
void clear_augment(u_int8_t index) {
    if (index == 255) {
        augmentations.clear();
        return;
    }

    if (index < augmentations.size()) {
        augmentations.erase(augmentations.begin() + index);
        augmentations.shrink_to_fit();
    }
}