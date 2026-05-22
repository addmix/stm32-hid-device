#pragma once
#include "pin_declaration.h"
#include "pin_mapping.h"
#include "input_augmentation.h"
#include "config.h"

//TODO: Clean up command list. Most of the commands for specific types (pins, mappings, augments), the list commands, and the clear commands aren't very useful.
// Maybe the simplified list could be: Reset, TextCommand, GetConfigs, SetConfigs, ListConfigs, ClearConfig,
enum Commands{
        Reset,
        TextCommand,
        GetConfigs,
    GetPins,
    GetMappings,
    GetAugments,
        SetConfigs,
    SetPins,
    SetMappings,
    SetAugments,
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

u_int16_t get_pin_data_size(u_int8_t index);
void get_pin_data(u_int8_t*& return_buffer, u_int8_t index = 255);
u_int16_t get_mapping_data_size(u_int8_t index);
void get_mapping_data(u_int8_t*& return_buffer, u_int8_t index = 255);
u_int16_t get_augment_data_size(u_int8_t index);
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

#define START_CODE_BYTE_SIZE 2
#define SECTION_HEADER_BYTE_SIZE 3
//2 bytes for start code, 2 bytes for payload length, >=1 byte for payload, and 1 byte for checksum
#define MINIMUM_MESSAGE_LENGTH START_CODE_BYTE_SIZE + SECTION_HEADER_BYTE_SIZE //this also represents the formatting data of commands, which excludes the payload

void parse_serial() {
    if (Serial.available() > MINIMUM_MESSAGE_LENGTH) {
        //Serial.println("Parsing serial");
        parse_line();
    }
}

//TODO: Verify that this actually works
void create_command(u_int8_t *buffer, u_int8_t command, u_int8_t *data, size_t length) {
    *buffer++ = 0xFF;
    *buffer++ = 0x00;
    
    //2 byte payload length
    *buffer++ = length >> 8 & 0xFF; //the bitwise and here may be redundant, because the buffer can only store uint8 anyways.
    *buffer++ = length & 0xFF; //the bitwise and here may be redundant, because the buffer can only store uint8 anyways.

    *buffer++ = command;

    //payload
    //command is included in the checksum because the command is considered as the first byte of the payload
    u_int8_t sum = command;
    for (size_t i = 0; i < length; ++i) {
        *buffer++ = data[i];
        //checksum
        sum += data[i];
    }
    *buffer++ = sum;
}


void get_configs() {
    u_int16_t data_length = get_config_data_size(); //+3 because each data section needs a command byte, and 2 size bytes. the +3 isn't baked into the size functions because those 3 bytes are not considered part of the section payload.
    u_int8_t data_buffer[data_length];
    
    u_int8_t* data_buffer_index = data_buffer;
    get_pin_data(data_buffer_index);
    get_mapping_data(data_buffer_index);
    get_augment_data(data_buffer_index);

    u_int16_t message_length = MINIMUM_MESSAGE_LENGTH + data_length + 1; //+1 for the command byte
    u_int8_t message_buffer[message_length];
    create_command(message_buffer, GetConfigs, data_buffer, data_length);

    Serial.write(message_buffer, message_length);
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
    //TODO can this casting be done differently?
    u_int16_t payload_size = (static_cast<u_int16_t>((u_int8_t) Serial.read()) << 8) | static_cast<u_int16_t>((u_int8_t) Serial.read());
    if (payload_size > MAX_PAYLOAD_BUFFER_SIZE) {
        //If sent message is too big, abort and clear buffer
        while(Serial.available() > 0) Serial.read();
        return;
    }
    
    u_int8_t payload_buffer[payload_size];
    
    uint16_t bytes_read = 0;
    //this must be read manually, because the default Serial.readBytes() function doesn't clear the buffer until it's completion.
    while (bytes_read < payload_size) {
        if (Serial.available()) {
            payload_buffer[bytes_read++] = Serial.read();
        }
        //else {
        //    delayMicroseconds(10); //there may be some better solution here
        //}
    }

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
            //return config data to godot app
            get_configs();
            break;
        }
        case GetPins: {
            u_int8_t index = *message_index++;

            u_int16_t data_length = get_pin_data_size(index);
            u_int8_t data_buffer[data_length];
            
            u_int8_t* data_buffer_index = data_buffer;
            get_pin_data(data_buffer_index, index);

            u_int16_t message_length = MINIMUM_MESSAGE_LENGTH + data_length + 1; //+1 for the command byte
            u_int8_t message_buffer[message_length];
            create_command(message_buffer, GetPins, data_buffer, data_length);

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

            u_int16_t message_length = MINIMUM_MESSAGE_LENGTH + data_length + 1; //+1 for the command byte
            u_int8_t message_buffer[message_length];
            create_command(message_buffer, GetMappings, data_buffer, data_length);

            Serial.write(message_buffer, message_length);
            break;
        }
        case GetAugments: {
            int index = *message_index++;
            
            u_int16_t data_length = get_augment_data_size(index);
            u_int8_t data_buffer[data_length];

            u_int8_t* data_buffer_index = data_buffer;
            get_augment_data(data_buffer_index, index);

            u_int16_t message_length = MINIMUM_MESSAGE_LENGTH + data_length + 1; //+1 for the command byte
            u_int8_t message_buffer[message_length];
            create_command(message_buffer, GetAugments, data_buffer, data_length);

            Serial.write(message_buffer, message_length);
            break;
        }
        case SetConfigs: {
            //receive config data from godot app
            Serial.println("set configs");
            
            
			//#TODO: these data_type values aren't actually used by the parser, but they should be.
			//#right now it only works because both codebases agree on the order
			
			//#PinDeclaration
			u_int8_t data_type = *message_index++;//var data_type : int = read_next_byte.call()
			Serial.println("data type=" + (String) data_type);//print("data type=", data_type, "=", Enums.Commands.find_key(data_type))
			u_int16_t section_length = ((u_int16_t) *message_index++ << 8 ) + (u_int16_t) *message_index++;//var section_length : int = (read_next_byte.call() << 8) + read_next_byte.call()
			Serial.println("section length=" + (String) section_length); //print("section length=", section_length)
			//print(read_buffer.slice(read_buffer_index, read_buffer_index + section_length))
			
            //get expected amount of pin objects to parse
            u_int8_t objects_to_parse = section_length / Pin::BYTE_SIZE;
            Pin pin_buffer[objects_to_parse];

            Pin* pin_buffer_index = pin_buffer;
            Pin::from_byte_array(message_index, pin_buffer_index, objects_to_parse); //var pins : Array[PinDeclaration] = PinDeclaration.from_byte_array(read_buffer.slice(read_buffer_index, read_buffer_index + section_length))

            clear_pin();

            Pin* pin_buffer_print_index = pin_buffer;
            for(size_t i = 0; i < objects_to_parse; ++i) {
                pin_map.insert({pin_buffer_print_index->pin_name, *pin_buffer_print_index});
                //pin_buffer_print_index->print();
                pin_buffer_print_index++;
            }
			

			//#InputMapping
			data_type = *message_index++;//data_type = read_next_byte.call()
			Serial.println("data type=" + (String) data_type);//print("data type=", data_type, "=", Enums.Commands.find_key(data_type))
			section_length = ((u_int16_t) *message_index++ << 8 ) + (u_int16_t) *message_index++;//section_length = (read_next_byte.call() << 8) + read_next_byte.call()
			Serial.println("section length=" + (String) section_length);//print("section length=", section_length)
			//print(read_buffer.slice(read_buffer_index, read_buffer_index + section_length))
			
            //get expected amount of mapping objects to parse
            objects_to_parse = section_length / PinMapping::BYTE_SIZE;
            PinMapping mapping_buffer[objects_to_parse];

            PinMapping* mapping_buffer_index = mapping_buffer;
            PinMapping::from_byte_array(message_index, mapping_buffer_index, objects_to_parse);//var mappings : Array[InputMapping] = InputMapping.from_byte_array(read_buffer.slice(read_buffer_index, read_buffer_index + section_length))
			
            clear_mapping();

            PinMapping* mapping_buffer_print_index = mapping_buffer;
            for(size_t i = 0; i < objects_to_parse; ++i) {
                pin_bindings.push_back(*mapping_buffer_print_index);
                //mapping_buffer_print_index->print();
                mapping_buffer_print_index++;
            }
            
			//#Augmentation
			data_type = *message_index++;//data_type = read_next_byte.call()
			Serial.println("data type=" + (String) data_type);//print("data type=", data_type, "=", Enums.Commands.find_key(data_type))
			section_length = ((u_int16_t) *message_index++ << 8 ) + (u_int16_t) *message_index++;//section_length = (read_next_byte.call() << 8) + read_next_byte.call()
			Serial.println("section length=" + (String) section_length);//print("section length=", section_length)
			//print(read_buffer.slice(read_buffer_index, read_buffer_index + section_length))
			
            //get expected amount of augment objects to parse
            objects_to_parse = section_length / InputAugmentation::BYTE_SIZE;
            InputAugmentation augment_buffer[objects_to_parse];

            InputAugmentation* augment_buffer_index = augment_buffer;
            InputAugmentation::from_byte_array(message_index, augment_buffer_index, objects_to_parse);//var augments : Array[Augmentation] = Augmentation.from_byte_array(read_buffer.slice(read_buffer_index, read_buffer_index + section_length))
			
            clear_augment();

            InputAugmentation* augment_buffer_print_index = augment_buffer;
            for(size_t i = 0; i < objects_to_parse; ++i) {
                augmentations.push_back(*augment_buffer_print_index);
                //augment_buffer_print_index->print();
                augment_buffer_print_index++;
            }
			

			//print("remaining bytes in buffer=", read_buffer.slice(read_buffer_index))

            delay(10);
            //This is a decent idea, however we don't want to overwrite values in the UI in the event there are serialization issues.
            //get_configs(); //echo the result back to godot for comparison 
            break; //TODO
        }
        case SetPins: {
            break; //TODO
        }
        case SetMappings: {
            break; //TODO
        }
        case SetAugments: {
            break; //TODO
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
            u_int16_t inactive_value = ((u_int16_t) *message_index++ << 8 ) + (u_int16_t) *message_index++;
            pin_map[pin] = Pin(pin, analog, inactive_value);
            Serial.println("Added new pin: pin number=" + (String) pin + " analog=" + (String) analog + " inactive value=" + (String) inactive_value);
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
            //TODO
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
        return PIN_BYTE_SIZE;
    }

    return pin_map.size() * PIN_BYTE_SIZE;
}
void get_pin_data(u_int8_t*& return_buffer, u_int8_t index) {
    *return_buffer++ = PinDeclaration;

    u_int16_t data_size = get_pin_data_size(index);
    *return_buffer++ = data_size >> 8 & 0xFF; //this bitwise and may be redundant
    *return_buffer++ = data_size & 0xFF; //this bitwise and may be redundant

    if (index != 255) {
        auto it = pin_map.find(index);
        if (it != pin_map.end()) {
            it->second.to_bytes(return_buffer);
        } else {
            memset(return_buffer, 0, PIN_BYTE_SIZE); //set 0s for the expected bytes of the invalid pin
            return_buffer += PIN_BYTE_SIZE; //increment the buffer pointer for those expected bytes
        }
        return;
    }
    
    for (const auto& [pin_index, pin] : pin_map) {
        pin.to_bytes(return_buffer);
    }
}



u_int16_t get_mapping_data_size(u_int8_t index) {
    if (index != 255) {
        return MAPPING_BYTE_SIZE;
    }

    return pin_bindings.size() * MAPPING_BYTE_SIZE;
}
void get_mapping_data(u_int8_t*& return_buffer, u_int8_t index) {
    *return_buffer++ = InputMapping;

    u_int16_t data_size = get_mapping_data_size(index);
    *return_buffer++ = data_size >> 8 & 0xFF; //this bitwise and may be redundant
    *return_buffer++ = data_size & 0xFF; //this bitwise and may be redundant

    if (index != 255) {
        if (index < augmentations.size()) {
            PinMapping& mapping = pin_bindings[index];
            mapping.to_bytes(return_buffer);
        } else {
            memset(return_buffer, 0, MAPPING_BYTE_SIZE); //set 0s for the expected bytes of the invalid pin
            return_buffer += MAPPING_BYTE_SIZE; //increment the buffer pointer for those expected bytes
        }
        return;
    }
    
    for (size_t mapping_index = 0; mapping_index < pin_bindings.size(); ++mapping_index) {
        PinMapping& mapping = pin_bindings[mapping_index];
        mapping.to_bytes(return_buffer);
    }
}







u_int16_t get_augment_data_size(u_int8_t index) {
    if (index != 255) {
        return AUGMENT_BYTE_SIZE;
    }

    return augmentations.size() * AUGMENT_BYTE_SIZE;
}
void get_augment_data(u_int8_t*& return_buffer, u_int8_t index) {
    *return_buffer++ = Augmentation;

    u_int16_t data_size = get_augment_data_size(index);
    *return_buffer++ = data_size >> 8 & 0xFF; //this bitwise and may be redundant
    *return_buffer++ = data_size & 0xFF; //this bitwise and may be redundant

    if (index != 255) {
        if (index >= augmentations.size()) {
            InputAugmentation& augment = augmentations[index];
            augment.to_bytes(return_buffer);
        } else {
            memset(return_buffer, 0, AUGMENT_BYTE_SIZE); //set 0s for the expected bytes of the invalid pin
            return_buffer += AUGMENT_BYTE_SIZE; //increment the buffer pointer for those expected bytesd
        }
        return;
    }
    
    for (size_t augment_index = 0; augment_index < augmentations.size(); ++augment_index) {
        InputAugmentation& augment = augmentations[augment_index];
        augment.to_bytes(return_buffer);
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

    Serial.print((String) index + ": ");
    pin.print();
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

    Serial.print((String) index + ": ");
    mapping.print();
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
    
    Serial.print((String) index + ": ");
    augment.print();
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